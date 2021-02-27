# chatserver
可以工作在nginx tcp负载均衡环境中的集群聊天服务器和客户端源码  基于muduo实现

#### 需要的环境

1. ubuntu linux环境
2. 安装Json开发库
3. 安装boost + muduo网络库开发环境，参考博客：
4. 安装redis环境
5. 安装mysql数据库环境
6. 安装nginx
7. 安装CMake环境
#### 数据库连接池
![1](https://github.com/donghe0313/chatserver/blob/main/pic/6.png)
可以看出单线程是连接池的优势很明显，但在多线程下优势有所下降，可能是因为频繁的枷锁和解锁耗费了很多时间，

改进方向：使用无锁队列

#### 整体结构
![1](https://github.com/donghe0313/chatserver/blob/main/pic/5.png)
#### 数据库设计
![2](https://github.com/donghe0313/chatserver/blob/main/pic/1.png)
##### offlinemessage表
![3](https://github.com/donghe0313/chatserver/blob/main/pic/2.png)
#### 服务器集群
![4](https://github.com/donghe0313/chatserver/blob/main/pic/3.png)
#### 跨服务器通信
![5](https://github.com/donghe0313/chatserver/blob/main/pic/4.png)

#### 遇到的问题 （1） 
  客户端偶尔出现登录以后没有响应，造成业务不能继续进行，
  原因：
  redisCommand发送的subscribe订阅命令，但是通过调用堆栈信息查看，redisCommand不仅可以发送subscribe订阅命令到redis server，还会以阻塞的方式等待redis server的响应。

  解决办法：
  redisCommand，它实际上相当于调用了这三个函数：

  redisAppendCommand 把命令写入本地发送缓冲区

  redisBufferWrite 把本地缓冲区的命令通过网络发送出去

  redisGetReply 阻塞等待redis server响应消息

  因此另起一个线程专门执行接收和收到后的回调函数
  ##### 遇到的问题（2）

切换到使用自己仿写的muduo库进行聊天时，因为客户端因为Json::parse()错误中断，但登录时返回的信息能正常返回。

原因：

登录时返回的是同一个线程内的，但聊天时，若两个用户不在一个loop内，则是不同的线程。问题就出在mymuduo的TcpConnection::send(const std::string &buf)这个函数中。c_str（）返回的是string中的char*；

因此在此线程中传完回调函数后

 loop_->runInLoop(std::bind(&TcpConnection::sendInLoop,this,buf.c_str(),buf.size())); ，把内存空间释放掉，因此在执行此函数的loop中的指针为空悬指针；

```c++
void TcpConnection::send(const std::string &buf)
{
    if (state_ == kConnected)
    {
        if (loop_->isInLoopThread())
        {
            sendInLoop(buf.c_str(), buf.size());
        }
        else
        {
            loop_->runInLoop(std::bind(
                &TcpConnection::sendInLoop,
                this,
                buf.c_str(),
                buf.size()
            ));
        }
    }
}
```

解决方案：

在类中新加一个成员变量std::string otherbuf ,且修改send函数，把传入的字符拷贝到此connection的otherbuf中

void TcpConnection::send(const std::string &buf)

    void TcpConnection::send(const std::string &buf)
    {
        if (state_ == kConnected)
        {
            if (loop_->isInLoopThread())
            {
                sendInLoop(buf.c_str(), buf.size());
            }
            else
            {
                otherbuf = buf;
                loop_->runInLoop(std::bind(
                    &TcpConnection::sendInLoop,
                    this,
                    otherbuf.c_str(),
                    otherbuf.size()
                ));
            }
        }
    }


