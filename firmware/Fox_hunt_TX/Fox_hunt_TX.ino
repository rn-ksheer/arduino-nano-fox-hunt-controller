
// =========================================================
// FOX HUNT TRANSMITTER / CONTROLLER
// =========================================================
//
// DTMF COMMANDS
// --------------
// 1 = Play audio file 9 and start automatic looping
// 2 = Play audio file 10 and start automatic looping
// 3 = Play audio file 11 and start automatic looping
// 4 = Play audio file 2 once
// 5 = Play audio file 8 once
// 6 = Play audio file 7 once
// 7 = Play audio file 6 once
// 9 = Stop all audio / looping
//
// LOOPING BEHAVIOR
// ----------------
// ON  phase  = 60 seconds
// OFF phase  = 60 seconds
//
// During the ON phase:
//   - Audio plays continuously.
//   - If the selected audio file finishes before 60 seconds,
//     it is started again immediately.
//
// During the OFF phase:
//   - PTT is LOW.
//   - External LED is OFF.
//   - No audio is transmitted.
//
// STATUS LED
// ----------
// Arduino onboard LED (pin 13):
//   - Slow blink while looping is active.
//   - 1 second ON / 1 second OFF.
//
// IMPORTANT
// ---------
// This is the working version. Comments have been added for
// easier modification and maintenance. The functional logic
// has intentionally NOT been changed.
//
// =========================================================


// ---------------------------------------------------------
// LIBRARIES
// ---------------------------------------------------------

#include "DTMF.h"                  // DTMF tone detection
#include "Arduino.h"               // Arduino core functions
#include "DFRobotDFPlayerMini.h"   // DFPlayer Mini control
#include <SoftwareSerial.h>        // Software serial for DFPlayer


// ---------------------------------------------------------
// DFPLAYER SERIAL CONFIGURATION
// ---------------------------------------------------------
//
// SoftwareSerial(RX, TX)
//
// Arduino pin 9  -> DFPlayer TX
// Arduino pin 10 -> DFPlayer RX
//
// If your hardware wiring is different, change the two pins
// below.
//
// NOTE:
// The DFPlayer is intentionally controlled using
// playMp3Folder(). Audio files therefore need to be stored
// according to the DFPlayer MP3 folder/indexing arrangement.
// ---------------------------------------------------------

SoftwareSerial softSerial(9, 10);

#define FPSerial softSerial

DFRobotDFPlayerMini myDFPlayer;


// ---------------------------------------------------------
// DEBUG CONFIGURATION
// ---------------------------------------------------------
//
// Set DEBUG to true when debugging is required.
//
// DEBUG = false
//   Serial debug messages are disabled.
//
// DEBUG = true
//   Serial debug messages are enabled.
//
// Serial baud rate is 9600.
// ---------------------------------------------------------

#define DEBUG false

#define DEBUG_SERIAL \
  if (DEBUG) Serial


// ---------------------------------------------------------
// HARDWARE PIN DEFINITIONS
// ---------------------------------------------------------
//
// PTT_PIN
// -------
// Controls the transmitter PTT input.
//
// HIGH = transmitter keyed / PTT active
// LOW  = transmitter unkeyed / PTT inactive
//
// SENSOR_PIN
// ----------
// Analog input used by the DTMF detector.
//
// ledPin
// ------
// Arduino onboard/status LED.
// Pin 13 is normally connected to the onboard LED.
//
// ext_led
// -------
// External LED used to indicate transmitter/audio activity.
// ---------------------------------------------------------

#define PTT_PIN    2
#define SENSOR_PIN A5
#define ledPin     13
#define ext_led    18


