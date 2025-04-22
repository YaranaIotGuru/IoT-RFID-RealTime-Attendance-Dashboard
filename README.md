# IoT RFID Real-Time Attendance System with Dashboard

A secure, IoT-based college attendance system using RFID technology, ESP32, relays, and a real-time user control dashboard. This project automates student attendance tracking, controls access via relays, and provides a dynamic web dashboard for admins to manage users and view logs in real-time. Perfect for engineering students, final year projects, and IoT enthusiasts.

📺 Watch the full tutorial on **Yarana IoT Guru's YouTube Channel** to build this project step-by-step!

**YouTube Tutorial**: [Watch the full video tutorial here](https://youtu.be/PTBI1_w4wr0)

## Table of Contents

- [Project Overview](#project-overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [ESP32 Code Explanation](#esp32-code-explanation)
- [Admin Panel/Dashboard](#admin-paneldashboard)
- [Setup Instructions](#setup-instructions)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Connect with Yarana IoT Guru](#connect-with-yarana-iot-guru)

## Project Overview

This project creates a secure college attendance system that uses RFID cards to track student attendance in real-time, controls relays for access management (e.g., doors or lights), and provides a web-based dashboard for administrators. Built with ESP32, RFID modules, relays, and IoT technologies, it integrates seamlessly with a MySQL database and a dynamic dashboard for user management and real-time control.

The system ensures secure authentication, scalability, and real-time data updates, making it ideal for educational institutions, engineering projects, and smart campus solutions.

## Features

- **RFID Attendance Tracking**: Logs student attendance using RFID cards (EM-18 module) and sends data to a server.
- **Relay Control**: Manages up to four relays (e.g., for doors or lights) based on server commands.
- **Real-Time Dashboard**: Admins can view attendance logs, manage users, and control relays via a web interface.
- **Secure Data Handling**: Stores unsent RFID data in EEPROM during network failures for later sync.
- **Buzzer Feedback**: Provides audible feedback (success/error beeps) for RFID scans.
- **Scalable Design**: Easily adaptable for small classrooms or large campuses.
- **IoT Integration**: Uses Wi-Fi (ESP32) for cloud connectivity and real-time updates.

## Hardware Requirements

- **ESP32 Dev Module**: For Wi-Fi connectivity and processing.
- **EM-18 RFID Reader Module**: To scan RFID cards (9600 baud, 12-character tags).
- **Relay Module (4-channel, active LOW)**: For controlling external devices (e.g., doors, lights).
- **Buzzer**: For audible feedback on RFID scans.
- **RFID Cards/Tags**: Compatible with EM-18 (125 kHz).
- **Jumper Wires & Breadboard**: For connections.
- **Power Supply**: 3.3V/5V for ESP32 and peripherals.

## Software Requirements

- **Arduino IDE**: For programming the ESP32.
  - Libraries:
    - WiFi.h: For ESP32 Wi-Fi connectivity.
    - HTTPClient.h & WiFiClientSecure.h: For HTTP requests to the server.
    - ArduinoJson.h: For parsing JSON responses from the server.
    - EEPROM.h: For storing unsent RFID data.
- **MySQL Database**: To store attendance logs and user data.
- **Web Server**: Hosting the admin panel (PHP-based, e.g., at yaranaiotguru.in).
  - PHP APIs:
    - `insert_rfid_data.php`: To log RFID scans.
    - `get_relay_status.php`: To fetch relay states.

## ESP32 Code Explanation

The provided ESP32 code (`main.ino`) handles RFID scanning, relay control, server communication, and local data storage. Below is a detailed breakdown:

### Key Functions

- **connectWiFi**: Connects to the Wi-Fi network (YaranaFiberOffice) and logs the IP address.
- **checkRFID**: Reads RFID tags and processes them.
- **sendRFIDToServer**: Sends RFID tags to the server and stores them in EEPROM if the server is unreachable.
- **fetchRelayStatus**: Fetches relay states and updates relay pins accordingly.
- **beepSuccess & beepError**: Controls buzzer feedback for success/error.
- **EEPROM Functions**: Handles the storage of unsent RFID data in case of network issues.

## Admin Panel/Dashboard

The **admin panel** is a web-based interface hosted at `https://yaranaiotguru.in/college`. It provides real-time control and monitoring for the attendance system. Below are its key features and login credentials:

### Admin Panel Login
- **Username**: `yarana`
- **Password**: `1234`

### Key Features

1. **Dashboard System**: Provides a comprehensive overview, including real-time attendance data and relay control status.
2. **RFID Logs**: View and filter RFID logs in real-time, displaying student names and timestamps for each scan.
3. **Relay Smart Control System**: Control relays (e.g., unlocking doors or controlling lights) via the dashboard.
4. **User Management System**: Add, edit, delete, or manage users (students, teachers, admins), assign RFID tags, and define user roles.
5. **Users Report**: Download user-related attendance reports in **CSV** or **PDF** format for analysis and reporting.
6. **Settings**: Modify system settings like college name, add new admin users, update user credentials, and delete user accounts.

### Admin Dashboard Example

The admin dashboard allows you to:

- View real-time logs and relay statuses.
- Control relays with a toggle switch (ON/OFF).
- Generate reports of user attendance in CSV/PDF formats.
- Add or delete admin users and set their login credentials.

## Setup Instructions

### Hardware Setup:

1. **Connect the EM-18 RFID reader** to ESP32 (RX to GPIO 16, TX not used).
2. **Connect the 4-channel relay module** to GPIO 13, 12, 27, 26.
3. **Attach the buzzer** to GPIO 14.
4. Power the ESP32 and peripherals with a 3.3V/5V supply.

### Software Setup:

1. Install **Arduino IDE** and add ESP32 board support.
2. Install required libraries: **WiFi**, **HTTPClient**, **WiFiClientSecure**, **ArduinoJson**, **EEPROM**.
3. Update the code with your Wi-Fi credentials (SSID, password) and server URLs.

### Server Setup:

1. Set up a **MySQL database** with tables for attendance, users, and relay states.
2. Deploy the PHP APIs on the server.
3. Host the admin panel on the same server with HTTPS.

### Upload Code:

1. Open `main.ino` in Arduino IDE.
2. Select your ESP32 board and upload the code.
3. Monitor the **Serial Monitor** for debugging.

### Test the System:

1. Scan an RFID card to verify logging (check Serial Monitor and dashboard).
2. Toggle relays via the dashboard and confirm physical relay states.
3. Disconnect Wi-Fi to test EEPROM storage and buzzer feedback.

## Usage

- **Attendance Tracking**: Students scan their RFID cards. Admins view logs in real-time.
- **Relay Control**: Admins toggle relays via the dashboard.
- **Offline Handling**: RFID tags are stored in EEPROM if Wi-Fi is down.

### Dashboard Access:

- Log in to the admin panel (e.g., `https://yaranaiotguru.in/college`).
- Use the provided **username** (`yarana`) and **password** (`1234`) to access the dashboard.
- Manage users, view logs, control relays, and analyze trends.

## Contributing

Contributions are welcome! To contribute:

1. Fork this repository.
2. Create a new branch: `git checkout -b feature/your-feature`.
3. Make your changes and commit: `git commit -m 'Add your feature'`.
4. Push to the branch: `git push origin feature/your-feature`.
5. Open a **Pull Request**.

Ensure your code follows the existing style and includes comments for clarity.

## License

This project is licensed under the **MIT License**. Feel free to use, modify, and distribute the code.

## Connect with Yarana IoT Guru

Built by **Yarana IoT Guru**, your go-to source for innovative IoT projects and tutorials! Subscribe to our YouTube channel for more tech content:

- 📺 **Yarana IoT Guru YouTube**  
  - **Subscribers**: 1.7K+  
  - **Video Link**: [Full Tutorial on YouTube](https://youtu.be/PTBI1_w4wr0)
- 📱 **Contact**: +917052722734  
- 📘 **Facebook**: [Yarana IoT Guru Facebook](https://www.facebook.com/yaranaiotguru)  
- 📸 **Instagram**: [Yarana IoT Guru Instagram](https://www.instagram.com/yaranaiotguru/)  
- 📢 **LinkedIn**: [Yarana IoT Guru LinkedIn](https://www.linkedin.com/in/yarana-iot-guru)  
- 🐦 **Twitter**: [Yarana IoT Guru Twitter](https://twitter.com/YaranaIotGuru)  
- 🔗 **Reddit**: [Yarana IoT Guru Reddit](https://www.reddit.com/user/YaranaIotGuru)  
- 🔧 **Hackaday**: [Yarana IoT Guru Hackaday](https://hackaday.io/YaranaIotGuru)  
- 📁 **Github**: [Yarana IoT Guru GitHub](https://github.com/YaranaIotGuru)  
- 💻 **Website**: [Yarana IoT Guru Website](https://yaranaiotguru.in/)  
- 🛠️ **Hackster**: [Yarana IoT Guru Hackster](https://www.hackster.io/yaranaiotguru)

🌟 Star this repository and share it with your peers to support innovative IoT projects!

---

### YouTube Thumbnail for the Tutorial
![YouTube Thumbnail](https://img.youtube.com/vi/PTBI1_w4wr0/maxresdefault.jpg)

---

Feel free to modify this further if you need any additional sections. This will now include your YouTube link, channel details, and video thumbnail.
