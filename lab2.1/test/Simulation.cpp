#include "Simulation.h"
//#include "Reg_def.h"
using namespace std;

extern void read_elf(char* file_name);
extern unsigned int cadr;
extern unsigned int csize;
extern unsigned int vadr;
extern unsigned long long gp;
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

	//vadr=vadr>>2;
	//csize=csize>>2;
	fclose(file);
}

int main(int argc, char** argv)
{
	//解析elf文件
	read_elf(argv[1]);
	
	//加载内存
	load_memory();

	//设置入口地址
	PC=entry>>2;
	
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
		ID();
		EX();
		MEM();
		WB();

		//更新中间寄存器
		IF_ID=IF_ID_old;
		ID_EX=ID_EX_old;
		EX_MEM=EX_MEM_old;
		MEM_WB=MEM_WB_old;

		if(exit_flag==1)
			break;

		reg[0]=0;//一直为零

	}
}


//取指令
void IF()
{
	//write IF_ID_old
	IF_ID_old.inst=read_mem_4(PC);
	PC=PC+4;
	IF_ID_old.PC=PC;
}

//译码
void ID()
{
	//Read IF_ID
	unsigned int inst=IF_ID.inst;
	int EXTop=0;
	unsigned int EXTsrc=0;

	char RegDst,ALUSrc;
	branch_cond Branch;
	char MemRead,MemWrite;
	char RegWrite,MemtoReg;
	ALUOPs ALUop;

	OP=getbit(inst,0,6);
	rd=getbit(inst,7,11);
	fuc3=getbit(inst,12,14);
	rs=getbit(inst,15,19);
	rt=getbit(inst,20,24);
	fuc7=getbit(inst,25,31);
	//....


	if(OP==OP_R)
	{
		RegDst=0;
		Branch=NEVER;
		MemRead=0;
		MemWrite=0;
		ALUSrc=0;
		RegWrite=1;
		MemtoReg=0;
		EXTop=0;
		if(fuc3==F3_ADD && fuc7==F7_ADD)//add
		{
			ALUop=ADD;
		}
		else if(fuc3==F3_MUL && fuc7==F7_MUL){//mul
			ALUop=MUL;
		}
		else if(fuc3==F3_SUB && fuc7==F7_SUB){//sub
			ALUop=SUB;
		}
		else if(fuc3==F3_SLL && fuc7==F7_SLL){//sll
			ALUop=SLL;
		}
		else if(fuc3==F3_MULH && fuc7==F7_MULH){//mulh
			ALUop=MUL;
		}
		else if(fuc3==F3_SLT && fuc7==F7_SLT){//slt
			ALUop=SUB;
		}
		else if(fuc3==F3_XOR && fuc7==F7_XOR){//xor
			ALUop=XOR;
		}
		else if(fuc3==F3_DIV && fuc7==F7_DIV){//div
			ALUop=DIV;
		}
		else if(fuc3==F3_SRL && fuc7==F7_SRL){//srl
			ALUop=SRL;
		}
		else if(fuc3==F3_SRA && fuc7==F7_SRA){//sra
			ALUop=SRA;
		}
		else if(fuc3==F3_OR && fuc7==F7_OR){//or
			ALUop=OR;
		}
		else if(fuc3==F3_REM && fuc7==F7_REM){//rem
			ALUop=DIV;
		}
		else if(fuc3==F3_AND && fuc7==F7_AND){//and
			ALUop=AND;
		}
		else
		{
			ALUop=NONE;
		}
	}
	else if(OP==OP_I)
	{
		EXTsrc = getbit(inst, 20, 31);
		RegDst=0;
		Branch=NEVER;
		MemRead=0;
		MemWrite=0;
		ALUSrc=1; //from imm
		RegWrite=1;
		MemtoReg=0;
		EXTop=0;
		if(fuc3==F3_ADDI){//addi
			ALUop=ADD;
		}
		else if(fuc3==F3_SLLI && fuc7==F7_SLLI){//slli
			ALUop=SLL;
		}
		else if(fuc3==F3_SLTI){//slti
			ALUop=SUB;
		}
		else if(fuc3==F3_XORI){//xori
			ALUop=XOR;
		}
		else if(fuc3==F3_SRLI && fuc7==F7_SRLI){//srli
			ALUop=SRL;
		}
		else if(fuc3==F3_SRAI && fuc7==F7_SRAI){//srai
			ALUop=SRA;
		}
		else if(fuc3==F3_ORI){//ori
			ALUop=OR;
		}
		else if(fuc3==F3_ANDI){//andi
			ALUop=AND;
		}
		else{
			ALUop=NONE;
		}
	}
	else if(OP==OP_SW)
	{
		unsigned imm4_0 = getbit(inst,7,11);
		unsigned imm11_5 = getbit(inst,25,31);
		EXTsrc = (imm11_5<<5)|imm4_0;
		RegDst=0;//unclear
		Branch=NEVER;
		MemRead=0;
		ALUSrc=1; //from imm
		RegWrite=0;
		MemtoReg=0;
		EXTop=0;
		ALUop=ADD;//add
		if(fuc3==F3_SB){
			MemWrite=1;
		}
		else if(fuc3==F3_SH){
			MemWrite=2;
		}
		else if(fuc3==F3_SW){
			MemWrite=4;
		}
		else if(fuc3==F3_SD){
			MemWrite=8;
		}
		else{
			MemWrite=0;
		}
	}
	else if(OP==OP_LW)
	{
		EXTsrc = getbit(inst, 20, 31);
		RegDst=0;
		Branch=NEVER;
		MemWrite=0;
		ALUSrc=1; //from imm
		RegWrite=1;
		MemtoReg=1;
		EXTop=0;
		ALUop=ADD;//add
		if(fuc3==F3_LB){
			EXTop=1;
			MemRead=1;
		}
		else if(fuc3==F3_LH){
			EXTop=1;
			MemRead=2;
		}
		else if(fuc3==F3_LW){
			MemRead=4;
		}
		else if(fuc3==F3_LD){
			MemRead=8;
		}
		else{
			MemRead=0;
		}
	}
	else if(OP==OP_BEQ)
	{
		unsigned imm11_11 = getbit(inst,31,31);
		unsigned imm10_10 = getbit(inst,7,7);
		unsigned imm9_4 = getbit(inst,25,30);
		unsigned imm3_0 = getbit(inst,8,11);
		EXTsrc = (imm11_11<<11) | (imm10_10<<10) | (imm9_4<<4) | imm3_0;
		RegDst=0;
		MemRead=0;
		MemWrite=0;
		ALUSrc=0; //from reg
		RegWrite=0;
		MemtoReg=0;
		EXTop=0;
		ALUop=SUB;//sub
		if(fuc3==F3_BEQ){
			Branch=EQ;
		}
		else if(fuc3==F3_BNE){
			Branch=NE;
		}
		else if(fuc3==F3_BLT){
			Branch=LT;
		}
		else if(fuc3==F3_BGE){
			Branch=GE;
		}	
		else{
			Branch=NEVER;
		   
		}
	}
	else if(OP==OP_JAL)
	{
		
	}
	else
	{
		
	}

	//write ID_EX_old
	ID_EX_old.Rd=rd;
	ID_EX_old.Rt=rt;
	ID_EX_old.Imm=ext_signed(EXTsrc,EXTop);
	ID_EX_old.Reg_Rs=reg[rs];
	ID_EX_old.Reg_Rt=reg[rt];
	//...

	ID_EX_old.Ctrl_EX_ALUOp=ALUop;
	ID_EX_old.Ctrl_EX_ALUSrc=ALUSrc;
	ID_EX_old.Ctrl_EX_RegDst=RegDst;
	ID_EX_old.Ctrl_M_Branch=Branch;
	ID_EX_old.Ctrl_M_MemWrite=MemWrite;
	ID_EX_old.Ctrl_M_MemRead=MemRead;
	ID_EX_old.Ctrl_WB_MemtoReg;
	ID_EX_old.Ctrl_WB_RegWrite=RegWrite;
	//....

}

