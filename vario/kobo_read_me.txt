Notes for using SkyBean with Kobo
=================================

0. The button will not work!!! Since button pin is only avalible RX pin for GPS.
1. Update firmware to skybean_kobo_experimental.ebin
2. Connect GND of GPS and Skybean 
3. Baud rate of GPS is 9600 (you can change baud in pc.cc:83)
4. Baud rate of SkyBean output is 115200 (you can change baud in pc.cc:83)
5. To enable serial output you need to edit saved config file (from chrome configurator) 
    change serial_output to value 2 ("serial_output":2) and then load it to configurator
    and save to skybean.

Good luck
