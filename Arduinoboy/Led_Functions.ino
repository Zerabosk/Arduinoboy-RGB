/*
   showSelectedMode1 turns off the last mode led, turns on the new mode led
   and delays for a period of time to reduce jitter behavior from the mode
   changing too fast.
 */
void showSelectedMode()
{
  digitalWrite(pinStatusLed, LOW);

  switch (memory[MEM_MODE]) {
    case 0: // Mode 1 - LSDJ PC Keyboard mode [Mode 3]
      // Blue
      analogWrite(pinRedLed, 0);
      analogWrite(pinGreenLed, 0);
      analogWrite(pinBlueLed, 255);
      break;
    case 1: // Mode 2 - LSDJ as MIDI Master Sync [Mode 2]
      // Green
      analogWrite(pinRedLed, 0);
      analogWrite(pinGreenLed, 255);
      analogWrite(pinBlueLed, 0);
      break;
    case 2: // Mode 3 - LSDJ as MIDI Slave Sync [Mode 1]
      // Red
      analogWrite(pinRedLed, 255);
      analogWrite(pinGreenLed, 0);
      analogWrite(pinBlueLed, 0);
      break;
    case 3: // Mode 4 - Full MIDI (mGB) [Mode 5]
      // Cyan
      analogWrite(pinRedLed, 0);
      analogWrite(pinGreenLed, 255);
      analogWrite(pinBlueLed, 255);
      break;
    case 4: // Mode 5 - Nanoloop [Mode 4]
      // Yellow
      analogWrite(pinRedLed, 255);
      analogWrite(pinGreenLed, 255);
      analogWrite(pinBlueLed, 0);
      break;
    case 5: // Mode 6 - LSDJ MIDIMAP [Mode 6]
      // Magenta
      analogWrite(pinRedLed, 255);
      analogWrite(pinGreenLed, 0);
      analogWrite(pinBlueLed, 255);
      break;
    case 6: // Mode 7 - LSDJ MIDIOUT [Mode 7]
      // White
      analogWrite(pinRedLed, 255);
      analogWrite(pinGreenLed, 255);
      analogWrite(pinBlueLed, 255);
      break;
  }
  lastMode = memory[MEM_MODE];
  delay(300);
}

void updateVisualSync()
{
    if(!countSyncTime) {
      if(!blinkSwitch[1]) digitalWrite(pinStatusLed,HIGH);
      switch (memory[MEM_MODE]) {
        case 0: // Mode 1 - LSDJ PC Keyboard mode [Mode 3]
         // Blue
         analogWrite(pinRedLed, 0);
         analogWrite(pinGreenLed, 0);
         analogWrite(pinBlueLed, 255);
         break;
       case 1: // Mode 2 - LSDJ as MIDI Master Sync [Mode 2]
         // Green
         analogWrite(pinRedLed, 0);
         analogWrite(pinGreenLed, 255);
         analogWrite(pinBlueLed, 0);
         break;
       case 2: // Mode 3 - LSDJ as MIDI Slave Sync [Mode 1]
         // Red
         analogWrite(pinRedLed, 255);
         analogWrite(pinGreenLed, 0);
         analogWrite(pinBlueLed, 0);
         break;
       case 3: // Mode 4 - Full MIDI (mGB) [Mode 5]
         // Cyan
         analogWrite(pinRedLed, 0);
         analogWrite(pinGreenLed, 255);
         analogWrite(pinBlueLed, 255);
         break;
       case 4: // Mode 5 - Nanoloop [Mode 4]
         // Yellow
         analogWrite(pinRedLed, 255);
         analogWrite(pinGreenLed, 255);
         analogWrite(pinBlueLed, 0);
         break;
       case 5: // Mode 6 - LSDJ MIDIMAP [Mode 6]
         // Magenta
         analogWrite(pinRedLed, 255);
         analogWrite(pinGreenLed, 0);
         analogWrite(pinBlueLed, 255);
         break;
       case 6: // Mode 7 - LSDJ MIDIOUT [Mode 7]
         // White
         analogWrite(pinRedLed, 255);
         analogWrite(pinGreenLed, 255);
         analogWrite(pinBlueLed, 255);
         break;
      }
      blinkSwitch[1]=1;
      blinkSwitchTime[1]=0;
      countSyncLightTime = 0;
    }
    countSyncTime++;
    if(countSyncTime == 24) countSyncTime=0;
}

