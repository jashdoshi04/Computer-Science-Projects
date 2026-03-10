#!/usr/bin/env python

import sys
import copy
import time
import rospy
from tqdm import tqdm
import numpy as np
from final_header import *
from final_func import *
from testcv import *


################ Pre-defined parameters and functions below (can change if needed) ################

# 20Hz
SPIN_RATE = 20  

# UR3 home location
home = [270*PI/180.0, -90*PI/180.0, 90*PI/180.0, -90*PI/180.0, -90*PI/180.0, 135*PI/180.0]  
home2 = [161*PI/180.0, -108*PI/180.0, 117*PI/180.0, -98*PI/180.0, -97*PI/180.0, 44*PI/180.0]
# UR3 current position, using home position for initialization
current_position = copy.deepcopy(home)  

thetas = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

digital_in_0 = 0
analog_in_0 = 0.0

suction_on = True
suction_off = False

current_io_0 = False
current_position_set = False


"""
Whenever ur3/gripper_input publishes info this callback function is called.
"""
def input_callback(msg):
    global digital_in_0
    digital_in_0 = msg.DIGIN
    digital_in_0 = digital_in_0 & 1 # Only look at least significant bit, meaning index 0


"""
Whenever ur3/position publishes info, this callback function is called.
"""
def position_callback(msg):
    global thetas
    global current_position
    global current_position_set

    thetas[0] = msg.position[0]
    thetas[1] = msg.position[1]
    thetas[2] = msg.position[2]
    thetas[3] = msg.position[3]
    thetas[4] = msg.position[4]
    thetas[5] = msg.position[5]

    current_position[0] = thetas[0]
    current_position[1] = thetas[1]
    current_position[2] = thetas[2]
    current_position[3] = thetas[3]
    current_position[4] = thetas[4]
    current_position[5] = thetas[5]

    current_position_set = True


"""
Function to control the suction cup on/off
"""
def gripper(pub_cmd, loop_rate, io_0):
    global SPIN_RATE
    global thetas
    global current_io_0
    global current_position

    error = 0
    spin_count = 0
    at_goal = 0

    current_io_0 = io_0

    driver_msg = command()
    driver_msg.destination = current_position
    driver_msg.v = 1.0
    driver_msg.a = 1.0
    driver_msg.io_0 = io_0
    pub_cmd.publish(driver_msg)

    while(at_goal == 0):

        if( abs(thetas[0]-driver_msg.destination[0]) < 0.0005 and \
            abs(thetas[1]-driver_msg.destination[1]) < 0.0005 and \
            abs(thetas[2]-driver_msg.destination[2]) < 0.0005 and \
            abs(thetas[3]-driver_msg.destination[3]) < 0.0005 and \
            abs(thetas[4]-driver_msg.destination[4]) < 0.0005 and \
            abs(thetas[5]-driver_msg.destination[5]) < 0.0005 ):

            rospy.loginfo("Goal is reached!")
            at_goal = 1

        loop_rate.sleep()

        if(spin_count >  SPIN_RATE*5):

            pub_cmd.publish(driver_msg)
            rospy.loginfo("Just published again driver_msg")
            spin_count = 0

        spin_count = spin_count + 1

    return error


"""
Move robot arm from one position to another
"""
def move_arm(pub_cmd, loop_rate, dest, vel, accel, move_type):
    global thetas
    global SPIN_RATE

    error = 0
    spin_count = 0
    at_goal = 0

    driver_msg = command()
    driver_msg.destination = dest
    driver_msg.v = vel
    driver_msg.a = accel
    driver_msg.io_0 = current_io_0
    driver_msg.move_type = move_type  # Move type (MoveJ or MoveL)
    pub_cmd.publish(driver_msg)

    loop_rate.sleep()

    while(at_goal == 0):

        if( abs(thetas[0]-driver_msg.destination[0]) < 0.0005 and \
            abs(thetas[1]-driver_msg.destination[1]) < 0.0005 and \
            abs(thetas[2]-driver_msg.destination[2]) < 0.0005 and \
            abs(thetas[3]-driver_msg.destination[3]) < 0.0005 and \
            abs(thetas[4]-driver_msg.destination[4]) < 0.0005 and \
            abs(thetas[5]-driver_msg.destination[5]) < 0.0005 ):

            at_goal = 1
            rospy.loginfo("Goal is reached!")

        loop_rate.sleep()

        if(spin_count >  SPIN_RATE*5):

            pub_cmd.publish(driver_msg)
            rospy.loginfo("Just published again driver_msg")
            spin_count = 0

        spin_count = spin_count + 1

    return error

