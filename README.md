# Robotics_Project2
This is a demonstration of a SLAM approach by using the rtabmap ROS package

To execute the Project:

1. Clone the Repository "Robotics_Project2" to your /home/<user> directory
	
        cd ~
        git clone https://github.com/andreslc47/Robotics_Project2.git
        cd ~/Robotics_Project2/
        
	
2. Compile the Workspace by using the script provided:
	
        ./compile_all
 
	
3. Open 2 terminals.

    3.1. First Terminal: 

        cd ~/Robotics_Project2/
        source devel/setup.bash
        roslaunch my_robot world.launch

    3.2. Second Terminal:

        cd ~/Robotics_Project2/
        source devel/setup.bash
        roslaunch ball_chaser ball_chaser.launch


4. In Gazebo, move the white ball in front of the robot so that it can follow it.

	
5. To close everything:

        cd ~/Robotics_Project2/
        ./pkill_all
