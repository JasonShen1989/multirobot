gnome-terminal --tab -e 'bash -c "roscore; exec bash"' \
--tab -e 'bash -c "sleep 3;roslaunch swarm_control swarm0.launch; exec bash"' \
--tab -e 'bash -c "sleep 3;roslaunch swarm_control swarm1.launch; exec bash"' \
--tab -e 'bash -c "sleep 3;roslaunch swarm_control swarm2.launch; exec bash"' \



