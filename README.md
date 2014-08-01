##SensorWebEnergy

_In collaboration with CNR-Ibimet Firenza and Università degli Studi di Firenze_
http://www.ibionet.eu/servizi_monitoraggio.php

###Description

The remote monitoring is based on a platform SensorWebEnergy based on an architecture of the type "open" or able to be modified and adapted to different requirements of monitoring. Each plant has in fact different characteristics, eg.: One or two boilers, producing heat only or combined production of heat and power (CHP), single or multiple users, etc. ..
The device has been configured to send the data via the SIM card to the server every N minutes (where N is configured at the server side). The data are formatted in a string containing the following information:


![alt tag](https://raw.github.com/loweherz/SensorWebEnergy/master/images/string.jpg)


###Hardware

The system is based on Seeduino Stalker, which it can be programmed easily via the Arduino IDE.


![alt tag](https://raw.github.com/loweherz/SensorWebEnergy/master/images/schema.png)


#####Sensors

* DHT22 (link),

* DIGITAL TEMPERATURE WATERPROOF (link), which use ONE WIRE

* XL MAX SONAR (link), reading from RS232

* CT SENSOR (link)

* CONTATTI REED

* FLUSSOMETRO


###Sketch

The software has been optimized to run on devices with limited memory. The sketch makes it possible to perform the reading of the sensors mentioned above and reading them to the server where the information will be saved.

###Web application

http://www.ibionet.eu/servizi_monitoraggio.php

by Alessandro Zaldei
