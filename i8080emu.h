#ifndef __I8080_EMU_H__
#define __I8080_EMU_H__

#include "Arduino.h"

#define HD_READ 4
#define HD_WRITE 12

#define SYSTEM_MEMORY_SIZE 65536  // Size of emulator RAM

#define RXD2 16
#define TXD2 17

#define println_hex(X) (Serial2.println((X > 0x0F ? "0x" : "0x0")+String(X, HEX)))
#define print_hex(X)   (Serial2.print((X > 0x0F ? " 0x" : " 0x0")+String(X, HEX)))

extern volatile uint8_t PC_MEM[];     // Size of RAM for this system

void setup(void);
void loop(void);


#endif
