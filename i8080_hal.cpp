// Intel 8080 (KR580VM80A) microprocessor core model
//
// Copyright (C) 2012 Alexander Demin <alexander@demin.ws>
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// Modified in 2021 by Patrick Ferguson for use on an Espressif ESP32

#include "SPIFFS.h"
#include "i8080_hal.h"
#include "i8080emu.h"

uint8_t fileBuffer[512];  // file IO buffer. Used for transferring data to/from disk files.

File bootDisk;
char currentDisk[7] = {'/','d','i','s','k','0',00};
char diskptr[10] = {'0','1','2','3','4','5','6','7','8','9'};

void i8080_hal_setupSpiffs() {
  if (!SPIFFS.begin(true)) {
    Serial2.println("Mount failed. Restarting in 2 seconds");
    delay(2000);
    ESP.restart();
  } else {
    delay(500);
  }
}

uint16_t i8080_hal_memory_read_word(uint16_t addr) {
  return
    (i8080_hal_memory_read_byte(addr + 1) << 8) |
    i8080_hal_memory_read_byte(addr);
}

void i8080_hal_memory_write_word(uint16_t addr, uint16_t word) {
  i8080_hal_memory_write_byte(addr, word & 0xff);
  i8080_hal_memory_write_byte(addr + 1, (word >> 8) & 0xff);
}

uint8_t i8080_hal_memory_read_byte(uint16_t addr) {
  uint8_t data;
  data = PC_MEM[addr];
  return data;
}

void i8080_hal_memory_write_byte(uint16_t addr, uint8_t byte) {
  PC_MEM[addr & 0xffff] = byte;
}

void ReadRDSector(unsigned long RDS, byte *fileBuffer) {
  if (int error = readBlock(RDS, fileBuffer) == 0) {
    Serial2.print("Disk read error: ");
    Serial2.println(error, HEX);
  }
}

uint8_t readBlock(uint32_t block, uint8_t* dst) {
  return readData(block, 0, 512, dst);
}

uint8_t readData(uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst) {
  if (count == 0) {
    return true;
  }
  if ((count + offset) > 512) {
    return false;
  }

  if (SPIFFS.exists(currentDisk)) {
    bootDisk = SPIFFS.open(currentDisk, "r");
    if (!bootDisk) {
      Serial2.print("Invalid Drive Specified: ");
      Serial2.println(currentDisk);
      return false;
    }
  } else {
    Serial2.print("Drive is empty: ");
    Serial2.println(currentDisk);

    return false;
  }

  uint32_t startingPos = (block * count) + offset;
  uint8_t data = 0;

  bootDisk.seek(startingPos, SeekSet);
  for (uint16_t i = 0; i < count; i++ ) {
    digitalWrite(HD_READ, HIGH);
    data = bootDisk.read();
    dst[i] = data;
    digitalWrite(HD_READ, LOW);
  }
  bootDisk.close();
  return true;
}

void WriteRDSector(unsigned long RDS, uint8_t *fileBuffer) {
  if (int error = writeBlock(RDS, fileBuffer) == 0) {
    Serial2.print("Disk write error: ");
    Serial2.println(error, HEX);
  }
}

uint8_t writeBlock(uint32_t blockNumber, uint8_t* src) {
  return writeData(blockNumber, 512, src);
}

uint8_t writeData(uint32_t block, uint16_t count, uint8_t* src) {
  uint8_t data = 0;
  uint32_t startingPos = block * count;

  if (SPIFFS.exists(currentDisk)) {
    bootDisk = SPIFFS.open(currentDisk, "r +");
    if (!bootDisk) {
      Serial2.print("Invalid Drive Specified: ");
      Serial2.println(currentDisk);
      return false;
    }
  } else {
    Serial2.print("No Disk in  ");
    Serial2.println(currentDisk);

    return false;
  }

  bootDisk.seek(startingPos, SeekSet);
  for (uint16_t i = 0; i < count; i++ ) {
    digitalWrite(HD_WRITE, HIGH);
    data = src[i];
    bootDisk.write(data);
    digitalWrite(HD_WRITE, LOW);
  }
  bootDisk.close();
  return true;
}

