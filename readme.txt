Objective:
This software is to simulate INMS interface board. This board is to measure the
 current of 3.3v and 5v bus on interface board and the temperature of INMS and 
interface board itself.


file input:
4 file required put in USB disk.
file name should be "IFB_5V" "IFB_3_3V" "IFB_INMS" and "IFB_IFB".
each file contain one corresponding parameter, and the size of parameter is 2 
byte. (Big Endian)

I2c address: 0x6d

I2c communication example:

write transfer: 0xd0
read transfer: 0x12 0x13 0xff 0xd0 (if system request 4 byte.)