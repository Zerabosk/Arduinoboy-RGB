/**************************************************************************
 * Name:    Timothy Lamb                                                  *
 * Email:   trash80@gmail.com                                             *
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
/* ***************************************************************************/
/* "Mode" Functions. Deals with changing the setup of arduino.              */
/* ***************************************************************************/

 /*
   setMode will check if the push button is depressed, If it is it will 
   increment the mode number and make sure its in the 
   range 0 to 4 by mod (%). It will then write the mode to memory,
   set the leds to display the mode, and switch the code over to the
   right function.
 */
 

void setMode()
{
  buttonDepressed = digitalRead(pinButtonMode);
  if(!memory[MEM_FORCE_MODE] && buttonDepressed) { //if the button is pressed
    memory[MEM_MODE]++;                           //increment the mode number
    if(memory[MEM_MODE] > (NUMBER_OF_MODES - 1)) memory[MEM_MODE]=0;  //if the mode is greater then 4 it will wrap back to 0
    #ifndef USE_DUE
    if(!memory[MEM_FORCE_MODE]) EEPROM.write(MEM_MODE, memory[MEM_MODE]); //write mode to eeprom if we arnt forcing a mode in the config
    #endif
    showSelectedMode();            //set the LEDS
    switchMode();
  }
}


 /*
   switchMode is only called from setMode. its responsible for
   linking the mode number to its corrisponding function, 
   and then calling that function. function. function.
 */
void switchMode()
{
  switch(memory[MEM_MODE])
  {
    case 0:
      modeLSDJSlaveSyncSetup();
      break;
    case 1:
      modeLSDJMasterSyncSetup();
      break;
    case 2:
      modeLSDJKeyboardSetup();
      break;
    case 3:
      modeNanoloopSetup();
      break;
    case 4:
      modeMidiGbSetup();
      break;
    case 5:
      modeLSDJMapSetup();
      break;
    case 6:
      modeLSDJMidioutSetup();
      break;
  }
}


/* ***************************************************************************/
/* General Global Functions Used in more then one of the modes               */
/* ***************************************************************************/

 /*
   sequencerStart is called when either LSDJ has started to play in master mode, 
   or when a MIDI Start or continue command is received in lsdj slave mode.
   Basically it just resets some counters we use and sets a "start" flag.
 */
 
void sequencerStart()
{
  sequencerStarted = true; //Sequencer has started?
  countSyncPulse = 0;      //Used for status LED, counts 24 ticks (quarter notes)
  countSyncTime = 0;       //Used to count a custom amount of clock ticks (2/4/8) for sync effects
  countSyncLightTime=0;
}

 /*
   sequencerStop is called when either LSDJ has stopped sending sync commands for
   some time in LSDJ Master mode, or when a MIDI Stop command is received in
   lsdj slave mode.
   Basically it just resets some counters we use and sets the "start" flag to false.
 */
void sequencerStop()
{
  midiSyncEffectsTime = false;//Turn off MIDI sync effects in LSDJ slave mode
  sequencerStarted = false;   //Sequencer has started?
  countSyncPulse = 0;         //Used for status LED, counts 24 ticks (quarter notes)
  countSyncTime = 0;          //Used to count a custom amount of clock ticks (2/4/8) for sync effects
  countSyncLightTime=0;
  // Ignore the LEDs for now - Maybe dont need this as we arn't using the RGB for displaying activity.
  // digitalWrite(pinLeds[0],LOW);
  // digitalWrite(pinLeds[1],LOW);
  // digitalWrite(pinLeds[2],LOW);
  // digitalWrite(pinLeds[3],LOW);
  // digitalWrite(pinLeds[memory[MEM_MODE]],HIGH);
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
}
