# Password-wakeup-device
usb device to simulate the wake up routine for a computer

# Materials
- ESP8266
- sparkfun pro micro
- 3V voltage regulator

## ESP8622
  The esp8266.i file contains the setup to achieve internet comunication and the connection to alexa, it creates a Access Point to get the initial conection and saves the credentials and password on the EEPROM it also expose the configuration pages on the port 3000 in case new Config is needed
## Pro Micro
  The promicro.ino file creates the virtual keyboard, and sets up the routin so that when it receives a string on the serial1 port it will type it
  