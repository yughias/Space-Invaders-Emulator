#ifndef _I8080_H_
#define _I8080_H_

// flag masks to set/clear registers
#define SET_S   0b10000000
#define SET_Z   0b01000000
#define SET_A   0b00010000
#define SET_P   0b00000100
#define SET_C   0b00000001
#define CLEAR_S 0b01111111
#define CLEAR_Z 0b10111111
#define CLEAR_A 0b11101111
#define CLEAR_P 0b11111011
#define CLEAR_C 0b11111110

// check if cpu is working
extern bool STOPPED;
extern bool INTERRUPT_ENABLED;

// registers in 16 bit mode
extern uint16_t PSW_16[1];
extern uint16_t B_16[1];
extern uint16_t D_16[1];
extern uint16_t H_16[1];

// registers in 8 bit mode
extern uint8_t* A_8;
extern uint8_t* F_8;
extern uint8_t* B_8;
extern uint8_t* C_8;
extern uint8_t* D_8;
extern uint8_t* E_8;
extern uint8_t* H_8;
extern uint8_t* L_8;

// other 16 bit registers
extern uint16_t SP[1];
extern uint16_t PC[1];

// cycles
extern uint64_t cycles;

typedef struct Opcode {
    char name[15];
    void* (*arg1)(uint16_t*);
    void* (*arg2)(uint16_t*);
    void (*instruction)(void*, void*);
    int byteLength;
} Opcode;

extern Opcode table[256];

void initCPU();
void infoCPU();
void stepCPU();
void execute(uint16_t*);
void generateInterrupt(uint8_t);

// CPU instruction set

// Misc/control instructions
void NOP();
void HLT();
void IN(uint8_t);
void OUT(uint8_t);
void DI();
void EI();

// 8bit load/store/move instructions
void STA(uint16_t);
void STAX(uint16_t*);
void MOV(uint8_t*, uint8_t*);
void MVI(uint8_t*, uint8_t);
void LDA(uint16_t);
void LDAX(uint16_t*);

// Jumps/calls instructions
void RNZ();
void RNC();
void RPO();
void RP();
void JNZ(uint16_t);
void JNC(uint16_t);
void JPO(uint16_t);
void JP(uint16_t);
void CNZ(uint16_t);
void CNC(uint16_t);
void CPO(uint16_t);
void CP(uint16_t);
void RST(uint8_t);
void RZ();
void RC();
void RPE();
void RM();
void RET();
void JZ(uint16_t);
void JC(uint16_t);
void JPE(uint16_t);
void JM(uint16_t);
void JMP(uint16_t);
void CZ(uint16_t);
void CC(uint16_t);
void CPE(uint16_t);
void CM(uint16_t);
void CALL(uint16_t);

// 16bit load/store/move instructions
void POP(uint16_t*);
void PUSH(uint16_t*);
void LXI(uint16_t*, uint16_t);
void SHLD(uint16_t);
void LHLD(uint16_t);
void XTHL();
void SPHL();
void XCHG();
void PCHL();

// 16bit arithmetic/logical instructions
void INX(uint16_t*);
void DAD(uint16_t*);
void DCX(uint16_t*);

// 8bit arithmetic/logical instructions
void INR(uint8_t*);
void DCR(uint8_t*);
void RLC();
void RAL();
void RRC();
void RAR();
void DAA();
void STC();
void CMC();
void CMA();
void ADD(uint8_t*);
void ADC(uint8_t*);
void SUB(uint8_t*);
void SBB(uint8_t*);
void ANA(uint8_t*);
void ORA(uint8_t*);
void XRA(uint8_t*);
void CMP(uint8_t*);
void ADI(uint8_t);
void SUI(uint8_t);
void ANI(uint8_t);
void ORI(uint8_t);
void ACI(uint8_t);
void SBI(uint8_t);
void XRI(uint8_t);
void CPI(uint8_t);

// arguments getter functions;
void* getNULL(uint16_t*);
void* getSingleReg8(uint16_t*);
void* getSrcReg8(uint16_t*);
void* getDstReg8(uint16_t*);
void* getSingleReg16(uint16_t*);
void* getImm16(uint16_t*);
void* getImm8(uint16_t*);
void* getRST(uint16_t*);

#endif