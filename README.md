# Nunchuck Mouse

## Purpose:
This project turns a wii nunchuck into a mouse, complete with left and right buttons, mouse movement with the joystick, and scrolling.  This project is working toward a one handed computer interface for coding.  It'll be paired with speech to text to make coding with one hand (and your voice) possible.

## Operation:
At the moment, the controller has 2 modes: mouse mode and scroll mode.

Mouse mode is active when the controller is held upright in ones hand (the joystick is perpendicular to the ground).  In mouse mode, the joystick moves the mouse, and the c and z buttons control the left and right mouse clicks, respectively.
Scroll mdoe is active when the controller is on its left or right side.  In scroll mode, the mouse buttons still work, but the joystick y axis controlls the mouse's scrolling action.

## Hardware:
This should work on any RP2040 although you might have to change some pin numbers for your board.  I used the tiny2040 by pimoroni.  You also need a Wii Nunchuck controller.  Connect 3.3V, Gnd, i2c data, and i2c clock to the Wii Nunchuck.  A tri-color LED is used to report the mode the controller is in.

## Build instructions:
1. Have the pico-sdk and submodules installed.  It's also useful to have the pico-examples (for seeing other usb examples) and picotool (for easy flashing of the RP2040) installed, but not required.
2. ```mkdir build```
3. ```cd build```
4. ```cmake ..```
5. ```make```
6. Put RP2040 in bootloader mode.
7. ```picotool load nunchuck_mouse.elf```
8. ```picotool reboot```
9. Reset the RP2040, after approximately 1 second, the code should start.
