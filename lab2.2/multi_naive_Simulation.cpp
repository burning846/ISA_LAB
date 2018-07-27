#include "Simulation.h"
#include <iomanip>
using namespace std;

extern bool read_elf(char* file_name);
extern unsigned int cadr;	//code
extern unsigned int dadr;	//data
extern unsigned int badr;	//bss
extern unsigned int csize;
extern unsigned int dsize;
extern unsigned int bsize;
extern unsigned int vadr;		//code
extern unsigned int dvadr;		//data
extern unsigned long long gp;
extern unsigned long long gb;
extern unsigned int madr;
extern unsigned int endPC;
extern unsigned int entry;
extern FILE *file;

//#define DEBUG
//#define STEP

//指令运行数
long long inst_num=0;
long long cycle = 0;
double CPI = 0.0;

//系统调用退出指示
int exit_flag=0;

//quit
int quit = 0;

//debug mode
int debug = 0;

int load_file = 0;
char file_name[256];

//加载代码段
//初始化PC
void load_memory()
{
	memset(memory, 0, sizeof(memory));

	fseek(file,cadr,SEEK_SET);
	fread(&memory[vadr],1,csize,file);
	fseek(file,dadr,SEEK_SET);
	fread(&memory[dvadr],1,dsize,file);
	fseek(file,badr,SEEK_SET);
	fread(&memory[gb],1,bsize,file);

	fclose(file);
}

void shell()
{
	char cmd[256];
	while(1)
	{
		printf("$ ");
		cin >> cmd;
		if(strcmp(cmd, "m") == 0)
		{
			int s, num;
			cin >> s >> num;
			printf("check memory from 0x%x to 0x%x\n", s, s+num);
			for(int i = 0; i < num; ++i)
			{
				if(i % 16 == 0)
					printf("%x: ", s+i);
				printf("%02x ", memory[s+i]);
				if(i % 16 == 15)
					printf("\n");
			}
			printf("\n");
		}
		else if(strcmp(cmd, "q") == 0)
		{
			quit = 1;
			break;
		}
		else if(strcmp(cmd, "l") == 0)
		{
			cin >> file_name;
			if(read_elf(file_name))
			{
				load_file = 1;
				printf("file loaded\n");
			}
		}
		else if(strcmp(cmd, "d") == 0)
		{
			debug = !debug;
			if(debug)
			{
				printf("step mode\n");
			}
			else
			{
				printf("step mode canceled\n");
			}
		}
		else if(strcmp(cmd, "r") == 0)
		{
			if(!load_file)
			{
				printf("No file loaded!\n");
				continue;
			}
			printf("the program begin to run\n");
			break;
		}
		else
		{
			printf("Invalid command!\n");	
		}
	}
}

void step(){
	char cmd[256];
	while(1)
	{
		printf(">>");
		cin >> cmd;
		if(strcmp(cmd, "m") == 0)
		{
			int s, num;
			cin >> s >> num;
			printf("check memory from 0x%x to 0x%x\n", s, s+num);
			for(int i = 0; i < num; ++i)
			{
				if(i % 16 == 0)
					printf("%x: ", s+i);
				printf("%02x ", memory[s+i]);
				if(i % 16 == 15)
					printf("\n");
			}
			printf("\n");
		}
		else if(strcmp(cmd, "r") == 0)
		{
			for(int i = 0; i < 32; ++i)
			{
				printf("reg[%d]: %x\n", i, reg[i]);
			}
		}
		else if(strcmp(cmd, "c") == 0)
		{
			printf("continue...\n");
			printf("Fetch instruction at %x\n", PC);
			break;
		}
		else if(strcmp(cmd, "g") == 0)
		{
			debug = 0;
			break;
		}
		else if(strcmp(cmd, "k") == 0)
		{
			exit_flag = 1;
			printf("kill the program\n");
			break;
		}
		else
		{
			printf("Invalid command!\n");
		}
	}
	
}

int main(int argc, char** argv)
{
	if(argc > 1)
	{
		if(read_elf(argv[1]))
		{
			load_file = 1;
			printf("file loaded\n");
		}
	}
	while(1)
	{
		shell();
		if(quit == 1)
			break;

		//解析elf文件
		//read_elf(argv[1]);
		
		//加载内存
		load_memory();

		//设置入口地址
		PC=entry;
		
		//设置全局数据段地址寄存器
		reg[3]=gp;
		
		reg[2]=MAX/2;//栈基址 （sp寄存器）

		reg[0] = 0;
		inst_num = 0;
		cycle = 0;
		CPI=0.0;
		simulate();
		CPI = (double)cycle / inst_num;
		cout <<"simulate over!"<<endl;
		cout << "instruction number: " << inst_num << endl;	
		cout << "CPI: " << CPI << endl;

		debug = 0;
		exit_flag = 0;
		quit = 0;
	}
	
	return 0;
}

