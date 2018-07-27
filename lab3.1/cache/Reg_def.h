typedef unsigned long long REG;

struct IFID{
	unsigned int inst;
	int PC;
}IF_ID,IF_ID_old;


struct IDEX{
	unsigned int op,fuc3,fuc7;
	unsigned int inst;
	int Rs1,Rs2,Rd;
	int PC, dst_PC;
	unsigned long long Imm;
	REG Reg_Rs1,Reg_Rs2;

	char Ctrl_EX_ALUSrc;
	char Ctrl_EX_ALUOp;
	char Ctrl_EX_RegDst;

	char Ctrl_M_Branch;
	char Ctrl_M_MemWrite;
	char Ctrl_M_MemRead;

	char Ctrl_WB_RegWrite;
	char Ctrl_WB_MemtoReg;

}ID_EX,ID_EX_old;

struct EXMEM{
	unsigned int op,fuc3,fuc7;
	int PC;
	int Reg_dst;
	REG ALU_out;
	int Zero;
	REG Reg_Rs2;

	char Ctrl_M_Branch;
	char Ctrl_M_MemWrite;
	char Ctrl_M_MemRead;

	char Ctrl_WB_RegWrite;
	char Ctrl_WB_MemtoReg;

}EX_MEM,EX_MEM_old;

struct MEMWB{
	int PC;
	unsigned int op,fuc3,fuc7;
	unsigned long long Mem_read;
	REG ALU_out;
	int Reg_dst;
		
	char Ctrl_WB_RegWrite;
	char Ctrl_WB_MemtoReg;

}MEM_WB,MEM_WB_old;