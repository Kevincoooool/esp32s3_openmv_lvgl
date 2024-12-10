# Untitled - By: Admin - Sun Mar 17 2024
from machine import Pin, PWM
# 舵机控制原理：生成50HZ方波
#使用TIM12，channels 1
#控制舵机需要输出2.5%-12.5%的PWM
#0度----0.5/20*100=2.5;(要看实际的舵机是否支持0度，一般运动不到0度，需要把这个值调大一点)
#45度----1.0/20*100=5;
#90度----1.5/20*100=7.5;
#135度----2.0/20*100=10;
#180度----2.5/20*100=12.5;
class USERVO:
    def __init__(self,pin):
        self.servo = PWM(Pin(pin), freq=50, duty=76)  # create and configure in one go
        ############################舵机驱动############################
        #初始角度90度
        self.sangle=90
    def angle(self,servo_angle=90,soffset=0):
        servo_angle=servo_angle+soffset
        if servo_angle<=1:
            servo_angle=1
        if servo_angle>=180:
            servo_angle=180
        self.sangle=servo_angle
        duty=int((servo_angle*256/45)+256)
        duty=int(duty/10)
        self.servo.duty(duty)
        return self.sangle