void simulate()
{
	//结束PC的设置
	int end=endPC;
	while(PC!=end)
	{
		//运行
		IF();
		IF_ID=IF_ID_old;
		ID();
		inst_num++;

		if(debug)
			step();
#ifdef DEBUG
	for(int i = 0; i < 20; ++i)
	{
		printf("reg[%d]: %d\n", i, reg[i]);
	}
	int* p = (int*)&memory[69136];
	for(int i = 0; i < 10; ++i)
		printf("%d ", *(p+i));
	printf("\n");
#endif


#ifdef STEP
		cin.get();
#endif
		//ID_EX=ID_EX_old;
		//EX();
		//EX_MEM=EX_MEM_old;
		//MEM();
		//MEM_WB=MEM_WB_old;
		//WB();

		//更新中间寄存器
		//IF_ID=IF_ID_old;
		//ID_EX=ID_EX_old;
		//EX_MEM=EX_MEM_old;
		//MEM_WB=MEM_WB_old;

        if(exit_flag==1)
            break;
        if(quit == 1)
        	break;

        reg[0]=0;//一直为零

	}
}

void add(int rd, int rs1, int rs2)
{
	reg[rd] = reg[rs1] + reg[rs2];
	return;
}
void addw(int rd, int rs1, int rs2)
{
	int s1 = reg[rs1];
	int s2 = reg[rs2];
	reg[rd] = ext_signed(s1 + s2, 32);
	return;
}
void mul(int rd, int rs1, int rs2)
{
	reg[rd] = reg[rs1] * reg[rs2];
	return;
}
void sub(int rd, int rs1, int rs2)
{
	reg[rd] = reg[rs1] - reg[rs2];
	return;
}
void sll(int rd, int rs1, int rs2)
{
	reg[rd] = reg[rs1] << reg[rs2];
	return;
}
void mulh(int rd, int rs1, int rs2)
{
	long long s1 = reg[rs1];
	long long s2 = reg[rs2];
	long long result = 0;
	for(int i = 0; i < 32; ++i)
	{
		result = (result >> 1) + (s2 & 1) ? s1 : 0;
		s2 >>= 1;
	}
	reg[rd] = result;
	return;
}
void mulw(int rd, int rs1, int rs2)
{
	int s1 = reg[rs1] & ((1LL << 32) - 1);
	int s2 = reg[rs2] & ((1LL << 32) - 1);
	long long result = s1 * s2;
	reg[rd] = ext_signed(result & ((1LL << 32) - 1), 32);
}
void slt(int rd, int rs1, int rs2)
{
	if((long long)reg[rs1] < (long long)reg[rs2])
		reg[rd] = 1;
	else
		reg[rd] = 0;
	return;
}
void alu_xor(int rd, int rs1, int rs2)
{
	reg[rd] = reg[rs1] ^ reg[rs2];
	return;
}
void alu_div(int rd, int rs1, int rs2)
{
	reg[rd] = reg[rs1] / reg[rs2];
	return;
}
void srl(int rd, int rs1, int rs2)
{
	reg[rd] = reg[rs1] >> reg[rs2];
	return;
}
void sra(int rd, int rs1, int rs2)
{
	reg[rd] = (unsigned long long)((long long)reg[rs1] >> reg[rs2]);
	return;
}
void alu_or(int rd, int rs1, int rs2)
{
	reg[rd] = reg[rs1] | reg[rs2];
	return;
}
void rem(int rd, int rs1, int rs2)
{
	reg[rd] = reg[rs1] % reg[rs2];
	return;
}
void alu_and(int rd, int rs1, int rs2)
{
	reg[rd] = reg[rs1] & reg[rs2];
	return;
}
void lb(int rd, int rs1, unsigned long long offset)
{
	unsigned long long addr = reg[rs1] + ext_signed(offset, 12);
	unsigned char ans = memory[addr];
	reg[rd] = ext_signed((unsigned long long)ans, 8);
}
void lh(int rd, int rs1, unsigned long long offset)
{
	unsigned long long addr = reg[rs1] + ext_signed(offset, 12);
	unsigned short ans = mem2(addr);
	reg[rd] = ext_signed((unsigned long long)ans, 16);
}
void lw(int rd, int rs1, unsigned long long offset)
{
	unsigned long long addr = reg[rs1] + ext_signed(offset, 12);
	unsigned int ans = mem4(addr);
	reg[rd] = ext_signed((unsigned long long)ans, 32);
}
void ld(int rd, int rs1, unsigned long long offset)
{
	unsigned long long addr = reg[rs1] + ext_signed(offset, 12);
	reg[rd] = mem8(addr);
}
void addi(int rd, int rs1, unsigned long long imm)
{
	reg[rd] = reg[rs1] + ext_signed(imm, 12);
}
void slli(int rd, int rs1, unsigned long long imm)
{
	reg[rd] = reg[rs1] << (ext_signed(imm, 12) & 63);
}
void slti(int rd, int rs1, unsigned long long imm)
{
	reg[rd] = ((long long)reg[rs1] < ext_signed(imm, 12))? 1 : 0;
}
void xori(int rd, int rs1, unsigned long long imm)
{
	reg[rd] = reg[rs1] ^ ext_signed(imm, 12);
}
void srli(int rd, int rs1, unsigned long long imm)
{
	reg[rd] = reg[rs1] >> (ext_signed(imm, 12) & 63);
}
void srai(int rd, int rs1, unsigned long long imm)
{
	reg[rd] = (unsigned long long)((long long)reg[rs1] >> (ext_signed(imm, 12) & 63));
}
void ori(int rd, int rs1, unsigned long long imm)
{
	reg[rd] = reg[rs1] | ext_signed(imm, 12);
}
void andi(int rd, int rs1, unsigned long long imm)
{
	reg[rd] = reg[rs1] & ext_signed(imm, 12);
}
void addiw(int rd, int rs1, unsigned long long imm)
{
	long long temp = (1LL << 32) - 1;
	reg[rd] = ext_signed(((reg[rs1] + ext_signed(imm, 12)) & temp), 32);
	return;
}
void slliw(int rd, int rs1, unsigned long long imm){
	reg[rd] = reg[rs1] << (imm & 63);
	return;
}
void srliw(int rd, int rs1, unsigned long long imm){
	reg[rd] = reg[rs1] >> (imm & 63);
	return;
}
void jalr(int rd, int rs1, unsigned long long imm)
{
	unsigned long long temp_reg = reg[rs1];
	if(rd != 0)
		reg[rd] = PC + 4;
	PC = (temp_reg + ext_signed(imm, 12)) & -2ll;
	return;
}
void ecall()
{
	return;
}
void sb(int rs1, int rs2, unsigned long long offset)
{
	long long addr = reg[rs1] + ext_signed(offset, 12);
	memcpy(&memory[addr], &reg[rs2], 1);
}
void sh(int rs1, int rs2, unsigned long long offset)
{
	long long addr = reg[rs1] + ext_signed(offset, 12);
	memcpy(&memory[addr], &reg[rs2], 2);
}
void sw(int rs1, int rs2, unsigned long long offset)
{
	long long addr = reg[rs1] + ext_signed(offset, 12);
	memcpy(&memory[addr], &reg[rs2], 4);
}
void sd(int rs1, int rs2, unsigned long long offset)
{
	long long addr = reg[rs1] + ext_signed(offset, 12);
	memcpy(&memory[addr], &reg[rs2], 8);
}
void beq(int rs1, int rs2, unsigned long long offset)
{
	if(reg[rs1] == reg[rs2])
		//PC = PC + (offset << 1);
		PC = PC + ext_signed(offset, 13);
	else 
		PC = PC + 4;
}
void bne(int rs1, int rs2, unsigned long long offset)
{
	if(reg[rs1] != reg[rs2])
		//PC = PC + (offset << 1);
		PC = PC + ext_signed(offset, 13);
	else 
		PC = PC + 4;
}
void blt(int rs1, int rs2, unsigned long long offset)
{
	if((long long)reg[rs1] < (long long)reg[rs2])
		//PC = PC + (offset << 1);
		PC = PC + ext_signed(offset, 13);
	else 
		PC = PC + 4;
}
void bge(int rs1, int rs2, unsigned long long offset)
{
	if((long long)reg[rs1] >= (long long)reg[rs2])
		//PC = PC + (offset << 1);
		PC = PC + ext_signed(offset, 13);
	else 
		PC = PC + 4;
}
void auipc(int rd, unsigned long long offset)
{
	reg[rd] = PC + ext_signed(offset, 32);
}
void lui(int rd, unsigned long long offset)
{
	reg[rd] = ext_signed(offset, 32);
}
void jal(int rd, unsigned long long imm)
{
	reg[rd] = PC + 4;
	//PC = PC + imm << 1;
	PC = PC + ext_signed(imm, 21);
}

