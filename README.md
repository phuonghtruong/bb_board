# BeagleBone Black Project

This repository contains code and resources for a project utilizing the BeagleBone Black single-board computer. The project aims to demonstrate various functionalities and applications of the BeagleBone Black, including interfacing with sensors, controlling actuators, and implementing communication protocols.
## Table of Contents

- [Getting Started](#getting-started)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Installation](#installation)
- [Usage](#usage)

Getting Started
----------------
To get started with this project, you will need a BeagleBone Black board and the necessary hardware components as specified in the Hardware Requirements section.
## Hardware Requirements
- BeagleBone Black board
- USB cable for power and programming
- Breadboard and jumper wires
- Bme280 sensor
- USB Camera
- Power Supply (5V 2A recommended) in case connecting usb power is not sufficient
## Software Requirements
- A Linux computer (Ubuntu recommended)
- GCC compiler
- Make utility
- BeagleBone Black Debian image
- Python 3.x for Python applications
- Required Python libraries (listed in requirements.txt)
## Installation
1. Clone this repository to your local machine:
2. Set up the BeagleBone Black with the Debian image and connect it to your computer.
3. Install the required Python libraries for python applications
    ```
    pip install -r requirements.txt
    ```

4. Compile any C/C++ code if necessary using the provided Makefile:
    ```
    make APP=camera -DEBUG=1
    make APP=main -DEBUG=1
    make APP=camera
    ```
## Usage
1. Connect the Bme280 sensor and USB camera to the BeagleBone Black as per the wiring diagram provided in the documentation.
2. Load application binary into beaglebone black using:
    ```
    sudo ./load_app.sh main
    ```
3. SSH to beaglebone black using
4. Run the application:
```    sudo ./bb_camera_app
```
   ./bb_camera_app
## License
This project is licensed under the MIT License. See the LICENSE file for details.
