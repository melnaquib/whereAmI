#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    
    client.call(srv);    
}

void drive_to(int width, int col) {
	
	const float VEL_LIN_X_FACTOR = 2.0, VEL_ANG_Z_FACTOR = 1.0;
	
	if(col < width / 3.0)
			drive_robot(0, VEL_ANG_Z_FACTOR);
	else if(col < width * 2.0 / 3.0)
		drive_robot(VEL_LIN_X_FACTOR, 0);
	else
		drive_robot(0, -1.0 * VEL_ANG_Z_FACTOR);
}

void drive_stop() {
	drive_robot(0, 0);
}
		
// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
        
    int white_col_min = img.width, white_col_max = 0;
    const int PIXEL_DEPTH = 3;
    for (int i = 0; i < img.height * img.step ; i += PIXEL_DEPTH) {
			if (white_pixel == img.data[i] && white_pixel == img.data[i + 1] && white_pixel == img.data[i + 2]) {
				
				int col = (i / PIXEL_DEPTH) % img.width;
				
				if(white_col_min > col)
					white_col_min = col;
				if(white_col_max < col)
					white_col_max = col;
			}
		}
		
		if( white_col_max > white_col_min) // white points found
			drive_to(img.width, (white_col_max + white_col_min) / 2);
		else
			drive_stop();
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
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
