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
