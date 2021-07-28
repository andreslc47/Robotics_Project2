#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include <math.h>

ros::ServiceClient client;
float forward_factor = 10.0;
float middle_factor = 5.0;
float turning_factor = 0.5;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
	// Request a service and pass the velocities to it to drive the robot
	ROS_INFO_STREAM("Moving the robot to the white ball");
	
	ball_chaser::DriveToTarget srv;
	srv.request.linear_x = lin_x;
	srv.request.angular_z = ang_z;

	// Call the safe_move service and pass the requested joint angles
	if (!client.call(srv))
		ROS_ERROR("Failed to call service command_robot");
	
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
	int white_pixel = 255;
	bool white_ball_seen = false;
	int absolute_white_position = 0;
	int horizontal_white_position = 0;
	int left_side = round(img.width*3/10);
	int forward_side = round(img.width*2/5);
	int counters[img.height] = {0}; // amount of white pixels in every row
	int r = 0; // index of every row.
	int counter_x = 0;
	int counter_y = 0;
	int counter_z = 0;
	bool first_white_pixel = false;

	for (int i = 0; i < img.height * img.step; i+=3){
		if (img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel) {
			white_ball_seen = true;
			r = 0;
			for (int row = 0; row < img.height * img.step; row+=img.step){
				
				for (int column = 0; column < 3*img.width; column+=3){
					if (img.data[row + column] == white_pixel && img.data[row + column + 1] == white_pixel && img.data[row + column + 2] == white_pixel){
						counters[r]++;				
					}
				}
				if (r != 0 && counters[r] < counters[r-1]){
					counter_x = 0;
					for (int x = 0; x < 3*left_side; x+=3){
						if (img.data[x + row - img.step] == white_pixel && img.data[x + row - img.step + 1] == white_pixel && img.data[x + row - img.step + 2] == white_pixel){
							counter_x++;
							if (first_white_pixel == false){
								first_white_pixel = true;						
							}
						}else{
							if (first_white_pixel == true){
								goto end_loop;					
							}
						}
							
					}	
					counter_y = 0;
					for (int y = 3*left_side; y < 3*(left_side + forward_side); y+=3){
						if (img.data[y + row - img.step] == white_pixel && img.data[y + row - img.step + 1] == white_pixel && img.data[y + row - img.step + 2] == white_pixel){
							counter_y++;
						if (first_white_pixel == false){
								first_white_pixel = true;						
							}
						}else{
							if (first_white_pixel == true){
								goto end_loop;					
							}
						}
					}	
					counter_z = 0;				
					for (int z = 3*(left_side + forward_side); z < 3*img.width; z+=3){
						if (img.data[z + row - img.step] == white_pixel && img.data[z + row - img.step + 1] == white_pixel && img.data[z + row - img.step + 2] == white_pixel){
							counter_z++;
						if (first_white_pixel == false){
								first_white_pixel = true;						
							}
						}else{
							if (first_white_pixel == true){
								goto end_loop;					
							}
						}
					}
					end_loop:
					break;				
				}
				r++;					
			}
			break;		
		}
	}



	if (white_ball_seen == true){

		if (counter_x != 0){
			if (counter_y != 0){
				if (counter_z != 0){
					if (counter_x + counter_y + counter_z == img.step){
						drive_robot(0.0,0.0);	// stop
					}else{	
						drive_robot(forward_factor,0.0);	// go forward				
					}
				}else{
					if (counter_y >= 2*counter_x){
						drive_robot(middle_factor,turning_factor);	// go forward with a slight left turn 				
					}else{
						drive_robot(0.0,turning_factor);	// turn left					
					}
				}						
			}else{
				if (counter_z == 0){
					drive_robot(0.0,turning_factor);	// turn left			
				}			
			}
		}else{
			if (counter_y != 0){
				if (counter_z != 0){
					if (counter_y >= 2*counter_z){
						drive_robot(middle_factor,-turning_factor);	// go forward with a slight right turn					
					}else{
						drive_robot(0.0,-turning_factor);	// turn right					
					}
				}else{
					drive_robot(forward_factor,0.0);	// go forward	
				}
			}else{
				if (counter_z != 0){
					drive_robot(0.0,-turning_factor);	// turn right			
				}else{
					drive_robot(0.0,0.0);
				}
			}
		}



	}else{
		drive_robot(0.0,0.0);	// stop
	}
}



int main(int argc, char** argv)
{
	// Initialize the process_image node and create a handle to it.
	ros::init(argc, argv, "process_image");
	ros::NodeHandle n;

	// Define a client service capable of requesting services from command_robot
	client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");
	
	// Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
	ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);
	
	// Handle ROS communication events
	ros::spin();

	return 0;
}



