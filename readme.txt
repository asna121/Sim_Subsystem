Objective:
This software is to simulate NanoPower P31U. The NanoPower provides a number of measurement points that enables monitoring of the condition of the system.


file input:
1 file required put in USB disk.
file name should be "EPS_16s".
each file contain one corresponding parameter, and the size of parameter is 2 
byte. (Big Endian)

I2c address: 0x02

I2c communication example:

Register 0x08 and total 45 byte can return:

write transfer: 0x08
read transfer: 0x08 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x34 0x12
(If system request 10 byte.)