################ Pre-defined parameters and functions above (can change if needed) ################

##========= TODO: Helper Functions =========##

def find_keypoints(image):
    """Gets keypoints from the given image

    Parameters
    ----------
    image : np.ndarray
        The given image (before or after preprocessing)

    Returns
    -------
    keypoints
        a list of keypoints detected in image coordinates
    """
    keypoints = []
    keypoints = get_lines(image)
    return keypoints

theta = 0.0
beta = 0.0
tx = 0.0
ty = 0.0

x1 = 356
y1 = 232 
x2 = 425
y2 = 216 
dist = ((x1-x2)**2 + (y1-y2)**2)**0.5
fx = dist/100  #mm
beta = fx  #mm
# print(beta)
theta = np.arccos(99.99/100)
tx = 250 - ((207-240)/beta) # 296
ty = 250 - ((459-320)/beta) # 54
# print(tx)
# print(ty)

# Function that converts image coord to world coord
# def IMG2W(col, row, image):
#     Or = 240
#     Oc = 320
#     Xc = (row - Or)/beta 
#     Yc = (col - Oc)/beta 
#     Xg = (Xc + tx)*np.cos(theta) - (Yc + ty)*np.sin(theta)
#     Yg = (Xc + tx)*np.sin(theta) + (Yc + ty)*np.cos(theta)

#     return Xg,Yg

def IMG2W(row, col, image):
    """Transform image coordinates to world coordinates

    Parameters
    ----------
    row : int
        Pixel row position
    col : int
        Pixel column position
    image : np.ndarray
        The given image (before or after preprocessing)

    Returns
    -------
    x : float
        x position in the world frame
    y : float
        y position in the world frame
    """
    x, y = 0.0, 0.0
    # print(image.shape)
    height, width, _ = image.shape
    print(height)
    print(width)
    startx = 58 + 30
    starty = 115
    endx =  270 
    endy = 397
    # print(row)
    # print(col)
    x = startx + ((row/height)*(endx - startx))
    y = starty + ((col/width)*(endy - starty))
    # print(row)
    # print(col)
    # print(height)
    # print(width)
    # print(x)
    # print(y)

    return x, y

import math
def is_pose_valid(pose):
    """Check if any element in the pose is NaN."""
    return not np.isnan(pose[0]) and not np.isnan(pose[1]) and not np.isnan(pose[2])

