## clock based on ESP32 and HMI

This project aims to create multifunction clock that can be part of ESPNOW's network.  The project includes the firmware for the screen, which is located in the "dwin" folder, and the source code for creating a firmware for the ESP32 MCU. The screens firmware consists of a binary file and images. The first flash of the esp32 is done via wires, then it can be flashed over the air (via a configuration server after connecting to special AP).

## Pages (device modes)

### Main

![Image alt](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/main_screen.jpg)
###### main screen

![Image alt](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/weather_details_screen.jpg)
###### weather details screen

###### show clock and weather information
The main purpose of the device is to show the time and weather information. This requires Internet access and a subscription to openweather.com services. Information about the temperature in the room is obtained from the bmp280 sensor. Information about the weather for a day is available by tapping on the upper right corner of the screen. The operation of the weather service requires installation the openweather.com key and the city name. This data can be set via the NETWORK setup item.

### Clock

![Image alt](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/clock_screen.jpg)
######   clock screen (the mode selection button and changing the clock shift area are marked)

##### set time and change mode clock
This device is capable of displaying the time, which can be set manually or via the SNTP service. Setting the time is available in the CLOCK menu item. The tapping on the top of a screen toggle mode getting time. If manual mode is selected, you can set the time that will be stored in the RTC module. If the mode of obtaining time from the network is selected, then the relative time offset for the given time zone must be set. In the absence of an Internet connection, the device will receive time from the RTC module of the screen, or from other similar devices included in the ESPNOW network. You can also set the time using the settings server by clicking the appropriate button on the website. This device can be used as a timer and signal when the count is over. These functions are available through the main menu or through a tap in the lower right corner of the screen..

### Notification

![Image alt](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/notification_screen.jpg)
######  notification screen 

##### show and configure notification 
You can use notifications, the settings are available in the menu NOTIFICATIONS item. You can set up to 4 notifications for each day. A few minutes before the notification, a corresponding icon will be displayed on the main screen and an alarm will sound at selected time. Notification settings are also available through the configuration server (when AP is activated). Also, the alarm will sound when there is a new hour. The signal will not sound from 11:00 p.m. to 6:00 a.m. or if the device sounds are turned off (through the DEVICE STATUS item). 

### Network (settings)

![Image alt](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/setting_screen.jpg)
###### setting screen (the search SSID button is marked)

##### configure WiFi and api data
To set the network settings, select the NETWORK item in the main menu. The connection status is displayed at the top of the screen. If you click on [SEARH SSID] button the search screen for available networks will open and some information about them will be displayed. The list of networks scrolls. To select a network, double-click on it.

### Set color

![Image alt](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/color_screen.jpg)
######  color selection screen (the color selection area is marked)

##### select text color 
The SET COLOR menu item opens the text color setting screen. To save the color in memory, you must press [SET COLOR], otherwise the information will disappear on reboot.

### Device status

![Image alt](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/state_screen.jpg)
######  device status screen (the mode selection area is marked)

##### show and change some settings
The DEVICE STATUS menu item shows the status of the device and contains buttons on the left side that change the behavior of the device. The ESPNOW button is responsible for activating the ESPNOW service. The SECURITY button is used to hide the password from WIFI. If the SECURITY function is turn off old password will be clean. The SOUND button turns on sounds on the device. The SNTP button is used for change mode the time service. 

### Info

![Image alt](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/device_info_screen.jpg)
######  device info screen

##### show information about the device
The INFO menu item shows some information about this device, for example how a data for accessing the settings server.

### AP

![Image alt](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/AP.jpg)
######  access point screen (the server access data is marked)

![Image alt](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/setting_esp_page.jpg)
###### esp32 settings page

![Image alt](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/dwin_img.jpg)
###### touch screen settings page

##### remote mode to configure the device
The AP menu item activates AP and shows the login data and IP this device. When you go to the specified address, the configuration site will open. The ESP32 page provides OTAs, notification settings, device operating modes, network data, and the function of establishing time. On the DWIN page you can send commands and flash dwin screen device.

### Devices

![Image alt](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/espnow_device_screen.jpg)
###### linked devices screen

##### show espnow devices
ESPNOW allows devices based on modules from espressif to exchange information (for example network settings, time and sensor data). This connection is established automatically if espnow is activated. Communication data is encrypted using a 16-byte key specified via menuconfig.


#### service model
![Image alt](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/Model.png)

#### Demonstration
[![Clock demonstration](https://img.youtube.com/vi/c-kLCFXso-g/0.jpg)](https://www.youtube.com/watch?v=c-kLCFXso-g)
