**ESP32 CAN Bus Display Campro Cps**
This project utilizes an ESP32 microcontroller to read and display data from a CAN (Controller Area Network) bus. It is designed to visualize various vehicle parameters such as RPM, temperature, speed, and additional data through an OLED display.

Features
CAN Bus Communication: Reads messages from the CAN bus with specified IDs.
OLED Display: Utilizes the U8g2 library to render data on a 128x64 pixel OLED display.
Data Visualization: Displays RPM, temperature, speed, and additional unknown parameters in a user-friendly format.
Hexadecimal Data Display: Shows raw data from specific CAN messages in hexadecimal format.
Timeout Handling: Clears the display if no data is received within a specified timeout interval.
Components Required
ESP32 Development Board
CAN Bus Module (compatible with ESP32)
128x64 OLED Display (SH1106 or similar)
Jumper wires and breadboard (for connections)
Installation
Clone this repository to your local machine.
Open the project in the Arduino IDE.
Install the required libraries:
U8g2lib
ESP32 CAN
Connect the hardware components as per the pin configuration in the code.
Upload the code to your ESP32 board.
Usage
The display will show the logo for 3 seconds upon startup.
After that, it will continuously update the display with the latest data received from the CAN bus.
If no data is received for 1 minute, the display will clear.
Code Overview
setup(): Initializes the serial communication, OLED display, and CAN bus.
loop(): Continuously checks for incoming CAN messages and updates the display.
processCANMessage(): Processes incoming CAN messages based on their IDs and updates the relevant variables.
draw(): Renders the current values of RPM, temperature, speed, and additional data on the OLED display.
displayDataHex(): Displays the raw data from specific CAN messages in hexadecimal format.
License
This project is licensed under the MIT License. Feel free to modify and use the code as per your requirements.

