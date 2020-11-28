#include "ros/ros.h"
#include "std_msgs/String.h"
#include "px4_control_cfg.h"
#include "geometry_msgs/Twist.h" //??
#include "nlink_parser/LinktrackAnchorframe0.h"
#include "std_msgs/Int32.h"
#include "nlink_parser/LinktrackTagframe0.h"
#include <tf/transform_datatypes.h>
#include <sensor_msgs/Imu.h>

using namespace std;
//using namespace Eigen;

float expectPos_carx_v1;
float expectPos_cary_v1;
float expectPos_carx_v2;
float expectPos_cary_v2;
float vel_P;
float vel_D;
float vel_I;
float w_P;

float expectPos_car[3];
float uav_pos[3] = {0,0,0};
float car0_pos[3] = {0,0,0};
float car1_pos[3] = {0,0,0};
geometry_msgs::Twist cmd_vel;

S_PID_ITEM PidItemX;
S_PID_ITEM PidItemY;

std_msgs::Int32 command;
float yaw_angle;
float expectYawAngle;


geometry_msgs::Twist VelPIDController(float car_pos[3],float uavPos[3],float expectPos[3]);
void controlCallback(const nlink_parser::LinktrackAnchorframe0::ConstPtr& msg);


geometry_msgs::Twist VelPIDController(float car_pos[3],float uavPos[3],float expectPos[3])
{
	geometry_msgs::Twist cmd_vel;

	// vx in body frame  body forward,UWB coordinate frame backward
	PidItemX.difference = uavPos[0]+expectPos[0]-car_pos[0];
	PidItemX.differential = PidItemX.difference - PidItemX.tempDiffer;
	PidItemX.integral += PidItemX.difference;
	PidItemX.tempDiffer = PidItemX.difference;
	cmd_vel.linear.x = vel_P*PidItemX.difference + vel_D*PidItemX.differential + vel_I*PidItemX.integral;
	cmd_vel.linear.x = -cmd_vel.linear.x;	

	// vy in body frame  body left ,UWB coordinate frame right
	PidItemY.difference = uavPos[1]+expectPos[1]-car_pos[1];
	PidItemY.differential = PidItemY.difference - PidItemY.tempDiffer;
	PidItemY.integral += PidItemY.difference;
	PidItemY.tempDiffer = PidItemY.difference;
	cmd_vel.linear.y = vel_P*PidItemY.difference + vel_D*PidItemY.differential + vel_I*PidItemY.integral;
	cmd_vel.linear.y = -cmd_vel.linear.y;

	cmd_vel.linear.z = 0;
	cmd_vel.angular.x = 0;
	cmd_vel.angular.y = 0;
	cmd_vel.angular.z = w_P*(expectYawAngle-yaw_angle);

	return cmd_vel;
}


void controlCallback(const nlink_parser::LinktrackAnchorframe0::ConstPtr& msg)
{
	for(auto &item : msg->nodes)
		{
			if(item.id == 5)
				{
				uav_pos[0] =  item.pos_3d[0];
				uav_pos[1] =  item.pos_3d[1];
				uav_pos[2] =  item.pos_3d[2];
				}
			if(item.id == 6)
				{
				car0_pos[0] = item.pos_3d[0];
				car0_pos[1] = item.pos_3d[1];
				car0_pos[2] = item.pos_3d[2];
				}
			if(item.id == 7)
				{
				car1_pos[0] = item.pos_3d[0];
				car1_pos[1] = item.pos_3d[1];
				car1_pos[2] = item.pos_3d[2];
				}
		}

}


void cmdCallback(const std_msgs::Int32::ConstPtr& msg)
{
	command = *msg;
	cout << "receive command: " << command.data << endl;
}

void angleCallback(const sensor_msgs::Imu::ConstPtr& msg)
{
	double car_roll,car_pitch,car_yaw;
	sensor_msgs::Imu IMUdata;
	IMUdata = *msg;
	tf::Quaternion quat;
      	tf::quaternionMsgToTF(IMUdata.orientation,quat);
      	tf::Matrix3x3(quat).getRPY(car_roll,car_pitch,car_yaw);

	car_roll = car_roll*180/3.14159;
	car_pitch = car_pitch*180/3.14159;
	car_yaw = car_yaw*180/3.14159;

	cout << "IMU angle: "<< car_roll << " " << car_pitch <<" " << car_yaw << endl;

	yaw_angle = car_yaw;
}



