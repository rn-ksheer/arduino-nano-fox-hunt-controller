# Arduino Nano Fox Hunt Transmitter

> **A one-of-a-kind Fox Hunt Transmitter with remotely switchable audio from an SD card.** <img src="https://github.com/user-attachments/assets/5e93cae8-8d00-4fab-9e4f-e9ad273243ce" alt="Fox Hunt Transmitter" width="6%" align="middle">

--- 
## 📡 About the Project
This is an **Arduino-based Fox Hunt Transmitter** designed for amateur-radio fox hunting.
What makes this transmitter unique is that **audio stored on an SD card can be remotely selected and switched using DTMF tones**. Instead of being limited to a single fixed audio message, different audio tracks can be stored on the DFPlayer Mini and selected remotely through the radio. The audio files can also be replaced or customized without changing the transmitter hardware.

### Features
- 🎙️ Remote DTMF audio selection
-  💾 Multiple audio tracks stored on an SD card
-  🔄 Automatic audio looping
- ⏱️ Programmable ON/OFF transmission timing
- 📻 Radio PTT control
- 💡 External transmission indicator LED
- 💡 Arduino onboard status LED
- 🔌 Simple and inexpensive hardware
- 🛠️ Can be built on a custom PCB or perfboard

---

## 📷 perfboard PCB-Final
<img src="https://github.com/user-attachments/assets/390b666c-7621-414e-b672-c2eb38b4eda0" width="50%" alt="PXL_20260906_175223771" />

---

# 🔧 Hardware
The circuit can be built on a **custom PCB or a general-purpose perfboard**.
| Component | Qty |
|---|---| 
| Arduino Nano / Uno | 1 |
| DFPlayer Mini | 1 | 
| 3.5 mm Audio Jack | 2 | 
| BS170 / 2N7000 | 2 |
| 10K Resistor | 1 |
| 100Ω Resistor | 1 |
| 15Ω Resistor | 1 | 
| 470Ω Resistor | 1 | 
| Connecting wires | 1 |
| DC Jack | 1 | 
| LED | 1 |
|Perfboard | 1 |

### ⚠️ BS170 / 2N7000 Warning 

**BS170 and 2N7000 are NOT pin-compatible.**
- Their pinouts are different. Always check the transistor datasheet and verify the pin orientation before replacing one with the other.

---

# Schematic

<img width="70%" alt="Schematic_fox-tx_2026-09-10" src="https://github.com/user-attachments/assets/1e3ae334-4121-47a1-9a64-6901572e5bb1" />

---

# 📐 Reference / Pinout Information 

## DFPlayer Mini

<img width="35%" alt="image" src="https://github.com/user-attachments/assets/e6e69390-ad71-4ff9-b402-bed04d1bd954" />


## BS170 / 2N7000

<img src="https://github.com/user-attachments/assets/eba941ef-e4e2-4066-8b78-6e4be3481cab" width="40%" alt="image" /> <img src="https://github.com/user-attachments/assets/78f1288d-f91f-4550-9fe2-8b52d4caf3ac" width="40%" alt="image" />

 **Important:** Check the actual transistor being used before assembling the circuit. BS170 and 2N7000 have different pin arrangements.


## 3.5 mm Audio Connector 

<img width="35%" alt="image" src="https://github.com/user-attachments/assets/c04f93dc-5c5f-4bd9-8dbc-c0643771f61a" /> <img width="40%" alt="image" src="https://github.com/user-attachments/assets/8192d3f9-c5e6-4bcb-986e-fd6c916e1d71" /> <img width="50%" alt="PJ306" src="https://github.com/user-attachments/assets/b58698d1-f3a2-42b2-bc28-a2501262e594" />

---

# 💻 Software Requirements

- Arduino IDE
- Arduino Nano or Arduino Uno
- DFRobot DFPlayer Mini library/DTMF library/SoftwareSerial library are included.
- The project has been **tested on an Arduino Nano**.
- It should also work on other Arduino-compatible microcontrollers supported by the Arduino IDE, provided that the required libraries, memory, pins, and serial interfaces are available.

---


# 📥 Installation

## 1. Download or clone the **entire repository**. 

- Do not copy only the main `.ino` file.
- All required source files are part of the project.
---


## 2. Keep All Source Files Together

Make sure all required `.cpp` and `.h` files are located in the **same folder as the main Arduino sketch**.
Example project structure: 

```text
arduino-nano-fox-hunt-transmitter/
 │
 ├── fox_hunt_transmitter.ino
 ├── DTMF.cpp
 ├── DTMF.h
 ├── other_required.cpp
 ├── other_required.h
 │
 └─── mp3/
     ├── 0001.mp3
     ├── 0002.mp3
     ├── 0003.mp3
     └── ...

```
---

## 💾 SD Card Setup

The repository contains a folder named:

```text
mp3
```

Copy the **entire** **`mp3`** **folder** to the SD card that will be inserted into the DFPlayer Mini.

### ⚠️ Do NOT rename the folder

The folder must remain:

```text
mp3
```

The audio files inside the folder can be replaced with your own recordings.

However, the **four-digit numbering format must be maintained**.

Use:

```text
0001.mp3
0002.mp3
0003.mp3
0004.mp3
....
```

Example:

```text
mp3/
├── 0001.mp3
├── 0002.mp3
├── 0003.mp3
├── 0004.mp3
├── 0005.mp3
└── ....
```

### Important

