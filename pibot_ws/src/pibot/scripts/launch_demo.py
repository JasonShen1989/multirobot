import subprocess
import rospy
import rosnode

class launch_demo:
    def __init__(self, cmd=None):
        self.cmd = cmd

    def launch(self):
        self.child = subprocess.Popen(self.cmd)
        return True

    def shutdown(self):
        self.child.terminate()
        self.child.wait()
        return True

if __name__ == "__main__":
    rospy.init_node('launch_demo',anonymous=True)

    launch_nav = launch_demo(["roslaunch", "pibot_simulator", "nav.launch"])

    launch_nav.launch()

    r = rospy.Rate(0.2)
    r.sleep()

    rospy.loginfo("switch map...")
    r = rospy.Rate(1)
    r.sleep()

    rosnode.kill_nodes(['map_server'])

    map_name = "/home/turtlebot/pibot/ros_ws/src/pibot_simulator/maps/blank_map_with_obstacle.yaml"

    map_node = subprocess.Popen(["rosrun", "map_server", "map_server", map_name, "__name:=map_server"])

    while not rospy.is_shutdown():
        r.sleep()