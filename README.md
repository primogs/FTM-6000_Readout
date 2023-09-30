# FTM-6000_Readout

The FTM-6000 is a mobile transceiver from the brand Yaesu. The device has a serial interface which can be used to transfered the configuration to a computer. For this you will need a adapter cable and a program. I recomend to use the program FTM-6000 Programmer ADMS-13 (C) from Yaesu.
![screenshot](/FTM-6000_Readout/documentation/images/radio.jpg)
FTM-6000_Readout instead is experimantal and comes without any warranty. However, it can also read the device configuration and in future may also write it back to the device. The purpose of the program is more of an educational nature.
![screenshot](/FTM-6000_Readout/documentation/images/screenshot.jpg)

## The physical connection ##
To connect the radio with the computer I use a USB to RS232 converter and an selfmade adapter. The serial interface of the radio is accessible over the 10 pin mini-DIN socket at the backside that is labeld with data. You can use some jumper wires if you do not have a mini-DIN plug that fits.
![screenshot](/FTM-6000_Readout/documentation/images/adapter.jpg)
![screenshot](/FTM-6000_Readout/documentation/images/radio_backside.jpg)
The mapping from the DE-9 connector to the mini-DIN connected is as follow:
| mini-DIN | DE-9 |
| --- | --- |
| 2 GND | 5 GND |
| 7 TXD | 2 RXD |
| 8 RXD | 3 TXD |
| 9 CTS | 8 CTS |
| 10 RTS | 7 RTS |
CTS and RTS is may not used and can be left out. See also chapter "
Connecting an external device" in the FTM-6000 advance manual.
![screenshot](/FTM-6000_Readout/documentation/images/pinout_minidin_socket.png)

The radio uses the ADM3202 IC to drive the RS-232 interface. Thus, the voltage swings is ±6 V. 
![screenshot](/FTM-6000_Readout/documentation/images/scope1.jpg)

## The serial interface ##
The serial interface has a baud rate of 38400, 8 data bits, no parity bit and one stop bit. The transmission to the computer is initiated at the radio. See chapter "Clone" in the FTM-6000 advance manual. If transmission is started, the radio will output the content of the internal EEPROM ( S24CM01C ) in packages. Each package has 131 byte and must be answered with an ACK (ASCII character 6). The first two bytes of the package are the memory address within the EEPROM followed by 128 byte payload and a 1 byte sum. The sum is realy just the sum of all bytes in the package execept the byte of the sum itself. Leading bits that not fit into the byte get cut off. The first package contains AH079$ which maybe the codename for the radio? The package with the address 128 and 1024 is left out, probably because it contains device specific calibration data.
![screenshot](/FTM-6000_Readout/documentation/images/scope2.jpg)
The EEPROM has two "pages" of 64 K-byte each. Thus, the address is reset to zero if the second page send.
