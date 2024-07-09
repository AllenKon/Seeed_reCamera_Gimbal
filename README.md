# reCamera Gimbal Programmable Development Kit V1.0

## Introduction

Welcome to reCamera Gimbal Programmable Development Kit. We provide sample code to debug the gimbal motor's operation and status output. Additionally, we offer program entry points to drive the gimbal motor using RS485, enabling precise control and functionality.

## Motor

MS3008 Motor          |  MS3008 Motor Parameters
:-------------------------:|:-------------------------:
![MS3008 Motor](https://raw.githubusercontent.com/AllenKon/Seeed_Motor_RS485/main/MS_Motor/Pic/MS3008_Motor.jpg)  |  ![MS3008 Motor Parameters](https://raw.githubusercontent.com/AllenKon/Seeed_Motor_RS485/main/MS_Motor/Pic/MS3008_Parameters.png)

Featuring a high-performance 32-bit MCU, the MS3008 allows seamless switching between torque, speed, and position control modes. Constructed with high-quality silicon steel sheets and high-temperature-resistant strong magnetic permanent magnets, it offers superior performance. The multi-level flat outer rotor design provides higher torque, making it ideal for space-constrained applications. The brushless structure, combined with high-performance bearings, ensures smoother rotation and longer lifespan. With high-precision, low-temperature-drift alloy resistors for current sampling, it guarantees precise current control. The full N-channel MOSFET three-phase full-bridge drive delivers low internal resistance and high current capacity. It supports RS485 communication for reliable and efficient data transmission.

## USB-RS485 Analyzer

If you want a RS485 Bus Analyzer to debug your RS485 Bus, this [USB-RS485 Analyzer](https://www.seeedstudio.com/USB-TO-RS232--RS485--TTL-Industrial-Isolated-Converter-p-3231.html) is recommended.

![MS3008 Motor](https://media-cdn.seeedstudio.com/media/catalog/product/cache/bb49d3ec4ee05b6f018e93f896b8a25d/h/t/httpsstatics3.seeedstudio.comseeedfile2018-12bazaar1004130_usbtors232485ttl01.jpg)

## Upper computer-Motor Command

### RS485 Bus Parameters

**Baud Rate (Normal Mode, Single Motor Command):**
- 9600bps
- 19200bps
- 38400bps
- 57600bps
- 115200bps (default)
- 230400bps
- 460800bps
- 1Mbps
- 2Mbps
- 4Mbps

**Baud Rate (Broadcast Mode, Multi-Motor Command):**
- 1Mbps
- 2Mbps
- 4Mbps

**Data Bits:** 8  
**Parity:** None  
**Stop Bits:** 1

### Motor ID Set

Up to 32 motors can be connected on the same bus (depending on bus load). To prevent bus conflicts, each motor must be assigned a unique ID ranging from 1 to 32.

![Connect of Devices](https://raw.githubusercontent.com/AllenKon/Seeed_Motor_RS485/main/MS_Motor/Pic/Connect.png)

ID changes can be made using MS_Motor/LK motor tool V2.35.exe. Connect the PC (Windows) to the USB-RS485 module, with A, B, and GND of the USB-RS485 module connected to A, B, and V- of the motor. Additionally, connect the positive and negative terminals of a 12V power supply to V+ and V- of the motor to enable parameter changes. For further instructions on using LK motor tool V2.35.exe, refer to MS_Motor/Upper_monitor_motor_debuging_instruction.pdf.

The upper computer sends this command to control the motor position (multi-turn angle), the control value angleControl is of type int64_t, corresponding to the actual position of 0.01degree/LSB. The control value angleControl is of type int64_t, which corresponds to the actual position of 0.01degree/LSB, i.e. 36000 represents 360°, and the direction of rotation of the motor is determined by the difference between the target position and the current position.

----

This software is written by [Seeed Technology Inc.](http://www.seeed.cc) and is licensed under [The MIT License](http://opensource.org/licenses/mit-license.php). Check License.txt/LICENSE for the details of MIT license.<br>

Contributing to this software is warmly welcomed. You can do this basically by<br>
[forking](https://help.github.com/articles/fork-a-repo), committing modifications and then [pulling requests](https://help.github.com/articles/using-pull-requests) (follow the links above<br>
for operating guide). Adding change log and your contact into file header is encouraged.<br>
Thanks for your contribution.

Seeed is a hardware innovation platform for makers to grow inspirations into differentiating products. By working closely with technology providers of all scale, Seeed provides accessible technologies with quality, speed and supply chain knowledge. When prototypes are ready to iterate, Seeed helps productize 1 to 1,000 pcs using in-house engineering, supply chain management and agile manufacture forces. Seeed also team up with incubators, Chinese tech ecosystem, investors and distribution channels to portal Maker startups beyond.

[![Analytics](https://ga-beacon.appspot.com/UA-46589105-3/NFC_Tag_M24LR6E)](https://github.com/igrigorik/ga-beacon)
