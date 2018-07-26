typedef unsigned long long REG;
enum ALUOPs {NONE, ADD, SUB, MUL, SLL, SRA, SRL, AND, XOR, OR, DIV};
enum branch_cond {NEVER, EQ, NE, LT, GE};

struct IFID{
	unsigned int inst;
	int PC;
}IF_ID,IF_ID_old;


struct IDEX{
	int Rd,Rt;
	int PC;
	int Imm;
	REG Reg_Rs,Reg_Rt;

	char Ctrl_EX_ALUSrc;
	ALUOPs Ctrl_EX_ALUOp;
	char Ctrl_EX_RegDst;

	branch_cond Ctrl_M_Branch; //condition for branch
	char Ctrl_M_MemWrite; //num of bytns written
	char Ctrl_M_MemRead;  //num of bytes read

	char Ctrl_WB_RegWrite;
	char Ctrl_WB_MemtoReg;

}ID_EX,ID_EX_old;

struct EXMEM{
	int PC;
	int Reg_dst;
	REG ALU_out, rem;
	int Zero, Sign;
	REG Reg_Rt;

	branch_cond Ctrl_M_Branch; //condition for branch
	char Ctrl_M_MemWrite; //num of bytns written
	char Ctrl_M_MemRead;  //num of bytes read

	char Ctrl_WB_RegWrite;
	char Ctrl_WB_MemtoReg;

}EX_MEM,EX_MEM_old;

struct MEMWB{
	unsigned long long Mem_read;
	REG ALU_out;
	int Reg_dst;
		
	char Ctrl_WB_RegWrite;
	char Ctrl_WB_MemtoReg;

}MEM_WB,MEM_WB_old;