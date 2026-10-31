# 4WD Robotic Car with Obstacle Avoidance

A four-wheel-drive robotic chassis built using an ESP32, DC geared motors, and ultrasonic sensors. The robot can navigate around obstacles and supports line-following applications in structured environments such as plant nurseries.

## Project Overview

This project is a prototype for a mobile robot that can navigate through controlled environments. It uses three front-facing ultrasonic sensors to detect obstacles and make movement decisions.

The robot also includes IR sensors for line following and additional sensors for environmental monitoring and future motion analysis.

## Features

- **Obstacle avoidance:** Three ultrasonic sensors continuously measure the distance to nearby obstacles. The robot uses these measurements to avoid collisions while moving.
- **Line following:** IR sensors support line-following applications, such as navigating between rows of plants in a nursery.
- **Environmental monitoring:** A DHT22 sensor measures surrounding temperature and humidity.
- **Motion sensing:** An MPU6050 IMU provides orientation and motion measurements.
- **Wheel encoders:** Encoder modules are installed for future odometry and motion analysis.
- **Four-wheel drive:** Four DC geared motors provide movement through a motor driver.

## Hardware Components

| Component | Quantity |
|---|---:|
| ESP32 development board | 1 |
| Ultrasonic sensors | 3 |
| IR sensors | 2 |
| DC geared motors | 4 |
| Motor driver | 1 |
| DHT22 temperature and humidity sensor | 1 |
| MPU6050 IMU | 1 |
| Wheel encoder modules | 2 |
| Buck converter | 1 |
| 11.1V LiPo battery | 1 |
| Breadboard and jumper wires | As required |

## System Architecture

The ESP32 acts as the main controller, receiving data from the sensors and controlling the motors through the motor driver.

The power system uses a rechargeable LiPo battery and a buck converter to supply the required voltages to the electronics.

The ultrasonic sensors are used for obstacle avoidance, while the IR sensors support line following. The IMU and wheel encoders provide additional data for future navigation and motion analysis.

## Demonstration

A video demonstrating the robot's obstacle avoidance will be added here.

## Future Improvements

- Implement wheel encoder-based odometry.
- Integrate IMU data for improved motion estimation.
- Develop more advanced navigation and path-planning algorithms.
- Integrate plant detection and monitoring capabilities.

## Project Status

Working prototype. Obstacle avoidance has been implemented and demonstrated. Additional sensing and navigation capabilities are intended for future development.

## Technologies

ESP32 · Embedded Systems · Robotics · Sensor Integration · Obstacle Avoidance · Line Following
