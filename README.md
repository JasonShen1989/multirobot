# multirobot

#### 介绍
多机器人项目空地协同编队ros代码。2020.11.26

#### 项目说明
一个飞机、两个车、一个地面站、一个路由器、一套uwb

地面站（我的笔记本）:
1、运行roscore，作为rosmaster。
2、usb连接console，运行roslaunch nlink……，发布所有标签位置的topic
3、运行每个车的速度控制节点（其实也可以放在车上运行）。订阅所有标签位置并发送速度指令的node，这个node还会订阅车的磁罗盘数据并纠正车头方向。


飞机：
1、pixhawk或者tx2给tag标签供电。
2、遥控器自稳或者定高模式，飞手遥控。后来改成uwb连接pixhawk(apm)飞控实现室内航迹飞行，但是没有用机载计算机

车0：（ros_master改成地面站）
1、NUC工控机或者充电宝给tag标签供电。
2、工控机运行一个rosnode（bringup_with_imu):订阅速度指令控制小车前进的node，读取stm32的磁罗盘数据并发布。

车1：
同车0

车2：
同车0

路由器：
地面站、车等需要ros的电脑都要连接同一个wifi

nlink_parser 的ros包编译前要先安装nlink的serial包!还要安装ros-melodic-serial*
运行roslaunch nlink_parser linktrack.launch 
报错/home/shupeixuan/linktrack_ws/devel/lib/nlink_parser/linktrack: error while loading shared libraries: libserial.so: cannot open shared object file: No such file or directory
还要安装ros-melodic-serial*


IP:
GroundStation: 192.168.10.150
CAR0: 192.168.10.140
CAR1: 192.168.10.141
CAR2: 192.168.10.142
这些IP都可以自己在对应电脑wifi设置上自己设置
他们都连接 WAVLINK-AC 都是分别手动设置wifi setting改manual IP
netmask子网掩码填255.255.255.0 Gateway网关填192.168.10.1 同一网段的最小数字1

roslaunch swarm_control swarm0.launch 启动了nlink_parser标签位置发布节点，也启动了car0的控制节点
swarm1.launch和swarm2.launch启动car1、car2的控制节点
rostopic pub /command std_msgs/Int32 "data: 0" 可以发布控制指令。0（车全停并设置期望车头方向为此时方向）、1（车运动至第一个队形）、2（车运动至第二个队形）、5（车全停但不改变车头期望方向）可选