uint8_t i8080_hal_io_input(uint8_t port) {
  uint8_t inputByte = 0;

  switch (port) {
    case 0x00:
      if (Serial2.available()) {
        return 0xff;
      }
      return 0x00;

    case 0x01:
      while (!Serial2.available()) ;
      inputByte = Serial2.read();
      return (inputByte & 0x7F);  // return the character, stripping MSB

    case 0x3:
      if (Serial.available()) {
        return 0xff;
      }
      return 0x00;

    case 0x04:
      while (!Serial.available()) ;
      inputByte = Serial.read();
      return (inputByte);  // return the character, not stripping MSB

    case 0x06:  // 2SIO Port A Data Register
      while (!Serial.available()) ;
      inputByte = Serial.read();
      return (inputByte);  // return the character, not stripping MSB

    case 0x7:  // 2SIO Port A Control Register
      if (Serial.available()) {
        return 0xE0;    // character ready
      }
      return 0x60;    // character not ready

    case 0x08:  // 2SIO Port A Data Register
      while (!Serial.available()) ;
      inputByte = Serial.read();
      return (inputByte);  // return the character, not stripping MSB

    case 0x9:  // 2SIO Port A Control Register
      if (Serial.available()) {
        return 0xE0;    // character ready
      }
      return 0x60;    // character not ready
  }
  return 0;
}

void i8080_hal_io_output(uint8_t port, uint8_t value) {
  byte temp = 0;
  static int DMAAddr = 0;
  static byte track = 0;
  static byte sector = 0;
  unsigned long TotalOffset;
  unsigned int BlockOffset;
  unsigned long RDBlockNumber;

  switch (port) {
    case 0x02:  // console ouput
      Serial2.write(value);
      break;

    case 0x05:  // port A & B output
      Serial.write(value);
      break;

    case 0x06:  // 2SIO Port A Data Register
      Serial.write(value);
      break;

    case 0x07:
      break;

    case 0x08:  // 2SIO Port B Data Register
      Serial.write(value);
      break;

    case 0x09:
      break;

    case 0x10:  // set track
      track = value;
      break;

    case 0x11:  // set Active Disk
      if (value < 10) {
        currentDisk[5] = diskptr[value];
      }
      break;

    case 0x12:  // set sector
      sector = value;
      break;

    case 0x14:  // DMA low byte
      DMAAddr = (DMAAddr & 0xff00) | value;
      break;

    case 0x15:  // DMA high byte
      DMAAddr = (DMAAddr & 0x00ff) | (value << 8);
      break;

    case 0x16:  // DMA transfer
      // TotalOffset is the offset from track 0 sector 0
      TotalOffset = (track * 0xD00) + (sector * 0x80);
      // BlockOffset is the offset within the given block number
      BlockOffset = TotalOffset % 0x200;
      // RDBlockNumber is the RAM Disk block number from the given track & sector
      RDBlockNumber = TotalOffset / 0x200;

      // Read a 512 byte SD block into emulator memory
      ReadRDSector(RDBlockNumber, fileBuffer);

      switch (value) {
        case 1:  // 1 = read from disk (SPIFF Filesystem Disk)
          for (byte i = 0; i < 0x80; i++) {
            PC_MEM[DMAAddr + i] = fileBuffer[BlockOffset + i];
          }
          break;

        case 2:  // 2 = write to disk (SPIFF Filesystem Disk)
          for (byte i = 0; i < 0x80; i++) {
            fileBuffer[BlockOffset + i] = PC_MEM[DMAAddr + i];
          }
          WriteRDSector(RDBlockNumber, fileBuffer);   //  write the buffer back to the SPIFFS file
          break;

        default:
          Serial.println("Unknown IO operation requested");
      }
      break;

    default:
      Serial2.print("Unhandled write: ");
      Serial2.print(value, HEX);
      Serial2.print(" to port: ");
      Serial2.println(port, HEX);
      {}
  }
}

void i8080_hal_iff(uint8_t on) {
  // Nothing.
}

volatile uint8_t* i8080_hal_memory(void) {
  return &PC_MEM[0];
}
