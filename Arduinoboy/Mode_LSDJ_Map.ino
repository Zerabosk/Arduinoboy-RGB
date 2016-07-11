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

void modeLSDJMapSetup()
{
  digitalWrite(pinStatusLed,LOW);
  pinMode(pinGBClock,OUTPUT);
  digitalWrite(pinGBClock, HIGH);
 #ifdef MIDI_INTERFACE
  usbMIDI.setHandleRealTimeSystem(usbMidiLSDJMapRealtimeMessage);
 #endif
  blinkMaxCount=1000;
  modeLSDJMap();
}

void modeLSDJMap()
{
  while(1){  //Loop forever

  modeLSDJMapUsbMidiReceive();
  checkMapQueue();
  if (serial->available()) {                 //If MIDI Byte Availaibleleleiel
    incomingMidiByte = serial->read();           //Read it

    checkForProgrammerSysex(incomingMidiByte);

    if(incomingMidiByte & 0x80) {                //If we have received a MIDI Status Byte
      switch (incomingMidiByte) {
        case 0xF8:
          setMapByte(0xFF);
          usbMidiSendRTMessage(incomingMidiByte);
          break;
        case 0xFA:                                // Case: Transport Start Message
        case 0xFB:                                // and Case: Transport Continue Message
          sequencerStart();                       // Start the sequencer
          usbMidiSendRTMessage(incomingMidiByte);
          break;
        case 0xFC:                                // Case: Transport Stop Message
          sequencerStop();
          setMapByte(0xFE);
          usbMidiSendRTMessage(incomingMidiByte);
          break;
        default:
          midiData[0] = incomingMidiByte;
          midiNoteOnMode = true;

          if(midiData[0] == (0x90+memory[MEM_LIVEMAP_CH])
             || midiData[0] == (0x90+(memory[MEM_LIVEMAP_CH]+1))) resetMapCue();
        }
    } else if(midiNoteOnMode) {   //if we've received a message thats not a status and our note capture mode is true

      midiNoteOnMode = false;
      midiData[1] = incomingMidiByte;

      usbMidiSendTwoByteMessage(midiData[0],midiData[1]);
      if(midiData[0] == (0x90+memory[MEM_LIVEMAP_CH])
         || midiData[0] == (0x90+(memory[MEM_LIVEMAP_CH]+1))) resetMapCue();

    } else {
      midiNoteOnMode = true;
      if(midiData[0] == (0x90+memory[MEM_LIVEMAP_CH])
        || midiData[0] == (0x90+(memory[MEM_LIVEMAP_CH]+1))) {
          if(incomingMidiByte) {
              if(midiData[0] == (0x90+(memory[MEM_LIVEMAP_CH]+1))) {
                  setMapByte(128+midiData[1]);
              } else {
                  setMapByte(midiData[1]);
              }
          } else {
              setMapByte(0xFE);
          }
      } else if (midiData[0] == (0x80+memory[MEM_LIVEMAP_CH])
                 || midiData[0] == (0x80+(memory[MEM_LIVEMAP_CH]+1))) {
          setMapByte(0xFE);
      }
      usbMidiSendThreeByteMessage(midiData[0], midiData[1], incomingMidiByte);
      checkMapQueue();
    }
  } else {
    setMode();         //Check if the mode button was depressed
    updateStatusLight();
    checkMapQueue();
    updateBlinkLights();
  }
  }
}

void setMapByte(uint8_t b)
{
    switch(b) {
      case 0xFF:
        setMapQueueMessage(0xFF);
        break;
      case 0xFE:
        if(!sequencerStarted) {
            sendByteToGameboy(0xFE);
        } else if (mapCurrentRow >= 0) {
            setMapQueueMessage(mapCurrentRow);
        }
        break;
      default:
        mapCurrentRow = b;
        sendByteToGameboy(b);
        resetMapCue();
    }
}

void setMapQueueMessage(uint8_t m)
{
    if(mapQueueMessage == -1 || mapQueueMessage == 0xFF) {
        mapQueueTime=micros()+mapQueueWait;
        mapQueueMessage=m;
    }
}

void resetMapCue()
{
    mapQueueMessage=-1;
}

void checkMapQueue()
{
  if(mapQueueMessage >= 0 && micros()>mapQueueTime) {
      if(mapQueueMessage == 0xFF) {
          sendByteToGameboy(mapQueueMessage);
      } else {
          if(mapQueueMessage == 0xFE || mapCurrentRow == mapQueueMessage) {
              // Only kill playback if the row is the last one that's been played.
              mapCurrentRow = -1;
              sendByteToGameboy(0xFE);
          }
      }
      mapQueueMessage=-1;
      updateVisualSync();
  }
}


void usbMidiLSDJMapRealtimeMessage(uint8_t message)
{
    switch(message) {
      case 0xF8:
        setMapByte(0xFF);
      break;
      case 0xFA:                                // Case: Transport Start Message
      case 0xFB:                                // and Case: Transport Continue Message
        sequencerStart();                     // Start the sequencer
      break;
      case 0xFC:                                // Case: Transport Stop Message
        sequencerStop();                        // Stop the sequencer
        setMapByte(0xFE);
      break;
    }
}

void modeLSDJMapUsbMidiReceive()
{
#ifdef MIDI_INTERFACE

    while(usbMIDI.read()) {
        uint8_t ch = usbMIDI.getChannel() - 1;
        if(ch != memory[MEM_LIVEMAP_CH] && ch != (memory[MEM_LIVEMAP_CH] + 1)){
            continue;
        }

        switch(usbMIDI.getType()) {
            case 0: // note off
                setMapByte(0xFE);
            break;
            case 1: // note on
                if(ch == (memory[MEM_LIVEMAP_CH] + 1)) {
                    setMapByte(128+usbMIDI.getData1());
                } else {
                    setMapByte(usbMIDI.getData1());
                }
            break;
            /*
            case 3: // CC
            break;
            case 4: // PG
            break;
            case 5: // AT
            break;
            case 6: // PB
            break;
            */
        }
    }
#endif
}