//执行
void EX()
{
	//read ID_EX
	int temp_PC=ID_EX.PC;
	char RegDst=ID_EX.Ctrl_EX_RegDst;
	char ALUOp=ID_EX.Ctrl_EX_ALUOp;
	char ALUSrc=ID_EX.Ctrl_EX_ALUSrc;
	//Branch PC calulate
	int branchPC = temp_PC + ID_EX.Imm;

	//ALU operating numbers
	REG ALU_a = ID_EX.Reg_Rs, ALU_b = 0;

	//choose ALU input number
	//...
	if(ALUSrc == 0){ //using register
		ALU_b = reg[ID_EX.Rt];
	}
	else if(ALUSrc == 1){//using imm
		ALU_b = (REG)ID_EX.Imm;
	}

	//alu calculate
	int Zero = 0, Sign = 0;
	REG ALUout = 0, rem = 0;
	switch(ALUOp){
		case ADD:
			ALUout = ALU_a + ALU_b;
			break;
		case SUB:
			ALUout = ALU_a - ALU_b;
			Zero = (ALUout == 0);
			Sign = (ALU_a < ALU_b);
			break;
		case MUL:
			ALUout = ALU_a * ALU_b;
			break;
		case SLL:
			ALUout = ALU_a << ALU_b;
			break;
		case SRL:
			ALUout = ALU_a >> ALU_b;
			break;
		case SRA:
			ALUout = (REG)(((long long)ALU_a)>>ALU_b);
			break;
		case AND:
			ALUout = ALU_a & ALU_b;
			break;
		case XOR:
			ALUout = ALU_a ^ ALU_b;
			break;
		case OR:
			ALUout = ALU_a | ALU_b;
			break;
		case DIV:
			ALUout = ALU_a / ALU_b;
			rem = ALU_a % ALU_b;
			break;
	default:;
	}

	//choose reg dst address
	int Reg_Dst = 0;
	if(RegDst){
		Reg_Dst = rd;
	}

	//write EX_MEM_old
	EX_MEM_old.ALU_out=ALUout;
	EX_MEM_old.rem=rem;
	EX_MEM_old.PC=temp_PC;
	EX_MEM_old.Reg_dst=Reg_Dst;
	EX_MEM_old.Zero=Zero;
	EX_MEM_old.Sign=Sign;
	EX_MEM_old.Reg_Rt=ID_EX.Reg_Rt; //???
	EX_MEM_old.Ctrl_M_Branch=ID_EX.Ctrl_M_Branch;
	EX_MEM_old.Ctrl_M_MemRead=ID_EX.Ctrl_M_MemRead;
	EX_MEM_old.Ctrl_M_MemWrite=ID_EX.Ctrl_M_MemWrite;
	EX_MEM_old.Ctrl_WB_RegWrite=ID_EX.Ctrl_WB_RegWrite;
	EX_MEM_old.Ctrl_WB_MemtoReg=ID_EX.Ctrl_WB_MemtoReg;
	//.....
}

