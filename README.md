# multirobot

多机器人项目空地协同编队ros代码。2020.11.26

#### 项目概述
一个四旋翼飞机、三个麦克纳姆轮无人车形成空地协同编队。四旋翼为领导者，三个车为跟随者。
一个地面站、一个路由器、一套nooploop空循环uwb

地面站（我的笔记本）:
1、运行roscore，作为rosmaster。
2、usb口连接console，运行roslaunch nlink……，读取uwb的console数据发布所有标签位置的topic
3、运行每个车的速度控制节点（其实也可以放在车上运行）。订阅所有标签位置并发送速度指令的node，这个node还会订阅车的磁罗盘数据并纠正车头方向。

飞机：
1、pixhawk或者tx2给uwb的tag标签供电即可。
2、遥控器自稳或者定高模式，飞手遥控。
注：后来改成uwb的tag标签连接pixhawk(apm)飞控gps串口实现室内航迹飞行，但是也没有用机载计算机

车0：（ros_master改成地面站）
1、NUC工控机或者充电宝给tag标签供电。
2、工控机运行（roslaunch pibot_bringup bringup_with_imu.launch):订阅速度指令控制小车前进的node，读取stm32的磁罗盘数据并发布。

车1：
同车0，但命名空间是car1

车2：
同车0，但命名空间是car2

路由器：
地面站、车等需要ros的电脑都要连接同一个wifi


#### 配置说明：

##### IP设置
地面站和三个车都要连接同一个wifi，并固定所连wifi的IP：
如IP:
GroundStation: 192.168.10.150
CAR0: 192.168.10.140
CAR1: 192.168.10.141
CAR2: 192.168.10.142
这些IP都可以自己在对应电脑wifi设置上自己设置
如此时他们都连接wifi：WAVLINK-AC， 都是分别手动设置wifi setting改IPv4为manual IP
netmask子网掩码填255.255.255.0 
Gateway网关填192.168.10.1 同一网段的最小数字1

##### ROS master设置
在地面站和三个车的电脑上都要修改/etc/hostname，修改/etc/hosts的127.0.1.1为hostname里本机的名称，并添加其他车和地面站的IP、名称。
最后在~/.bashrc中添加ROS_IP,ROS_HOSTNAME,ROS_MASTER_URI等。如CAR0添加：
export ROS_IP=CAR0
export ROS_HOSTNAME=CAR0
export ROS_MASTER_URI=http://GroundStation:11311
其中CAR0、GroundStation等同于car0和地面站的IP，因为在/etc/hosts文件里已经设置了IP对应的名称，所以这里可以直接用名称代替IP
可以通过ping GroundStation的方式检查是否和地面站wifi连接正常
也可以地面站和car上分别运行turtlesim的小海龟节点和键盘控制节点，看看能不能控制好

##### 编译
地面站：
1、在nooploop官网下载空循环uwb的ros包nlink_parser并单独编译。
nlink_parser 的ros包编译前要先安装nlink的serial包!还要安装ros-melodic-serial*
否则运行roslaunch nlink_parser linktrack.launch 会
报错/home/shupeixuan/linktrack_ws/devel/lib/nlink_parser/linktrack: error while loading shared libraries: libserial.so: cannot open shared object file: No such file or directory
2、下载本项目的multirobot_ws文件夹，并在该文件夹下编译catkin_make即可，里面包含三个小车的控制节点
3、使用时运行以下sh文件即可启动nlink_parser标签读取和三个小车的控制节点：
cd ${your multirobot_ws path}/multirobot_ws/src/swarm_control/readme
./command.sh
4、地面站还需要单独开一个终端，做为指令发布的窗口。输入：
rostopic pub /command std_msgs/Int32 "data: 0" 可以发布控制指令。
  0（车全停并设置期望车头方向为此时方向）
  1（车运动至第一个队形）
  2（车运动至第二个队形）
  5（车全停但不改变车头期望方向）可选

CAR0：
1、下载本项目的pibot_ws文件夹，并在该文件夹下编译catkin_make即可，里面包含小车的驱动以及读取stm32磁罗盘的节点等等
2、修改pibot_ws/pibot_bringup/launch/bringup_with_imu.launch，在<launch>下一行加上<group ns="car0">。并在</launch>上一行加上</group>,这样命名空间就在car0下了，所有话题前都会多一个car0/
3、使用时运行roslaunch pibot_bringup bringup_with_imu.launch即可

CAR1：
同CAR0，改<group ns="car1">

CAR2同理

Shu Peixuan
2020.11.29
shupeixuan@qq.com
