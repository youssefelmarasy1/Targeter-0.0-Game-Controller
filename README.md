# Targeter Game Controller

The Targeter Game Controller is a project implemented on the ATMega328P microcontroller, which communicates with a game using UART (Universal Asynchronous Receiver-Transmitter). The controller is equipped with an HW-504 joystick to provide input to the game, allowing players to navigate and interact within the gaming environment.

## Features

- Utilizes the ATMega328P microcontroller for handling game controls.
- Communicates with the game using UART for real-time interaction.
- Incorporates an HW-504 joystick for precise and dynamic control.
- Displays essential game information on an attached LCD screen.
- Supports features such as button status, joystick position, ammunition count, and player score.

## Hardware Requirements

- ATMega328P Microcontroller
- HW-504 Joystick
- UART Communication
- LCD Display

## Features

- **Joystick Input:** Reads analog signals from the joystick's x and y axes to determine the position.
- **Button Status:** Monitors the status of a pushbutton to indicate whether it is pressed or not.
- **UART Communication:** Establishes communication with the game console using UART for sending and receiving data.
- **LCD Display:** Displays relevant information such as the game name, player handle, ammunition, and score on an LCD screen.

## Hardware Configuration

- Microcontroller: ATmega328P
- Crystal Frequency: 14.7456 MHz
- Joystick: Analog input from ADC0 and ADC1
- Pushbutton: Connected to PB3

## Fuse Bits Configuration

Fuse bits are configured to work with the external crystal.

## Code Structure

- **defines.h:** Header file containing necessary definitions.
- **hd44780.h and lcd.h:** Files for interfacing with the LCD.
- **uart.h:** File for UART communication.
- **newmain3.c:** Main source code implementing joystick controller functionality.

## Building and Flashing

1. Configure fuse bits to match the hardware specifications.
2. Build the project using your preferred toolchain (e.g., AVR-GCC).
3. Flash the compiled binary onto the ATmega328P microcontroller.

## Getting Started

1. Clone the repository.
2. Set up the ATMega328P microcontroller and connect the HW-504 joystick, UART communication, and LCD display as per the provided circuit.
3. Compile and upload the code to the ATMega328P microcontroller.
4. Run the system and observe real-time communication and control with the connected game.

## Usage

- Navigate the game environment using the HW-504 joystick.
- Monitor ammunition count, player score, and other game information on the attached LCD screen.
- Experience responsive and accurate control through UART communication.

## Author

Youssef Elmarasy (GitHub: [youssefelmarasy1](https://github.com/youssefelmarasy1))

## License

This project is licensed under the [Apache 2.0 License](LICENSE).
