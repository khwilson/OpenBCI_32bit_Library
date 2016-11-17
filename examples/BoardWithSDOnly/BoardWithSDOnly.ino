#include <DSPI.h>
#include <OBCI32_SD.h>
#include <EEPROM.h>
#include <OpenBCI_32bit_Library.h>
#include <OpenBCI_32bit_Library_Definitions.h>

// Booleans Required for SD_Card_Stuff.ino
boolean addAccelToSD = false; // On writeDataToSDcard() call adds Accel data to SD card write
boolean addAuxToSD = false; // On writeDataToSDCard() call adds Aux data to SD card write
boolean SDfileOpen = false; // Set true by SD_Card_Stuff.ino on successful file open

void setup() {
  // Bring up the OpenBCI Board
  board.begin();

  // Notify the board we want to use accel data
  board.useAccel = true;
}

void loop() {
  byte* singlePacket[33];

  if (board.streaming) {
    if (board.channelDataAvailable) {
      // Read from the ADS(s), store data, set channelDataAvailable flag to false
      board.updateChannelData();

      // Check to see if accel has new data
      if(board.accelHasNewData()) {
        // Get new accel data
        board.accelUpdateAxisData();

        // Tell the SD_Card_Stuff.ino to add accel data in the next write to SD
        addAccelToSD = true; // Set false after writeDataToSDcard()
      }

      board.writeChannelDataToBuffer(singlePacket, board.timeSynced);
      if (SDfileOpen) {
        writeBufferToSD(singlePacket);
      }
      if (board.writeToSerial) {
        for (int i=0; i<33; ++i) {
          Serial0.write(singlePacket[i]);
        }
      }
    }
  }
  // Check serial 0 for new data
  if (board.hasDataSerial0()) {
    // Read one char from the serial 0 port
    char newChar = board.getCharSerial0();

    // Send to the sd library for processing
    sdProcessChar(newChar);

    // Send to the board library
    board.processChar(newChar);
  }
}
