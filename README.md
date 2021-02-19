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
##### offlinmessage表
![3](https://github.com/donghe0313/chatserver/blob/main/pic/2.png)
#### 服务器集群
![4](https://github.com/donghe0313/chatserver/blob/main/pic/3.png)
#### 跨服务器通信
![5](https://github.com/donghe0313/chatserver/blob/main/pic/4.png)


