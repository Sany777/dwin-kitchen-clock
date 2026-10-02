# ESP32 & HMI Multifunction Clock

A multifunction clock built around an ESP32 MCU and a DWIN HMI display.
- **Hardware:** ESP32 MCU + DWIN HMI display.
- **Firmware:** ESP-IDF C code (MCU) and DWIN binary/images (Display).
- **Features:** Time (SNTP/RTC), Weather (OpenWeather API), Notifications, OTA updates via AP mode.

---

## Features & Interfaces

### 1. Main Screen
![Main Screen](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/main_screen.jpg)

Displays current time and weather. 
- Requires internet access and OpenWeather API key.
- Temperature and forecast data come from the configured weather service.
- Tap the top right corner for a daily weather forecast.

![Weather Details](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/weather_details_screen.jpg)

### 2. Clock & Timer
![Clock Screen](https://github.com/Sany777/dwin-kitchen-clock/raw/main/src_help/clock_screen.jpg)

Configure time manually or via SNTP.
- Tap the top of the screen to toggle time synchronization mode.
- In manual mode, time is saved to the RTC.
- In SNTP mode, configure your time zone offset.
- Fallback time source: display's internal RTC.
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

---

## Use Cases

```mermaid
flowchart LR
    %% Actors
    User((User))
    Browser((Browser Client))

    %% Use Cases
    Clock(Use as Clock)
    Time(Get Time Data)
    SNTP(SNTP Server)
    RTC(Internal RTC)

    Weather(Use as Meteostation)
    OWM(OpenWeather API)

    Timer(Use as Timer)
    
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
    Browser --- Remote
    Remote -. "«include»" .-> AP
```

## Video Demonstration
[![Clock demonstration](https://img.youtube.com/vi/c-kLCFXso-g/0.jpg)](https://www.youtube.com/watch?v=c-kLCFXso-g)