You can replace the audio content with your own recordings, but **do not change the required file numbering format**.

For example, `0009.mp3` can contain any audio message you want, but the filename must remain:

`0009.mp3`

The **four-digit numbering format** is required for the DFPlayer Mini to correctly recognize and access the audio files used by this project.

---

## 🛠️ Arduino IDE Setup

Open the main `.ino` file in Arduino IDE.

Select the appropriate board.

### Arduino Nano
Tools → Board → Arduino AVR Boards → Arduino Nano

or:

### Arduino Uno
Tools → Board → Arduino AVR Boards → Arduino Uno

For an Arduino Nano, select the appropriate processor/bootloader option for your particular Nano if required.

##### Then select the correct COM port:

Tools → Port

Compile and upload the sketch.

---

## 🎙️ DTMF Audio Switching

The transmitter uses ***DTMF tones received through the radio/audio input*** to remotely select audio tracks.

The basic operation is:
```text
Remote Radio
     │
   DTMF
     ▼
Fox Radio
  Receiver
     │
     ▼
  Arduino
DTMF Decoder
     │
     ▼
DFPlayer Mini
     │
 Selected Audio
     ▼
Fox Radio
Transmitter
     │
    RF
     ▼
Remote Radio
```

The remote operator simply transmits the required DTMF key.

The Arduino detects the DTMF command and selects the corresponding audio file from the DFPlayer Mini.

---

## 🔢 DTMF Commands

| DTMF Key | Action                      | Audio File |
| -------- | --------------------------- | ---------- |
| `1`      | Start automatic looping     | `0009.mp3` |
| `2`      | Start automatic looping     | `0010.mp3` |
| `3`      | Start automatic looping     | `0011.mp3` |
| `4`      | Play once                   | `0002.mp3` |
| `5`      | Play once                   | `0008.mp3` |
| `6`      | Play once                   | `0007.mp3` |
| `7`      | Play once                   | `0006.mp3` |
| `9`      | Stop looping / transmission | —          |

### Example

If the remote operator sends:

```text
1
```

the transmitter selects:

```text
mp3/0009.mp3
```

and starts the automatic transmission loop.

Similarly:

```text
2 → 0010.mp3
3 → 0011.mp3
```

This makes it possible to remotely switch between different audio messages without physically accessing the transmitter.

---


## 🔁 Automatic Audio Loop

DTMF commands `1`, `2`, and `3` start the automatic audio loop.

The default timing is:

        ┌───────────────────┐
        │     60 SEC ON     │
        │    Audio TX ON    │
        └─────────┬─────────┘
                  │
                  ▼
        ┌───────────────────┐
        │    60 SEC OFF     │
        │     No TX         │
        └─────────┬─────────┘
                  │
                  └──────────────► Repeat

During the **ON phase**, the selected audio plays continuously.

If the audio file finishes before the 60-second ON period is complete, the same audio file is automatically restarted.

After 60 seconds:

- PTT is released
- Audio transmission stops
- External LED turns OFF
- The transmitter remains OFF for 60 seconds

After the OFF period, the selected audio starts again automatically.

---

## ⏱️ Changing the Loop Duration

The ON and OFF duration can be changed in the Arduino source code.

The main setting is:

const unsigned long LOOP_DURATION = 60000UL;

Examples:

30000UL   = 30 seconds

60000UL   = 60 seconds

120000UL  = 120 seconds

The same value is used for both the ON and OFF periods.

---

## 💡 LED Indicators
### Arduino Onboard LED

The Arduino onboard LED indicates that automatic looping is active.

##### Default blink timing:

ON  → 1 second
OFF → 1 second

The blink speed can be changed using:

const unsigned long LED_BLINK_MS = 1000UL;

#### External LED

The external LED indicates transmitter/audio activity.

It turns ON when the transmitter is keyed and turns OFF when the transmission/audio ends.

---
## 📺 Testing Videos

Add your YouTube testing videos below.

Test Video 1

▶️ DTMF Audio Switching Test

Test Video 2

▶️ Audio Loop Test

Test Video 3

▶️ Fox Hunt Transmitter Test

Test Video 4

▶️ Field Test

Replace YOUR_YOUTUBE_URL_HERE with the actual YouTube video URLs.

---

## 📝 Important Notes
- The project has been tested on an **Arduino Nano** with **Baofeng UV-5R and UV-82** radios.
- **Arduino Uno** is also supported by the current design.
- Other Arduino-compatible microcontrollers may work if they are supported by the **Arduino IDE** and the required libraries.
- Keep the **`mp3`** folder name unchanged.
- Maintain the **four-digit MP3 file numbering format** (for example, `0001.mp3`, `0002.mp3`, `0009.mp3`).
- **BS170 and 2N7000 are not pin-compatible.** Always check the transistor pinout before substituting one for the other.
- Use a properly formatted SD card that is compatible with the **DFPlayer Mini**.
- Always verify the **radio, transmitter, audio, PTT, and power connections** before operating the transmitter.

---
## 📜 Credits

This project is free to use, modify, and fork.
You are welcome to build upon the project and adapt it for your own Fox Hunt or amateur-radio application.
If you use, modify, or redistribute this project, please give credit to the original author: ***VU25RK***

> Built for amateur-radio experimentation, Fox Hunting, and the amateur-radio community.

---
## 🦊 Have Fun Fox Hunting!

Listen. Track. Find the Fox.

73 de **VU25RK**

---
