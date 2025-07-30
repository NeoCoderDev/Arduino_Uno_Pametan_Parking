### Smart Parking System – Arduino Mega Based ###

This project is a fully functional **Smart Parking System** developed using an **Arduino Mega 2560**, **ESP8266**, **various sensors**, **DFPlayer Mini**, and a custom **web dashboard**. It tracks parking space availability, displays real-time status on an LCD, manages ramp control, and integrates features like fire and water level detection.

-----------------------------------------------------------------------------

## Features ##

- 🚘 Detects parked vehicles using ultrasonic sensors
- 📡 Communicates status via ESP8266 to web dashboard
- 🎵 Plays audio feedback with DFPlayer Mini
- 💧 Water level monitoring
- 🔥 Fire detection with flame sensor and buzzer alarm
- 🌡️ Displays temperature and humidity via DHT11
- 🚦 Controls traffic lights (2 independent semaphores)
- 🎛️ Servo-controlled ramp opening/closing with physical button & IR sensors
- 🖥️ Outputs current state on ST7920 LCD screen
- 🔄 Web communication supports remote control commands (open/close/reserve)

-----------------------------------------------------------------------------

## Components Used ## 

| Component              | Function                              |
|------------------------|---------------------------------------|
| Arduino Mega 2560      | Main microcontroller                  |
| ESP8266 WiFi module    | Communication with web dashboard      |
| DHT11                  | Temperature & Humidity                |
| DFPlayer Mini + Speaker| Plays audio feedback                  |
| 2x Ultrasonic Sensors  | Detects vehicle presence              |
| IR Sensors             | Entry/Exit detection                  |
| Flame Sensor           | Fire detection                        |
| Water Level Sensor     | Analog water detection                |
| Piezo Buzzer           | Audible alarms                        |
| Servo Motor            | Ramp barrier control                  |
| ST7920 (U8g2lib) LCD   | Real-time system display              |
| 2x Traffic Semaphores  | LED control for entry/exit            |
| Relay Modules          | Fan and water pump control            |

---

## Code Highlights ##

- Full `loop()` logic handles:
  - Vehicle detection
  - Ramp logic
  - ESP commands (`OPEN`, `CLOSE`, `RESERVE1`, `GET_STATUS`, `PLAY`, `STOP`)
  - Water level reading & flame detection
  - Servo control with `slowMove()`
  - Traffic light logic via `enum` states

---

## 📡 Web Communication

ESP8266 communicates over `Serial2`, receiving and responding to commands:
- `GET_STATUS`: sends available/occupied parking status
- `RESERVE1`: reserves a parking space if available
- `OPEN` / `CLOSE`: opens or closes the ramp
- `PLAY` / `STOP`: controls MP3 playback

---

## Display Information ##

The LCD shows:
- Free and occupied parking slots
- Distance readings from ultrasonic sensors
- Water level status
- “Parking Full” warning when no space is available

---

## Project Structure ##
Arduino_Smart_Parking/
├── ArduinoMega_TestKod.cpp # Full Arduino logic
├── Pametan_Parking_sajt/ # Web interface (HTML + CSS)
├── Documentation/ # PDF/DOC reports
├── ParkingCss.css / index.html


---

## ▶️ How to Use

1. Upload `ArduinoMega_TestKod.cpp` to Arduino Mega.
2. Connect ESP8266 to `TX2/RX2`.
3. Open serial monitor and website interface.
4. Interact via button, IR sensors, or browser.
5. Monitor parking status live on LCD and web.

---

## Author

Developed by **Nikola Pejak** as an advanced real-world IoT + embedded systems project integrating sensors, communication, and user interaction.

---

## 📃 License

This project is open-source and free for educational use.

