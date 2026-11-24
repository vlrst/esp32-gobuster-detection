ESP-32 WiFi Scanner

Using ESP32 and C++
Detects nearby WiFi networks, collections information, and saves the results ot the ESP32's internal flash storage using LittleFS

Information Collected (5 Scans)
    SSID: WiFi network name
    RSSI: Signal strength in dBm
    Security: Network encryption type

Information is printed to terminal as well as saved to a scan.txt file which is stored under timestamp-named folder on the ESP32


How the project works
1. ESP32 starts Serial communication at 115200 baud and initializes LittleFS
2. Folder created using Unix timestamp
3. ESP32 performs 5 scan and retrieves SSID, RSSI, and Security
4. Results are saved using LittleFS
5. Terminal mode (Work in Progress) after the five scans allows the user to enter commands
    'ls' - Lists the items in the root directory of LittleFS
    'memory' - Displays the total, used, and free LittleFS storage

Connecting through MacOS Terminal
1. Identify serial port (mine appeared as /dev/cu.SLAB_USBtoUART)
    ls /dev/cu.*
2. Connect to serial port (last value must match the baud rate configured in the code)
    screen /dev/cu.SLAB_USBtoUART 115200


Screen
1. List all ports being used
    screen -ls
2. Kill the Session ID being used
    screen -X -S {SESSION_ID} quit
    OR
    kill {SESSION_ID}
3. When all ports are killed, connect to serial port 
    screen /dev/cu.SLAB_USBtoUART 115200