// ---------------------------------------------------------
// DTMF CONFIGURATION
// ---------------------------------------------------------
//
// dtmfN
// -----
// Number of samples used by the DTMF detector.
//
// dtmfSamplingRate
// -----------------
// Sampling rate used by the DTMF detector.
//
// These values are part of the DTMF detection configuration.
// Change them only if you understand the requirements of
// the DTMF library and your audio sampling setup.
//
// d_mags
// ------
// Stores the detected DTMF frequency magnitudes.
//
// dtmfChar
// --------
// Contains the detected DTMF character.
//
// pauseTime
// ---------
// Used by the older loop1() function.
// It is retained here because this is the working code.
// ---------------------------------------------------------

float dtmfN = 128.0;
float dtmfSamplingRate = 8926.0;

DTMF dtmf(dtmfN, dtmfSamplingRate);

float d_mags[8];
char dtmfChar;

int pauseTime = 1400;


// ---------------------------------------------------------
// AUDIO LOOPING SYSTEM
// ---------------------------------------------------------
//
// loopingEnabled
// --------------
// TRUE  = automatic audio looping is active
// FALSE = automatic audio looping is disabled
//
// loopingFile
// -----------
// Contains the DFPlayer file number currently being looped.
//
// Example:
//   9  = audio file 9
//   10 = audio file 10
//   11 = audio file 11
//
// loopON
// ------
// TRUE  = currently in the 60-second ON phase
// FALSE = currently in the 60-second OFF phase
//
// loopPhaseStart
// ---------------
// Stores the millis() timestamp at which the current
// ON or OFF phase started.
//
// audioFinished
// -------------
// Set TRUE when DFPlayer reports that the current audio
// file has finished playing.
//
// The looping handler uses this flag to restart the audio
// immediately during the ON phase.
// ---------------------------------------------------------

bool loopingEnabled = false;
int loopingFile = 0;

bool loopON = false;
unsigned long loopPhaseStart = 0;

bool audioFinished = false;


// ---------------------------------------------------------
// ONBOARD LED BLINK CONFIGURATION
// ---------------------------------------------------------
//
// ledTimer
// --------
// Stores the last time the onboard LED state was changed.
//
// ledState
// --------
// Current state of the onboard LED.
//
// LED_BLINK_MS
// ------------
// Time between LED state changes.
//
// Current setting:
//   1000 ms = 1 second
//
// Therefore the LED produces approximately:
//   1 second ON
//   1 second OFF
//
// To make the LED blink faster, reduce this value.
//
// Example:
//   500UL = 0.5 second ON / 0.5 second OFF
//
// To make it slower:
//   2000UL = 2 second ON / 2 second OFF
// ---------------------------------------------------------

unsigned long ledTimer = 0;

bool ledState = false;

const unsigned long LED_BLINK_MS = 1000UL;


// ---------------------------------------------------------
// AUDIO LOOP DURATION
// ---------------------------------------------------------
//
// This controls BOTH:
//
//   ON duration
//   OFF duration
//
// Current setting:
//   60000 ms = 60 seconds
//
// Examples:
//
//   30000UL = 30 seconds
//   60000UL = 60 seconds
//   120000UL = 120 seconds
//
// IMPORTANT:
// Change this value instead of searching through the code
// for individual 60000 values.
// ---------------------------------------------------------

const unsigned long LOOP_DURATION = 60000UL;


// =========================================================
// SETUP
// =========================================================

