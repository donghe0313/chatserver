/*
muduo网络库给用户提供了两个主要的类
TcpServer ： 用于编写服务器程序的
TcpClient ： 用于编写客户端程序的

epoll + 线程池
好处：能够把网络I/O的代码和业务代码区分开
                        用户的连接和断开       用户的可读写事件
*/
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
//#include <muduo/base/Logging.h>

#include <iostream>
#include <functional>
#include <string>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

/*基于muduo网络库开发服务器程序
1.组合TcpServer对象
2.创建EventLoop事件循环对象的指针
3.明确TcpServer构造函数需要什么参数，输出ChatServer的构造函数
4.在当前服务器类的构造函数当中，注册处理连接的回调函数和处理读写时间的回调函数
5.设置合适的服务端线程数量，muduo库会自己分配I/O线程和worker线程
*/


// class ChatServer
// {
// public:
//     ChatServer(EventLoop *loop,               // 事件循环
//                const InetAddress &listenAddr, // IP+Port
//                const string &nameArg)
//         : _server(loop, listenAddr, nameArg), _loop(loop)
//     {
//         // 给服务器注册用户连接的创建和断开回调
//         _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

//         // 给服务器注册用户读写事件回调
//         _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

//         // 设置服务器端的线程数量 1个I/O线程   4个worker线程
//         _server.setThreadNum(11);
//     }

//     void start()
//     {
//         _server.start();
//     }

// private:
//     // 专门处理用户的连接创建和断开  epoll listenfd accept
//     void onConnection(const TcpConnectionPtr &conn)
//     {
//         if (conn->connected())
//         {
//             cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << " state:online" << endl;
//         }
//         else
//         {
//             cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << " state:offline" << endl;
//             conn->shutdown(); // close(fd)
//             // _loop->quit();
//         }
//     }

//     // 专门处理用户的读写事件
//     void onMessage(const TcpConnectionPtr &conn, // 连接
//                    Buffer *buffer,               // 缓冲区
//                    Timestamp time)               // 接收到数据的时间信息
//     {
//         string buf = buffer->retrieveAllAsString();
//         cout << "recv data:" << buf << " time:" << time.toFormattedString() << endl;
//         conn->send(buf);
//     }

//     TcpServer _server; // #1
//     EventLoop *_loop;  // #2 epoll
// };

// int main()
// {
//     EventLoop loop; // epoll
//     InetAddress addr("127.0.0.1", 6000);
//     ChatServer server(&loop, addr, "ChatServer");

//     server.start(); // listenfd epoll_ctl=>epoll
//     loop.loop();    // epoll_wait以阻塞方式等待新用户连接，已连接用户的读写事件等
// }


class EchoServer
{
public:
    EchoServer(EventLoop *loop,
            const InetAddress &addr, 
            const std::string &name)
        : server_(loop, addr, name)
        , loop_(loop)
    {
        // 注册回调函数
        server_.setConnectionCallback(
            std::bind(&EchoServer::onConnection, this, std::placeholders::_1)
        );

        server_.setMessageCallback(
            std::bind(&EchoServer::onMessage, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        );

        // 设置合适的loop线程数量 loopthread
        server_.setThreadNum(11);
    }
    void start()
    {
        server_.start();
    }
private:
    // 连接建立或者断开的回调
    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            std::cout<<"success"<<std::endl;
            //LOG_INFO("Connection UP : %s", conn->peerAddress().toIpPort().c_str());
        }
        else
        {
            //LOG_INFO("Connection DOWN : %s", conn->peerAddress().toIpPort().c_str());
            std::cout<<"error"<<std::endl;

        }
    }

    // 可读写事件回调
    void onMessage(const TcpConnectionPtr &conn,
                Buffer *buf,
                Timestamp time)
    {
        std::string msg = buf->retrieveAllAsString();
        conn->send(msg);
        conn->shutdown(); // 写端   EPOLLHUP =》 closeCallback_
    }

    EventLoop *loop_;
    TcpServer server_;
};

int main()
{
    EventLoop loop;
    InetAddress addr(6000);
    EchoServer server(&loop, addr, "EchoServer-01"); // Acceptor non-blocking listenfd  create bind 
    server.start(); // listen  loopthread  listenfd => acceptChannel => mainLoop =>
    loop.loop(); // 启动mainLoop的底层Poller

    return 0;
}