//访问存储器
void MEM()
{
	//read EX_MEM
	branch_cond Branch = EX_MEM.Ctrl_M_Branch;
	REG ALUout = EX_MEM.ALU_out;
    REG Reg_Rt = EX_MEM.Reg_Rt;
	int Zero = EX_MEM.Zero;
	int Sign = EX_MEM.Sign;
	char MemWrite = EX_MEM.Ctrl_M_MemWrite;
	char MemRead = EX_MEM.Ctrl_M_MemRead;
	int temp_PC = EX_MEM.PC;
	int newPC;
	unsigned long long Mem_read = 0;
	//complete Branch instruction PC change
	newPC = temp_PC + imm12<<1;
	switch(Branch){
		case EQ:
			if(Zero) PC = newPC;
			break;
		case NE:
			if(!Zero) PC = newPC;
			break;
		case LT:
			if(Sign) PC = newPC;
			break;
		case GE:
			if(!Sign) PC = newPC;
			break;
		default:;
	}

	//read / write memory
    unsigned long long address = ALUout;
	if(MemRead==8){
        Mem_read = read_mem_8(address);
	}
    else if(MemRead==4){
        Mem_read = read_mem_4(address);
    }
    else if(MemRead==2){
        Mem_read = read_mem_2(address);
    }
    else if(MemRead==1){
        Mem_read = memory[address];
    }

    if(MemWrite==8){
        write_mem_8(address, Reg_Rt);
    }
    else if(MemWrite==4){
        write_mem_4(address, (unsigned int)Reg_Rt);
    }
    else if(MemWrite==2){
        write_mem_2(address, (unsigned short)Reg_Rt);
    }
    else if(MemWrite==1){
        memory[address] = (unsigned char)Reg_Rt;
    }
	//write MEM_WB_old
    MEM_WB_old.Mem_read = Mem_read;
    MEM_WB_old.ALU_out = ALUout;
    MEM_WB_old.Reg_dst = EX_MEM.Reg_dst;
    MEM_WB_old.Ctrl_WB_MemtoReg = EX_MEM.Ctrl_WB_MemtoReg;
    MEM_WB_old.Ctrl_WB_RegWrite = EX_MEM.Ctrl_WB_RegWrite;
}


//写回
void WB()
{
	//read MEM_WB
    int Reg_Dst = MEM_WB.Reg_dst;
    char RegWrite = MEM_WB.Ctrl_WB_RegWrite;
    char MemtoReg = MEM_WB.Ctrl_WB_MemtoReg;
    REG ALUout = MEM_WB.ALU_out;
    unsigned long long Mem_read = MEM_WB.Mem_read;

    //write reg
    if(RegWrite){
        if(MemtoReg){
            reg[Reg_Dst] = Mem_read;
        }
        else{
            reg[Reg_Dst] = ALUout;
        }
    }
}