void setup() {

  // -------------------------------------------------------
  // Start communication with DFPlayer Mini.
  // -------------------------------------------------------
  //
  // DFPlayer communication speed:
  //   9600 baud
  //
  // If your DFPlayer wiring uses the SoftwareSerial pins
  // defined above, no changes are normally required here.
  // -------------------------------------------------------

  FPSerial.begin(9600);


  // -------------------------------------------------------
  // Start hardware Serial for optional debugging.
  //
  // DEBUG_SERIAL automatically does nothing when DEBUG
  // is false.
  // -------------------------------------------------------

  DEBUG_SERIAL.begin(9600);


  // -------------------------------------------------------
  // Configure transmitter PTT pin.
  //
  // LOW at startup ensures that the transmitter is not
  // keyed immediately after power-up.
  // -------------------------------------------------------

  pinMode(PTT_PIN, OUTPUT);
  digitalWrite(PTT_PIN, LOW);


  // -------------------------------------------------------
  // Configure onboard status LED.
  //
  // LED is OFF during startup.
  // -------------------------------------------------------

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);


  // -------------------------------------------------------
  // Configure external activity LED.
  //
  // LED is OFF during startup.
  // -------------------------------------------------------

  pinMode(ext_led, OUTPUT);
  digitalWrite(ext_led, LOW);


  // -------------------------------------------------------
  // Initialize DFPlayer Mini.
  //
  // The program stays here if the DFPlayer cannot be
  // initialized.
  // -------------------------------------------------------

  if (!myDFPlayer.begin(FPSerial, true, true)) {

    // DFPlayer initialization failed.
    // Stay here until the problem is corrected/reset.

    while (1) delay(10);
  }


  // -------------------------------------------------------
  // Set DFPlayer volume.
  //
  // DFPlayer volume range is normally 0–30.
  //
  // Current setting:
  //   15
  //
  // Change this value if a different audio level is required.
  // -------------------------------------------------------

  myDFPlayer.volume(15);
}


// =========================================================
// MAIN LOOP
// =========================================================
//
// The main loop performs three jobs:
//
// 1. Sample and decode DTMF.
// 2. Manage the 60-second ON/OFF audio loop.
// 3. Process DFPlayer status messages.
//
// Everything is based on millis() where possible so the
// controller can continue operating without a long blocking
// delay.
// =========================================================

unsigned long lastDTMF = 0;

void loop() {

  unsigned long now = millis();


  // -------------------------------------------------------
  // DTMF SAMPLING
  // -------------------------------------------------------
  //
  // DTMF is sampled approximately every 1 ms.
  //
  // dtmf.sample()
  //     Reads the analog DTMF input.
  //
  // dtmf.detect()
  //     Calculates the DTMF frequency magnitudes.
  //
  // dtmf.button()
  //     Converts the detected frequencies into a keypad
  //     character such as '1', '2', '3', etc.
  // -------------------------------------------------------

  if (now - lastDTMF >= 1) {

    lastDTMF = now;

    dtmf.sample(SENSOR_PIN);

    dtmf.detect(d_mags, 506);

    dtmfChar = dtmf.button(d_mags, 1800.0);


    // -----------------------------------------------------
    // If a valid DTMF character was detected, process it.
    // -----------------------------------------------------

    if (dtmfChar) {
      processDTMF(dtmfChar);
    }
  }


  // -------------------------------------------------------
  // Handle the automatic 60-second ON/OFF audio loop.
  // -------------------------------------------------------

  handleLoopingAudio();


  // -------------------------------------------------------
  // Check whether DFPlayer has sent a status message,
  // including DFPlayerPlayFinished.
  // -------------------------------------------------------

  checkDFPlayer();
}


// =========================================================
// OLD / ALTERNATIVE MAIN LOOP
// =========================================================
//
// This function is NOT called by the active program.
//
// It is retained from an earlier version of the controller.
//
// If you do not need it anymore, it can be removed safely
// after confirming that no other code calls loop1().
// =========================================================

void loop1() {

  // DTMF Check
  for (int i = 0; i < pauseTime; i++) {

    dtmf.sample(SENSOR_PIN);

    dtmf.detect(d_mags, 506);

    dtmfChar = dtmf.button(d_mags, 1800.0);


    if (dtmfChar) {
      processDTMF(dtmfChar);
    }


    checkDFPlayer();

    delay(1);
  }


  handleLoopingAudio();

  checkDFPlayer();
}


