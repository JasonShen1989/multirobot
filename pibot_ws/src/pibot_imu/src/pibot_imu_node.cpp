#include "pibot_imu/pibot_imu.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "pibot_imu");
  ros::NodeHandle nh, pnh("~");
  PibotIMU pibot_imu(nh, pnh);

  ros::spin();

  return 0;
}
