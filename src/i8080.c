#include <hardware.h>
#include <cpu_utility.h>

// check if cpu is working
bool STOPPED;
bool INTERRUPT_ENABLED;

// registers in 16 bit mode
uint16_t PSW_16[1];
uint16_t B_16[1];
uint16_t D_16[1];
uint16_t H_16[1];

// registers in 8 bit mode
uint8_t* A_8;
uint8_t* F_8;
uint8_t* B_8;
uint8_t* C_8;
uint8_t* D_8;
uint8_t* E_8;
uint8_t* H_8;
uint8_t* L_8;

// other 16 bit registers
uint16_t SP[1];
uint16_t PC[1];

// cycles
uint64_t cycles;

Opcode table[256] = {
//        x0                                              x1                                                  x2                                                x3                                                  x4                                              x5                                                 x6                                               x7                                              x8                                              x9                                                xA                                                 xB                                                 xC                                             xD                                                xE                                               xF     
/* x0 */ {"NOP",     getNULL,       getNULL,    NOP, 1}, {"LXI B,d16",  getSingleReg16, getImm16,   LXI, 3}, {"STAX B",  getSingleReg16, getNULL,    STAX, 1}, {"INX B",   getSingleReg16, getNULL,    INX,    1}, {"INR B",   getSingleReg8, getNULL,    INR, 1}, {"DCR B",    getSingleReg8,  getNULL,    DCR,  1}, {"MVI B,d8", getDstReg8,    getImm8,    MVI, 2}, {"RLC",     getNULL,       getNULL,    RLC, 1}, {"NOP",     getNULL,       getNULL,    NOP, 1}, {"DAD B",   getSingleReg16, getNULL,    DAD,  1}, {"LDAX B",   getSingleReg16, getNULL,    LDAX, 1}, {"DCX B",   getSingleReg16, getNULL,    DCX,   1}, {"INR C",   getSingleReg8, getNULL,    INR, 1}, {"DCR C",    getSingleReg8, getNULL,    DCR,  1}, {"MVI C,d8", getDstReg8,    getImm8,    MVI, 2}, {"RRC",     getNULL,       getNULL,    RRC, 1},
/* x1 */ {"NOP",     getNULL,       getNULL,    NOP, 1}, {"LXI D,d16",  getSingleReg16, getImm16,   LXI, 3}, {"STAX D",  getSingleReg16, getNULL,    STAX, 1}, {"INX D",   getSingleReg16, getNULL,    INX,    1}, {"INR D",   getSingleReg8, getNULL,    INR, 1}, {"DCR D",    getSingleReg8,  getNULL,    DCR,  1}, {"MVI D,d8", getDstReg8,    getImm8,    MVI, 2}, {"RAL",     getNULL,       getNULL,    RAL, 1}, {"NOP",     getNULL,       getNULL,    NOP, 1}, {"DAD D",   getSingleReg16, getNULL,    DAD,  1}, {"LDAX D",   getSingleReg16, getNULL,    LDAX, 1}, {"DCX D",   getSingleReg16, getNULL,    DCX,   1}, {"INR E",   getSingleReg8, getNULL,    INR, 1}, {"DCR E",    getSingleReg8, getNULL,    DCR,  1}, {"MVI E,d8", getDstReg8,    getImm8,    MVI, 2}, {"RAR",     getNULL,       getNULL,    RAR, 1},
/* x2 */ {"NOP",     getNULL,       getNULL,    NOP, 1}, {"LXI H,d16",  getSingleReg16, getImm16,   LXI, 3}, {"SHLD",    getImm16,       getNULL,    SHLD, 3}, {"INX H",   getSingleReg16, getNULL,    INX,    1}, {"INR H",   getSingleReg8, getNULL,    INR, 1}, {"DCR H",    getSingleReg8,  getNULL,    DCR,  1}, {"MVI H,d8", getDstReg8,    getImm8,    MVI, 2}, {"DAA",     getNULL,       getNULL,    DAA, 1}, {"NOP",     getNULL,       getNULL,    NOP, 1}, {"DAD H",   getSingleReg16, getNULL,    DAD,  1}, {"LHLD a16", getImm16,       getNULL,    LHLD, 3}, {"DCX H",   getSingleReg16, getNULL,    DCX,   1}, {"INR L",   getSingleReg8, getNULL,    INR, 1}, {"DCR L",    getSingleReg8, getNULL,    DCR,  1}, {"MVI L,d8", getDstReg8,    getImm8,    MVI, 2}, {"CMA",     getNULL,       getNULL,    CMA, 1},
/* x3 */ {"NOP",     getNULL,       getNULL,    NOP, 1}, {"LXI SP,d16", getSingleReg16, getImm16,   LXI, 3}, {"STA a16", getImm16,       getNULL,    STA,  3}, {"INX SP",  getSingleReg16, getNULL,    INX,    1}, {"INR M",   getSingleReg8, getNULL,    INR, 1}, {"DCR M",    getSingleReg8,  getNULL,    DCR,  1}, {"MVI M,d8", getDstReg8,    getImm8,    MVI, 2}, {"STC",     getNULL,       getNULL,    STC, 1}, {"NOP",     getNULL,       getNULL,    NOP, 1}, {"DAD SP",  getSingleReg16, getNULL,    DAD,  1}, {"LDA a16",  getImm16,       getNULL,    LDA,  3}, {"DCX SP",  getSingleReg16, getNULL,    DCX,   1}, {"INR A",   getSingleReg8, getNULL,    INR, 1}, {"DCR A",    getSingleReg8, getNULL,    DCR,  1}, {"MVI A,d8", getDstReg8,    getImm8,    MVI, 2}, {"CMC",     getNULL,       getNULL,    CMC, 1},
/* x4 */ {"MOV B,B", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV B,C",    getDstReg8,     getSrcReg8, MOV, 1}, {"MOV B,D", getDstReg8,     getSrcReg8, MOV,  1}, {"MOV B,E", getDstReg8,     getSrcReg8, MOV,    1}, {"MOV B,H", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV B,L",  getDstReg8,     getSrcReg8, MOV,  1}, {"MOV B,M",  getDstReg8,    getSrcReg8, MOV, 1}, {"MOV B,A", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV C,B", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV C,C", getDstReg8,     getSrcReg8, MOV,  1}, {"MOV C,D",  getDstReg8,     getSrcReg8, MOV,  1}, {"MOV C,E", getDstReg8,     getSrcReg8, MOV,   1}, {"MOV C,H", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV C,L",  getDstReg8,    getSrcReg8, MOV,  1}, {"MOV C,M",  getDstReg8,    getSrcReg8, MOV, 1}, {"MOV C,A", getDstReg8,    getSrcReg8, MOV, 1},
/* x5 */ {"MOV D,B", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV D,C",    getDstReg8,     getSrcReg8, MOV, 1}, {"MOV D,D", getDstReg8,     getSrcReg8, MOV,  1}, {"MOV D,E", getDstReg8,     getSrcReg8, MOV,    1}, {"MOV D,H", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV D,L",  getDstReg8,     getSrcReg8, MOV,  1}, {"MOV D,M",  getDstReg8,    getSrcReg8, MOV, 1}, {"MOV D,A", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV E,B", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV E,C", getDstReg8,     getSrcReg8, MOV,  1}, {"MOV E,D",  getDstReg8,     getSrcReg8, MOV,  1}, {"MOV E,E", getDstReg8,     getSrcReg8, MOV,   1}, {"MOV E,H", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV E,L",  getDstReg8,    getSrcReg8, MOV,  1}, {"MOV E,M",  getDstReg8,    getSrcReg8, MOV, 1}, {"MOV E,A", getDstReg8,    getSrcReg8, MOV, 1},
/* x6 */ {"MOV H,B", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV H,C",    getDstReg8,     getSrcReg8, MOV, 1}, {"MOV H,D", getDstReg8,     getSrcReg8, MOV,  1}, {"MOV H,E", getDstReg8,     getSrcReg8, MOV,    1}, {"MOV H,H", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV H,L",  getDstReg8,     getSrcReg8, MOV,  1}, {"MOV H,M",  getDstReg8,    getSrcReg8, MOV, 1}, {"MOV H,A", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV L,B", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV L,C", getDstReg8,     getSrcReg8, MOV,  1}, {"MOV L,D",  getDstReg8,     getSrcReg8, MOV,  1}, {"MOV L,E", getDstReg8,     getSrcReg8, MOV,   1}, {"MOV L,H", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV L,L",  getDstReg8,    getSrcReg8, MOV,  1}, {"MOV L,M",  getDstReg8,    getSrcReg8, MOV, 1}, {"MOV L,A", getDstReg8,    getSrcReg8, MOV, 1},
/* x7 */ {"MOV M,B", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV M,C",    getDstReg8,     getSrcReg8, MOV, 1}, {"MOV M,D", getDstReg8,     getSrcReg8, MOV,  1}, {"MOV M,E", getDstReg8,     getSrcReg8, MOV,    1}, {"MOV M,H", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV M,L",  getDstReg8,     getSrcReg8, MOV,  1}, {"HLT",      getNULL,       getNULL,    HLT, 1}, {"MOV M,A", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV A,B", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV A,C", getDstReg8,     getSrcReg8, MOV,  1}, {"MOV A,D",  getDstReg8,     getSrcReg8, MOV,  1}, {"MOV A,E", getDstReg8,     getSrcReg8, MOV,   1}, {"MOV A,H", getDstReg8,    getSrcReg8, MOV, 1}, {"MOV A,L",  getDstReg8,    getSrcReg8, MOV,  1}, {"MOV A,M",  getDstReg8,    getSrcReg8, MOV, 1}, {"MOV A,A", getDstReg8,    getSrcReg8, MOV, 1},
/* x8 */ {"ADD B",   getSrcReg8,    getNULL,    ADD, 1}, {"ADD C",      getSrcReg8,     getNULL,    ADD, 1}, {"ADD D",   getSrcReg8,     getNULL,    ADD,  1}, {"ADD E",   getSrcReg8,     getNULL,    ADD,    1}, {"ADD H",   getSrcReg8,    getNULL,    ADD, 1}, {"ADD L",    getSrcReg8,     getNULL,    ADD,  1}, {"ADD M",    getSrcReg8,    getNULL,    ADD, 1}, {"ADD A",   getSrcReg8,    getNULL,    ADD, 1}, {"ADC B",   getSrcReg8,    getNULL,    ADC, 1}, {"ADC C",   getSrcReg8,     getNULL,    ADC,  1}, {"ADC D",    getSrcReg8,     getNULL,    ADC,  1}, {"ADC E",   getSrcReg8,     getNULL,    ADC,   1}, {"ADC H",   getSrcReg8,    getNULL,    ADC, 1}, {"ADC L",    getSrcReg8,    getNULL,    ADC,  1}, {"ADC M",    getSrcReg8,    getNULL,    ADC, 1}, {"ADC A",   getSrcReg8,    getNULL,    ADC, 1}, 
/* x9 */ {"SUB B",   getSrcReg8,    getNULL,    SUB, 1}, {"SUB C",      getSrcReg8,     getNULL,    SUB, 1}, {"SUB D",   getSrcReg8,     getNULL,    SUB,  1}, {"SUB E",   getSrcReg8,     getNULL,    SUB,    1}, {"SUB H",   getSrcReg8,    getNULL,    SUB, 1}, {"SUB L",    getSrcReg8,     getNULL,    SUB,  1}, {"SUB M",    getSrcReg8,    getNULL,    SUB, 1}, {"SUB A",   getSrcReg8,    getNULL,    SUB, 1}, {"SBB B",   getSrcReg8,    getNULL,    SBB, 1}, {"SBB C",   getSrcReg8,     getNULL,    SBB,  1}, {"SBB D",    getSrcReg8,     getNULL,    SBB,  1}, {"SBB E",   getSrcReg8,     getNULL,    SBB,   1}, {"SBB H",   getSrcReg8,    getNULL,    SBB, 1}, {"SBB L",    getSrcReg8,    getNULL,    SBB,  1}, {"SBB M",    getSrcReg8,    getNULL,    SBB, 1}, {"SBB A",   getSrcReg8,    getNULL,    SBB, 1}, 
/* xA */ {"ANA B",   getSrcReg8,    getNULL,    ANA, 1}, {"ANA C",      getSrcReg8,     getNULL,    ANA, 1}, {"ANA D",   getSrcReg8,     getNULL,    ANA,  1}, {"ANA E",   getSrcReg8,     getNULL,    ANA,    1}, {"ANA H",   getSrcReg8,    getNULL,    ANA, 1}, {"ANA L",    getSrcReg8,     getNULL,    ANA,  1}, {"ANA M",    getSrcReg8,    getNULL,    ANA, 1}, {"ANA A",   getSrcReg8,    getNULL,    ANA, 1}, {"XRA B",   getSrcReg8,    getNULL,    XRA, 1}, {"XRA C",   getSrcReg8,     getNULL,    XRA,  1}, {"XRA D",    getSrcReg8,     getNULL,    XRA,  1}, {"XRA E",   getSrcReg8,     getNULL,    XRA,   1}, {"XRA H",   getSrcReg8,    getNULL,    XRA, 1}, {"XRA L",    getSrcReg8,    getNULL,    XRA,  1}, {"XRA M",    getSrcReg8,    getNULL,    XRA, 1}, {"XRA A",   getSrcReg8,    getNULL,    XRA, 1}, 
/* xB */ {"ORA B",   getSrcReg8,    getNULL,    ORA, 1}, {"ORA C",      getSrcReg8,     getNULL,    ORA, 1}, {"ORA D",   getSrcReg8,     getNULL,    ORA,  1}, {"ORA E",   getSrcReg8,     getNULL,    ORA,    1}, {"ORA H",   getSrcReg8,    getNULL,    ORA, 1}, {"ORA L",    getSrcReg8,     getNULL,    ORA,  1}, {"ORA M",    getSrcReg8,    getNULL,    ORA, 1}, {"ORA A",   getSrcReg8,    getNULL,    ORA, 1}, {"CMP B",   getSrcReg8,    getNULL,    CMP, 1}, {"CMP C",   getSrcReg8,     getNULL,    CMP,  1}, {"CMP D",    getSrcReg8,     getNULL,    CMP,  1}, {"CMP E",   getSrcReg8,     getNULL,    CMP,   1}, {"CMP H",   getSrcReg8,    getNULL,    CMP, 1}, {"CMP L",    getSrcReg8,    getNULL,    CMP,  1}, {"CMP M",    getSrcReg8,    getNULL,    CMP, 1}, {"CMP A",   getSrcReg8,    getNULL,    CMP, 1}, 
/* xC */ {"RNZ",     getNULL,       getNULL,    RNZ, 1}, {"POP B",      getSingleReg16, getNULL,    POP, 1}, {"JNZ a16", getImm16,       getNULL,    JNZ,  3}, {"JMP a16", getImm16,       getNULL,    JMP,    3}, {"CNZ a16", getImm16,      getNULL,    CNZ, 3}, {"PUSH B",   getSingleReg16, getNULL,    PUSH, 1}, {"ADI d8",   getImm8,       getNULL,    ADI, 2}, {"RST 0",   getRST,        getNULL,    RST, 1}, {"RZ",      getNULL,       getNULL,    RZ,  1}, {"RET",     getNULL,        getNULL,    RET,  1}, {"JZ a16",   getImm16,       getNULL,    JZ,   3}, {"JMP a16", getImm16,       getNULL,    JMP,   3}, {"CZ a16",  getImm16,      getNULL,    CZ,  3}, {"CALL a16", getImm16,      getNULL,    CALL, 3}, {"ACI d8",   getImm8,       getNULL,    ACI, 2}, {"RST 1",   getRST,        getNULL,    RST, 1},
/* xD */ {"RNC",     getNULL,       getNULL,    RNC, 1}, {"POP D",      getSingleReg16, getNULL,    POP, 1}, {"JNC a16", getImm16,       getNULL,    JNC,  3}, {"OUT d8",  getImm8,        getNULL,    OP_OUT, 2}, {"CNC a16", getImm16,      getNULL,    CNC, 3}, {"PUSH D",   getSingleReg16, getNULL,    PUSH, 1}, {"SUI d8",   getImm8,       getNULL,    SUI, 2}, {"RST 2",   getRST,        getNULL,    RST, 1}, {"RC",      getNULL,       getNULL,    RC,  1}, {"RET",     getNULL,        getNULL,    RET,  1}, {"JC a16",   getImm16,       getNULL,    JC,   3}, {"IN d8",   getImm8,        getNULL,    OP_IN, 2}, {"CC a16",  getImm16,      getNULL,    CC,  3}, {"CALL a16", getImm16,      getNULL,    CALL, 3}, {"SBI d8",   getImm8,       getNULL,    SBI, 2}, {"RST 3",   getRST,        getNULL,    RST, 1},
/* xE */ {"RPO",     getNULL,       getNULL,    RPO, 1}, {"POP H",      getSingleReg16, getNULL,    POP, 1}, {"JPO a16", getImm16,       getNULL,    JPO,  3}, {"XHTL",    getNULL,        getNULL,    XTHL,   1}, {"CPO a16", getImm16,      getNULL,    CPO, 3}, {"PUSH H",   getSingleReg16, getNULL,    PUSH, 1}, {"ANI d8",   getImm8,       getNULL,    ANI, 2}, {"RST 4",   getRST,        getNULL,    RST, 1}, {"RPE",     getNULL,       getNULL,    RPE, 1}, {"PCHL",    getNULL,        getNULL,    PCHL, 1}, {"JPE a16",  getImm16,       getNULL,    JPE,  3}, {"XCHG",    getNULL,        getNULL,    XCHG,  1}, {"CPE a16", getImm16,      getNULL,    CPE, 3}, {"CALL a16", getImm16,      getNULL,    CALL, 3}, {"XRI d8",   getImm8,       getNULL,    XRI, 2}, {"RST 5",   getRST,        getNULL,    RST, 1},
/* xF */ {"RP",      getNULL,       getNULL,    RP , 1}, {"POP PSW",    getSingleReg16, getNULL,    POP, 1}, {"JP a16",  getImm16,       getNULL,    JP,   3}, {"DI",      getNULL,        getNULL,    DI,     1}, {"CP a16",  getImm16,      getNULL,    CP,  3}, {"PUSH PSW", getSingleReg16, getNULL,    PUSH, 1}, {"ORI d8",   getImm8,       getNULL,    ORI, 2}, {"RST 6",   getRST,        getNULL,    RST, 1}, {"RM",      getNULL,       getNULL,    RM,  1}, {"SPHL",    getNULL,        getNULL,    SPHL, 1}, {"JM a16",   getImm16,       getNULL,    JM,   3}, {"EI",      getNULL,        getNULL,    EI,    1}, {"CM a16",  getImm16,      getNULL,    CM,  3}, {"CALL a16", getImm16,      getNULL,    CALL, 3}, {"CPI d8",   getImm8,       getNULL,    CPI, 2}, {"RST 7",   getRST,        getNULL,    RST, 1}
};

void initCPU(){
    cycles = 0;
    F_8 = (uint8_t*)PSW_16;
    A_8 = F_8 + 1;
    C_8 = (uint8_t*)B_16;
    B_8 = C_8 + 1;
    E_8 = (uint8_t*)D_16;
    D_8 = E_8 + 1;
    L_8 = (uint8_t*)H_16;
    H_8 = L_8 + 1;
    // WARNING: normally registers could have everything inside of it
    // but for clarity, we assume they are empty
    // set everything to 0
    *PSW_16 = 0;
    *B_16 = 0;
    *D_16 = 0;
    *H_16 = 0;
    *SP = 0;
    *PC = 0;
    // set default bit of the flag register
    *F_8 |= 0b10;
    STOPPED = false;
    INTERRUPT_ENABLED = false;
}

void infoCPU(){
    fprintf(stderr, "%llu ", cycles);
    fprintf(stderr, "$%04X\t", *PC);
    fprintf(stderr, "%-10s ", table[MEMORY[*PC]].name);
    fprintf(stderr, "A :0x%02X B: 0x%02X C: 0x%02X ", *A_8, *B_8, *C_8);
    fprintf(stderr, "DE: 0x%04X HL: 0x%04X\t", *D_16, *H_16);
    fprintf(stderr, "S: %d ", (bool)(*F_8 & SET_S));
    fprintf(stderr, "Z: %d ", (bool)(*F_8 & SET_Z));
    fprintf(stderr, "C: %d ", (bool)(*F_8 & SET_C));
    fprintf(stderr, "P: %d ", (bool)(*F_8 & SET_P));
    fprintf(stderr, "A: %d ", (bool)(*F_8 & SET_A));
    fprintf(stderr, "SP: 0x%04X ", *SP);
    fprintf(stderr, "Stack: 0x%04X", *(uint16_t*)(MEMORY+*SP));
    fprintf(stderr, "\n");
}

void stepCPU(){
    uint16_t tmp_ptr = *PC;
    #ifdef DEBUG
        infoCPU();
    #endif
    uint8_t index = MEMORY[*PC];
    *PC = *PC + table[index].byteLength;
    execute(&tmp_ptr); 
}

void execute(uint16_t* ptr){
    uint8_t index = MEMORY[*ptr];
    getArgFunc arg1Getter = table[index].arg1;
    getArgFunc arg2Getter = table[index].arg2; 

    if(arg1Getter == getNULL && arg2Getter == getNULL){
        noArgOpcode opcode = table[index].instruction;
        (*opcode)();
    } else if(arg2Getter == getNULL){
        void* arg = (*arg1Getter)(ptr);
        oneArgOpcode opcode = table[index].instruction;
        (*opcode)(arg);
    } else {
        void* arg1 = (*arg1Getter)(ptr);
        void* arg2 = (*arg2Getter)(ptr);
        twoArgOpcode opcode = table[index].instruction;
        (*opcode)(arg1, arg2);
    }

}

void generateInterrupt(uint8_t val){
    if(INTERRUPT_ENABLED){
        RST(val);
        INTERRUPT_ENABLED = false;
    }
}

// CPU instruction set

// Misc/control instructions
void NOP(){
    cycles += 4;
}

void HLT(){
    STOPPED = true;
    cycles += 7;
}

void OP_IN(uint8_t d8){
    *A_8 = IO_R[d8];
    cycles += 10;
}

void OP_OUT(uint8_t d8){
    IO_W[d8] = *A_8;
    cycles += 10;
}

void DI(){
    INTERRUPT_ENABLED = false;
    cycles += 4;
}

void EI(){
    INTERRUPT_ENABLED = true;
    cycles += 4;
}


// 8bit arithmetic/logical instructions
void STA(uint16_t a16){
    MEMORY[addrMirror(a16)] = *A_8;
    cycles += 3;
}

void STAX(uint16_t* r16){
    MEMORY[addrMirror(*r16)] = *A_8;
    cycles += 7;
}

void MOV(uint8_t* dst, uint8_t* src){
    *dst = *src;
    cycles += 7;
}

void MVI(uint8_t* r8, uint8_t d8){
    *r8 = d8;
    cycles += 10;
}

void LDA(uint16_t a16){
    *A_8 = MEMORY[addrMirror(a16)];
    cycles += 13;
}

void LDAX(uint16_t* r16){
    *A_8 = MEMORY[addrMirror(*r16)];
    cycles += 7;
}


// Jump/calls instructions
void RNZ(){
    if((*F_8 & SET_Z) == 0){
        RET();
        cycles++;
    } else
        cycles += 5;
}

void RNC(){
    if((*F_8 & SET_C) == 0){
        RET();
        cycles++;
    } else
        cycles += 5;
}

void RPO(){
    if((*F_8 & SET_P) == 0){
        RET();
        cycles++;
    } else
        cycles += 5;
}

void RP(){
    if((*F_8 & SET_S) == 0){
        RET();
        cycles++;
    } else
        cycles += 5;
}

void JNZ(uint16_t a16){
    if((*F_8 & SET_Z) == 0)
        JMP(a16);
    else
        cycles += 10;
}

void JNC(uint16_t a16){
    if((*F_8 & SET_C) == 0)
        JMP(a16);
    else
        cycles += 10;
}

void JPO(uint16_t a16){
    if((*F_8 & SET_P) == 0)
        JMP(a16);
    else
        cycles += 10;
}

void JP(uint16_t a16){
    if((*F_8 & SET_S) == 0)
        JMP(a16);
    else
        cycles += 10;
}

void CNZ(uint16_t a16){
    if((*F_8 & SET_Z) == 0)
        CALL(a16);
    else
        cycles += 11;
}

void CNC(uint16_t a16){
    if((*F_8 & SET_C) == 0)
        CALL(a16);
    else
        cycles += 11;
}

void CPO(uint16_t a16){
    if((*F_8 & SET_P) == 0)
        CALL(a16);
    else
        cycles += 11;
}

void CP(uint16_t a16){
    if((*F_8 & SET_S) == 0)
        CALL(a16);
    else
        cycles += 11;
}

void RST(uint8_t addr){
    CALL(addr);
    cycles -= 6;
};

void RZ(){
    if((*F_8 & SET_Z) != 0){
        RET();
        cycles++;
    } else
        cycles += 5;
}

void RC(){
    if((*F_8 & SET_C) != 0){
        RET();
        cycles++;
    } else
        cycles += 5;
}

void RPE(){
    if((*F_8 & SET_P) != 0){
        RET();
        cycles++;
    } else
        cycles += 5;
}

void RM(){
    if((*F_8 & SET_S) != 0) {
        RET();
        cycles++;
    } else
        cycles += 5;
}

void RET(){
    POP(PC);
}

void JZ(uint16_t a16){
    if((*F_8 & SET_Z) != 0)
        JMP(a16);
    else
        cycles += 10;
}

void JC(uint16_t a16){
    if((*F_8 & SET_C) != 0)
        JMP(a16);
    else
        cycles += 10;
}

void JPE(uint16_t a16){
    if((*F_8 & SET_P) != 0)
        JMP(a16);
    else
        cycles += 10;
}

void JM(uint16_t a16){
    if((*F_8 & SET_S) != 0)
        JMP(a16);
    else
        cycles += 10;
}

void JMP(uint16_t a16){
    *PC = addrMirror(a16);
    cycles += 10;
}

void CZ(uint16_t a16){
    if((*F_8 & SET_Z) != 0)
        CALL(a16);
    else
        cycles += 11;
}

void CC(uint16_t a16){
    if((*F_8 & SET_C) != 0)
        CALL(a16);
    else
        cycles += 11;
}

void CPE(uint16_t a16){
    if((*F_8 & SET_P) != 0)
        CALL(a16);
    else
        cycles += 11;
}

void CM(uint16_t a16){
    if((*F_8 & SET_S) != 0)
        CALL(a16);
    else
        cycles += 11;
}

void CALL(uint16_t a16){
    PUSH(PC);
    *PC = addrMirror(a16);
    cycles += 7;
}

// 16bit load/store/move instructions
void POP(uint16_t* r16){
    *r16 = *(uint16_t*)(MEMORY+*SP);
    *SP = *SP + 2;
    cycles += 10;
}

void PUSH(uint16_t* r16){
    *SP = *SP - 2;
    memcpy(&MEMORY[*SP], r16, 2);
    cycles += 11;
}

void LXI(uint16_t* r16, uint16_t d16){
    *r16 = d16;
    cycles += 10;
}

void SHLD(uint16_t a16){
    MEMORY[addrMirror(a16)] = *L_8;
    MEMORY[addrMirror(a16+1)] = *H_8;
    cycles += 16;
}

void LHLD(uint16_t a16){
    *L_8 = MEMORY[addrMirror(a16)];
    *H_8 = MEMORY[addrMirror(a16+1)];
    cycles += 16;
}

void XTHL(){
    uint8_t new_H = MEMORY[addrMirror(*SP+1)];
    uint8_t new_L = MEMORY[addrMirror(*SP)]; 
    MEMORY[addrMirror(*SP+1)] = *H_8;
    MEMORY[addrMirror(*SP)] = *L_8;
    *H_8 = new_H;
    *L_8 = new_L;
    cycles += 18;
}

void SPHL(){
    *SP = *H_16;
    cycles += 5;
}

void XCHG(){
    uint16_t tmp = *D_16;
    *D_16 = *H_16;
    *H_16 = tmp;
    cycles += 5;
}

void PCHL(){
    *PC = *H_16;
    cycles += 5;
}

// 16bit arithmetic/logical instructions
void INX(uint16_t* r16){
    (*r16)++;
    cycles += 5;
}

void DAD(uint16_t* r16){
    bool carry;

    *H_16 += *r16;
    carry = *H_16 < *r16;

    if(carry)
        *F_8 |= SET_C;
    else
        *F_8 &= CLEAR_C;
    cycles += 10;
}

void DCX(uint16_t* r16){
    (*r16)--;
    cycles += 5;
}

// 8bit arithmetic/logical instructions
void INR(uint8_t* m8){
    (*m8)++;
    
    setSign8Bit(*m8);
    setZero(*m8);
    setParity(*m8);
    cycles += 5;
}

void DCR(uint8_t* m8){
    (*m8)--;
    
    setSign8Bit(*m8);
    setZero(*m8);
    setParity(*m8);
    cycles += 5;
}

void RLC(){
    bool carry = *A_8 >> 7;

    *A_8 = (*A_8 << 1) | carry;

    if(carry)
        *F_8 |= SET_C;
    else
        *F_8 &= CLEAR_C;
    cycles += 4;
}

void RAL(){
    bool in_carry = *F_8 & SET_C;
    bool out_carry = *A_8 >> 7;

    *A_8 = (*A_8 << 1) | in_carry;

    if(out_carry)
        *F_8 |= SET_C;
    else
        *F_8 &= CLEAR_C;
    cycles += 4;
}

void RRC(){
    bool carry = *A_8 & 1;
    *A_8 = (*A_8 >> 1) | (carry << 7);

    if(carry)
        *F_8 |= SET_C;
    else
        *F_8 &= CLEAR_C;
    cycles += 4;
}

void RAR(){
    bool in_carry = *F_8 & SET_C;
    bool out_carry = *A_8 & 1;

    *A_8 = (*A_8 >> 1) | (in_carry << 7);

    if(out_carry)
        *F_8 |= SET_C;
    else
        *F_8 &= CLEAR_C;
    cycles += 4;
}

void DAA(){
    uint8_t old_lower = *A_8 & 0x0F;
    uint8_t old_higher = (*A_8 & 0xF0) >> 4;

    if( ((*A_8 & 0xF) > 9) || (*F_8 & SET_A))
        *A_8 += 0x6;

    if(old_lower > (*A_8 & 0xF))
        *F_8 |= SET_A;
    else
        *F_8 &= CLEAR_A;

    if( (((*A_8 & 0xF0) >> 4) > 9) || (*F_8 & SET_C))
        *A_8 += 0x60;

    if(old_higher > ((*A_8 & 0xF0) >> 4))
        *F_8 |= SET_C;

    setSign8Bit(*A_8);
    setParity(*A_8);
    setSign8Bit(*A_8);
    setZero(*A_8);
    cycles += 4;
}

void STC(){
    *F_8 |= SET_C;
    cycles += 4;
}

void CMC(){
    bool carry = (*F_8 & SET_C);
    if(carry)
        *F_8 &= CLEAR_C;
    else
        *F_8 |= SET_C;
    cycles += 4;
}

void CMA(){
    *A_8 = ~(*A_8);
    cycles += 4;
}

void ADD(uint8_t* m8){
    uint8_t old_lower = (*A_8) & 0xF;
    bool out_carry;

    *A_8 += *m8;
    out_carry = *A_8 < *m8;

    if( ((*A_8) & 0xF) < old_lower)
        *F_8 |= SET_A;
    else
        *F_8 &= CLEAR_A;

    if(out_carry)
        *F_8 |= SET_C;
    else
        *F_8 &= CLEAR_C;

    setSign8Bit(*A_8);
    setZero(*A_8);
    setParity(*A_8);
    cycles += 4;
}

void ADC(uint8_t* m8){
    uint8_t old_lower = *A_8 & 0xF;
    bool out_carry = false;
    bool in_carry = *F_8 & SET_C;
    
    *A_8 += *m8 + in_carry;
    out_carry = *A_8 < *m8;

    if( ((*A_8) & 0xF) < old_lower)
        *F_8 |= SET_A;
    else
        *F_8 &= CLEAR_A;

    if(out_carry)
        *F_8 |= SET_C;
    else
        *F_8 &= CLEAR_C;

    setSign8Bit(*A_8);
    setZero(*A_8);
    setParity(*A_8);
    cycles += 4;
}

void SUB(uint8_t* m8){
    uint8_t tmp = (~*m8) + 1;
    ADD(&tmp);
    CMC();
    cycles -= 4;
}

void SBB(uint8_t* m8){
    uint8_t tmp = (~*m8) + 1;
    ADC(&tmp);
    CMC();
    cycles -= 4;   
}

void ANA(uint8_t* m8){
    *A_8 &= *m8;

    *F_8 &= CLEAR_C;
    if((*A_8 & 0x8) | (*m8 & 0x08))
        *F_8 |= SET_A;
    else
        *F_8 &= CLEAR_A;
    setSign8Bit(*A_8);
    setZero(*A_8);
    setParity(*A_8);
    cycles += 4;
}

void ORA(uint8_t* m8){
    *A_8 |= *m8;

    *F_8 &= CLEAR_A;
    *F_8 &= CLEAR_C;
    setSign8Bit(*A_8);
    setZero(*A_8);
    setParity(*A_8);
    cycles += 4;
}

void XRA(uint8_t* m8){
    *A_8 ^= *m8;

    *F_8 &= CLEAR_A;
    *F_8 &= CLEAR_C;
    setSign8Bit(*A_8);
    setZero(*A_8);
    setParity(*A_8);
    cycles += 4;
}

void CMP(uint8_t* m8){
    uint8_t tmp = *A_8;
    SUB(m8);
    *A_8 = tmp;
}

void ADI(uint8_t d8){
    ADD(&d8);
    cycles += 3;
}

void SUI(uint8_t d8){
    SUB(&d8);
    cycles += 3;
}

void ANI(uint8_t d8){
    ANA(&d8);
    cycles += 3;
}

void ORI(uint8_t d8){
    ORA(&d8);
    cycles += 3;
}

void ACI(uint8_t d8){
    ADC(&d8);
    cycles += 3;
}

void SBI(uint8_t d8){
    SBB(&d8);
    cycles += 3;
}

void XRI(uint8_t d8){
    XRA(&d8);
    cycles += 3;
}

void CPI(uint8_t d8){
    CMP(&d8);
    cycles += 3;
}

// arguments getter functions
void* getNULL(uint16_t* ptr){
    return (void*)0;
}

void* getSingleReg8(uint16_t* ptr){
    uint8_t reg = (MEMORY[(*ptr)] & 0b111000) >> 3;
    if(reg == 0b000)
        return (void*)B_8;
    if(reg == 0b001)
        return (void*)C_8;
    if(reg == 0b010)
        return (void*)D_8;
    if(reg == 0b011)
        return (void*)E_8;
    if(reg == 0b100)
        return (void*)H_8;
    if(reg == 0b101)
        return (void*)L_8;
    if(reg == 0b110)
        return (void*)(MEMORY + addrMirror(*H_16));
    if(reg == 0b111)
        return (void*)A_8;
    fprintf(stderr, "ERROR\n");
    return NULL;
}

void* getSrcReg8(uint16_t* ptr){
    uint8_t reg = (MEMORY[(*ptr)] & 0b111);
    if(reg == 0b000)
        return (void*)B_8;
    if(reg == 0b001)
        return (void*)C_8;
    if(reg == 0b010)
        return (void*)D_8;
    if(reg == 0b011)
        return (void*)E_8;
    if(reg == 0b100)
        return (void*)H_8;
    if(reg == 0b101)
        return (void*)L_8;
    if(reg == 0b110)
        return (void*)(MEMORY + addrMirror(*H_16));
    if(reg == 0b111)
        return (void*)A_8;
    fprintf(stderr, "ERROR\n");
    return NULL;
}

void* getDstReg8(uint16_t* ptr){
    return getSingleReg8(ptr);
}

void* getSingleReg16(uint16_t* ptr){
    uint8_t rp = MEMORY[(*ptr)] >> 4;
    if((rp & 0b11) == 0b00)
        return (void*)(B_16);
    if((rp & 0b11) == 0b01)
        return (void*)(D_16);
    if((rp & 0b11) == 0b10)
        return (void*)(H_16);
    if((rp & 0b11) == 0b11){
        if((rp & 0b1100) == 0b0000)
            return (void*)(SP);
        if((rp & 0b1100) == 0b1100)
            return (void*)(PSW_16);
    }
    fprintf(stderr, "ERROR\n");
    return NULL;
}

void* getImm16(uint16_t* ptr){
    // WARNING: IT IS VERY STRANGE BUT IT WORKS
    // UINPTR_T SIZE = 4 BYTE
    // UINT16_T SIZE = 2 BYTE
    uintptr_t d16 = MEMORY[(*ptr)+1] | (MEMORY[(*ptr)+2] << 8);
    return (void*)d16;
}

void* getImm8(uint16_t* ptr){
    // WARNING: IT IS VERY STRANGE BUT IT WORKS
    // UINPTR_T SIZE = 4 BYTE
    // UINT8_T SIZE = 1 BYTE
    uintptr_t d8 = MEMORY[(*ptr)+1];
    return (void*)d8;
}

void* getRST(uint16_t* ptr){
    uintptr_t exp = MEMORY[*ptr] & 0b111000;
    return (void*)exp;
}