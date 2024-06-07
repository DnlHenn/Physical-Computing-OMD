# Physical-Computing-OMD
Project OMD (Occupancy Measurement Device)

Functionality:
Initial measurement of the distance from the sensor to the nearest object
Every time someone passes the sensors, the difference between the initial measurement and the new measurement is calculated
If somebody passes the first sensor first and second sensor after that: The counter adds one to the current measurement
If somebody passes the second sensor first and first sensor after that: The counter reduces the current measurement by one
You can adjust the maximum capacity of the room with the buttons

Used Hardware:
- Ultrasonic Sensor - HC-SR04 (Generic) (x2)
- Buzzer v1.2 (x1)
- Grove-LCD RGB Backlight v4.0 (x1)
- Button (x3)
- Arduino Mega 2560 Rev3 (x1)
