# STM32 BME280 Environmental Sensor Using I2C

An STM32 project demonstrating communication with a **BME280 environmental sensor over I2C** to measure temperature, humidity, and atmospheric pressure.

This project was developed as the sensor interface for my larger **STM32-Based Environmental Monitoring System**. The goal was to learn how an STM32 communicates with an external digital sensor by configuring device registers, reading raw sensor data, retrieving calibration parameters, and converting the raw measurements into usable environmental readings.

## Features

* Communicates with a BME280 environmental sensor using I2C
* Configures temperature, pressure, and humidity measurement registers
* Reads raw temperature, pressure, and humidity data
* Retrieves sensor-specific calibration parameters from the BME280
* Converts multi-byte sensor data using bit shifting and bitwise operations
* Applies compensation calculations to raw sensor measurements
* Outputs temperature, humidity, and pressure through UART
* Includes basic I2C read/write error checking

## I2C Communication

The BME280 communicates with the STM32 through the **I2C1 peripheral** using a 7-bit device address.

```c
#define BME280_addr (0x76 << 1)
```

During initialization, the STM32 writes configuration values to the BME280's humidity and temperature/pressure control registers using `HAL_I2C_Mem_Write()`.

```c
HAL_I2C_Mem_Write(&hi2c1, BME280_addr, 0xF2,
                  I2C_MEMADD_SIZE_8BIT,
                  &config_hum, 1, HAL_MAX_DELAY);

HAL_I2C_Mem_Write(&hi2c1, BME280_addr, 0xF4,
                  I2C_MEMADD_SIZE_8BIT,
                  &config_pres_temp, 1, HAL_MAX_DELAY);
```

This allowed me to practice communicating directly with sensor registers rather than relying on a prebuilt BME280 library.

## Reading Sensor Data

Eight bytes are read beginning at register `0xF7`:

```c
HAL_I2C_Mem_Read(&hi2c1, BME280_addr, 0xF7,
                 I2C_MEMADD_SIZE_8BIT,
                 sensor_data, 8, HAL_MAX_DELAY);
```

These bytes contain the raw:

* Pressure measurement
* Temperature measurement
* Humidity measurement

Bit shifting and bitwise OR operations are then used to reconstruct the individual raw sensor values.

## Calibration Parameters

The BME280 stores factory calibration parameters internally. These values must be read from the sensor and used to compensate the raw measurements.

The STM32 reads the calibration data from two register regions:

* `0x88` — temperature and pressure calibration data
* `0xE1` — additional humidity calibration data

The individual calibration coefficients are reconstructed from the returned bytes and stored for use in the compensation calculations.

## Sensor Compensation

Raw BME280 measurements cannot be directly interpreted as temperature, pressure, and humidity.

The project applies the BME280 compensation calculations using the sensor's calibration parameters to produce usable measurements.

The resulting values are converted into:

* **Temperature:** °C
* **Pressure:** hPa
* **Humidity:** %RH

## UART Output

The calculated environmental measurements are transmitted over **USART2 at 115200 baud** for monitoring and debugging.

Example output:

```text
Pressure: 1012.35 hPa
Temperature: 23.48 °C
Humidity: 45.72 %RH
```

`printf()` is redirected through USART2 using `HAL_UART_Transmit()`, allowing the sensor readings to be viewed through a serial terminal.

## Hardware

* STM32F303K8 / NUCLEO-F303K8
* BME280 temperature, humidity, and pressure sensor
* Breadboard
* Jumper wires

## Software

* C
* STM32CubeIDE
* STM32CubeMX
* STM32 HAL
* I2C
* UART

## Project Photo

<p align="center">
  <img width="3212" height="2155" alt="BME280_circuit" src="https://github.com/user-attachments/assets/2235889c-262b-446d-9efc-2bc613e99131" />
</p>

## UART Output

Sensor measurements were transmitted over UART for real-time monitoring and debugging.

<p align="center">
  <img width="337" height="542" alt="SS_serial_monitor" src="https://github.com/user-attachments/assets/b6e2b4c7-85d6-466c-b227-4f908883bf2b" />
</p>

## What I Learned

This project helped me develop a better understanding of:

* I2C communication
* Sensor register configuration
* Reading hardware datasheets and register maps
* Device addressing
* Multi-byte sensor data
* Bit shifting and bitwise operations
* Sensor calibration parameters
* Raw sensor data compensation
* UART-based debugging
* Error handling for peripheral communication

## Role in Final Project

This project serves as the environmental sensing component of my **STM32-Based Environmental Monitoring System**.

The BME280 provides temperature, humidity, and pressure measurements over I2C. The sensor interface developed here is later combined with the multiplexed 4-digit 7-segment display, dual 74HC595 shift registers, pushbutton controls, and UART monitoring to form the complete system.
