# Arduino Nano Fox Hunt Transmitter

**A one-of-a-kind Fox Hunt Transmitter with remotely switchable audio from an SD card.** <img src="https://github.com/user-attachments/assets/5e93cae8-8d00-4fab-9e4f-e9ad273243ce" alt="Fox Hunt Transmitter" width="80" align="middle">

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

## 📷 PCB
_Add your PCB photograph here._
![Fox Hunt Transmitter PCB](assets/pcb.jpg) 

---

# 🔧 Hardware
The circuit can be built on a **custom PCB or a general-purpose perfboard**.
| Component | Qty |
|---|---| 
| Arduino Nano / Uno | 1 |
| DFPlayer Mini | 1 | 
| 3.5 mm Audio Jack | 2 | 
| BS170 / 2N7000 | 2 |
| 10K Resistor | As required |
| 100Ω Resistor | As required |
| 15Ω Resistor | As required | 
| 470Ω Resistor | As required | 
| Connecting wires | As required |
| DC Jack | 1 | 
| LED | As required |

### ⚠️ BS170 / 2N7000 Warning 

**BS170 and 2N7000 are NOT pin-compatible.**
Their pinouts are different.
Always check the transistor datasheet and verify the pin orientation before replacing one with the other.

---


