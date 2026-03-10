# Student name: 

import math
import numpy as np
import rclpy
from rclpy.node import Node

from geometry_msgs.msg import Point, Pose, Quaternion, Twist, Vector3, PoseStamped, TransformStamped
from std_msgs.msg import String, Float32
from nav_msgs.msg import Odometry
from sensor_msgs.msg import Imu, LaserScan
import matplotlib.pyplot as plt
import time
from tf2_msgs.msg import TFMessage
from visualization_msgs.msg import Marker
from typing import List, Tuple, Optional

# Further info:
# On markers: http://wiki.ros.org/rviz/DisplayTypes/Marker
# Laser Scan message: http://docs.ros.org/en/melodic/api/sensor_msgs/html/msg/LaserScan.html

class CodingExercise3(Node):

    def __init__(self):
        super().__init__('CodingExercise3')

        self.ranges = [] # lidar measurements
        self.pose: Optional[Tuple[float, float, float]] = None  # (x, y, yaw)

        # Parameters for line extraction
        self.range_jump_threshold = 0.30  # meters, separates scan segments
        self.split_threshold = 0.08     # meters, max allowed distance to fitted line before splitting
        self.min_segment_points = 2
        self.corner_min_angle = math.radians(25)
        self.max_map_segments = 4000
        self.max_corner_points = 4000

        # Data structures for map building
        self.point_list: List[Point] = []      # Line marker point pairs in odom frame
        self.corner_points: List[Point] = []   # Corner markers in odom frame
        self.line = Marker()
        self.corners_marker = Marker()
        self.line_marker_init(self.line)
        self.corner_marker_init(self.corners_marker)


        # Ros subscribers and publishers
        self.subscription_ekf = self.create_subscription(Odometry, 'terrasentia/ekf', self.callback_ekf, 10)
        self.subscription_scan = self.create_subscription(LaserScan, 'terrasentia/scan', self.callback_scan, 10)
        self.pub_lines = self.create_publisher(Marker, 'lines', 10)
        self.pub_corners = self.create_publisher(Marker, 'corners', 10)
        self.timer_publish_markers = self.create_timer(0.2, self.publish_markers)

    
    def callback_ekf(self, msg):
        # You will need this function to read the translation and rotation of the robot with respect to the odometry frame
        position = msg.pose.pose.position
        orientation = msg.pose.pose.orientation
        yaw = self.quaternion_to_yaw(orientation)
        self.pose = (position.x, position.y, yaw)
   
    def callback_scan(self, msg):
        self.ranges = list(msg.ranges) # Lidar measurements
        if self.pose is None:
            # Wait until we have a valid odometry estimate to place lines on the map
            return
        self.process_scan(msg)

    def process_scan(self, msg: LaserScan):
        """
        Convert the LaserScan into Cartesian points, segment the scan, run
        split-and-merge to fit line segments, detect corners, and accumulate a
        simple geometric map in the odom frame.
        """
        ranges = np.asarray(msg.ranges, dtype=float)
        angles = msg.angle_min + np.arange(len(ranges)) * msg.angle_increment

        finite_mask = np.isfinite(ranges)
        valid = finite_mask & (ranges >= msg.range_min) & (ranges <= msg.range_max)
        ranges = ranges[valid]
        angles = angles[valid]
        if ranges.size < 2:
            return

        xs = ranges * np.cos(angles)
        ys = ranges * np.sin(angles)
        scan_points = np.column_stack((xs, ys))

        # Separate scan into contiguous segments to avoid fitting across large jumps
        segments = self.segment_scan(scan_points, ranges)

        fitted_segments: List[Tuple[np.ndarray, np.ndarray]] = []
        for seg in segments:
            fitted_segments.extend(self.split_and_merge(seg))

        corners_local = self.find_corners(fitted_segments)
        self.update_map(fitted_segments, corners_local)

    def line_marker_init(self, line):
        line.header.frame_id="/odom"
        line.header.stamp=self.get_clock().now().to_msg()

        line.ns = "markers"
        line.id = 0

        line.type=Marker.LINE_LIST
        line.action = Marker.ADD
        line.pose.orientation.w = 1.0

        line.scale.x = 0.05
        line.scale.y= 0.05
        
        line.color.r = 1.0
        line.color.a = 1.0

    def corner_marker_init(self, marker: Marker):
        marker.header.frame_id="/odom"
        marker.header.stamp=self.get_clock().now().to_msg()
        marker.ns = "corners"
        marker.id = 1
        marker.type = Marker.SPHERE_LIST
        marker.action = Marker.ADD
        marker.scale.x = 0.12
        marker.scale.y = 0.12
        marker.scale.z = 0.12
        marker.color.g = 1.0
        marker.color.a = 1.0

    def publish_markers(self):
        self.line.header.stamp = self.get_clock().now().to_msg()
        self.line.points = list(self.point_list)
        self.pub_lines.publish(self.line)

        self.corners_marker.header.stamp = self.line.header.stamp
        self.corners_marker.points = list(self.corner_points)
        if self.corner_points:
            self.pub_corners.publish(self.corners_marker)

    def segment_scan(self, points: np.ndarray, ranges: np.ndarray) -> List[np.ndarray]:
        """Split the scan whenever consecutive ranges jump too much."""
        segments: List[List[np.ndarray]] = []
        current: List[np.ndarray] = []
        for idx in range(points.shape[0]):
            if not current:
                current.append(points[idx])
                continue
            if abs(ranges[idx] - ranges[idx - 1]) > self.range_jump_threshold:
                if len(current) >= self.min_segment_points:
                    segments.append(np.array(current))
                current = [points[idx]]
            else:
                current.append(points[idx])
        if len(current) >= self.min_segment_points:
            segments.append(np.array(current))
        return segments

    def split_and_merge(self, segment: np.ndarray) -> List[Tuple[np.ndarray, np.ndarray]]:
        """
        Recursive split-and-merge line fitting. Returns list of (p0, p1) pairs in
        the local (lidar) frame.
        """
        if segment.shape[0] < self.min_segment_points:
            return []

        p0, p1 = segment[0], segment[-1]
        line_vec = p1 - p0
        norm = np.linalg.norm(line_vec)
        if norm < 1e-6:
            return []

        # Perpendicular distances of all points to the line defined by p0->p1
        distances = np.abs(np.cross(line_vec, segment - p0) / norm)
        split_idx = int(np.argmax(distances))
        max_dist = distances[split_idx]

        if max_dist > self.split_threshold:
            left = self.split_and_merge(segment[: split_idx + 1])
            right = self.split_and_merge(segment[split_idx:])
            return left + right
        else:
            # Optionally refine endpoints by projecting onto the fitted line
            line_dir = line_vec / norm
            proj = np.dot(segment - p0, line_dir)
            p_start = p0 + proj.min() * line_dir
            p_end = p0 + proj.max() * line_dir
            return [(p_start, p_end)]

    def find_corners(self, segments: List[Tuple[np.ndarray, np.ndarray]]) -> List[np.ndarray]:
        """Estimate corners as intersections between consecutive, non-parallel lines."""
        corners: List[np.ndarray] = []
        for i in range(len(segments) - 1):
            a0, a1 = segments[i]
            b0, b1 = segments[i + 1]
            v1 = a1 - a0
            v2 = b1 - b0
            n1 = np.linalg.norm(v1)
            n2 = np.linalg.norm(v2)
            if n1 < 1e-6 or n2 < 1e-6:
                continue
            cos_ang = np.dot(v1, v2) / (n1 * n2)
            cos_ang = float(np.clip(cos_ang, -1.0, 1.0))
            angle = math.acos(cos_ang)
            if angle < self.corner_min_angle:
                continue
            intersection = self.line_intersection(a0, a1, b0, b1)
            if intersection is not None:
                corners.append(intersection)
        return corners

    def line_intersection(self, p1: np.ndarray, p2: np.ndarray, p3: np.ndarray, p4: np.ndarray) -> Optional[np.ndarray]:
        """Compute intersection point of two lines (not bounded segments) in 2D."""
        x1, y1 = p1
        x2, y2 = p2
        x3, y3 = p3
        x4, y4 = p4
        denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4)
        if abs(denom) < 1e-9:
            return None
        px = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / denom
        py = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / denom
        return np.array([px, py])

    def update_map(self, segments: List[Tuple[np.ndarray, np.ndarray]], corners_local: List[np.ndarray]):
        """Transform lines/corners into odom frame and accumulate them for mapping."""
        if self.pose is None:
            return
        x, y, yaw = self.pose
        cos_yaw = math.cos(yaw)
        sin_yaw = math.sin(yaw)

        def transform_point(pt: np.ndarray) -> Tuple[float, float]:
            return (
                x + cos_yaw * pt[0] - sin_yaw * pt[1],
                y + sin_yaw * pt[0] + cos_yaw * pt[1],
            )

        for p0, p1 in segments:
            g0 = transform_point(p0)
            g1 = transform_point(p1)
            self.point_list.append(self.point_to_msg(g0))
            self.point_list.append(self.point_to_msg(g1))
        if len(self.point_list) > 2 * self.max_map_segments:
            # Keep a sliding window to avoid unbounded marker growth
            self.point_list = self.point_list[-2 * self.max_map_segments :]

        for c in corners_local:
            gc = transform_point(c)
            self.corner_points.append(self.point_to_msg(gc))
        if len(self.corner_points) > self.max_corner_points:
            self.corner_points = self.corner_points[-self.max_corner_points :]

    def point_to_msg(self, xy: Tuple[float, float]) -> Point:
        p = Point()
        p.x = float(xy[0])
        p.y = float(xy[1])
        p.z = 0.0
        return p

    def quaternion_to_yaw(self, q: Quaternion) -> float:
        """Return yaw (heading) from a quaternion message."""
        siny_cosp = 2.0 * (q.w * q.z + q.x * q.y)
        cosy_cosp = 1.0 - 2.0 * (q.y * q.y + q.z * q.z)
        return math.atan2(siny_cosp, cosy_cosp)


def main(args=None):
    rclpy.init(args=args)

    cod3_node = CodingExercise3()
    
    rclpy.spin(cod3_node)

    cod3_node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
