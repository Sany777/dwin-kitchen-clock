### About
This project aims to create multifunction clock that can be part of ESPNOW's network of connected devices. The final device is based on a Dwin based screen and an ESP32. The project includes the firmware for the screen, which is located in the "dwin" folder, and the source code for creating a firmware for an ESP32. The project structure is standard for espressif projects, in this case VS code with an extension from Espressif was used. The screen firmware consists of a binary file and images. For flashing a dwin screen you can use UART interface or over air with esp32. The first flash of the esp32 is done via uart, then it can be flashed over the air (via a configuration server after connecting to the access point).

### ESPNOW and WiFi connection
```
idf.py menuconfig
```
Open the project configuration menu (`idf.py menuconfig`) to configure Wi-Fi and espnow conection data.


### Pages (device modes)
## Main
# show clock and weather information
The main purpose of the device is to show the time and weather information. This requires Internet access and a subscription to openweather.com services. Information about the temperature in the room is obtained from the bmp280 sensor. Information about the weather for a day is available by tapping on the upper right corner of the screen. The operation of the weather service requires installation the openweather.com key and the city name. This data can be set via the NETWORK setup item.

## Clock
# set time and change mode clock
This device is capable of showing the time, which can be set both manually and with the help of the SNTP service. Setting the time is available in the CLOCK menu item. The operating mode is set by tapping on the upper part of the screen, if the manual mode is selected, it is possible to set the time, which will be saved in a RTC module. If the mode of obtaining time from the network is selected, then the relative time offset for the given time zone must be set. In the absence of an Internet connection, the device will receive time from the RTC module of the screen, or from other similar devices included in the ESPNOW network. Setting the time is also possible with the help of the setting server, if you click the relevant button on the website. This device can be used as a timer and give a signal when the account ends. Access to this functionality is available through the main menu or through a tap in the lower right corner of the screen.

## Notification
# show and configure notification 
This device can notify, the setting is available through the NOTIFICATION item in the main menu. You can set up to 4 notifications for each day. A few minutes before the notification, a corresponding icon will be displayed on the main screen, next to the clock, and an alarm will sound at a certain time. Notification settings are also available through the configuration server (when AP is activated). Also, the alarm will sound when there is a new hour. The signal will not sound from 11:00 p.m. to 6:00 a.m. or if the device sounds are turned off (through the DEVICE STATUS item). 

## Network (settings)
# configure WiFi and api data
To set the network settings, select the NETWORK item in the main menu. The upper part of the screen shows the connection status. If you click on [SEARH SSID], the search screen for available networks will open and some information about them will be displayed. The list of networks scrolls. To select a network, double-click on it.
The SET COLOR menu item opens the text color setting screen. To save the color in memory, you must press [SET COLOR], otherwise the information will not be saved when restarting.

## Device status
# show and change some settings
The DEVICE STATUS menu item shows the status of the device and contains buttons on the left side that change the behavior of the device. The ESPNOW button is responsible for activating the ESPNOW service. The SECURITY button is used to hide the password from WIFI. If the SECURITY function is turn off old password will be clean. The SOUND button turns on sounds on the device. The SNTP button is used for change mode the time service. 

## Info
# show information about the device
The INFO menu item shows some information about the device, for example how a data for accessing the settings server.

## AP
# remote mode to configure the device
The AP menu item activates AP and shows the login data. The ESP32 page provides OTAs, notification settings, device operating modes, network data, and the function of establishing time. On the DWIN page you can send commands and flash dwin screen device. The FUN page speaks for itself.

## Devices
# show espnow devices
The DEVICES menu item opens a page that displays information about devices connected using ESPNOW technology. The ESPNOW allows devices based on modules from espressif to communicate, exchanging information (in this case about network settings, time and sensor data). This connection is established automatically, if espnow is activated. Data exchange is encrypted with a 16-byte key specified via menuconfig.