int main(int argc,char **argv)
{
	ros::init(argc,argv,"car1");
	ros::NodeHandle nh("~");
	ros::Subscriber pos_sub = nh.subscribe("/nlink_linktrack_anchorframe0",10,controlCallback); //??
	ros::Subscriber cmd_sub = nh.subscribe("/command",10,cmdCallback); 
	ros::Subscriber angle_sub = nh.subscribe("/car1/imu/data",10,angleCallback); 
	ros::Publisher vel_pub = nh.advertise<geometry_msgs::Twist>("/car1/cmd_vel",10); 

	nh.param<float>("expectPos_carx_v1",expectPos_carx_v1,0); //必须用nh("~")才能传入参数
	nh.param<float>("expectPos_cary_v1",expectPos_cary_v1,0);
	nh.param<float>("expectPos_carx_v2",expectPos_carx_v2,0); //必须用nh("~")才能传入参数
	nh.param<float>("expectPos_cary_v2",expectPos_cary_v2,0);
	nh.param<float>("vel_P",vel_P,1.0);
	nh.param<float>("vel_D",vel_D,0);
	nh.param<float>("vel_I",vel_I,0);
	nh.param<float>("w_P",w_P,1.0);

	cout << "expectPos_carx_v1: "<< expectPos_carx_v1 << endl;
	cout << "expectPos_cary_v1: "<< expectPos_cary_v1 << endl;
	cout << "expectPos_carx_v2: "<< expectPos_carx_v2 << endl;
	cout << "expectPos_cary_v2: "<< expectPos_cary_v2 << endl;
	cout << "vel_P: "<< vel_P << endl;
	cout << "vel_D: "<< vel_D << endl;
	cout << "vel_I: "<< vel_I << endl;
	cout << "w_P: "<< w_P << endl;
	cout << "Waiting for IMU self calibration... "<< w_P << endl;

	expectPos_car[0] = expectPos_carx_v1;
	expectPos_car[1] = expectPos_cary_v1;
	expectPos_car[2] = 0;
	cmd_vel.linear.x = 0;
	cmd_vel.linear.y = 0;
	cmd_vel.linear.z = 0;
	cmd_vel.angular.x = 0;
	cmd_vel.angular.y = 0;
	cmd_vel.angular.z = 0;
	PidItemX.tempDiffer = 0;
	PidItemX.integral = 0;
	PidItemY.tempDiffer = 0;
	PidItemY.integral = 0;
	command.data = 0;
	yaw_angle = 0;
	expectYawAngle = 0;

	ros::Rate loop_rate(30);

	while(ros::ok())
	{
		if(command.data == 0) // 停车并重标期望偏航角
			{
				expectYawAngle = yaw_angle;
				//cout << "expectYawAngle: " << expectYawAngle << endl;

				cmd_vel.linear.x = 0;
				cmd_vel.linear.y = 0;
				cmd_vel.linear.z = 0;
				cmd_vel.angular.x = 0;
				cmd_vel.angular.y = 0;
				cmd_vel.angular.z = 0;
				vel_pub.publish(cmd_vel);

				PidItemX.tempDiffer = 0;
				PidItemX.integral = 0;
				PidItemY.tempDiffer = 0;
				PidItemY.integral = 0;	
			}			

		if(command.data == 1)
			{	
				expectPos_car[0] = expectPos_carx_v1;
				expectPos_car[1] = expectPos_cary_v1;
				expectPos_car[2] = 0;	
				cmd_vel = VelPIDController(car1_pos,uav_pos,expectPos_car); 
				vel_pub.publish(cmd_vel);	
			}	
		if(command.data == 2)
			{		
				expectPos_car[0] = expectPos_carx_v2;
				expectPos_car[1] = expectPos_cary_v2;
				expectPos_car[2] = 0;	
				cmd_vel = VelPIDController(car1_pos,uav_pos,expectPos_car); 
				vel_pub.publish(cmd_vel);	
			}	
		if (command.data == 5)  // 只停车，不重标期望偏航角
			{
				cmd_vel.linear.x = 0;
				cmd_vel.linear.y = 0;
				cmd_vel.linear.z = 0;
				cmd_vel.angular.x = 0;
				cmd_vel.angular.y = 0;
				cmd_vel.angular.z = 0;
				vel_pub.publish(cmd_vel);

				PidItemX.tempDiffer = 0;
				PidItemX.integral = 0;
				PidItemY.tempDiffer = 0;
				PidItemY.integral = 0;	
			}
		ros::spinOnce();
		loop_rate.sleep();
	}  //无法在回调函数里发布话题，报错函数里没有定义vel_pub!只能在main里面发布了

	return 0;
}
