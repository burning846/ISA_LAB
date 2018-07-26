#include<iostream>
#include<stdio.h>
#include<math.h>
//#include <io.h>
//#include <process.h>
#include<time.h>
#include<stdlib.h>
#include"Reg_def.h"


#define OP_R 51

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

#define F7_ADD 0
#define F7_MUL 1
#define F7_SUB 32
#define F7_SLL 0
#define F7_MULH 0
#define F7_SLT 0
#define F7_XOR 0
#define F7_DIV 1
#define F7_SRL 0
#define F7_SRA 32
#define F7_OR 1
#define F7_REM 0
#define F7_AND 0

#define F7_MSE 1     //mult, div, remain
#define F7_ADD 0
#define F7_SIGN 32  //sub, sra

#define OP_I 19
#define F3_ADDI 0
#define F3_SLLI 1
#define F3_SLTI 2
#define F3_XORI 4
#define F3_SRLI 5
#define F3_SRAI 5
#define F3_ORI 6
#define F3_ANDI 7

#define F7_SLLI 0
#define F7_SRLI 0
#define F7_SRAI 32

#define OP_SW 35
#define F3_SB 0
#define F3_SH 1
#define F3_SW 2
#define F3_SD 3

#define OP_LW 3
#define F3_LB 0
#define F3_LH 1
#define F3_LW 2
#define F3_LD 3

#define OP_BEQ 99
#define F3_BEQ 0
#define F3_BNE 1
#define F3_BLT 4
#define F3_BGE 5

#define OP_IW 27
#define F3_ADDIW 0

#define OP_RW 59
#define F3_ADDW 0
#define F7_ADDW 0


#define OP_SCALL 115 //ECALL
#define F3_SCALL 0
#define F7_SCALL 0

#define OP_JALR 113
#define F3_JALR 0

#define OP_AUIPC 23
#define OP_LUI 55
#define OP_JAL 111

#define MAX 100000000
#define INST_LEN 32


//主存
unsigned char memory[MAX]={0};
//寄存器堆
REG reg[32]={0};
//PC
int PC=0;


//各个指令解析段
unsigned int OP=0;
unsigned int fuc3=0,fuc7=0;
int shamt=0;
int rs=0,rt=0,rd=0;
unsigned int imm12=0;
unsigned int imm20=0;
unsigned int imm7=0;
unsigned int imm5=0;



//加载内存
void load_memory();

void simulate();

void IF();

void ID();

void EX();

void MEM();

void WB();


//符号扩展
int ext_signed(unsigned int src,int bit);

//获取指定位
unsigned int getbit(unsigned inst, int s,int e);//???

unsigned int getbit(unsigned inst, int s,int e)
{
	if(s>=0 && e<INST_LEN && s<=e){
		unsigned mask = 0xffffffff;
		return (inst>>s) & (mask>>(INST_LEN-e+s-1));
	}
	return 0;
}

//??????
int ext_signed(unsigned int src,int bit)
{

    return 0;
}

unsigned short read_mem_2(unsigned long long address){
	return memory[address] | (memory[address+1]<<8);
}
unsigned int read_mem_4(unsigned long long address){
	return memory[address] | (memory[address+1]<<8) | (memory[address+2]<<16) | (memory[address+3]<<24);
}
unsigned long long read_mem_8(unsigned long long address){
	return memory[address] | (memory[address+1]<<8) | (memory[address+2]<<16) | (memory[address+3]<<24)
	| (memory[address+4]<<32) | (memory[address+5]<<40) | (memory[address+6]<<48) | (memory[address+7]<<56);
}

void write_mem_2(unsigned long long address, unsigned short val){
	memory[address] = (unsigned char)val;
	memory[address+1] = (unsigned char)(val>>8);
}
void write_mem_4(unsigned long long address, unsigned int val){
	memory[address] = (unsigned char)val;
	memory[address+1] = (unsigned char)(val>>8);
	memory[address+2] = (unsigned char)(val>>16);
	memory[address+3] = (unsigned char)(val>>24);
}
void write_mem_8(unsigned long long address, unsigned long long val){
	memory[address] = (unsigned char)val;
	memory[address+1] = (unsigned char)(val>>8);
	memory[address+2] = (unsigned char)(val>>16);
	memory[address+3] = (unsigned char)(val>>24);
	memory[address+4] = (unsigned char)(val>>32);
	memory[address+5] = (unsigned char)(val>>40);
	memory[address+6] = (unsigned char)(val>>48);
	memory[address+7] = (unsigned char)(val>>56);
}
