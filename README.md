# ESP32_DJI_VirtualFlight_RC_radio_interface
convert any RC radio (PPM signal) into a BLE joystick compatible with DJI VirtualFlight simulator

DJI remote controller is a HID Joystick with X, Y, Z, Rx and Ry "analog sticks" and 7 active buttons. 

Here is the law table to map the actions on the remote2 to the corresponding joystick events.

All sticks output values between +-660
Right stick
  Left/right: Joystick X axis
  Up/Down: Joystick Y axis
Left stick
  Left/Right: Joystick Rx axis
  Up/Down: Joystick Z axis
gimbal wheel: Joystick Ry
buttons
  C1: Joystick button 1
  Start: Joystick button 2
  home: Joystick button 3
  photo: Joystick button 4
  
  Full description of the project is on hackaday : https://hackaday.io/project/188679-a-poor-mans-fpv-journey
  
  This repo contains the source code for the ESP32 board.
