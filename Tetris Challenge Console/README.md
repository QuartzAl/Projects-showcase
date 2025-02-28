# Arduino Nano Tetris Game Console

This project presents a Tetris game console built using an Arduino Nano, MAX7219 dot matrix display, TM1637 seven-segment module, and tactile switches. It features a modular, daisy-chainable design for multiplayer functionality and various game modes.

**(Further project description is available in Tetris-UAS-Mikro)**

## Features

* **Arduino Nano Based:** Utilizes the Arduino Nano for game logic and control.
* **MAX7219 Dot Matrix Display:** Displays the Tetris game board and falling pieces.
* **TM1637 Seven-Segment Module:** Displays the score and game status.
* **Tactile Switches:** Provides user input for game control (movement, rotation, etc.).
* **Modular Design:** Features a master module and two player modules, allowing for easy expansion and customization.
* **Daisy-Chain Capability:** Enables multiplayer Tetris gameplay by connecting multiple player modules to the master module.
* **Multiplayer Support:** Supports two-player Tetris gameplay.
* **Multiple Challenge Modes:** Implements different game modes for varied gameplay experiences.

## Hardware

* **Microcontroller:** Arduino Nano (for both master and slaves all use arduino nanos)
* **Display:** MAX7219 Dot Matrix Display
* **Score Display:** TM1637 Seven-Segment Module
* **Input:** Tactile Switches
* **Modules:** Master Module, Player Modules

## Software

* **Programming Language:** Arduino (C++)
* **Functionality:**
    * Game logic (Tetris algorithm).
    * Display control for the MAX7219 and TM1637.
    * Input handling from tactile switches.
    * Communication between master and player modules.
    * Implementation of different game modes.

## Usage

1.  Connect the modules as described in the schematics.
2.  Upload the Arduino code to the master module .
3.  Power the console.
4.  Use the tactile switches to control the Tetris game.
5.  For multiplayer, connect the player modules to the master module.

## Future Improvements

* Adding sound effects.
* Implementing more game modes.
* Improving the user interface.
* Optimizing the code for performance.
* Adding a high score save feature.
* Creating a better enclosure.

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bug fixes, feature requests, or general feedback.
