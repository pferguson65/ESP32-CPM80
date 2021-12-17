// Intel 8080 Emulator for ESP32
//
// Copyright (C) 2021 Patrick Ferguson
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

#include <SPIFFS.h>
#include "i8080emu.h"
#include "i8080_hal.h"
#include "i8080.h"

// Create the memory space that will emulate RAM
volatile uint8_t PC_MEM[SYSTEM_MEMORY_SIZE];

void setup() {
  pinMode(HD_READ, OUTPUT);
  pinMode(HD_WRITE, OUTPUT);

  digitalWrite(HD_READ, LOW);
  digitalWrite(HD_WRITE, LOW);

  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  Serial2.print("\e[2J");

  // Print banner & info
  Serial2.println();
  Serial2.println("Intel 8080 Emulator on ESP32 WROOM-32");
  Serial2.print(SYSTEM_MEMORY_SIZE / 1024);
  Serial2.print("K bytes");
  Serial2.println(" of RAM available");

  i8080_hal_setupSpiffs();

  Serial2.println();
  Serial2.print("Starting RAM tests...");
  uint32_t i;
  for (i = 0; i < SYSTEM_MEMORY_SIZE; i++) {
    PC_MEM[i] = 0xcb;
  }

  // verify memory
  bool memTestPass = true;
  for (i = 0; i < SYSTEM_MEMORY_SIZE; i++) {
    if (PC_MEM[i] != 0xcb) {
      memTestPass = false;
      Serial2.print(" failed at ");
      Serial2.println(i, HEX);
    }
  }

  if (memTestPass == true) {
    Serial2.println("Passed");
  }

  // clear memory to 0
  for (i = 0; i < SYSTEM_MEMORY_SIZE; i++) {
    PC_MEM[i] = 0;
  }

  // Load the bootloader
  i8080_hal_io_output(0x10, 0);  // track=0
  i8080_hal_io_output(0x12, 0);  // sector=0
  i8080_hal_io_output(0x14, 0x00);  // DMA low addr=00
  i8080_hal_io_output(0x15, 0x20);  // DMA high addr=20
  i8080_hal_io_output(0x16, 1);  // read sector into RAM

  i8080_init();
  i8080_jump(0x2000);
}

int cpucycles = 0;
void loop() {
  cpucycles = i8080_instruction();
}
