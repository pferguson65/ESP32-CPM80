#ifndef I8080_H
#define I8080_H

extern void i8080_init(void);
extern uint8_t i8080_instruction(void);

extern void i8080_jump(uint16_t addr);

extern uint16_t i8080_pc(void);
extern uint16_t i8080_regs_bc(void);
extern uint16_t i8080_regs_de(void);
extern uint16_t i8080_regs_hl(void);
extern uint16_t i8080_regs_sp(void);

extern uint8_t i8080_regs_a(void);
extern uint8_t i8080_regs_b(void);
extern uint8_t i8080_regs_c(void);
extern uint8_t i8080_regs_d(void);
extern uint8_t i8080_regs_e(void);
extern uint8_t i8080_regs_h(void);
extern uint8_t i8080_regs_l(void);

#endif