void updateBlinkLights()
{
  updateBlinkRGB();
  updateStatusLight();
}

void updateBlinkRGB()
{
  if(blinkSwitch[0]) {
    blinkSwitchTime[0]++;
    if(blinkSwitchTime[0] == blinkMaxCount) {
      blinkSwitch[0]=0;
      blinkSwitchTime[0]=0;
      analogWrite(pinRedLed, 0);
      analogWrite(pinGreenLed, 0);
      analogWrite(pinBlueLed, 0);
    }
  }
}

void updateStatusLight()
{
  if(blinkSwitch[1]) {
    blinkSwitchTime[1]++;
    if(blinkSwitchTime[1] == blinkMaxCount) {
      blinkSwitch[1]=0;
      blinkSwitchTime[1]=0;
      digitalWrite(pinStatusLed,LOW);
    }
  }
}

void blinkLight(byte midiMessage)
{
  switch(midiMessage) {
    case 0xE0:
    case 0xE1:
    case 0xE2:
    case 0xE3:
    case 0xE4:
    case 0xB0:
    case 0xB1:
    case 0xB2:
    case 0xB3:
    case 0xB4:
      if(!blinkSwitch[1]) digitalWrite(pinStatusLed,HIGH);
      blinkSwitch[1]=1;
      blinkSwitchTime[1]=0;
      break;
    default:
      break;
  }
}

void updateProgrammerLeds()
{
  if(miscLedTime == miscLedMaxTime) {
    if(sysexProgrammingConnected) {
      miscLedMaxTime = 400;
      blinkRGB();
    } else {
      blinkPurple();
      miscLedMaxTime = 3000;
    }
    miscLedTime=0;
  }
  miscLedTime++;
  updateBlinkLights();
}
 /*
   updateStatusLed should be placed inside of the main loop cycle of a mode function. It counts to a
   certain number to delay the action of turning off the status led, so the blink is visible to the human eye. ;)>
   I guess this could be called the blinking routine.
 */
void updateStatusLed()
{
  if(statusLedIsOn) {                  //is the led on?
    countStatusLedOn++;                //then increment the counter by 1
    if(countStatusLedOn > 3000) {      //if the counter is pretty high
      countStatusLedOn = 0;            //then reset it to zero.
       digitalWrite(pinStatusLed,LOW); //and turn off the status led
       statusLedIsOn  = false;         //and set our "is it on?" to false, cause its off now. ;p

    } else if (statusLedBlink && countStatusLedOn == 1) {  //someone told me to blink, because i was already on
       digitalWrite(pinStatusLed,LOW);                     //so I'll turn off and turn back on later..

    } else if (statusLedBlink && countStatusLedOn > 1000) {//Now that I've waited long enough I'll finish my blink.
       statusLedBlink = false;                             //Turn off the issued blink
       digitalWrite(pinStatusLed,HIGH);                    //... and finally turn back on.
    }
  }
}

 /*
   statusLedOn is the function to call when we want the status led to blink for us.
   all it does is check if its been already asked to turn on, if it has it will set a flag
   to make it blink. Either way it will reset the blink timer and turn on the LED
 */
void statusLedOn()
{
  if(statusLedIsOn) {
    statusLedBlink = true;   //Make it blink even though its already on
  }
  statusLedIsOn  = true;     //This is the flag the updator function looks for to know if its ok to increment the timer and wait to turn off the led
  countStatusLedOn = 0;      //Reset the timer
  digitalWrite(pinStatusLed,HIGH); //Turn on the led
}

/* cute startup sequence */
void startupSequence()
{
  // Define RGB values for each color
  int colors[7][3] = {
    {255, 0, 0},   // Red
    {0, 255, 0},   // Green
    {0, 0, 255},   // Blue
    {255, 255, 0}, // Yellow
    {0, 255, 255}, // Cyan
    {255, 0, 255}, // Magenta
    {255, 255, 255} // White
  };

  for (int cycle = 0; cycle < 2; cycle++) {
    for (int i = 0; i < 7; i++) {
      analogWrite(pinRedLed, colors[i][0]);
      analogWrite(pinGreenLed, colors[i][1]);
      analogWrite(pinBlueLed, colors[i][2]);
      delay(100);
    }
  }

  // Turn off the RGB LED
  analogWrite(pinRedLed, 0);
  analogWrite(pinGreenLed, 0);
  analogWrite(pinBlueLed, 0);
  delay(500);
}
