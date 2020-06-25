# DECIM8
This project is an exploration of programming a [Lego Mindstorms](https://www.lego.com/en-gb/themes/mindstorms/about) robot in C++17.

## The Robot
The robot is based on the fan-made KRAZ3 (instruction can be found [here](https://www.lego.com/en-gb/themes/mindstorms/buildarobot) on the offical Lego website, scroll down for fan models) with a few modifications; where the colour sensor is supposed to be, I have used the gyro sensor and the colour sensor is now attached to one of the arms.

Image to be included

The reason for the name "DECIM8" comes from the evil eyebrows; this robot wants to corrupt your soul and destroy the world!

## The Software Infrastructure
There are several ways to program a Lego Mindstorms robot but I chose to go with the custom Linux-based OS [ev3dev](https://www.ev3dev.org/).  Ev3dev defines its own API ontop of the Mindstorms hardware to offers support for a lot of programming languages, including C++.  C++ is supported with language binding from the [ev3dev-lang-cpp](https://github.com/ddemidov/ev3dev-lang-cpp) project and this is what this project uses; the header and source files for these binds are included in this project already and they are under the MIT License (see [here](https://github.com/ddemidov/ev3dev-lang-cpp/blob/master/LICENSE.md)) as well.

I develop on a 64-bit processor with Ubuntu and this is far removed from the hardware of the Lego Mindstorms intelligent brick which uses an ARM 9 processor.  Therefore, the best way I have found to compile code for the brick is with the docker image created by the masterminds behind ev3dev itself.  You can find instructions to setup and use the docker container [here](https://www.ev3dev.org/docs/tutorials/using-docker-to-cross-compile/).  Compile with the following command inside the container:
```
arm-linux-gnueabi-g++ -std=c++17 -pthread -o OUTPUT_FILE_NAME ev3dev.cpp SOURCE_FILES.cpp -Wall -Wextra
```
Hints and tips:
- Replace `OUTPUT_FILE_NAME` with something appropriate.  
- Replace `SOURCE_FILES.cpp` with all the cpp files needed.  
- Set any extra warning flags you might like.

## Summary of Dependencies
- Lego Mindstorms robot built to [KRAZ3](https://www.lego.com/cdn/cs/set/assets/blt5fb1f3a1f49f29b7/KRAZ3.pdf) instructions with two modification; gyro sensor where the colour sensor is and the colour sensor on the opposite arm to the touch sensor.
- [ev3dev](https://www.ev3dev.org/docs/getting-started/) set up and running on the Mindstorms intelligent brick.
- [ev3dev docker image for cross-compiling](https://www.ev3dev.org/docs/tutorials/using-docker-to-cross-compile/)
- A way to get files over from your development machine to the robot.  I use `ssh` to connect and `scp` to copy files over.
