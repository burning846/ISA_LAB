#include "Simulation.h"
using namespace std;

extern void read_elf(char* file_name);
extern unsigned int cadr;	//code
extern unsigned int dadr;	//data
extern unsigned int badr;	//bss
extern unsigned int csize;
extern unsigned int dsize;
extern unsigned int bsize;
extern unsigned int vadr;		//code
extern unsigned long long gp;	//data
extern unsigned long long gb;
extern unsigned int madr;
extern unsigned int endPC;
extern unsigned int entry;
extern FILE *file;


//指令运行数
long long inst_num=0;

//系统调用退出指示
int exit_flag=0;

//加载代码段
//初始化PC
void load_memory()
{
	fseek(file,cadr,SEEK_SET);
	fread(&memory[vadr],1,csize,file);
	fseek(file,dadr,SEEK_SET);
	fread(&memory[gp],1,dsize,file);
	fseek(file,badr,SEEK_SET);
	fread(&memory[gb],1,bsize,file);

	fclose(file);
}

int main(int argc, char** argv)
{
	//解析elf文件
	read_elf(argv[1]);
	
	//加载内存
	load_memory();

	//设置入口地址
	PC=entry;
	
	//设置全局数据段地址寄存器
	reg[3]=gp;
	
	reg[2]=MAX/2;//栈基址 （sp寄存器）

	simulate();

	cout <<"simulate over!"<<endl;

	return 0;
}

void simulate()
{
	//结束PC的设置
	int end=(int)endPC/4-1;
	while(PC!=end)
	{
		//运行
		IF();
		IF_ID=IF_ID_old;
		ID();
		ID_EX=ID_EX_old;
		EX();
		EX_MEM=EX_MEM_old;
		MEM();
		MEM_WB=MEM_WB_old;
		WB();

		//更新中间寄存器
		//IF_ID=IF_ID_old;
		//ID_EX=ID_EX_old;
		//EX_MEM=EX_MEM_old;
		//MEM_WB=MEM_WB_old;

        if(exit_flag==1)
            break;

        reg[0]=0;//一直为零

	}
}


//取指令
void IF()
{
	//write IF_ID_old
	IF_ID_old.inst=memory[PC];
	PC=PC+1;
	IF_ID_old.PC=PC;
}

