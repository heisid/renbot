/* 
   -- RenBot, Ridiculous Robot by Dumbest Engineer Ever -- 
    
   This source code of graphical user interface  
   has been generated automatically by RemoteXY editor. 
   To compile this code using RemoteXY library 2.3.3 or later version  
   download by link http://remotexy.com/en/library/ 
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/                    
     - for ANDROID 4.1.1 or later version; 
     - for iOS 1.2.1 or later version; 
     
   This source code is free software; you can redistribute it and/or 
   modify it under the terms of the GNU Lesser General Public 
   License as published by the Free Software Foundation; either 
   version 2.1 of the License, or (at your option) any later version.     
*/ 

////////////////////////////////////////////// 
//        RemoteXY include library          // 
////////////////////////////////////////////// 

// RemoteXY select connection mode and include library  
#define REMOTEXY_MODE__SOFTSERIAL
#include <SoftwareSerial.h> 

#include <RemoteXY.h> 

// RemoteXY connection settings  
#define REMOTEXY_SERIAL_RX A0 
#define REMOTEXY_SERIAL_TX A1 
#define REMOTEXY_SERIAL_SPEED 9600 


// RemoteXY configurate   
#pragma pack(push, 1) 
uint8_t RemoteXY_CONF[] = 
  { 255,5,0,1,0,159,0,8,24,0,
  5,47,7,23,30,30,6,26,31,5,
  1,63,23,30,30,6,26,31,2,1,
  7,10,29,10,6,26,31,31,77,65,
  78,85,65,76,0,65,85,84,79,0,
  66,65,44,14,7,36,1,26,129,0,
  51,3,45,6,16,82,101,110,66,111,
  116,32,67,111,110,116,114,111,108,0,
  129,0,64,12,19,4,16,83,105,100,
  64,50,48,49,56,0,129,0,45,53,
  6,4,16,68,105,115,32,40,99,109,
  41,0,129,0,19,56,9,4,16,76,
  101,103,115,0,129,0,73,56,9,4,
  16,78,101,99,107,0,129,0,53,13,
  2,4,16,48,0,129,0,53,48,2,
  4,16,49,48,48,0,129,0,53,29,
  7,4,16,53,48,0 }; 
   
// this structure defines all the variables of your control interface  
struct { 

    // input variable
  int8_t joystick_legs_x; // =-100..100 x-coordinate joystick position 
  int8_t joystick_legs_y; // =-100..100 y-coordinate joystick position 
  int8_t joystick_neck_x; // =-100..100 x-coordinate joystick position 
  int8_t joystick_neck_y; // =-100..100 y-coordinate joystick position 
  uint8_t switch_mode; // =1 if switch ON and =0 if OFF 

    // output variable
  int8_t level_distance; // =0..100 level position 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY; 
#pragma pack(pop) 

///////////////////////////////////////////// 
//           END RemoteXY include          // 
///////////////////////////////////////////// 


/*
 * My Fucking Code Goes Here 
 *
 */

#include <AFMotor.h>
#include <PWMServo.h>
#include <NewPing.h>

#define AUTO 0
#define MANUAL 1
#define TRIG 9
#define ECHO A3 
#define MIN_DISTANCE 10 // Minimum distance for auto mode
#define MAX_DISTANCE 100 // Maximum distance for reading

AF_DCMotor right_leg(2);
AF_DCMotor left_leg(1);
PWMServo neck;
NewPing sonar(TRIG, ECHO);

int neck_angle = 80;  // Initial neck position
long distance;  // distance as measured by SR04

void move(AF_DCMotor &leg, int speed) {

  if (speed > 100) speed = 100;
  if (speed < -100) speed = -100;
  if (speed > 0) {
    leg.setSpeed(speed * 2.55);
    leg.run(FORWARD);
  } else if (speed<0) {
    leg.setSpeed((-speed) * 2.55);
    leg.run(BACKWARD);
  } else leg.run(RELEASE);

}


// Manual control, achieved by toggling switch_mode to "MANUAL"
void manual_mode() {
  
  move(right_leg, RemoteXY.joystick_legs_y - RemoteXY.joystick_legs_x);
  move(left_leg, RemoteXY.joystick_legs_y + RemoteXY.joystick_legs_x);

  neck_angle = -RemoteXY.joystick_neck_x;
  neck_angle = map(neck_angle, -100, 100, 0, 180);
  neck.write(neck_angle);

}

void auto_mode() {
  if (distance <= MIN_DISTANCE) {
    move(right_leg, 0);
    move(left_leg, 0);
    delay(400);
    move(right_leg, -255);
    move(left_leg, -255);
    delay(500);
    move(right_leg, 0);
    move(left_leg, 0);
    delay(400);
    
    if (random(2)) {
      neck.write(45);
      update_distance();
      delay(400);
      if (distance > MIN_DISTANCE) {
        neck.write(80);
        move(right_leg, -255);
        move(left_leg, 255);
        delay(300);
      }
    
    } else {

      neck.write(135);
      update_distance();
      delay(400);
      if (distance > MIN_DISTANCE) {
        neck.write(80);
        move(right_leg, 255);
        move(left_leg, -255);
        delay(300);
      }
    }

  } else {
    move(right_leg, 255);
    move(left_leg, 255);
  }

}

void update_distance() {

  distance = sonar.ping_cm(); 
  if (distance > MAX_DISTANCE) distance = MAX_DISTANCE;
  RemoteXY.level_distance = distance;

}

void setup() { 
  RemoteXY_Init ();  
  RemoteXY.joystick_neck_x = 0;
  RemoteXY.switch_mode = AUTO;
   
  neck.attach(10);
  neck.write(neck_angle);

  randomSeed(analogRead(A2));

} 

void loop() { 

  RemoteXY_Handler ();
  
  update_distance();

  if (RemoteXY.switch_mode == MANUAL) {
    manual_mode();
  } else {
    auto_mode();
  }

}
