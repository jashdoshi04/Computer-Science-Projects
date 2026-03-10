# Student name: Jash Doshi  

import math
import numpy as np
from numpy import linalg as LA
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import TransformStamped, Accel
from tf2_ros import TransformBroadcaster

from std_msgs.msg import String, Float32
from nav_msgs.msg import Odometry
from sensor_msgs.msg import Imu
import matplotlib.pyplot as plt
import time
from mobile_robotics.utils import quaternion_from_euler, lonlat2xyz, quat2euler
from geometry_msgs.msg import Quaternion, Pose, Point, Twist


class ExtendedKalmanFilter(Node):

    
    def __init__(self):
        super().__init__('ExtendedKalmanFilter')
        
        #array to save the sensor measurements from the rosbag file
        #measure = [p, q, r, fx, fy, fz, x, y, z, vx, vy, vz] 
        self.measure = np.zeros(12)
        
        #Initialization of the variables used to generate the plots.
        self.PHI = []  
        self.PSI = []
        self.THETA = []
        self.P_R = []
        self.P_R1 = []
        self.P_R2 = []
        self.Pos = []
        self.Vel = []
        self.Quater = []
        self.measure_PosX = []
        self.measure_PosY = []
        self.measure_PosZ = []
        self.P_angular = []
        self.Q_angular = []
        self.R_angular = []
        self.P_raw_angular = []
        self.Q_raw_angular = []
        self.R_raw_angular = []
        self.Bias =[]
        
        self.POS_X = []
        self.POS_Y = []
        
        
        #Initialization of the variables used in the EKF
        
        # initial bias values, these are gyroscope and accelerometer biases
        self.bp= 0.0
        self.bq= 0.0
        self.br= 0.0
        self.bfx = 0.0
        self.bfy = 0.0
        self.bfz = 0.0
        # initial rotation
        #self.q2, self.q3, self.q4, self.q1 = quaternion_from_euler(0.0, 0.0, np.pi/2) #[qx,qy,qz,qw]
        qx, qy, qz, qw = quaternion_from_euler(0.0, 0.0, np.pi/2)
        self.q1, self.q2, self.q3, self.q4 = qw, qx, qy, qz


        #initialize the state vector [x y z vx vy vz          quat    gyro-bias accl-bias]
        self.xhat = np.array([[0, 0, 0, 0, 0, 0, self.q1, self.q2, self.q3, self.q4, self.bp, self.bq, self.br, self.bfx, self.bfy, self.bfz]]).T

        self.rgps=np.array([-0.15, 0 ,0]) #This is the location of the GPS wrt CG, this is very important
        
        #noise params process noise (my gift to you :))
        self.Q = np.diag([0.1, 0.1, 0.1, 0.01, 0.01, 0.01, 0.5, 0.5, 0.5, 0.5, 0.01, 0.01, 0.01, 0.001, 0.001, 0.001])
        #measurement noise
        #GPS position and velocity
        self.R = np.diag([10, 10, 10, 2, 2, 2])
        
       
        #Initialize P, the covariance matrix
        self.P = np.diag([30, 30, 30, 3, 3, 3, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1])
        self.Pdot=self.P*0.0
        
        self.time = []
        self.loop_t = 0

        # You might find these blocks useful when assembling the transition matrices
        self.Z = np.zeros((3,3))
        self.I = np.eye(3,3)
        self.Z34 = np.zeros((3,4))
        self.Z43 = np.zeros((4,3))
        self.Z36 = np.zeros((3,6))

        self.lat = 0
        self.lon = 0
        self.lat0 = 0
        self.lon0 = 0
        self.flag_lat = False
        self.flag_lon = False
        self.dt = 0.0125 #set sample time

        # Ros subscribers and publishers
        self.subscription_imu = self.create_subscription(Imu, 'terrasentia/imu', self.callback_imu, 10)
        self.subscription_gps_lat = self.create_subscription(Float32, 'gps_latitude', self.callback_gps_lat, 10)
        self.subscription_gps_lon = self.create_subscription(Float32, 'gps_longitude', self.callback_gps_lon, 10)
        self.subscription_gps_speed_north = self.create_subscription(Float32, 'gps_speed_north', self.callback_gps_speed_north, 10)
        self.subscription_gps_speend_east = self.create_subscription(Float32, 'gps_speed_east', self.callback_gps_speed_east, 10)
        
        self.timer_ekf = self.create_timer(self.dt, self.ekf_callback)
        self.timer_plot = self.create_timer(1, self.plot_data_callback)

    
    def callback_imu(self,msg):
        #measurement vector = [p, q, r, fx, fy, fz, x, y, z, vx, vy, vz]
        # In practice, the IMU measurements should be filtered. In this coding exercise, we are just going to clip
        # the values of velocity and acceleration to keep them in physically possible intervals.
        self.measure[0] = np.clip(msg.angular_velocity.x,-5,5) #(-5,5)
        self.measure[1] = np.clip(msg.angular_velocity.y,-5,5) #..(-5,5)
        self.measure[2] = np.clip(msg.angular_velocity.z,-5,5) #..(-5,5)
        self.measure[3] = np.clip(msg.linear_acceleration.x,-6,6) #..(-16,4)
        self.measure[4] = np.clip(msg.linear_acceleration.y,-6,6) #..(-16,4)
        self.measure[5] = np.clip(msg.linear_acceleration.z,-16,-4) #..(-16,4)
 
    def callback_gps_lat(self, msg):
        self.lat = msg.data
        if (self.flag_lat == False): #just a trick to recover the initial value of latitude
            self.lat0 = msg.data
            self.flag_lat = True
        
        if (self.flag_lat and self.flag_lon): 
            x, y = lonlat2xyz(self.lat, self.lon, self.lat0, self.lon0) # convert latitude and longitude to x and y coordinates
            self.measure[6] = x
            self.measure[7] = y
            self.measure[8] = 0.0 

    
    def callback_gps_lon(self, msg):
        self.lon = msg.data
        if (self.flag_lon == False): #just a trick to recover the initial value of longitude
            self.lon0 = msg.data
            self.flag_lon = True    
    
    def callback_gps_speed_east(self, msg): 
        self.measure[9] = msg.data # vx
        self.measure[11] = 0.0 # vz

    def callback_gps_speed_north(self, msg):
        self.measure[10] = msg.data # vy

   
    def ekf_callback(self):
        #print("iteration:  ",self.loop_t)
        if (self.flag_lat and self.flag_lon):  #Trick  to sincronize rosbag with EKF
            self.ekf_function()
        else:
            print("Play the rosbag file...cho")
    def publish_odometry(self):
        """
        Publishes the EKF-estimated position and orientation as an Odometry message.
        Topic: /ekf_odom
        Frame: 'odom'
        Child frame: 'base_link'
        """
        odom_msg = Odometry()
        odom_msg.header.stamp = self.get_clock().now().to_msg()
        odom_msg.header.frame_id = "odom"
        odom_msg.child_frame_id = "base_link"

        # --- Position ---
        odom_msg.pose.pose.position.x = float(self.xhat[0, 0])
        odom_msg.pose.pose.position.y = float(self.xhat[1, 0])
        odom_msg.pose.pose.position.z = float(self.xhat[2, 0])

        # --- Orientation ---
        q1, q2, q3, q4 = self.xhat[6:10, 0]
        quat_msg = Quaternion()
        quat_msg.w = float(q1)
        quat_msg.x = float(q2)
        quat_msg.y = float(q3)
        quat_msg.z = float(q4)
        odom_msg.pose.pose.orientation = quat_msg

        # --- Velocity ---
        odom_msg.twist.twist.linear.x = float(self.xhat[3, 0])
        odom_msg.twist.twist.linear.y = float(self.xhat[4, 0])
        odom_msg.twist.twist.linear.z = float(self.xhat[5, 0])

        # publish message
        self.odom_pub.publish(odom_msg)
    # COMPLETE CORRECTED EKF_FUNCTION - Replace your entire ekf_function with this:
    def ekf_callback(self):
            #print("iteration:  ",self.loop_t)
            if (self.flag_lat and self.flag_lon):  #Trick  to sincronize rosbag with EKF
                self.ekf_function()
            else:
                print("Play the rosbag file...")
    
    def ekf_function(self):

        
            self.p  = (self.measure[0] - self.xhat[10,0])
            self.q  = (self.measure[1] - self.xhat[11,0])
            self.r  = (self.measure[2] - self.xhat[12,0])
            self.fx = (self.measure[3] - self.xhat[13,0])
            self.fy = (self.measure[4] - self.xhat[14,0])
            self.fz = (self.measure[5] - self.xhat[15,0])

            
            self.quat = np.array([[self.xhat[6,0], self.xhat[7,0], self.xhat[8,0], self.xhat[9,0]]]).T  # [q1 q2 q3 q4] with q1 scalar
            q1, q2, q3, q4 = self.quat.flatten()

            self.R_bi = np.array([
                [q1*q1 + q2*q2 - q3*q3 - q4*q4,   2*(q2*q3 + q1*q4),           2*(q2*q4 - q1*q3)],
                [2*(q2*q3 - q1*q4),               q1*q1 - q2*q2 + q3*q3 - q4*q4, 2*(q3*q4 + q1*q2)],
                [2*(q2*q4 + q1*q3),               2*(q3*q4 - q1*q2),           q1*q1 - q2*q2 - q3*q3 + q4*q4]
            ])
            
            aI = self.R_bi @ np.array([self.fx, self.fy, self.fz]) + np.array([0,0,9.81])

            # State derivatives
            pxdot, pydot, pzdot = self.xhat[3,0], self.xhat[4,0], self.xhat[5,0]    
            vxdot, vydot, vzdot = aI[0], aI[1], aI[2]                            

            # Quaternion rate: qdot = -0.5 * Omega(ω) * q
            Om = np.array([
                [ 0,  self.p,  self.q,  self.r],
                [-self.p,  0, -self.r,  self.q],
                [-self.q,  self.r,  0, -self.p],
                [-self.r, -self.q,  self.p,  0]
            ]) 
            qdot = -0.5 * (Om @ self.quat)

        
            self.xhat[0,0] += self.dt * pxdot
            self.xhat[1,0] += self.dt * pydot
            self.xhat[2,0] += self.dt * pzdot
            self.xhat[3,0] += self.dt * vxdot
            self.xhat[4,0] += self.dt * vydot
            self.xhat[5,0] += self.dt * (vzdot)
            self.xhat[6,0] += self.dt * qdot[0,0]
            self.xhat[7,0] += self.dt * qdot[1,0]
            self.xhat[8,0] += self.dt * qdot[2,0]
            self.xhat[9,0] += self.dt * qdot[3,0]

            print("x ekf: ", self.xhat[0,0])
            print("y ekf: ", self.xhat[1,0])
            print("z ekf: ", self.xhat[2,0])

            # Normalize quaternion (prediction step)
            self.quat = np.array([self.xhat[6,0], self.xhat[7,0], self.xhat[8,0], self.xhat[9,0]])
            self.quat = self.quat / np.linalg.norm(self.quat)
            # re-assign quat to state
            self.xhat[6,0] = self.quat[0]
            self.xhat[7,0] = self.quat[1]
            self.xhat[8,0] = self.quat[2]
            self.xhat[9,0] = self.quat[3]

            q1, q2, q3, q4 = self.quat.flatten()

    
            ax, ay, az = aI[0], aI[1], aI[2]
            Fvq = 2.0 * np.array([
                [ (q1*ax + q4*ay - q3*az), (q2*ax + q3*ay + q4*az), (-q3*ax + q2*ay + q1*az), (-q4*ax - q1*ay + q2*az) ],
                [ (q4*ax + q1*ay - q2*az), (q3*ax - q2*ay - q1*az), ( q2*ax + q3*ay + q4*az), ( q1*ax - q4*ay + q3*az) ],
                [ (-q3*ax + q2*ay + q1*az), (q4*ax + q1*ay - q2*az), (-q1*ax + q4*ay - q3*az), ( q2*ax + q3*ay + q4*az) ],
            ])

            # F_v b_a = -R_bi
            Fvba = -self.R_bi.copy()

            # F_qq = -0.5 * Ω(ω)  
            Fqq = -0.5 * Om

            # F_q b_ω = 0.5 * [[q2 q3 q4], [-q1 q4 -q3], [-q4 -q1 q2], [q3 -q2 -q1]]
            Fqbw = 0.5 * np.array([
                [ q2,  q3,  q4],
                [-q1,  q4, -q3],
                [-q4, -q1,  q2],
                [ q3, -q2, -q1]
            ])

        
            A = np.zeros((16,16))
            # ṗ wrt v
            A[0:3, 3:6]   = np.eye(3)                   
            # v̇ wrt q, ba
            A[3:6, 6:10]  = Fvq                         
            A[3:6, 13:16] = Fvba                        
            # q̇ wrt q, bω
            A[6:10, 6:10] = Fqq                         
            A[6:10,10:13] = Fqbw                        

        
            Pdot = A @ self.P + self.P @ A.T + self.Q
            self.P = self.P + self.dt * Pdot

        
            # z = [xGPS yGPS zGPS vxGPS vyGPS vzGPS]^T
            self.z = np.array([[self.measure[6], self.measure[7], self.measure[8],
                                self.measure[9], self.measure[10], self.measure[11]]]).T

        
            rgps_x = self.rgps[0]
        
            # Hxq (3x4)
            Hxq = 2.0 * rgps_x * np.array([
                [-q1, -q2,  q3,  q4],
                [-q4, -q3, -q2, -q1],
                [ q3, -q4,  q1, -q2]
            ]) 

            # Hvq (3x4) uses body rates
            P_, Q_, R_ = self.p, self.q, self.r
            Hvq = 2.0 * rgps_x * np.array([
                [ q3*Q_ + q4*R_,  q4*Q_ - q3*R_,  q1*Q_ - q2*R_,  q2*Q_ + q1*R_],
                [-q2*Q_ - q1*R_,  q2*R_ - q1*Q_,  q4*Q_ - q3*R_,  q3*Q_ + q4*R_],
                [ q1*Q_ - q2*R_, -q2*Q_ - q1*R_, -q3*Q_ - q4*R_,  q4*Q_ - q3*R_],
            ])

            # Full H (6x16)
            H = np.zeros((6,16))
            H[0:3, 0:3]   = np.eye(3)      # position wrt p
            H[0:3, 6:10]  = Hxq            # position wrt q
            H[3:6, 3:6]   = np.eye(3)      # velocity wrt v
            H[3:6, 6:10]  = Hvq            # velocity wrt q
        

            # Compute Kalman Gain
            S = H @ self.P @ H.T + self.R
            L = self.P @ H.T @ np.linalg.inv(S)

            self.xhat = self.xhat + (L @ (self.z - H @ self.xhat))

            # Re-normalize quaternion after correction and enforce continuity
            qcorr = self.xhat[6:10,0]
            qcorr = qcorr / np.linalg.norm(qcorr)
            # enforce quaternion hemisphere continuity against previous quaternion to avoid sign flips
            
            self.xhat[6:10,0] = qcorr

            # Covariance update (simple form)
            self.P = (np.eye(16,16) - L @ H) @ self.P

            # Now let us do some book-keeping 
            # Get some Euler angles from the corrected quaternion (use xhat to ensure we use post-update quat)
            q_for_euler = self.xhat[6:10,0]
            phi, theta, psi = quat2euler(q_for_euler.T)

            self.PHI.append(phi*180/math.pi)
            self.THETA.append(theta*180/math.pi)
            self.PSI.append(psi*180/math.pi)
        
            
            # Saving data for the plots. Uncomment the 4 lines below once you have finished the ekf function

            DP = np.diag(self.P)
            self.P_R.append(DP[0:3])
            self.P_R1.append(DP[3:6])
            self.P_R2.append(DP[6:10])
            self.Pos.append(self.xhat[0:3].T[0])
            self.POS_X.append(self.xhat[0,0])
            self.POS_Y.append(self.xhat[1,0])
            self.Vel.append(self.xhat[3:6].T[0])
            self.Quater.append(self.xhat[6:10].T[0])
            self.Bias.append(self.xhat[10:16].T[0])
            B = self.measure[6:9].T
            self.measure_PosX.append(B[0])
            self.measure_PosY.append(B[1])
            self.measure_PosZ.append(B[2])

            self.P_angular.append(self.p)
            self.Q_angular.append(self.q)
            self.R_angular.append(self.r)

            self.loop_t += 1
            self.time.append(self.loop_t*self.dt)
            # Monitor covariance growth
            P_diag = np.diag(self.P)
            if self.loop_t % 100 == 0:  # Every ~1.25 seconds
                P_diag = np.diag(self.P)
                print(f"t={self.loop_t*0.0125:.2f}s: P_diag max={np.max(P_diag):.2e}")

            # Check for divergence
            if np.max(P_diag) > 1e10:
                print("WARNING: Covariance exploding!")

            # Monitor Kalman gain magnitude
            # Monitor Kalman gain magnitude
            if hasattr(self, 'L'):
                L_max = np.max(np.abs(self.L))
                if L_max > 1.0:
                    print(f"WARNING: Large Kalman gain: {L_max:.2f}")

    def plot_data_callback(self):

        plt.figure(1)
        plt.clf()
        plt.plot(self.time,self.PHI,'b', self.time, self.THETA, 'g', self.time,self.PSI, 'r')
        plt.legend(['phi','theta','psi'])
        plt.title('Phi, Theta, Psi [deg]')

        plt.figure(2)
        plt.clf()
        plt.plot(self.measure_PosX, self.measure_PosY, self.POS_X, self.POS_Y)
        plt.title('xy trajectory')
        plt.legend(['GPS','EKF'])

        plt.figure(3)
        plt.clf()
        plt.plot(self.time,self.P_R)
        plt.title('Covariance of Position')
        plt.legend(['px','py','pz'])
        plt.figure(4)
        plt.clf()
        plt.plot(self.time,self.P_R1)
        plt.legend(['pxdot','pydot','pzdot'])
        plt.title('Covariance of Velocities')
        plt.figure(5)
        plt.clf()
        plt.plot(self.time,self.P_R2)
        plt.title('Covariance of Quaternions')
        plt.figure(6)
        plt.clf()
        plt.plot(self.time,self.Pos,self.time,self.measure_PosX,'r:', self.time,self.measure_PosY,'r:', self.time,self.measure_PosZ,'r:')
        plt.legend(['X_ekf', 'Y_ekf', 'Z_ekf','Xgps','Ygps','Z_0'])
        plt.title('Position')
        plt.figure(7)
        plt.clf()
        plt.plot(self.time,self.Vel)
        plt.title('vel x y z')
        plt.figure(8)
        plt.clf()
        plt.plot(self.time,self.Quater)
        plt.title('Quat')
        plt.figure(9)
        plt.clf()
        plt.plot(self.time,self.P_angular,self.time,self.Q_angular,self.time,self.R_angular)
        plt.title('OMEGA with Bias')
        plt.legend(['p','q','r'])
        plt.figure(10)
        plt.clf()
        plt.plot(self.time,self.Bias)
        plt.title('Gyroscope and accelerometer Bias')
        plt.legend(['bp','bq','br','bfx','bfy','bfz'])
               
        plt.ion()
        plt.show()
        plt.pause(0.0001)
        

def main(args=None):
    rclpy.init(args=args)

    ekf_node = ExtendedKalmanFilter()
    
    rclpy.spin(ekf_node)

   
    ekf_node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
