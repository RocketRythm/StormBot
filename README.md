# StormBot
A robot for driving down storm drains to get video footage of cats, snakes, etc.

This was originally developed using only Arduino's with motor controller shields, XBee shields, and some pots.  The new plan is to use the Arduino and motor controller shields on the bot, but connect the Arduino to a Raspberry Pi via USB on the bot.  The Raspberry Pi (the mission computer) will have a USB cable to power and send requested speeds to the Arduino.  Another Raspberry Pi will act as a WiFi router and read inputs from an XBox 360 controller. This input from the controller will be sent via USB packets to the Pi on the bot.  This should make a drivable bot over WiFi.
The next step will be to add a camer to the mission computer and determine some way to get video over IP back to the controller computer where it can be displayed.
I found an example of comminicating between the Raspberry Pi and the Arduino over USB cable. I verified that it works.

Links:
* Raspberry Pi to Arudino comms via USB cable (that also powers the Arduino): https://www.aranacorp.com/en/serial-communication-between-raspberry-pi-and-arduino/
* Using XBox 360 controller on Raspberry Pi: https://github.com/FRC4564/Xbox
* Raspberry Pi with camera and streaming over the network: https://www.instructables.com/IP-Camera-Using-the-Raspberry-Pi-Zero-Home-Surveil/
