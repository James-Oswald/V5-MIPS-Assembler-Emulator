
#include<iostream>
#include<iomanip>
#include<vector>
#include<cstdint>
#include<string>
#include<map>
#include<windows.h>

#include"Display.cpp"
#include"print.h"
#include"insUtil.h"
#include"assembler.cpp"

using namespace std;

inline uint32_t SE(uint16_t i)
{
	return i>>15&1 ? 0xffff<<16|i : i;
}

Display d;
vector<uint8_t> ram;
vector<uint32_t> registers;
string outputBuffer = "";
int sWidth = 256, sHeight = 144;
vector<uint32_t> frameBuffer(sWidth * sHeight, 0);
bool halt = false;
uint32_t pc = 0;

void syscall()
{
	switch(registers[2])
	{
		case 1:
		{
			outputBuffer += to_string(registers[4]);
			break;
		}
		case 4:
		{
			uint32_t i = registers[4];
			while(ram[i]!='\0')
			{
				outputBuffer += (char)ram[i];
				i++;
			}
			break;
		}
		case 10:
		{
			halt = true;
			break;
		}
		case 17:
		{
			if(registers[5] >= sHeight || registers[4] >= sWidth)
			{
				cout<<"invalid pixel ("<<registers[4]<<", "<<registers[5]<<")"<<endl;
			}
			frameBuffer[registers[5] * sWidth + registers[4]] = registers[6];
			break;
		}
		case 18:
		{
			d.setRegisters(registers, pc);
			d.setMemory(ram);
			d.setDisplay(frameBuffer);
			break;
		}
		case 19:
		{
			system("pause");
			break;
		}
		default:
		{
			cout<<"invalid Sys call "<<registers[2]<<endl;
		}
	}
}

map<uint8_t, void (*)(RTypeInstruction)> aluInstructions =
{
	{0,[](RTypeInstruction i){registers[i.rd]=registers[i.rt]<<i.shamt;pc+=4;print("sll");}},
	{8,[](RTypeInstruction i){pc=registers[i.rs];print("jr");}},
	{12,[](RTypeInstruction i){syscall();pc+=4;print("syscall");}},
	{37,[](RTypeInstruction i){registers[i.rd]=registers[i.rs]|registers[i.rt];pc+=4;print("or");}},
	{42,[](RTypeInstruction i){registers[i.rd]=((int)registers[i.rs]<(int)registers[i.rt]?1:0);pc+=4;print("slt");}},
};

map<uint8_t, void (*)(uint32_t)> instructions =
{
	{0,[](uint32_t u){RTypeInstruction i(u);aluInstructions[i.funct](i);}},
	{2,[](uint32_t u){JTypeInstruction i(u);pc=(pc&0xf0000000)|(i.address<<2);print("j");}},
	{3,[](uint32_t u){JTypeInstruction i(u);registers[31]=pc+4;pc=(pc&0xf0000000)|(i.address<<2);print("jal");}},
	{4,[](uint32_t u){ITypeInstruction i(u);pc=(int)pc+(int)(registers[i.rs]==registers[i.rt]?SE(i.immediate<<2):4);print("beq");}},
	{9,[](uint32_t u){ITypeInstruction i(u);registers[i.rt]=registers[i.rs]+SE(i.immediate);pc+=4;print("addiu");}},
	{10,[](uint32_t u){ITypeInstruction i(u);registers[i.rt]=((int)registers[i.rs]<(int)SE(i.immediate)?1:0);pc+=4;print("slti");}},
	{13,[](uint32_t u){ITypeInstruction i(u);registers[i.rt]=registers[i.rs]|i.immediate;pc+=4;print("ori");}},
	{15,[](uint32_t u){ITypeInstruction i(u);registers[i.rt]=i.immediate<<16;pc+=4;print("lui");}},
	{35,[](uint32_t u){ITypeInstruction i(u);uint32_t s=registers[i.rs]+i.immediate;registers[i.rt]=ram[s]<<24|ram[s+1]<<16|ram[s+2]<<8|ram[s+3];pc+=4;print("lw");}},
	{43,[](uint32_t u){ITypeInstruction i(u);uint32_t s=registers[i.rs]+i.immediate;uint32_t v=registers[i.rt];ram[s]=v>>24;ram[s+1]=v>>16&0xff;ram[s+2]=v>>8&0xff;ram[s+3]=v&0xff;pc+=4;print("sw");}},
};

/*
void printCpu()
{
	for(int i = 0; i < registers.size(); i++)
	{
		cout<<registerNames[i]<<":0x"<<hex<<setfill('0')<<setw(8)<<registers[i]<<":"<<dec<<registers[i]<<"\t\t";
		if(i % 4 == 0) cout<<"\n";
	}
	cout<<"pc:0x"<<setfill('0')<<setw(8)<<hex<<pc<<":"<<dec<<pc<<endl;
}

void printRam()
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	for(int i = 0; i < ram.size(); i++)
	{
		int color;
		if(i == pc) color = 79;
		else if(i == registers[29] || i == registers[30]) color = 95;
		else if(i <= ep) color = 6;
		else if(i >= registers[29]) color = 13;
		else color = 15;
		SetConsoleTextAttribute(console, color);
		cout<<(ram[i] <= 0xf ? "0" : "")<<hex<<(int)ram[i]<<" ";
	}
	SetConsoleTextAttribute(console, 15);
	cout<<dec;
}


void drawBuffer()
{
	HDC window = GetDC(GetConsoleWindow());
	for(int i = 0; i < sHeight; i++)
		for(int j = 0; j < sWidth; j++)
			SetPixelV(window, j, i + 40, frameBuffer[i * sWidth + j]);
}
*/

inline void printCpu()
{
	d.setRegisters(registers, pc);
}

inline void printRam()
{
	d.setMemory(ram);
}

inline void drawBuffer()
{
	d.setDisplay(frameBuffer);
}

inline void display()
{
	printCpu();
	printRam();
	drawBuffer();
}

void init(int ramSize)
{
	pc = 0;
	ram = vector<uint8_t>(ramSize, 0);
	registers = vector<uint32_t>(32, 0);
	halt = false;
	registers[29] = registers[30] = ramSize - 1;
}

void load(vector<uint8_t> program, uint32_t loc)
{
	for(uint32_t i = 0; i < program.size(); i++)
		ram[i + loc] = program[i];
}

void run()
{
	halt = false;
	while(!halt)
	{
		uint32_t ins = ram[pc]<<24|ram[pc+1]<<16|ram[pc+2]<<8|ram[pc+3];
		try
		{
			instructions.at(ins>>26)(ins);
		}
		catch(...)
		{
			cout<<"ERROR: no instruction with opcode: "<<(ins>>26)<<endl;
		}
		display();
		//system("pause");
	}
}

int main()
{
	//cout<<instructions[0]<<endl;
	vector<string> files = {"asm/header.asm", "asm/print.asm","asm/graphics.asm", "asm/gt.asm"};
	init(400);
	load(all(files, 0), 0);
	run();
	system("pause");
}