// =========================================================
// PLAY AUDIO
// =========================================================
//
// file
// ----
// DFPlayer MP3 file number to play.
//
// Example:
//   playVoice(9);
//   playVoice(10);
//   playVoice(11);
//
// Sequence:
//
//   1. PTT is activated.
//   2. External LED is turned ON.
//   3. 250 ms delay allows the transmitter to key.
//   4. DFPlayer starts the selected MP3 file.
//
// IMPORTANT:
// ---------------------------------------------------------
// Do NOT replace playMp3Folder() with play() unless you
// have confirmed that your DFPlayer file arrangement and
// library behavior support it.
//
// The current working version intentionally uses:
//
//     myDFPlayer.playMp3Folder(file);
//
// ---------------------------------------------------------

void playVoice(int file) {

  // Key transmitter.
  digitalWrite(PTT_PIN, HIGH);


  // Indicate transmission/audio activity.
  ext_led_on();


  // Allow PTT/transmitter to settle before audio starts.
  //
  // If your transmitter requires a different key-up/key-down
  // delay, this is one of the values you can modify.
  delay(250);


  // Old method - intentionally not used.
  // myDFPlayer.play(file);


  // Current working method.
  myDFPlayer.playMp3Folder(file);
}


// =========================================================
// DFPLAYER STATUS HANDLER
// =========================================================
//
// DFPlayer communicates status information asynchronously.
//
// This function checks whether a message is waiting.
//
// When DFPlayer reports DFPlayerPlayFinished:
//
//   - audioFinished is set TRUE.
//   - PTT is released.
//   - external LED is turned OFF.
//
// During an active ON loop, handleLoopingAudio() sees
// audioFinished == TRUE and starts the same file again.
// =========================================================

void checkDFPlayer() {

  // Nothing waiting from DFPlayer.
  if (!myDFPlayer.available()) {
    return;
  }


  // Read the DFPlayer message type.
  uint8_t type = myDFPlayer.readType();


  // -------------------------------------------------------
  // Current audio file has finished.
  // -------------------------------------------------------

  if (type == DFPlayerPlayFinished) {

    // Tell the looping handler that the audio has ended.
    audioFinished = true;


    // Release transmitter PTT.
    digitalWrite(PTT_PIN, LOW);


    // Turn OFF external activity LED.
    ext_led_off();
  }
}


// =========================================================
// OLD / ALTERNATIVE LOOP HANDLER
// =========================================================
//
// This is an older version of the looping handler.
//
// It is NOT called by the active loop().
//
// The active implementation is handleLoopingAudio()
// further below.
//
// This function is retained for reference.
// =========================================================

void handleLoopingAudio1() {

  // Nothing to do when looping is disabled.
  if (!loopingEnabled || loopingFile == 0) {
    return;
  }


  unsigned long now = millis();


  // -------------------------------------------------------
  // ONBOARD LED SLOW BLINK
  // -------------------------------------------------------

  if (now - ledTimer >= 1000) {

    digitalWrite(ledPin, !digitalRead(ledPin));

    ledTimer = now;
  }


  // -------------------------------------------------------
  // ON PHASE
  // -------------------------------------------------------

  if (loopON) {

    // Has the 60-second ON period ended?
    if (now - loopPhaseStart >= LOOP_DURATION) {

      // Release PTT.
      digitalWrite(PTT_PIN, LOW);

      // Turn OFF external LED.
      ext_led_off();

      // Change to OFF phase.
      loopON = false;

      // Start timing the OFF phase.
      loopPhaseStart = now;

      return;
    }


    // -----------------------------------------------------
    // Audio finished before the 60-second ON period ended.
    //
    // Restart the same audio file.
    // -----------------------------------------------------

    if (audioFinished) {

      audioFinished = false;

      digitalWrite(PTT_PIN, HIGH);

      ext_led_on();

      delay(50);

      playVoice(loopingFile);
    }


    return;
  }


  // -------------------------------------------------------
  // OFF PHASE
  // -------------------------------------------------------

  if (!loopON) {

    // Has the 60-second OFF period ended?
    if (now - loopPhaseStart >= LOOP_DURATION) {

      // Change to ON phase.
      loopON = true;

      loopPhaseStart = now;


      // Key transmitter.
      digitalWrite(PTT_PIN, HIGH);

      // Turn ON external LED.
      ext_led_on();

      delay(50);


      // Start the selected audio file.
      playVoice(loopingFile);


      audioFinished = false;
    }
  }
}


