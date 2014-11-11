# Roach 2 hardware monitoring

The r2hwmond application monitors the roach sensors via the **/sys/class/hwmon/** interface using the lm-sensors library.

Katcp is used to provide katcp logs/commands to take action once an alarm is triggered. Currently action is taken on the FPGA and PowerPC temperature alarms.
* FPGA alarm : Unprogram/unload the FPGA
* PowerPC alarm: Power the Roach down

Under typical usage the r2hwmond application will be started from within tcpborphserver:
**?job process r2hwmond exec://r2hwmond**

The application distinguish between a scan interval and an update interval:
* scan interval : check for alarms
* update interval: update sensor status (including value) information
    
The above intervals can also be set via the command line using the *-s* (scanTime) and *-u* (updateTime) switches respectively. 
    
The following files/modules were re-used from lm-sensors/prog/sendord:
* chips.c/h
* sense.c/h
* sensord.h

Dependencies:
* lm-sensors 
  * http://lm-sensors.org/
* katcp library
  * https://casper.berkeley.edu/wiki/KATCP
  * https://github.com/ska-sa/katcp_devel