//取指令
void IF()
{
	//write IF_ID_old
	IF_ID_old.inst=*((unsigned int*)&memory[PC]);
	IF_ID_old.PC=PC;
}

//译码
void ID()
{
	//Read IF_ID
	unsigned int inst=IF_ID.inst;
	int temp_PC = IF_ID.PC;

#ifdef DEBUG
	printf("%x\n", temp_PC);
	printf("%x\n", inst);
#endif

	int EXTop=0;
	unsigned int EXTsrc=0;
	unsigned long long imm;

	char Reg_Dst,ALUop,ALUSrc;
	char Branch,MemRead,MemWrite;
	char RegWrite,MemtoReg;

	rs1= getbit(inst, 19, 15);
	rs2= getbit(inst, 24, 20);
	rd = getbit(inst, 11, 7);
	fuc3=getbit(inst, 14, 12);
	fuc7=getbit(inst, 31, 25);
	OP = getbit(inst, 6, 0);
	immi = getbit(inst, 31, 20);
	imms = (getbit(inst, 31, 25) << 5) | getbit(inst, 11, 7);
	immsb = (getbit(inst, 31, 31) << 12) | (getbit(inst, 30, 25) << 5) | (getbit(inst, 11, 8) << 1) | (getbit(inst, 7, 7) << 11);
	immu = getbit(inst, 31, 12) << 12;
	immuj = (getbit(inst, 31, 31) << 20) | (getbit(inst, 30, 21) << 1) | (getbit(inst, 20, 20) << 11) | (getbit(inst, 19, 12) << 12);
#ifdef DEBUG
	printf("rs1: %d\n", rs1);
	printf("rs2: %d\n", rs2);
	printf("rd: %d\n", rd);
	printf("fuc3: %d\n", fuc3);
	printf("fuc7: %d\n", fuc7);
	printf("OP: %d\n", OP);
	printf("immi: %d\n", ext_signed(immi, 12));
	printf("imms: %d\n", ext_signed(imms, 12));
	printf("immsb: %d\n", ext_signed(immsb, 13));
	printf("immu: %d\n", ext_signed(immu, 32));
	printf("immuj: %d\n", ext_signed(immuj, 21));

#endif

	if(OP==OP_R)
	{
		if(fuc3==F3_ADD&&fuc7==F7_ADD)
		{
            add(rd, rs1, rs2);
		}
		else if(fuc3 == F3_MUL && fuc7 == F7_MUL)
		{
		   	mul(rd, rs1, rs2);
		   	cycle += 1;
		}
		else if(fuc3 == F3_SUB && fuc7 == F7_SUB)
		{
			sub(rd, rs1, rs2);
		}
		else if(fuc3 == F3_SLL && fuc7 == F7_SLL)
		{
			sll(rd, rs1, rs2);
		}
		else if(fuc3 == F3_MULH && fuc7 == F7_MULH)	//	[127:64]
		{
			mulh(rd, rs1, rs2);
			cycle += 1;
		}
		else if(fuc3 == F3_SLT && fuc7 == F7_SLT)
		{
			slt(rd, rs1, rs2);		
		}
		else if(fuc3 == F3_XOR && fuc7 == F7_XOR)
		{
			alu_xor(rd, rs1, rs2);
		}
		else if(fuc3 == F3_DIV && fuc7 == F7_DIV)
		{
			alu_div(rd, rs1, rs2);
			cycle += 39;
		}
		else if(fuc3 == F3_SRL && fuc7 == F7_SRL)
		{
			srl(rd, rs1, rs2);
		}
		else if(fuc3 == F3_SRA && fuc7 == F7_SRA)
		{
			sra(rd, rs1, rs2);
		}
		else if(fuc3 == F3_OR && fuc7 == F7_OR)
		{
			alu_or(rd, rs1, rs2);
		}
		else if(fuc3 == F3_REM && fuc7 == F7_REM)
		{
			rem(rd, rs1, rs2);
			cycle += 39;
		}
		else if(fuc3 == F3_AND && fuc7 == F7_AND)
		{
			alu_and(rd, rs1, rs2);
		}
		PC += 4;
		cycle += 4;
	}
	else if(OP==OP_WR)
	{
		if(fuc3 == F3_MULW && fuc7 == F7_MULW)
		{
			mulw(rd, rs1, rs2);
		}
		else if(fuc3 == F3_ADDW && fuc7 == F7_ADDW)
		{
			addw(rd, rs1, rs2);
		}
		PC += 4;
		cycle += 4;
	}
	else if(OP==OP_LW)
    {
        if(fuc3==F3_LB)
        {
            lb(rd, rs1, immi);
        }
        else if(fuc3 == F3_LH)
        {
            lh(rd, rs1, immi);
        }
        else if(fuc3 == F3_LW)
        {
        	lw(rd, rs1, immi);
        }
        else if(fuc3 == F3_LD)
        {
        	ld(rd, rs1, immi);
        }
        PC += 4;
        cycle += 5;
    }
    else if(OP==OP_I)
    {
        if(fuc3==F3_ADDI)
        {
            addi(rd, rs1, immi);
        }
        else if(fuc3 == F3_SLLI && fuc7 == F7_SLLI)
        {
           	slli(rd, rs1, immi);
        }
        else if(fuc3 == F3_SLTI)
        {
        	slti(rd, rs1, immi);
        }
        else if(fuc3 == F3_XORI)
        {
        	xori(rd, rs1, immi);
        }
        else if(fuc3 == F3_SRLI && fuc7 == F7_SRLI)
        {
        	srli(rd, rs1, immi);
        }
        else if (fuc3 == F3_SRAI && fuc7 == F7_SRAI)
        {
        	srai(rd, rs1, immi);
        }
        else if(fuc3 == F3_ORI)
        {
        	ori(rd, rs1, immi);
        }
        else if(fuc3 == F3_ANDI)
        {
        	andi(rd, rs1, immi);
        }
        else
        {
        	printf("Invalid\n");
        }
        PC += 4;
        cycle += 4;
    }
    else if(OP==OP_ADDIW)
    {
        if(fuc3==F3_ADDIW)
        {
			addiw(rd, rs1, immi);
        }
        else if(fuc3 == F3_SLLIW)
        {
        	slliw(rd, rs1, immi);
        }
        else if (fuc3 == F3_SRLIW)
        {
        	srliw(rd, rs1, immi);
        }
        else
        {
        	printf("Invalid\n");
        }
        PC += 4;
        cycle += 4;
    }
    else if(OP==OP_JALR)
    {
        if(fuc3==F3_JALR)
        {
			jalr(rd, rs1, immi);
        }
        else
        {
           	printf("Invalid\n");	
        }
        cycle += 3;
    }
    else if(OP==OP_ECALL)
    {
        if(fuc3 == F3_ECALL && fuc7 == F7_ECALL)
        {
        	ecall();
        }
        else
        {
        	printf("Invalid\n");
        }
        cycle += 3;
    }
    else if(OP == OP_S)
    {

		if(fuc3 == F3_SB)
		{
			sb(rs1, rs2, imms);
		}
		else if(fuc3 == F3_SH)
		{
			sh(rs1, rs2, imms);
		}
		else if (fuc3 == F3_SW)
		{
			sw(rs1, rs2, imms);
		}
		else if(fuc3 == F3_SD)
		{
			sd(rs1, rs2, imms);
		}
		else
		{
			printf("Invalid\n");
		}
		PC += 4;
		cycle += 4;
    }
    else if(OP == OP_SB)
    {
    	if(fuc3 == F3_BEQ)
    	{
    		beq(rs1, rs2, immsb);
    	}
    	else if(fuc3 == F3_BNE)
    	{
    		bne(rs1, rs2, immsb);
    	}
    	else if(fuc3 == F3_BLT)
    	{
    		blt(rs1, rs2, immsb);
    	}
    	else if(fuc3 == F3_BGE)
    	{
    		bge(rs1, rs2, immsb);
    	}
    	else
    	{
    		printf("Invalid\n");
    	}
    	cycle += 3;
    }
    else if(OP == OP_AUIPC)
    {
    	auipc(rd, immu);
    	PC += 4;
    	cycle += 4;
    }
    else if(OP == OP_LUI)
    {
    	lui(rd, immu);
    	PC += 4;
    	cycle += 4;
    }
    else if(OP == OP_JAL)
    {
    	jal(rd, immuj);
    	cycle += 2;
    }
    else
    {
    	printf("Invalid\n");
    }

	//write ID_EX_old
	ID_EX_old.Rd=rd;
	ID_EX_old.Rs1=rs1;
	ID_EX_old.PC = temp_PC;
	ID_EX_old.Imm = imm;
	//...

	ID_EX_old.Ctrl_EX_ALUOp = ALUop;
	ID_EX_old.Ctrl_EX_ALUSrc = ALUSrc;
	ID_EX_old.Ctrl_EX_RegDst = Reg_Dst;
	ID_EX_old.Ctrl_M_Branch = Branch;
	ID_EX_old.Ctrl_M_MemWrite = MemWrite;
	ID_EX_old.Ctrl_M_MemRead = MemRead;
	ID_EX_old.Ctrl_WB_RegWrite = RegWrite;
	ID_EX_old.Ctrl_WB_MemtoReg = MemtoReg;
	//....

}

