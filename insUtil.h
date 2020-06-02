

#include<vector>
#include<string>

using namespace std;

struct RTypeInstruction
{	
	uint8_t opcode:6, rs:5, rt:5, rd:5, shamt:5, funct:6;
	RTypeInstruction(uint32_t i){opcode=i>>26;rs=i>>21&31;rt=i>>16&31;rd=i>>11&31;shamt=i>>6&31;funct=i&63;}
	RTypeInstruction(uint8_t opcode_, uint8_t rs_, uint8_t rt_, uint8_t rd_, uint8_t shamt_, uint8_t funct_)
	:opcode(opcode_), rs(rs_), rt(rt_), rd(rd_), shamt(shamt_), funct(funct_){}
	operator uint32_t(){return opcode<<26|rs<<21|rt<<16|rd<<11|shamt<<6|funct;}
};

struct ITypeInstruction
{	
	uint8_t opcode:6, rs:5, rt:5;
	uint16_t immediate:16;
	ITypeInstruction(uint32_t i){opcode=i>>26;rs=i>>21&31;rt=i>>16&31;immediate=i&0xffff;}
	ITypeInstruction(uint8_t opcode_, uint8_t rs_, uint8_t rt_, uint16_t immediate_)
	:opcode(opcode_), rs(rs_), rt(rt_), immediate(immediate_){}
	operator uint32_t(){return opcode<<26|rs<<21|rt<<16|immediate;}
};

struct JTypeInstruction
{	
	uint8_t opcode:6;
	uint32_t address:26;
	JTypeInstruction(uint32_t i){opcode=i>>26;address=i&0x3ffffff;}
	JTypeInstruction(uint8_t opcode_, uint32_t address_)
	:opcode(opcode_),address(address_){}
	operator uint32_t(){return opcode<<26|address;}
};

const vector<string> registerNames = 
{
	"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
};