// =========================================================
// DTMF COMMAND PROCESSING
// =========================================================
//
// DTMF KEYS USED BY THIS CONTROLLER
//
//   1 -> Loop audio file 9
//   2 -> Loop audio file 10
//   3 -> Loop audio file 11
//
//   4 -> Play audio file 2 once
//   5 -> Play audio file 8 once
//   6 -> Play audio file 7 once
//   7 -> Play audio file 6 once
//
//   9 -> Stop looping / stop transmission
//
// ---------------------------------------------------------
// To change which audio file is associated with a DTMF key,
// modify the number passed to playVoice() or the value
// assigned to loopingFile.
// ---------------------------------------------------------

void processDTMF1(char key) {

  switch (key) {


    // -----------------------------------------------------
    // DTMF 1
    // -----------------------------------------------------
    // Start looping DFPlayer file 9.
    // -----------------------------------------------------

    case '1':

      loopingFile = 9;

      loopingEnabled = true;


      // Start ON phase immediately.
      loopON = true;

      loopPhaseStart = millis();

      audioFinished = false;


      // Turn onboard LED ON initially.
      digitalWrite(ledPin, HIGH);


      // Start audio immediately.
      playVoice(loopingFile);

      break;


    // -----------------------------------------------------
    // DTMF 2
    // -----------------------------------------------------
    // Start looping DFPlayer file 10.
    // -----------------------------------------------------

    case '2':

      loopingFile = 10;

      loopingEnabled = true;


      loopON = true;

      loopPhaseStart = millis();

      audioFinished = false;


      digitalWrite(ledPin, HIGH);

      playVoice(loopingFile);

      break;


    // -----------------------------------------------------
    // DTMF 3
    // -----------------------------------------------------
    // Start looping DFPlayer file 11.
    // -----------------------------------------------------

    case '3':

      loopingFile = 11;

      loopingEnabled = true;


      loopON = true;

      loopPhaseStart = millis();

      audioFinished = false;


      digitalWrite(ledPin, HIGH);

      playVoice(loopingFile);

      break;


    // -----------------------------------------------------
    // DTMF 9
    // -----------------------------------------------------
    // Stop all looping and release PTT.
    // -----------------------------------------------------

    case '9':

      loopingEnabled = false;

      loopingFile = 0;

      loopON = false;


      digitalWrite(ledPin, LOW);

      digitalWrite(PTT_PIN, LOW);

      ext_led_off();

      break;


    default:
      break;
  }
}


// =========================================================
// ACTIVE DTMF COMMAND PROCESSOR
// =========================================================
//
// This is the function actually called by loop().
//
// ---------------------------------------------------------
// AUDIO FILE ASSIGNMENTS
//
// DTMF 1 -> File 9
// DTMF 2 -> File 10
// DTMF 3 -> File 11
//
// DTMF 4 -> File 2
// DTMF 5 -> File 8
// DTMF 6 -> File 7
// DTMF 7 -> File 6
//
// DTMF 9 -> Stop
// =========================================================