//执行
void EX()
{
	//read ID_EX
	int temp_PC=ID_EX.PC;
	int Rd = ID_EX.Rd;
	int Rt = ID_EX.Rd;
	unsigned long long imm = ID_EX.Imm;
	REG Reg_Rs1 = ID_EX.Reg_Rs1;
	REG Reg_Rs2 = ID_EX.Reg_Rs2;
	char RegDst = ID_EX.Ctrl_EX_RegDst;
	char ALUOp = ID_EX.Ctrl_EX_ALUOp;
	char ALUSrc = ID_EX.Ctrl_EX_ALUSrc;

	EX_MEM_old.Ctrl_M_Branch = ID_EX.Ctrl_M_Branch;
	EX_MEM_old.Ctrl_M_MemWrite = ID_EX.Ctrl_M_MemWrite;
	EX_MEM_old.Ctrl_M_MemRead = ID_EX.Ctrl_M_MemRead;
	EX_MEM_old.Ctrl_WB_RegWrite = ID_EX.Ctrl_WB_RegWrite;
	EX_MEM_old.Ctrl_WB_MemtoReg = ID_EX.Ctrl_WB_MemtoReg;

	//Branch PC calulate
	//...

	//choose ALU input number
	//...

	//alu calculate
	int Zero;
	REG ALU_out;
	switch(ALUOp){
	default:;
	}

	//choose reg dst address
	int Reg_dst;
	if(RegDst)
	{

	}
	else
	{

	}

	//write EX_MEM_old
	EX_MEM_old.PC = temp_PC;
	EX_MEM_old.Reg_dst = Reg_dst;
	EX_MEM_old.ALU_out = ALU_out;
	EX_MEM_old.Zero = Zero;
	EX_MEM_old.Reg_Rs1 = Reg_Rs1;
    //.....
}

