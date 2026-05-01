# Automatic-Solar-Tracker
Zac Cresencio, Automatic Solar Tracker for EE1301 Final Project

< GENERAL DESCRIPTION >
This is a micro solar panel with automatic and manual control, and IoT data collection capabilities. The automatic tracking is achieved through the use of PDV-P8103 light-dependent resistors and comparing the outputs to determine where the brightest light is relative to the Buachois 0.3W 5V 60mA solar panel. The panel is mounted to a SG90s micro servo to rotate to face the light. To allow for manual control, a 10K Ω potentiometer and a momentary NO push button to toggle between auto and manual modes was added. An INA219 power monitor was intended to be used to monitor the solar panel output, but due to technical issues, a voltage divider using 2x10K Ω resistors was used in place to roughly measure the voltage and power output, and estimate the current flow through a resistor. Lastly, the Particle Photon 2 was used as the main development module and microcontroller to allow IoT communication, sending data of the voltage, current, and power from the solar panel and displaying it through a chart, as well as allowing for switching of modes and servo position through the web.


< PROJECT FILES >
SOLAR_TRACKER_MAIN.cpp – main code to run on Particle Photon 2. Contains all code needed for sensors and actuators, as well as cloud communication.
solar_tracker_data.html – contains html code to display data on web, with CSS and JS elements
solar_tracker_schematic.pdf – contains wiring diagram / schematic of breadboard and perfboard.
solar_tracker_BOM.txt – contains bill of materials and list of all parts used in construction
