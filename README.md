# reCamera Gimbal Programmable Development Kit V1.0

## Introduction

Welcome to reCamera Gimbal Programmable Development Kit. We provide sample code to debug the gimbal motor's operation and status output. We offer program entry points to drive the gimbal motor using RS485, enabling precise control and functionality.

<p float="left">
  <img src="https://raw.githubusercontent.com/AllenKon/Seeed_Motor_RS485/main/MS_Motor/Pic/reCamera_Font_View.jpg" width="300" />
</p>

## Gimbal Structure & Assembly

You can download the STL file of this gimbal for 3D printing and assemble it. (Url: https://makerworld.com/zh/models/536713) 

### Assembly

None.(Waiting to write)


## Instructions for using

### Motor ID Set

![Connect of Devices](https://raw.githubusercontent.com/AllenKon/Seeed_Motor_RS485/main/MS_Motor/Pic/Connect.png)

ID changes can be made using MS_Motor/LK motor tool V2.35.exe. Connect the PC (Windows) to the USB-RS485 module, with A, B, and GND of the USB-RS485 module connected to A, B, and V- of the motor. Connect the positive and negative terminals of a 12V power supply to V+ and V- of the motor to enable parameter changes. For further instructions on using LK motor tool V2.35.exe, refer to MS_Motor/Upper_monitor_motor_debuging_instruction.pdf. 

Only two motors are used in this head, so it is sufficient to set the IDs of the two motors to 01 and 02 respectively.

### USB-RS485 Analyzer

If you want a RS485 Bus Analyzer to debug your RS485 Bus, this [USB-RS485 Analyzer](https://www.seeedstudio.com/USB-TO-RS232--RS485--TTL-Industrial-Isolated-Converter-p-3231.html) is recommended.

![MS3008 Motor](https://media-cdn.seeedstudio.com/media/catalog/product/cache/bb49d3ec4ee05b6f018e93f896b8a25d/h/t/httpsstatics3.seeedstudio.comseeedfile2018-12bazaar1004130_usbtors232485ttl01.jpg)



## Hardware Overview

![Hardware Overview](https://raw.githubusercontent.com/AllenKon/Seeed_Motor_RS485/main/MS_Motor/Pic/Hardware_Overview.png)

The actual internal wiring diagram is as follows.

![Hardware Overview](https://raw.githubusercontent.com/AllenKon/Seeed_Motor_RS485/main/MS_Motor/Pic/Hardware_Wiring_Overview.png)

You can use the Debugging Port connections to control the motor and communicate with the reCamera via the upper monitor. Alternatively, you can control the motor directly through the reCamera.

![Hardware Overview](https://raw.githubusercontent.com/AllenKon/Seeed_Motor_RS485/main/MS_Motor/Pic/Upper_Wiring_Overview.png)

## Instructions for using

### Upper Monitor Control

#### Python

Open a terminal in the root directory while making sure Python is installed.

```
python Deploy_Python.py
```


----

This software is written by [Seeed Technology Inc.](http://www.seeed.cc) and is licensed under [The MIT License](http://opensource.org/licenses/mit-license.php). Check License.txt/LICENSE for the details of MIT license.<br>

Contributing to this software is warmly welcomed. You can do this basically by<br>
[forking](https://help.github.com/articles/fork-a-repo), committing modifications and then [pulling requests](https://help.github.com/articles/using-pull-requests) (follow the links above<br>
for operating guide). Adding change log and your contact into file header is encouraged.<br>
Thanks for your contribution.

Seeed is a hardware innovation platform for makers to grow inspirations into differentiating products. By working closely with technology providers of all scale, Seeed provides accessible technologies with quality, speed and supply chain knowledge. When prototypes are ready to iterate, Seeed helps productize 1 to 1,000 pcs using in-house engineering, supply chain management and agile manufacture forces. Seeed also team up with incubators, Chinese tech ecosystem, investors and distribution channels to portal Maker startups beyond.

[![Analytics](https://ga-beacon.appspot.com/UA-46589105-3/NFC_Tag_M24LR6E)](https://github.com/igrigorik/ga-beacon)
