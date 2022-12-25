/*
   Flight controller DJI virtual flght avata simulator
*/

#include <BleGamepad.h>


#define numOfButtons 7
#define numOfHatSwitches 1
#define enableX true
#define enableY true
#define enableZ true
#define enableRX true
#define enableRY true
#define enableRZ false
#define enableSlider1 false
#define enableSlider2 false
#define enableRudder false
#define enableThrottle false
#define enableAccelerator false
#define enableBrake false
#define enableSteering false

BleGamepad bleGamepad("BLE PPM2Joystick", "freedom2000", 100);


//touchpad
int threshold = 40;
bool touch3detected = false;
long debounce;
long lastBLEtime = 0;
int waitCount = 0;

#define LED_PIN 22
#define MODE_1    //comment this line to activate Mode 2 (throttle left stick) on the radio
#define PPM_3V    //comment this line if PPM signal is 5V TTL 

//PPM
unsigned long microsAtLastPulse = 0;
int currentChannel = 0;

#ifdef PPM_3V
  #define PPM_PIN 12      //pin 12 for PPM input at 3.3V
#else
  #define PPM_PIN 19      //Pin 19 for PPM input at 5V (uses mosfet as level shifter)
#endif
#define NB_CHANNELS 6
#define BLANK_TIME 3500
#define MIN_TIME 700
long rawValues[NB_CHANNELS];

void IRAM_ATTR ppmISR() {
  // Remember the current micros() and calculate the time since the last pulseReceived()
  unsigned long previousMicros = microsAtLastPulse;
  microsAtLastPulse = micros();
  unsigned long pulseDuration = microsAtLastPulse - previousMicros;

  if (pulseDuration < MIN_TIME)
  {
    microsAtLastPulse = previousMicros; //cancel the pulse
  }
  else if (pulseDuration > BLANK_TIME)
  {
    currentChannel = 0; // Blank detected: restart from channel 1
    digitalWrite (LED_PIN, !digitalRead(LED_PIN));
  }
  else
  {
    // Store times between pulses as channel values
    if (currentChannel < NB_CHANNELS)
    {
      if ((pulseDuration > 900) && (pulseDuration < 2500))
      {
        pulseDuration = constrain(pulseDuration, 1000, 2000);
        rawValues[currentChannel] = pulseDuration;
      }
    }
    currentChannel++ ;
  }
}

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(PPM_PIN, INPUT);
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BleGamepadConfiguration bleGamepadConfig;
  bleGamepadConfig.setAutoReport(false);
  bleGamepadConfig.setControllerType(CONTROLLER_TYPE_GAMEPAD); // CONTROLLER_TYPE_JOYSTICK, CONTROLLER_TYPE_GAMEPAD (DEFAULT), CONTROLLER_TYPE_MULTI_AXIS
  bleGamepadConfig.setButtonCount(numOfButtons);
  bleGamepadConfig.setWhichAxes(enableX, enableY, enableZ, enableRX, enableRY, enableRZ, enableSlider1, enableSlider2);      // Can also be done per-axis individually. All are true by default
  bleGamepadConfig.setWhichSimulationControls(enableRudder, enableThrottle, enableAccelerator, enableBrake, enableSteering); // Can also be done per-control individually. All are false by default
  bleGamepadConfig.setHatSwitchCount(numOfHatSwitches);                                                                      // 1 by default
  // Some non-Windows operating systems and web based gamepad testers don't like min axis set below 0, so 0 is set by default
  bleGamepadConfig.setAxesMin(-510); // -32767 --> int16_t - 16 bit signed integer - Can be in decimal or hexadecimal
  bleGamepadConfig.setAxesMax(510); // 32767 --> int16_t - 16 bit signed integer - Can be in decimal or hexadecimal
  //  bleGamepadConfig.setVid(0x045E);  //Xbox360 VID, PID --> does NOT work with DJI avata....
  //  bleGamepadConfig.setPid(0x028E);

  // Can also enable special buttons individually with the following <-- They are all disabled by default
  bleGamepadConfig.setIncludeStart(true);
  bleGamepadConfig.setIncludeSelect(true);
  bleGamepadConfig.setIncludeMenu(true);
  bleGamepadConfig.setIncludeHome(true);
  bleGamepadConfig.setIncludeBack(true);
  // bleGamepadConfig.setIncludeVolumeInc(true);
  // bleGamepadConfig.setIncludeVolumeDec(true);
  // bleGamepadConfig.setIncludeVolumeMute(true);

  bleGamepad.begin(&bleGamepadConfig);
  // The default bleGamepad.begin() above enables 16 buttons, all axes, one hat, and no simulation controls or special buttons


  //Set x and y axes and rudder to center
  bleGamepad.setX(0);
  bleGamepad.setY(0);

  //PPM
  attachInterrupt(PPM_PIN, ppmISR, RISING);
}

void loop()
{
  //if (bleGamepad.isConnected())
  {
    if ((millis() - lastBLEtime) > 20)
    {
      //bleGamepad.press(BUTTON_1); //C1 button
      //bleGamepad.press(BUTTON_2); //Sart button
      //bleGamepad.press(BUTTON_3); //Pause/home button
      //bleGamepad.press(BUTTON_4); //photo button
      //bleGamepad.press(BUTTON_5); //gimbal Up
      //bleGamepad.press(BUTTON_6); //gimbal Down. And buttons 5 and 6 released = gimbal center
      //bleGamepad.pressHome(); //exit App releaseHome();
      if ((rawValues[5] - 1500) > 250) //trainer button toggle
      {
        bleGamepad.press(BUTTON_7);     //toggle Manual to S
      }
      else bleGamepad.release(BUTTON_7);
      //map(value, fromLow, fromHigh, toLow, toHigh)
      bleGamepad.setX(map(rawValues[3], 1000, 2000, -660, 660)); //Right Stick horizontal
#ifdef MODE_1
      bleGamepad.setY(-map(rawValues[2], 1000, 2000, -660, 660)); //Right Stick vertical
      bleGamepad.setZ(map(rawValues[1], 1000, 2000, -660, 660));  //Left Stick vertical
#else
      bleGamepad.setY(map(rawValues[2], 1000, 2000, -660, 660));
      bleGamepad.setZ(-map(rawValues[1], 1000, 2000, -660, 660));
#endif
      bleGamepad.setRX(map(rawValues[0], 1000, 2000, -660, 660)); //Left Stick horizontal
      bleGamepad.setRY(-map(rawValues[4], 1000, 2000, -660, 660)); //gimbal on 3 states switch
      //bleGamepad.setRZ((rawValues[5] - 1500));
      bleGamepad.sendReport();
            Serial.print(map(rawValues[0],1000, 2000, -660, 660));
            Serial.print("\t");
            Serial.print(map(rawValues[1],1000, 2000, -660, 660));
            Serial.print("\t");
            Serial.print(map(rawValues[2],1000, 2000, -660, 660));
            Serial.print("\t");
            Serial.print(map(rawValues[3],1000, 2000, -660, 660));
            Serial.print("\t");
            Serial.print(map(rawValues[4],1000, 2000, -660, 660));
            Serial.print("\t");
            Serial.println(map(rawValues[5],1000, 2000, -660, 660));

      lastBLEtime = millis();
    }
  }
}
