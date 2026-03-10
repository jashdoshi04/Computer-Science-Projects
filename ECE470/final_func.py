#!/usr/bin/env python
import numpy as np
from scipy.linalg import expm, logm
from final_header import *

"""
Use 'expm' for matrix exponential.
Angles are in radian, distance are in meters.
"""
def Get_MS():
	# =================== Code from Lab 5 ====================#
	# Fill in the correct values for w1~6 and v1~6, as well as the M matrix
	M = np.eye(4)
	S = np.zeros((6,6))
	M = np.array([[0,-1,0,390],[0,0,-1,401],[1,0,0,215.5],[0,0,0,1]])
  # S = np.array([[   0,    0,    0,    0,    1,    0],
  #    [   0,    1,    1,    1,    0,    1],
  #    [   1,    0,    0,    0,    0,    0],
  #    [ 150, -162, -162, -162,    0, -162],
  #    [ 150,    0,    0,    0,  162,    0],
  #    [   0, -150,   94,  307, -260,  390]])
	S1 = np.array([[  0.,  -1.,   0., 150.],
		[  1.,   0.,   0., 150.],
		[  0.,   0.,   0.,   0.],
		[  0.,   0.,   0.,   0.]])
	# S2=np.array([   0,    1,    0, -162,    0, -150])
	S2 = np.array([[   0.,    0.,    1., -162.],
		[   0.,    0.,    0.,    0.],
		[  -1.,    0.,    0., -150.],
		[   0.,    0.,    0.,    0.]])
	
	# S3=np.array([   0,    1,    0, -162,    0,   94])
	S3 = np.array([[   0.,    0.,    1., -162.],
		[   0.,    0.,    0.,    0.],
		[  -1.,    0.,    0.,   94.],
		[   0.,    0.,    0.,    0.]])
	# S4=np.array([   0,    1,    0, -162,    0,  307])
	S4 = np.array([[   0.,    0.,    1., -162.],
		[   0.,    0.,    0.,    0.],
		[  -1.,    0.,    0.,  307.],
		[   0.,    0.,    0.,    0.]])
	# S5=np.array([   1,    0,    0,    0,  162, -260])
	S5 = np.array([[   0.,    0.,    0.,    0.],
		[   0.,    0.,   -1.,  162.],
		[   0.,    1.,    0., -260.],
		[   0.,    0.,    0.,    0.]])
	# S6=np.array([   0,    1,    0, -162,    0,  390])
	S6 = np.array([[   0.,    0.,    1., -162.],
		[   0.,    0.,    0.,    0.],
		[  -1.,    0.,    0.,  390.],
		[   0.,    0.,    0.,    0.]])
	S=np.array([S1,S2,S3,S4,S5,S6])




	# ==============================================================#
	return M, S


"""
Function that calculates encoder numbers for each motor
"""
def lab_fk(theta1, theta2, theta3, theta4, theta5, theta6):

	# Initialize the return_value
	return_value = [None, None, None, None, None, None]

	print("Foward kinematics calculated:\n")

	# =================== Code from Lab 5 ====================#
	# theta = np.array([theta1,theta2,theta3,theta4,theta5,theta6])
	# T = np.eye(4)

	# M, S = Get_MS()
	# T1 = expm(theta1*S[0])
	# T2 = expm(theta2*S[1])
	# T3 = expm(theta3*S[2])
	# T4 = expm(theta4*S[3])
	# T5 = expm(theta5*S[4])
	# T6 = expm(theta6*S[5])

	# T = T1 @ T2 @ T3 @ T4 @ T5 @ T6 @ M

	# print(str(T) + "\n")




	# ==============================================================#

	return_value[0] = theta1 + PI
	return_value[1] = theta2
	return_value[2] = theta3
	return_value[3] = theta4 - (0.5*PI)
	return_value[4] = theta5
	return_value[5] = theta6

	return return_value


"""
Function that calculates an elbow up Inverse Kinematic solution for the UR3
"""
def lab_invk(xWgrip, yWgrip, zWgrip, yaw_WgripDegree):
	# =================== Code from Lab 5 ====================#
	l1 = 152
	l2 = 120
	l3 = 244
	l4 = 93
	l5 = 213
	l6 = 83
	l7 = 83
	l8 = 82
	l9 = 53.5
	l10 = 59
	xWgrip = xWgrip+150
	yWgrip = yWgrip-150
	zWgrip = zWgrip - 10 - l1
	x_cen = xWgrip - l9*np.cos(yaw_WgripDegree)
	y_cen = yWgrip - l9*np.sin(yaw_WgripDegree)
	z_cen = zWgrip
	cx = x_cen 
	cy = y_cen 
	c = np.sqrt(cx*cx + cy*cy)
	a = l2-l4+l6
	deltheta = np.arcsin(a/c)

	theta1 = np.arctan2(cy,cx) - deltheta
	#    theta2 = sp.Symbol('theta2')
	# theta3 = 0.0
	# theta4 =
	theta5 = -np.pi/2
	theta6 = np.pi/2 - yaw_WgripDegree + theta1
	z_3end = l8 + l10 + z_cen
	x_3end = (c*np.cos(deltheta) - l7)*np.cos(theta1)
	y_3end = (c*np.cos(deltheta) - l7)*np.sin(theta1)
	# f = l3*np.cos(theta2) + l3*np.sin(theta2) +10 +l1
	base2end = np.sqrt((z_3end)**2 + x_3end**2 + y_3end**2)
	theta2_2 = np.arccos((l3**2 + base2end**2 - l5**2)/(2*l3*base2end))
	theta2_1 = np.arctan2((z_3end),np.sqrt(x_3end**2 + y_3end**2))
	theta2 = (theta2_1+theta2_2) 
	theta3sub = np.arccos((l3**2 - base2end**2 +l5**2)/(2*l3*l5))
	theta3 = np.pi - theta3sub
	theta4 = theta3 - theta2

	theta2 = -theta2
	theta4 = -theta4
	# return [theta1, theta2, theta3,theta4,theta5,theta6]
	# ==============================================================#
	return lab_fk(theta1, theta2, theta3, theta4, theta5, theta6)
