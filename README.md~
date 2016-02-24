Copyright 2013, 2014, 2015, 2016 Jesse Waite

(TLDR: Skip to the bottom directory listing if you're just looking for PIC32 device drivers for imu's, ping sensors,
or the adxl345 accelerometer over spi, etc.)

This project is a code-dump from 2013-2016 for an mcu capable of polling a bunch of sensor data and sending/receiving
data/commands. This repo isn't a single project, its actually a code dump for a bunch of my mcu and 
embedded projects. The main project was a balancing robot made out of the Digilent line-following robot
using FreeRTOS, but I removed the RTOS since the overhead never seemed necessary for hobby applications.

The project in its current state is set up as a testing platform for an MCU-based telemetry sensor (on the pic)
which sends/receives commands to a linux-based client. The linux client implemented a bunch of views
and data crunching algorithms, using unix sockets to demux telemetry data from the wireless mcu to whatever
subscriber processes existed. I fought to get the digilent wifi module to work for this board, only to find
it can't implement the required stack, and that's the last state in which I left this project before interviews
took over my summer.


Target architecture:
  Digilent MX4CK, PIC32MX460F512L

Client/server:
  Ubuntu/linux host with bluetooth


Others might find these useful:
  my_i2c.h/c: Useful I2C device wrappers
  my_imu.h/c: Implements I2C drivers porting a 10dof arduino imu to the pic32MX460F512L. These devices
              are awesome, precise, and amazingly inexpensive (there are lots of 10dof boards with all of
              them). This code is from 2013, so many of these devices have been updated with newer versions.
     Devices implemented:
        HMC5883L Magnetometer/compass
        BMP085 Barometer/altitude and temp sensor
        ADXL345 accelerometer
        L3G4200D gyroscope
  my_spi.h/c: Useful spi drivers. I think this has an additional driver for the Digilent ADXL345 breakout board,
        which is much faster than I2C.
  my_motors.h/c: Contains my motor control schemes for h-bridge switching, etc. Use at your own risk, or you may
         blow your h-bridges!
  ping.c/h: A polling-based implementation for ultrasonic ping sensors (their may be an aysnchronous version in here as well)

Other files/folders:
  client/, clientII/: My Ubuntu/linux telemetry client. The ultimate goal with this was to have my little linux
  box run the heavier telemetry data crunching algorithms and views, while the bot just sent telemetry data and received
  motor command feedback.

  encoding/: a small base-64 encoder for ascii channels