//访问存储器
void MEM()
{
	//read EX_MEM
	int temp_PC = EX_MEM.PC;
	int Reg_dst = EX_MEM.Reg_dst;
	REG ALU_out = EX_MEM.ALU_out;
	int Zero = EX_MEM.Zero;
	REG Reg_Rs1 = EX_MEM.Reg_Rs1;

	char Branch = EX_MEM.Ctrl_M_Branch;
	char MemWrite = EX_MEM.Ctrl_M_MemRead;
	char MemRead = EX_MEM.Ctrl_M_MemRead;

	unsigned int Mem_read;

	MEM_WB_old.Ctrl_WB_RegWrite = EX_MEM.Ctrl_WB_RegWrite;
	MEM_WB_old.Ctrl_WB_MemtoReg = EX_MEM.Ctrl_WB_MemtoReg;

	//complete Branch instruction PC change

	//read / write memory

	//write MEM_WB_old
	MEM_WB_old.Mem_read = Mem_read;
	MEM_WB_old.ALU_out = ALU_out;
	MEM_WB_old.Reg_dst = Reg_dst;
}


//写回
void WB()
{
	//read MEM_WB
	unsigned int Mem_read = MEM_WB.Mem_read;
	REG ALU_out = MEM_WB.ALU_out;
	int Reg_dst = MEM_WB.Reg_dst;

	char RegWrite = MEM_WB.Ctrl_WB_RegWrite;
	char MemtoReg = MEM_WB.Ctrl_WB_MemtoReg;

	//write reg
}