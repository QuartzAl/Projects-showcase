
<!-- PROJECT LOGO -->
<br />
<div align="center">

<h3 align="center">Nixie Tube I2C Driver</h3>

  <p align="center">
    very small
    <br />
    <!-- <a href="https://github.com/github_username/repo_name">View Demo</a> -->
  </p>
</div>



<!-- ABOUT THE PROJECT -->
<!-- ## About The Project -->

<!-- [![Product Name Screen Shot][product-screenshot]](https://example.com) -->

# Compact Nixie Tube Controller with CH32V003

This project details the development of a compact Nixie tube controller based on the CH32V003 RISC-V microcontroller. It's designed for driving IN-16 Nixie tubes and features a high-voltage flyback boost converter, I2C communication, and daisy-chaining capabilities.

The creation of this project was inspired mainly by cnlohr's [cnixxi](https://github.com/cnlohr/cnixxi) project. I redesigned the PCB, though the code didn't see much changes. It is mainly geared towards the I2C functionality of the driver and of course as a really really fun personal project as I had some nixie tubes I wanted to drive and wanted to make sure I could use them for whatever application I wanted. 

PCB design and schematics were originally made with [EasyEDA](https://oshwlab.com/stevmcg05/nixie-clock1) but I included gerbers and schematics in the root folder too. 


## Files

files for the firmware is located at: `/cnixxi/firmware`
there is optionbytes that need to be set to switch the ch32v003 pins to use the i2C functionality and the Wch-linkE has to be set up too

the `/i2c_slave` directory contain exmaple code copied from the ch32fun project to use the i2C functionality

the `/Nixie Clock Controller` directory is for the esp32 code I used for a demo for using these modules with some switches and an RTC


## Features

* **Compact Design:** The controller measures only 3x2.5cm, making it ideal for space-constrained applications.
* **CH32V003 Microcontroller:** Utilizes the low-cost, low-power CH32V003 RISC-V MCU.
* **IN-16 Nixie Tube Control:** Precisely controls IN-16 Nixie tubes via transistor switching.
* **200VDC Flyback Boost Converter:** Generates the necessary high voltage from a 5VDC input.
* **I2C Communication:** Enables easy integration with other devices via I2C for data input.
* **Daisy-Chain Capability:** Designed for modular expansion, allowing multiple controllers to be connected in series.
* **Custom PCB:** Utilizes a custom-designed PCB optimized for size and functionality, fabricated by JLCPCB.
* **C Programming:** Firmware developed in C for efficient control and performance.

## Hardware

* **Microcontroller:** CH32V003
* **Nixie Tube:** IN-16
* **PCB:** Custom designed, fabricated by JLCPCB

## Software

* **Programming Language:** C
* **Library Used:** [ch32fun by cnlohr](https://github.com/cnlohr/ch32fun)

## Usage

1.  Connect the controller to a 5VDC power supply.
2.  Connect the IN-16 Nixie tube to the designated pins.
3.  Use I2C to send data to the controller for display on the Nixie tube.
4.  For daisy-chaining, connect multiple controllers via the I2C bus.

## Future Improvements

* Improving I2C sometimes can bug out and doesn't work for some reason
* Adding example code for common I2C communication implementations.
* Adding more documentation for the I2C protocol used.

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bug fixes, feature requests, or general feedback.