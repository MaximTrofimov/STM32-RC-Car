# STM32-RC-Car
RC car with manual and autonomous modes, dual STM32 wireless communication (remote and car PCBs).

## Demo Videos

**Manual Mode**

<img width="426" height="240" alt="manual" src="https://github.com/user-attachments/assets/46da7b24-2646-4f55-a0b9-78148cf8d852" />




**Autonomous Mode**

<img width="426" height="240" alt="autonomous" src="https://github.com/user-attachments/assets/2d1d2ee3-6ee6-47ba-afe1-4426bb8ba589" />




## Photos

**RC Car and Remote**

<img width="500" src="https://github.com/user-attachments/assets/8ed064cd-994c-405b-8207-178f904d99b8" />




**Remote and Car PCBs**

<img width="250" src="https://github.com/user-attachments/assets/5dae2fcd-ede2-433f-99eb-eca6f3342112" />

## Overview
- Designed two custom PCBs (remote and car) for a wirelessly-controlled RC car
- Implemented wireless communication between two STM32 microcontrollers using embedded firmware (HAL library)
- Developed manual and autonomous modes

## Hardware
- STM32F446RE (x2 — one in car, one in remote)
- NucleoF446RE (x2 - prototyping)
- NRF24L01+PA+LNA wireless transceiver modules (x2 — one for car, one for remote)
- HC-SR04 ultrasonic sensor (x3 - front, left, right)
- SG90 servo motor (steering)
- TB6612FNG dual H-bridge motor driver (RWD)
- TT dual DC motor 3-6V (x2)
- KY-023 dual-axis analog joystick (remote)
- Custom PCBs (x2 - one for car, one for remote)
- 2S Lipo battery (car)
- 1s Li Ion battery (remote)

## Repo Structure
```
Firmware/
  car_mcu/       - STM32 embedded firmware for the car MCU
  remote_mcu/    - STM32 embedded firmware for the remote MCU
Hardware/
  car_pcb/       - Car PCB, schematic, BOM
  remote_pcb/    - Remote PCB, schematic, BOM
Mechanical/       - Wheels and steering servo enclosure CAD files (STEP)
```

## How It Works

**Manual Mode**
The X and Y coordinates of the joystick position are read into a two variable array, VR. The array is then transmitted alongside each packet using the remote PCB's NRF24L01 module, and received at the car PCB's NRF24L01 module. The DC Motors are controlled using the TB6612FNG where a maximum joystick value results in fully duty cycle, a neutral results in zero duty cycle, and a minimum results in maximum duty cycle spinning the motors the other way. The SG90 steering is controlled via a timer for PWM. If the joystick is extended to the far right or far left, the NRF24L01 sends a larger joystick value and therefore the pulse is set to high for longer. Both throttle and steering respond to the specific displacement of the joystick from its absolute center, giving it the ability to control speed, and steering degree. The firmware dictates what part of the packet is responsible for motors, what part for the steering angle, and what part for the mode, which is triggered via an interrupt. 

**Autonomous Mode**
If the joystick button is pressed, the mode is toggled to autonomous. Motors begin by running on half duty cycle and steering is set to a constant of about 20 degrees. The front, left, and right ultrasonic sensors notify the microcontroller when an obstacle is a certain distance away. Once this happens the car stops, checks its sides, goes back, and turns to the side where there is more free space. 

## Firmware Configuration

**Car MCU**
<p float="left">
  <img width="330" height="290" alt="image" src="https://github.com/user-attachments/assets/eea8dfe5-f855-4c4b-911a-75bf28a5ad72" />
  <img width="380" height="290" alt="image" src="https://github.com/user-attachments/assets/3d7787e1-350d-4479-9a5c-52129684c758" />
</p>

**Remote MCU**
<p float="left">
  <img width="330" height="290" alt="image" src="https://github.com/user-attachments/assets/5ed8ac23-c24f-4cfe-a3a4-89149bfc77a6" />
  <img width="380" height="290" alt="image" src="https://github.com/user-attachments/assets/3d7787e1-350d-4479-9a5c-52129684c758" />
</p>

## PCB Design

**Car PCB**
<p float="left">
  <img width="550" height="290" alt="image" src="https://github.com/user-attachments/assets/a40c2313-e5f9-4964-8d71-85f389cd710c" />
  <img width="420" height="290" alt="image" src="https://github.com/user-attachments/assets/be10933c-2023-479a-8d33-52003048a8f0" />
</p>

**Remote PCB**
<p float="left">
  <img width="550" height="290" alt="image" src="https://github.com/user-attachments/assets/2f732f9c-7d03-4907-a121-57011ae44a8a" />
  <img width="450" height="290" alt="image" src="https://github.com/user-attachments/assets/f993a68f-423e-4bb0-b3cd-8a1a192fa311" />
</p>

## Power

**Car PCB**
- 2S Lipo -> 3.3V (500 mA rating, STM32, NRF24L01, TB6612FNG logic): TPS62152RGTR, reference schematic on Texas Instruments Power Designer.
- 2S Lipo -> 5V (2A rating, TB6612FNG motor power, SG90, HCSR04): TPS82130SILR, reference schematic on Texas Instruments Power Designer.

**Remote PCB**
- 1S Li-Ion -> 3.3V (600 mA rating, STM32, KY-023, NRF24L01): AP2112K-3.3TRG1, reference schematic on datasheet. Used LDO due to lower voltage difference. 

## Credits
- Shawn Hymel STM32 Tutorial Series [https://www.youtube.com/watch?v=hyZS2p1tW-g&list=PLEBQazB0HUyRYuzfi4clXsKUSgorErmBv]
- Phil's Lab KiCad Tutorial [https://www.youtube.com/watch?v=aVUqaB0IMh4]
- NRF24L01 Tutorial Series [https://www.youtube.com/watch?v=mB7LsiscM78&t=996s]
- HCSR04 Tutorial Article [https://controllerstech.com/hcsr04-ultrasonic-sensor-and-stm32/]
- SG90 Tutorial Article [https://controllerstech.com/servo-motor-with-stm32/]
- TB6612FNG Tutorial Video [https://www.youtube.com/watch?v=zWYYvtU8Wt4]
- Joystick Module Tutorial Article [https://controllerstech.com/joystick-and-stm32/]
- Texas Instruments 2S Lipo -> 3.3V Buck Converter Circuit [https://webench.ti.com/power-designer/switching-regulator/customize/5?noparams=0]
- Texas Instruments 2S Lipo -> 5V Buck Converter Circuit [https://webench.ti.com/power-designer/switching-regulator/customize/6?noparams=0]
