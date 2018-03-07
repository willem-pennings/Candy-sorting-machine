# Candy sorting machine
This machine sorts M&Ms or Skittles by colour using a TCS34725 colour sensor. Two stepper motors actuate the moving parts (these are the "feed wheel" and the "carousel") while a modified micro servo motor mixes the pieces that enter the system.

# 3D models
The 3D models are provided in the STL format and are ready for 3D printing. The parts that I use are made of nylon (also referred to as PA-12 sometimes) and were printed in an SLS process.

# Source code
The machine uses two separate Arduino devices. One controls the sorting process (including sensors, actuators and logic) whereas the other only controls the RGB LEDs. It receives commands from the aforementioned (primary) Arduino device. There are three Arduino sketches available for download:

- The `sortingmachine` is the main sketch and contains all code necessary to run the sorting process.
- The `sortingmachine_calibration` sketch is much like the `sortingmachine` sketch, but is slightly modified such that the machine can be quickly calibrated using pre-sorted M&Ms or Skittles. For example, the carousel motor is disabled in this sketch and serial output is condensed for easy data exporting.
- The `sortingmachine_lighting` sketch controls the RGB leds. It communicates with the primary Arduino device through I2C (TWI) and should be connected accordingly. Part of this code was written by Bill Earl.

# Note
All files are provided as-is. I welcome any questions, but some effort will be needed to fully comprehend the code structure. Additionally, I appreciate it when others send in their derivative works. You can contact me through my website, https://willemmm.nl.