void processDTMF(char key) {

  switch (key) {


    // =====================================================
    // DTMF 1 -> LOOP FILE 9
    // =====================================================

    case '1':

      // Select DFPlayer file 9.
      loopingFile = 9;

      // Enable automatic looping.
      loopingEnabled = true;


      // Start the ON phase immediately.
      loopON = true;

      // Start the 60-second ON timer now.
      loopPhaseStart = millis();

      // Assume the newly started audio has not finished.
      audioFinished = false;


      // ---------------------------------------------------
      // Initialize onboard LED blinking.
      //
      // LED starts ON immediately rather than waiting for
      // the first 1-second interval.
      // ---------------------------------------------------

      ledState = true;

      digitalWrite(ledPin, HIGH);

      ledTimer = millis();


      // Start audio immediately.
      playVoice(loopingFile);

      break;


    // =====================================================
    // DTMF 2 -> LOOP FILE 10
    // =====================================================

    case '2':

      // Select DFPlayer file 10.
      loopingFile = 10;

      // Enable automatic looping.
      loopingEnabled = true;


      // Start ON phase immediately.
      loopON = true;

      loopPhaseStart = millis();

      audioFinished = false;


      // Initialize LED blinking.
      ledState = true;

      digitalWrite(ledPin, HIGH);

      ledTimer = millis();


      // Start audio immediately.
      playVoice(loopingFile);

      break;


    // =====================================================
    // DTMF 3 -> LOOP FILE 11
    // =====================================================

    case '3':

      // Select DFPlayer file 11.
      loopingFile = 11;

      // Enable automatic looping.
      loopingEnabled = true;


      // Start ON phase immediately.
      loopON = true;

      loopPhaseStart = millis();

      audioFinished = false;


      // Initialize LED blinking.
      ledState = true;

      digitalWrite(ledPin, HIGH);

      ledTimer = millis();


      // Start audio immediately.
      playVoice(loopingFile);

      break;


    // =====================================================
    // DTMF 4 -> PLAY FILE 2 ONCE
    // =====================================================
    //
    // loopingFile is set to 0 so the automatic looping
    // handler does not treat this as a looping command.
    // =====================================================

    case '4':

      loopingFile = 0;

      loopingEnabled = true;

      loopON = false;


      // Play file 2 once.
      playVoice(2);

      break;


    // =====================================================
    // DTMF 5 -> PLAY FILE 8 ONCE
    // =====================================================

    case '5':

      loopingFile = 0;

      loopingEnabled = true;

      loopON = false;


      // Play file 8 once.
      playVoice(8);

      break;


    // =====================================================
    // DTMF 6 -> PLAY FILE 7 ONCE
    // =====================================================

    case '6':

      loopingFile = 0;

      loopingEnabled = true;

      loopON = false;


      // Play file 7 once.
      playVoice(7);

      break;


    // =====================================================
    // DTMF 7 -> PLAY FILE 6 ONCE
    // =====================================================

    case '7':

      loopingFile = 0;

      loopingEnabled = true;

      loopON = false;


      // Play file 6 once.
      playVoice(6);

      break;


    // =====================================================
    // DTMF 9 -> STOP EVERYTHING
    // =====================================================
    //
    // This disables automatic looping, clears the selected
    // looping file, releases PTT and turns both LEDs OFF.
    // =====================================================

    case '9':

      // Disable automatic audio looping.
      loopingEnabled = false;

      // Clear selected looping file.
      loopingFile = 0;

      // Force loop state to OFF.
      loopON = false;


      // Stop onboard LED blinking.
      ledState = false;

      digitalWrite(ledPin, LOW);


      // Release transmitter PTT.
      digitalWrite(PTT_PIN, LOW);


      // Turn OFF external activity LED.
      ext_led_off();

      break;


    // =====================================================
    // UNKNOWN / UNUSED DTMF KEY
    // =====================================================

    default:

      // No action for unused DTMF keys.
      break;
  }
}


