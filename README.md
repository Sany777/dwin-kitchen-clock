# ESP32 & HMI Multifunction Clock

A multifunction clock capable of joining an ESP-NOW network. 
- **Hardware:** ESP32 MCU + DWIN HMI display + BMP280 sensor.
- **Firmware:** ESP-IDF C code (MCU) and DWIN binary/images (Display).
- **Features:** Time (NTP/RTC), Weather (OpenWeather API), Notifications, ESP-NOW sync, OTA updates via AP mode.

---

## Features & Interfaces

### 1. Main Screen
![Main Screen](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/main_screen.jpg)

Displays current time and weather. 
- Requires internet access and OpenWeather API key.
- Temperature is read from the local BMP280 sensor.
- Tap the top right corner for a daily weather forecast.

![Weather Details](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/weather_details_screen.jpg)

### 2. Clock & Timer
![Clock Screen](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/clock_screen.jpg)

Configure time manually or via SNTP.
- Tap the top of the screen to toggle time synchronization mode.
- In manual mode, time is saved to the RTC.
- In SNTP mode, configure your time zone offset.
- Fallback time sources: Display's internal RTC or other ESP-NOW devices.
- Includes a built-in timer with an audible alarm.

### 3. Notifications
![Notification Screen](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/notification_screen.jpg)

Configure up to 4 daily notifications.
- An icon and audible alarm will trigger at the set time.
- Hourly chimes are supported (silenced between 23:00 and 06:00).
- Manage notifications via the device screen or the AP Web Server.

### 4. Network Settings
![Setting Screen](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/setting_screen.jpg)

Manage Wi-Fi credentials and API keys.
- Tap **[SEARCH SSID]** to scan for available networks.
- Double-tap a network to select it.

### 5. Customization
![Color Screen](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/color_screen.jpg)

Customize text colors across the interface. 
- Tap **[SET COLOR]** to save preferences to non-volatile storage.

### 6. Device Status
![Status Screen](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/state_screen.jpg)

Manage global device states:
- **ESPNOW:** Toggle ESP-NOW network synchronization.
- **SECURITY:** Hide or clear saved Wi-Fi passwords.
- **SOUND:** Toggle all device sounds.
- **SNTP:** Toggle network time synchronization.

### 7. Access Point (AP) & Web Server
![AP Screen](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/AP.jpg)

Activate the built-in Access Point for remote configuration.
- **ESP32 Settings:** Manage OTA updates, network config, time, and notifications.
- **DWIN Settings:** Send commands and flash the DWIN display over the air.

![ESP32 Settings](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/setting_esp_page.jpg)
![DWIN Settings](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/dwin_img.jpg)

### 8. ESP-NOW Linked Devices
![Linked Devices](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/espnow_device_screen.jpg)

ESP-NOW automatically synchronizes time, weather, and sensor data between nearby Espressif devices without needing a router. Communication is AES-128 encrypted (configure key via `menuconfig`).

---

## Use Cases

```mermaid
flowchart LR
    %% Actors
    User((User))
    OtherDev((Other ESP32))
    Browser((Browser Client))

    %% Use Cases
    Clock(Use as Clock)
    Time(Get Time Data)
    SNTP(SNTP Server)
    RTC(Internal RTC)

    Weather(Use as Meteostation)
    OWM(OpenWeather API)
    BMP(Internal BMP280)
    ESPNOW_W(ESP-NOW Sensors)

    Timer(Use as Timer)
    
    DataSource(Act as Data Source)
    ESPNOW_C(ESP-NOW Connection)

    Remote(Remote Control)
    AP(Activate AP Mode)

    %% Relationships
    User --- Clock
    User --- Weather
    User --- Timer

    Clock -. "«include»" .-> Time
    Time -. "«extend»" .-> SNTP
    Time -. "«extend»" .-> RTC

    Weather -. "«extend»" .-> OWM
    Weather -. "«extend»" .-> BMP
    Weather -. "«extend»" .-> ESPNOW_W

    OtherDev --- DataSource
    DataSource -. "«include»" .-> ESPNOW_C

    Browser --- Remote
    Remote -. "«include»" .-> AP
```

## Video Demonstration
[![Clock demonstration](https://img.youtube.com/vi/c-kLCFXso-g/0.jpg)](https://www.youtube.com/watch?v=c-kLCFXso-g)
