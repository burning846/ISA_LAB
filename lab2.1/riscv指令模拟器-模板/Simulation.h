#include <iostream>
#include <stdio.h>
#include <math.h>
#include <cstring>
//#include <io.h>
//#include <process.h>
#include <time.h>
#include <stdlib.h>
#include "Reg_def.h"

/* Opcode */
#define OP_R 51
#define OP_LW 3		//lb, lh, lw, ld
#define OP_I 19
#define OP_ADDIW 27	//addlw
#define OP_JALR 103
#define OP_ECALL 115
#define OP_S 35		//sb, sh, sw, sd
#define OP_SB 99	//beq, bne, blt, bge
#define OP_AUIPC 23
#define OP_LUI 55
#define OP_JAL 111
#define OP_WR 59	//addw, mulw, subw, ect.	

/* Funct3 */
#define F3_ADD 0
#define F3_MUL 0
#define F3_SUB 0
#define F3_SLL 1
#define F3_MULH 1
#define F3_SLT 2
#define F3_XOR 4
#define F3_DIV 4
#define F3_SRL 5
#define F3_SRA 5
#define F3_OR 6
#define F3_REM 6
#define F3_AND 7
#define F3_LB 0
#define F3_LH 1
#define F3_LW 2
#define F3_LD 3
#define F3_ADDI 0
#define F3_SLLI 1
#define F3_SLTI 2
#define F3_XORI 4
#define F3_SRLI 5
#define F3_SRAI 5
#define F3_ORI 6
#define F3_ANDI 7
#define F3_ADDIW 0
#define F3_SLLIW 1
#define F3_SRLIW 5
#define F3_JALR 0
#define F3_ECALL 0
#define F3_SB 0
#define F3_SH 1
#define F3_SW 2
#define F3_SD 3
#define F3_BEQ 0
#define F3_BNE 1
#define F3_BLT 4
#define F3_BGE 5
#define F3_MULW 0
#define F3_ADDW 0
 
/* Funct7 */
#define F7_MUL 1
#define F7_MULW 1
#define F7_ADD 0
#define F7_ADDW 0
#define F7_SUB 32
#define F7_SLL 0
#define F7_MULH 1
#define F7_SLT 0
#define F7_XOR 0
#define F7_DIV 1
#define F7_SRL 0
#define F7_SRA 32
#define F7_OR 0
#define F7_REM 1
#define F7_AND 0
#define F7_SLLI 0
#define F7_SRLI 0
#define F7_SRAI 16
#define F7_ECALL 0

/* ALU_OP */
#define ALU_ADD 0
#define ALU_SUB 1
#define ALU_MUL 2
#define ALU_SLL 3
#define ALU_XOR 4
#define ALU_DIV 5
#define ALU_SRL 6
#define ALU_SRA 7
#define ALU_OR 8
#define ALU_REM 9
#define ALU_AND 10

/* ALU_SRC */
#define SRC_RT 0
#define SRC_IMM 1
#define SRC_PC 2

#define MAX 100000000

//主存
unsigned char memory[MAX]={0};
//寄存器堆
REG reg[32]={0};
//PC
int PC=0;


//各个指令解析段
unsigned int OP=0;
unsigned int fuc3=0,fuc7=0;
unsigned int shamt=0;
unsigned int rs=0,rt=0,rd=0,rs1=0,rs2=0;
unsigned int imm12=0;
unsigned int imm20=0;
unsigned int imm7=0;
unsigned int imm5=0;
unsigned long long immi = 0;
unsigned long long imms = 0;
unsigned long long immsb = 0;
unsigned long long immu = 0;
unsigned long long immuj = 0;



//加载内存
void load_memory();

void simulate();

void IF();

void ID();

void EX();

void MEM();

void WB();


//符号扩展
long long ext_signed(unsigned long long src,int bit);

//获取指定位
unsigned int getbit(int s,int e);

unsigned short mem2(unsigned long long addr)
{
	unsigned short ans = 0;
	memcpy(&ans, &memory[addr], 2);
	return ans;
}
unsigned int mem4(unsigned long long addr)
{
	unsigned int ans = 0;
	memcpy(&ans, &memory[addr], 4);
	return ans;
}
unsigned long long mem8(unsigned long long addr)
{
	unsigned long long ans = 0;
	memcpy(&ans, &memory[addr], 8);
	return ans;
}

unsigned long long getbit(unsigned int inst,int s,int e)
{
	return (unsigned long long)((inst << (31 - s)) >> (31 - s + e));
}

long long ext_signed(unsigned long long src,int bit)
{
	if(src & (1ll << (bit - 1)))
		return (long long)((-1ll << bit) | src);
    return (long long)src;
}

