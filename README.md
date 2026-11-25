# `ESP32 GOBUSTER DETECTION`

## `> overview`

```text
esp32-gobuster-detection

ESP32 SoftAP HTTP target with 404 spike detection:
├── Hosts Wi-Fi SoftAP ("ESP32-GOBUSTER-TARGET") on port 80
├── Tracks up to 10 client IPs in a 10-second sliding window
├── Triggers a Serial alert at 15 404s within 10 seconds
├── Counts total 404s and alerts per IP
└── Reads "count" and "alert" commands over Serial (115200 baud)

Current Mode:
└── Standalone AP + Serial monitor.
```

An ESP32 sketch that runs an HTTP server on a SoftAP and **logs an alert over Serial when a client triggers 15 HTTP 404 responses within a 10-second window**.

Includes a `/` endpoint returning `200` (`ESP32-BUST IS ACTIVE`), a catch-all `404` handler (`Not found`), and basic Serial commands to inspect per-IP 404 and alert counts.

---

## `> flow`

```text
                    ┌──────────────────┐
                    │  HTTP REQUEST    │
                    │  (Port 80 AP)    │
                    └────────┬─────────┘
                             │
              ┌──────────────┴──────────────┐
              ▼                             ▼
        GET / (200)                 Any Other Path (404)
    "ESP32-BUST IS ACTIVE"           handleNotFound()
                                            │
                                            ▼
                                     record404(IP)
                                            │
                         ┌──────────────────┴──────────────────┐
                         ▼                                     ▼
                  ip_counter[IP]++                  10s Window Tracker
                  (Total 404s)                    (Max 10 IPs, oldest evicted)
                                                             │
                                                             ▼
                                                  15 404s in 10s?
                                                             │
                                          ┌──────────────────┴──────────────────┐
                                          ▼                                     ▼
                                     [ALERT] Serial                      ip_alert_counter[IP]++
```

---

## `> tech_stack`

### Libraries & Language

<p>
<img src="https://skillicons.dev/icons?i=cpp,arduino" />
</p>

### Project Structure

```text
├── main.ino      # SoftAP setup, WebServer (port 80), 404 window & alert logic
├── Globals.h     # Extern declarations for ip_counter and ip_alert_counter
└── Commands.cpp  # Serial command parser ("count", "alert")
```

### Implemented Features

```text
SoftAP & HTTP Server   ████████████████████
404 Spike Detection    ████████████████████
Per-IP Counters        ████████████████████
Serial Commands        ████████████████░░░░
```

---

## `> setup`

1. Open `main.ino`, `Globals.h`, and `Commands.cpp` in the Arduino IDE (with ESP32 board support installed).
2. Flash to your ESP32 and open the Serial Monitor at **`115200` baud**.
3. Connect a device to the Wi-Fi network:
   * **SSID:** `ESP32-GOBUSTER-TARGET`
   * **Password:** `gobuster`
   * **ESP32 IP:** Printed on Serial at startup (`WiFi.softAPIP()`).

---

## `> usage`

Send a request to `/` or trigger `404` responses (for example with `gobuster dir`) against the ESP32's SoftAP IP:

```bash
gobuster dir -u http://<ESP32_AP_IP> -w /path/to/wordlist.txt
```

Example Serial output when a client hits 15 `404` responses within 10 seconds:

```text
[ALERT] 404 Spike detected from IP: <CLIENT_IP> (15 requests in 10s)
```

Send commands over Serial (newline-terminated) to print maps:

* `count` — prints total `404` count per client IP (or `NO REQUESTS YET...`)
* `alert` — prints alert trigger count per client IP (or `NO ALERTS YET...`)

---

## `> code_notes`

* **Client eviction:** `MAX_TRACKED_CLIENTS` is `10`; if a new IP arrives when full, the entry with the earliest `windowStart` is overwritten.
* **Alert latch:** Once `alert` becomes `true` for a client's current 10-second window, additional `404`s in that same window do not re-trigger the alert until the 10-second window resets.
* **Serial command casing:** `Commands.cpp` calls `cmd.toLowerCase()` (which mutates `cmd` in place in Arduino `String`), so send lowercase `count` or `alert`.
