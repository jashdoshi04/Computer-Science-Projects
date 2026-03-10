# version 0.0
# Jose Cuaran


import math
import numpy as np
import rclpy
from rclpy.node import Node
#from rclpy.clock import Clock

from geometry_msgs.msg import TransformStamped
from tf2_ros import TransformBroadcaster

from std_msgs.msg import String, Float32
from nav_msgs.msg import Odometry
from mobile_robotics.utils import quaternion_from_euler, lonlat2xyz #edit according to your package's name


class OdometryNode(Node):
    # Initialize some variables
    
    gyro_yaw = 0.0 #gyro yaw measurement
    blspeed = 0.0 #back left wheel speed
    flspeed = 0.0 #front left wheel speed
    brspeed = 0.0 #back right wheel speed
    frspeed = 0.0 #front right wheel speed



    x = 0.0 # x robot's position
    y = 0.0 # y robot's position
    last_x = 0.0
    last_y = 0.0
    last_theta = 0.0
    theta = 0.0 # heading angle
    l_wheels = 0.3 # Distance between right and left wheels

    last_time = 0.0
    current_time = 0.0

    def __init__(self):
        super().__init__('minimal_subscriber')
        self.lat = None
        self.lon = None
        self.lat0 = None
        self.lon0 = None
        self.have_origin = False
        
        # Declare subscribers to all the topics in the rosbag file, like in the example below. Add the corresponding callback functions.
        self.subscription_Gyro_yaw = self.create_subscription(Float32, 'Gyro_yaw', self.callback_Gy, 10)
        # your code here
        self.subscription_bl_speed = self.create_subscription(Float32, '/Blspeed', self.callback_bl_speed, 10)
        self.subscription_fl_speed = self.create_subscription(Float32, '/Flspeed', self.callback_fl_speed, 10)
        self.subscription_br_speed = self.create_subscription(Float32, '/Brspeed', self.callback_br_speed, 10)
        self.subscription_fr_speed = self.create_subscription(Float32, '/Frspeed', self.callback_fr_speed, 10)
        self.sub_lat = self.create_subscription(Float32, '/latitude', self.callback_lat, 10)
        self.sub_lon = self.create_subscription(Float32, '/longitude', self.callback_lon, 10)
        
        self.last_time = self.get_clock().now().nanoseconds/1e9
        
        self.odom_pub = self.create_publisher(Odometry, 'odom', 10) #keep in mind how to declare publishers for next assignments
        self.timer = self.create_timer(0.1, self.timer_callback_odom) #It creates a timer to periodically publish the odometry.
        
        self.tf_broadcaster = TransformBroadcaster(self) # To broadcast the transformation between coordinate frames.


        self.file_object_results  = open("results_part1.txt", "w+")
        self.timer2 = self.create_timer(0.1, self.callback_write_txt_file) #Another timer to record some results in a .txt file
        


    def callback_Gy(self, msg):
        self.gyro_yaw = msg.data

    #i think we need this as done for gyro to get wheel speeds at a contiuous rate   
    def callback_bl_speed(self, msg):
        self.blspeed = msg.data
    def callback_fl_speed(self, msg):
        self.flspeed = msg.data
    def callback_br_speed(self, msg):
        self.brspeed = msg.data
    def callback_fr_speed(self, msg):
        self.frspeed = msg.data
    def callback_lat(self, msg):
        self.lat = msg.data
    def callback_lon(self, msg):   
        self.lon = msg.data

    def timer_callback_odom(self):
        '''
        Compute the linear and angular velocity of the robot using the differential-drive robot kinematics
        Perform Euler integration to find the position x and y of the robot
        '''

        self.current_time = self.get_clock().now().nanoseconds/1e9
        dt = self.current_time - self.last_time # DeltaT
        dt = 0.1
         # ===== Convert current GPS to local x, y =====
        self.x, self.y = lonlat2xyz(self.lat, self.lon, self.lat0, self.lon0)

        # ===== Get current heading from gyro =====
        
        # ===== Compute linear and angular velocities =====
        dx = self.x - self.last_x
        dy = self.y - self.last_y
        dtheta = self.theta - self.last_theta

        v = math.sqrt(dx**2 + dy**2) / dt
        w = dtheta / dt
        self.theta = self.gyro_yaw*dt + self.last_theta  + w*dt #Heading angle  is the initial angle plus the omega*time elapsed i think

        # Update state
        self.last_x = self.x
        self.last_y = self.y
        self.last_theta = self.theta
         
        # vl = (self.blspeed + self.flspeed)/2.0  #Average Left-wheels speed
        # vr = (self.frspeed + self.brspeed)/2.0  # ... Your code here. Average right-wheels speed
        
        # v =  (vl + vr)/2 #Linear velocity of the robot should be the average of left and right wheels
        # w = (vl-vr)/(2*self.l_wheels) # Angular velocity of the robot , we use kinematics omega = v/r
        #self.x = self.x + v*math.cos(self.theta)*dt #.Position s = u + v*t (initial pos + vel*time elapsed)
        #self.y = self.y + v*math.sin(self.theta)*dt #Position same logic 
        #self.theta = self.theta + w*dt + self.gyro_yaw*dt  #Heading angle  is the initial angle plus the omega*time elapsed i think

        position = [self.x, self.y, 0.0]
        quater = quaternion_from_euler(0.0, 0.0, self.theta)
        print("position: ", position)
        print("orientation: ", quater)


        # We need to set an odometry message and publish the transformation between two coordinate frames
        # Further info about odometry message: https://docs.ros2.org/foxy/api/nav_msgs/msg/Odometry.html
        # Further info about tf2: https://docs.ros.org/en/humble/Tutorials/Intermediate/Tf2/Introduction-To-Tf2.html
        # Further info about coordinate frames in ROS: https://www.ros.org/reps/rep-0105.html

        frame_id = 'odom'
        child_frame_id = 'base_link'
        
        
        self.broadcast_tf(position, quater, frame_id, child_frame_id)  # Before creating the odometry message, go to the broadcast_tf function and complete it.
        
        odom = Odometry()
        odom.header.frame_id = frame_id
        odom.header.stamp = self.get_clock().now().to_msg()

        # set the pose. Uncomment next lines

        odom.pose.pose.position.x = position[0] 
        odom.pose.pose.position.y = position[1]
        odom.pose.pose.position.z = position[2] 
        odom.pose.pose.orientation.x = quater[0]
        odom.pose.pose.orientation.y = quater[1]
        odom.pose.pose.orientation.z = quater[2]
        odom.pose.pose.orientation.w = quater[3]

        # set the velocities. Uncomment next lines
        odom.child_frame_id = child_frame_id
        odom.twist.twist.linear.x = v*np.cos(self.theta) #linear velocity
        odom.twist.twist.linear.y = v*np.sin(self.theta)
        odom.twist.twist.angular.x = 0
        odom.twist.twist.angular.y = 0
        odom.twist.twist.angular.z = w #angular velocity

        self.odom_pub.publish(odom)

        self.last_time = self.current_time
        
    def broadcast_tf(self, pos, quater, frame_id, child_frame_id):
        '''
        It continuously publishes the transformation between two reference frames.
        Complete the translation and the rotation of this transformation
        '''
        t = TransformStamped()

        t.header.stamp = self.get_clock().now().to_msg()
        t.header.frame_id = frame_id
        t.child_frame_id = child_frame_id

        # Uncomment next lines and complete the code
        t.transform.translation.x = pos[0] 
        t.transform.translation.y = pos[1]
        t.transform.translation.z = pos[2]

        t.transform.rotation.x = quater[0]
        t.transform.rotation.y = quater[1]
        t.transform.rotation.z = quater[2]
        t.transform.rotation.w = quater[3]

        # Send the transformation
        self.tf_broadcaster.sendTransform(t)
    
    def callback_write_txt_file(self):
        if (self.x != 0 or self.y != 0 or self.theta != 0):
            self.file_object_results.write(str(self.current_time) + " " + str(self.x)+" "+str(self.y)+" "+str(self.theta)+"\n")

    
def main(args=None):
    rclpy.init(args=args)

    odom_node = OdometryNode()

    rclpy.spin(odom_node)
    odom_node.file_object_results.close()
    odom_node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