// =========================================================
// ACTIVE AUDIO LOOP HANDLER
// =========================================================
//
// This function implements the automatic:
//
//     60 sec ON
//     60 sec OFF
//
// cycle.
//
// ---------------------------------------------------------
// ON PHASE
// ---------------------------------------------------------
//
// - PTT is active.
// - Audio is playing.
// - If audio finishes before 60 seconds, it is restarted.
//
// ---------------------------------------------------------
// OFF PHASE
// ---------------------------------------------------------
//
// - PTT is released.
// - External LED is OFF.
// - After 60 seconds, the next ON phase begins.
//
// ---------------------------------------------------------
// ONBOARD LED
// ---------------------------------------------------------
//
// While looping is enabled, pin 13 toggles every
// LED_BLINK_MS milliseconds.
// =========================================================

void handleLoopingAudio() {


  // -------------------------------------------------------
  // If looping is not active, there is nothing to do.
  //
  // Also make sure the onboard LED is OFF.
  // -------------------------------------------------------

  if (!loopingEnabled || loopingFile == 0) {

    if (ledState) {

      ledState = false;

      digitalWrite(ledPin, LOW);
    }

    return;
  }


  unsigned long now = millis();


  // =======================================================
  // ONBOARD LED SLOW BLINK
  // =======================================================
  //
  // Toggle LED every LED_BLINK_MS.
  //
  // Default:
  //     1000 ms
  //
  // gives approximately:
  //     1 sec ON
  //     1 sec OFF
  // =======================================================

  if (now - ledTimer >= LED_BLINK_MS) {

    ledTimer = now;

    ledState = !ledState;

    digitalWrite(ledPin, ledState ? HIGH : LOW);
  }


  // =======================================================
  // AUDIO ON PHASE
  // =======================================================

  if (loopON) {


    // -----------------------------------------------------
    // Check whether the 60-second ON period has ended.
    // -----------------------------------------------------

    if (now - loopPhaseStart >= LOOP_DURATION) {

      // Release transmitter PTT.
      digitalWrite(PTT_PIN, LOW);


      // Turn OFF external activity LED.
      ext_led_off();


      // Change from ON phase to OFF phase.
      loopON = false;


      // Start timing the OFF phase.
      loopPhaseStart = now;

      return;
    }


    // -----------------------------------------------------
    // AUDIO FINISHED DURING ON PHASE
    // -----------------------------------------------------
    //
    // If the selected audio file is shorter than 60 seconds,
    // DFPlayer reports DFPlayerPlayFinished.
    //
    // checkDFPlayer() sets audioFinished = true.
    //
    // We then immediately start the same file again.
    // -----------------------------------------------------

    if (audioFinished) {

      // Clear the flag before restarting.
      audioFinished = false;


      // Keep transmitter keyed.
      digitalWrite(PTT_PIN, HIGH);


      // Keep external activity LED ON.
      ext_led_on();


      // Short delay before restarting audio.
      delay(50);


      // Restart the same selected audio file.
      playVoice(loopingFile);
    }


    return;
  }


  // =======================================================
  // AUDIO OFF PHASE
  // =======================================================

  if (!loopON) {


    // -----------------------------------------------------
    // Check whether the 60-second OFF period has ended.
    // -----------------------------------------------------

    if (now - loopPhaseStart >= LOOP_DURATION) {


      // Change from OFF phase to ON phase.
      loopON = true;


      // Start timing the new ON phase.
      loopPhaseStart = now;


      // Key transmitter.
      digitalWrite(PTT_PIN, HIGH);


      // Turn ON external activity LED.
      ext_led_on();


      // Short delay before starting audio.
      delay(50);


      // Start the selected audio file.
      playVoice(loopingFile);


      // Clear previous finished status.
      audioFinished = false;
    }
  }
}


// =========================================================
// EXTERNAL LED CONTROL
// =========================================================
//
// These two functions are kept separate so that the
// external LED can easily be changed later.
//
// ext_led_on()
//     Turns external activity LED ON.
//
// ext_led_off()
//     Turns external activity LED OFF.
// =========================================================

void ext_led_on() {

  digitalWrite(ext_led, HIGH);
}


void ext_led_off() {

  digitalWrite(ext_led, LOW);
}