def draw_image(world_lines, pub_command, loop_rate, vel, accel):
    """Draw the image based on detected keypoints in world coordinates.

    Parameters
    ----------
    world_lines:
        a list of keypoints detected in world coordinates
    """
    # Start by moving to the home position
    move_arm(pub_command, loop_rate, home2, vel, accel, 'J')
    print(f"Moving arm to home position: {home2}")
    
    prev = (0, 0)  # Previous point (for lifting the pen when necessary)
    processed_lines = set()  #
    for i, line in enumerate(world_lines):
        if i < 30:
            continue
        start_point = line[0]  # (x1, y1)
        end_point = line[1]    # (x2, y2)
        line_tuple = tuple(sorted([start_point, end_point]))
    
        # Skip the line if it has already been processed (in either direction)
        if line_tuple in processed_lines:
            print(f"Skipping already processed line {i}: {start_point} -> {end_point}")
            continue
        processed_lines.add(line_tuple)

        print(f"Processing line {i}: {start_point} -> {end_point}")
        distance = math.sqrt((start_point[0] - prev[0])**2 + (start_point[1] - prev[1])**2)

        # If the start of the current line is not the same as the previous point, lift the pen
        if distance > 1 and i != 0:
            # Lift the pen and move to the start of the next line at a height of 40 (z = 40)
            lift_pose = lab_invk(xWgrip=start_point[1]+30, yWgrip=start_point[0]+70-40, zWgrip=40, yaw_WgripDegree=0)
            
            # Check if the lift pose is valid (no NaN values)
            if not is_pose_valid(lift_pose):
                print(f"Invalid pose detected: {lift_pose}. Skipping move.")
                continue
            print(f"Lifting pen and moving to: {lift_pose}")
            move_arm(pub_command, loop_rate, lift_pose, vel, accel, 'L')

            # Move down to start drawing (z = 21)
            draw_pose = lab_invk(xWgrip=start_point[1]+0, yWgrip=start_point[0]+70-40, zWgrip=21, yaw_WgripDegree=0)
            
            # Check if the draw pose is valid (no NaN values)
            if not is_pose_valid(draw_pose):
                print(f"Invalid pose detected: {draw_pose}. Skipping move.")
                continue
            print(f"Lowering pen to: {draw_pose}")
            move_arm(pub_command, loop_rate, draw_pose, vel, accel, 'L')
        
        # Draw the first part of the line without lifting the pen
        draw_pose = lab_invk(xWgrip=start_point[1]+0, yWgrip=start_point[0]+70-40, zWgrip=21, yaw_WgripDegree=0)
        
        # Check if the draw pose is valid (no NaN values)
        if not is_pose_valid(draw_pose):
            print(f"Invalid pose detected: {draw_pose}. Skipping move.")
            continue
        print(f"Drawing from: {start_point} -> {draw_pose}")
        move_arm(pub_command, loop_rate, draw_pose, vel, accel, 'L')
        
        # Move to the second part of the line
        draw_pose = lab_invk(xWgrip=end_point[1]+0, yWgrip=end_point[0]+70-40, zWgrip=21, yaw_WgripDegree=0)
        
        # Check if the draw pose is valid (no NaN values)
        if not is_pose_valid(draw_pose):
            print(f"Invalid pose detected: {draw_pose}. Skipping move.")
            continue
        print(f"Drawing to: {end_point} -> {draw_pose}")
        move_arm(pub_command, loop_rate, draw_pose, vel, accel, 'L')

        # Update prev to the end point of the current line
        prev = end_point

    print("Finished drawing.")
    return

"""
Program run from here
"""
def main():
    global home
    # global variable1
    # global variable2

    # Initialize ROS node
    rospy.init_node('lab5node')

    # Initialize publisher for ur3/command with buffer size of 10
    pub_command = rospy.Publisher('ur3/command', command, queue_size=10)

    # Initialize subscriber to ur3/position & ur3/gripper_input and callback fuction
    # each time data is published
    sub_position = rospy.Subscriber('ur3/position', position, position_callback)
    sub_input = rospy.Subscriber('ur3/gripper_input', gripper_input, input_callback)

    # Check if ROS is ready for operation
    while(rospy.is_shutdown()):
        print("ROS is shutdown!")

    # Initialize the rate to publish to ur3/command
    loop_rate = rospy.Rate(SPIN_RATE)

    # Velocity and acceleration of the UR3 arm
    vel = 4.0
    accel = 4.0
    print("here")
    move_arm(pub_command, loop_rate, home, vel, accel, 'J')  # Move to the home position

    ##========= TODO: Read and draw a given image =========##
    image = cv2.imread('images/eagle.png')
    image = cv2.resize(image, (1024, 1024))
    keypoints = find_keypoints(image)
    # print(keypoints[1])
    # print(keypoints[0])
    world_points = []
    for i, p in enumerate(keypoints):
        # print(type(p))
        print((p))
        temp = []
        temp.append(IMG2W(p[0][0], p[0][1], image))
        temp.append(IMG2W(p[1][0], p[1][1], image))
        # print(type(temp))
        # print(temp[0])
        world_points.append(temp)
    print(world_points[0])
    print(world_points[1])
    draw_image(world_points, pub_command, loop_rate, vel, accel)
    move_arm(pub_command, loop_rate, home, vel, accel, 'J')  # Return to the home position
    rospy.loginfo("Task Completed!")
    print("Use Ctrl+C to exit program")
    rospy.spin()

if __name__ == '__main__':

    try:
        main()
    # When Ctrl+C is executed, it catches the exception
    except rospy.ROSInterruptException:
        pass
