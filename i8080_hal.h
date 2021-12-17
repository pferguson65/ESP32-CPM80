#ifndef I8080_HAL_H
#define I8080_HAL_H

#include <inttypes.h>

extern void i8080_hal_setupSpiffs();

extern uint16_t i8080_hal_memory_read_word(uint16_t addr);
extern void i8080_hal_memory_write_word(uint16_t addr, uint16_t word);

extern uint8_t i8080_hal_memory_read_byte(uint16_t addr);
extern void i8080_hal_memory_write_byte(uint16_t addr, uint8_t byte);

extern uint8_t i8080_hal_io_input(uint8_t port);
extern void i8080_hal_io_output(uint8_t port, uint8_t value);

extern void i8080_hal_iff(uint8_t on);

extern volatile uint8_t* i8080_hal_memory(void);

uint8_t readBlock(uint32_t block, uint8_t* dst);
uint8_t readData(uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst);
uint8_t writeBlock(uint32_t blockNumber, uint8_t* src);
uint8_t writeData(uint32_t block, uint16_t count, uint8_t* src);

#endif