//译码
void ID()
{
	//Read IF_ID
	unsigned int inst=IF_ID.inst;
	int temp_PC = IF_ID.PC;

	int EXTop=0;
	unsigned int EXTsrc=0;

	char RegDst,ALUop,ALUSrc;
	char Branch,MemRead,MemWrite;
	char RegWrite,MemtoReg;

	rs1= getbit(inst, 19, 15);
	rs2= getbit(inst, 24, 20);
	rd = getbit(inst, 11, 7);
	fuc3=getbit(inst, 14, 12);
	fuc7=getbit(inst, 31, 25);
	OP = getbit(inst, 6, 0);

	if(OP==OP_R)
	{
		if(fuc3==F3_ADD&&fuc7==F7_ADD)
		{
            EXTop=0;
			RegDst=rd;
			ALUop=ALU_ADD;
			ALUSrc=rt;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=1;
			MemtoReg=0;
		}
		else if(fuc3 == F3_MUL && fuc7 == F7_MUL)
		{
		   	EXTop=0;
			RegDst=rd;
			ALUop=ALU_MUL;
			ALUSrc=rt;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=1;
			MemtoReg=0;
		}
		else if(fuc3 == F3_SUB && fuc7 == F7_SUB)
		{
			EXTop=0;
			RegDst=rd;
			ALUop=ALU_SUB;
			ALUSrc=rt;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=1;
			MemtoReg=0;
		}
		else if(fuc3 == F3_SLL && fuc7 == F7_SLL)
		{
			EXTop=0;
			RegDst=rd;
			ALUop=ALU_SLL;
			ALUSrc=rt;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=1;
			MemtoReg=0;
		}
		else if(fuc3 == F3_MULH && fuc7 == F7_MULH)	//	[127:64]
		{
			EXTop=0;
			RegDst=rd;
			ALUop=ALU_MUL;
			ALUSrc=rt;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=1;
			MemtoReg=0;
		}
		else if(fuc3 == F3_SLT && fuc7 == F7_SLT)
		{
			EXTop=0;
			RegDst=rd;
			ALUop=ALU_SUBUB;
			ALUSrc=rt;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=1;
			MemtoReg=0;
		}
		else if(fuc3 == F3_XOR && fuc7 == F7_XOR)
		{
			EXTop=0;
			RegDst=rd;
			ALUop=ALU_XOR;
			ALUSrc=rt;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=1;
			MemtoReg=0;
		}
		else if(fuc3 == F3_DIV && fuc7 == F7_DIV)
		{
			EXTop=0;
			RegDst=rd;
			ALUop=ALU_DIV;
			ALUSrc=rt;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=1;
			MemtoReg=0;
		}
		else if(fuc3 == F3_SRL && fuc7 == F7_SRL)
		{
			EXTop=0;
			RegDst=rd;
			ALUop=ALU_SRL;
			ALUSrc=rt;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=1;
			MemtoReg=0;
		}
		else if(fuc3 == F3_SRA && fuc7 == F7_SRA)
		{
			EXTop=0;
			RegDst=rd;
			ALUop=ALU_SRA;
			ALUSrc=rt;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=1;
			MemtoReg=0;
		}
		else if(fuc3 == F3_OR && fuc7 == F7_OR)
		{
			EXTop=0;
			RegDst=rd;
			ALUop=ALU_OR;
			ALUSrc=rt;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=1;
			MemtoReg=0;
		}
		else if(fuc3 == F3_REM && fuc7 == F7_REM)
		{
			EXTop=0;
			RegDst=rd;
			ALUop=ALU_REM;
			ALUSrc=rt;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=1;
			MemtoReg=0;
		}
		else if(fuc3 == F3_AND && fuc7 == F7_AND)
		{
			EXTop=0;
			RegDst=rd;
			ALUop=ALU_AND;
			ALUSrc=rt;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=1;
			MemtoReg=0;
		}
	}
	else if(OP==OP_LW)
    {
        if(fuc3==F3_LB)
        {
            EXTop=1;
			RegDst=rd;
			ALUop=ALU_ADD;
			ALUSrc=;
			Branch=0;
			MemRead=1;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=1;
        }
        else if(fuc3 == F3_LH)
        {
           EXTop=1;
			RegDst=rd;
			ALUop=ALU_ADD;
			ALUSrc=;
			Branch=0;
			MemRead=1;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=1;
        }
        else if(fuc3 == F3_LW)
        {
        	EXTop=1;
			RegDst=rd;
			ALUop=ALU_ADD;
			ALUSrc=;
			Branch=0;
			MemRead=1;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=1;
        }
        else if(fuc3 == F3_LD)
        {
        	EXTop=1;
			RegDst=rd;
			ALUop=ALU_ADD;
			ALUSrc=;
			Branch=0;
			MemRead=1;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=1;
        }
    }
    else if(OP==OP_I)
    {
        if(fuc3==F3_ADDI)
        {
           EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
        }
        else if(fuc3 == F3_SLLI && fuc7 == F7_SLLI)
        {
           EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
        }
        else if(fuc3 == F3_SLTI)
        {
        	EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
        }
        else if(fuc3 == F3_XORI)
        {
        	EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
        }
        else if(fuc3 == F3_SRLI && fuc7 == F7_SRLI)
        {
        	EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
        }
        else if (fuc3 == F3_SRAI && fuc7 == F7_SRAI)
        {
        	EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
        }
        else if(fuc3 == F3_ORI)
        {
        	EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
        }
        else if(fuc3 == F3_ANDI)
        {
        	EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
        }
    }
    else if(OP==OP_IW)
    {
        if(fuc3==F3_IW)
        {
			EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
        }
        else
        {
        	//invalid
        }
    }
    else if(OP==OP_JALR)
    {
        if(fuc3==F3_JALR)
        {
			EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
        }
        else
        {
           
        }
    }
    else if(OP==OP_ECALL)
    {
        if(fuc3 == F3_ECALL && fuc7 == F7_ECALL)
        {
        	EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
        }
        else
        {

        }
    }
    else if(OP == OP_S)
    {
		if(fuc3 == F3_SB)
		{
			EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
		}
		else if(fuc3 == F3_SH)
		{
			EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
		}
		else if (fuc3 == F3_SW)
		{
			EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
		}
		else
		{
			//invalid
		}
    }
    else if(OP == OP_SB)
    {
    	if(fuc3 == F3_BEQ)
    	{
    		EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
    	}
    	else if(fuc3 == F3_BNE)
    	{
    		EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
    	}
    	else if(fuc3 == F3_BLT)
    	{
    		EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
    	}
    	else if(fuc3 == F3_BGE)
    	{
    		EXTop=0;
			RegDst=0;
			ALUop=0;
			ALUSrc=0;
			Branch=0;
			MemRead=0;
			MemWrite=0;
			RegWrite=0;
			MemtoReg=0;
    	}
    	else
    	{
    		//invalid
    	}
    }
    else if(OP == OP_AUIPC)
    {

    }
    else if(OP == OP_LUI)
    {

    }
    else if(OP == OP_JAL)
    {

    }
    else
    {
    	//invalid
    }

	//write ID_EX_old
	ID_EX_old.Rd=rd;
	ID_EX_old.Rt=rt;
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
	int Imm = ID_EX.Imm;
	REG Reg_Rs = ID_EX.Reg_Rs;
	REG Reg_Rt = ID_EX.Reg_Rt;
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
	EX_MEM_old.Reg_Rt = Reg_Rt;
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
	REG Reg_Rt = EX_MEM.Reg_Rt;

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