#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "ball_chaser/DriveToTarget.h"

ros::Publisher motor_command_publisher;

bool handle_drive_request(ball_chaser::DriveToTarget::Request& req, ball_chaser::DriveToTarget::Response& res)
{
	ROS_INFO("DriveToTargetRequest received - linear.x:%1.2f, angular.z:%1.2f", (float)req.linear_x, (float)req.angular_z);
	geometry_msgs::Twist my_message;
	my_message.linear.x = req.linear_x;
	my_message.angular.z = req.angular_z;
	
	motor_command_publisher.publish(my_message);
		
	ros::Duration(0.01).sleep();

	res.msg_feedback = "Speed set - linear.x: " + std::to_string(my_message.linear.x) + " , angular.z: " + std::to_string(my_message.angular.z);
	ROS_INFO_STREAM(res.msg_feedback);

	return true;
}



int main(int argc, char** argv)
{
	// Initialize a ROS node
	ros::init(argc, argv, "drive_bot");
	
	// Create a ROS NodeHandle object
	ros::NodeHandle n;

	// Inform ROS master that we will be publishing a message of type geometry_msgs::Twist on the robot actuation topic with a publishing queue size of 10
	motor_command_publisher = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
	
	// Define a drive /ball_chaser/command_robot service with a handle_drive_request callback function
	ros::ServiceServer service = n.advertiseService("/ball_chaser/command_robot", handle_drive_request);
	ROS_INFO("Ready to send drive commands");

	// Handle ROS communication events
	ros::spin();
	
	return 0;
}







