
#include<vector>
#include<map>
#include<string>
#include<cstdint>
#include<iterator>
#include<fstream>

#include<iostream>
#include<windows.h>

//#include"instructions.h"
//#include"print.h"

using namespace std;

//utilities 
inline vector<string> split(string s, char delim, int maxSplits = 0xffff) //allow 2^16 splits by default
{
	vector<string> rv;
	string cur = "";
	int splits = 0;
	for(uint32_t i = 0; i < s.length(); i++)
		if(s[i] == delim && splits < maxSplits)
		{
			rv.push_back(cur);
			cur = "";
			splits++;
		}
		else
			cur += s[i];
	rv.push_back(cur);
	return rv;
}

inline void push32(vector<uint8_t>& a, uint32_t b)
{
	a.push_back((b & 0xff000000)>>24);
	a.push_back((b & 0x00ff0000)>>16);
	a.push_back((b & 0x0000ff00)>>8);
	a.push_back(b & 0x000000ff);
}

template<typename T>
int linearSearch(vector<T> v, T item)
{
	for(int i = 0; i < v.size(); i++)
		if(v[i] == item)
			return i;
	return -1;
}

template<typename T>
vector<T> concat(vector<T> v1, vector<T> v2)
{
	vector<T> rv;
	for(int i = 0; i < v1.size(); i++)
		rv.push_back(v1[i]);
	for(int i = 0; i < v2.size(); i++)
		rv.push_back(v2[i]);
	return rv;
}

//Data

const map<char, char> specialCharaters
{
	{'n', '\n'},
	{'t', '\t'},
	{'0', '\0'}
};

const vector<string> psudoNames =
{
	"li", "move", "nop", "j", "b", "slt"
};

#define _hi(a)(a >> 16)
#define _lo(a)(a & 0xffff)
#define _numeric(a)(a >= '0' && a <= '9' || a == '-')
#define _alphabetic(a)(a >= 'a' && a <= 'z' || (a >= 'A' && a <= 'Z'))
#define _legalStart(a)(_alphabetic(a) || a == '_')

map<string, vector<vector<string>> (*)(vector<string>)> pseudoIMap = //pseudo-Instruction definition Map 
{
	{"li", [](vector<string> a){vector<vector<string>> r = {{"lui", "1", to_string(_hi(stoi(a[2])))}, {"ori", a[1], "1", to_string(_lo(stoi(a[2])))}}; return r;}},
	{"move", [](vector<string> a){vector<vector<string>> r = {{"addiu", a[1], a[2], "0"}}; return r;}},
	{"nop", [](vector<string> a){vector<vector<string>> r = {{"sll", "0", "0", "0"}}; return r;}},
	{"j", [](vector<string> a){vector<vector<string>> r = {{(_numeric(a[1][0]) ? "jr" : "j"), a[1]}}; return r;}},
	{"b", [](vector<string> a){vector<vector<string>> r = {{"beq", "0", "0", a[1]}}; return r;}},
	{"slt", [](vector<string> a){vector<vector<string>> r = {{(stoi(a[3]) > 0 ? "slti" : "slt"), a[1], a[2], a[3]}}; return r;}}, //temporary until $ issue is revised //make all slt slti's
};

map<string, uint32_t (*)(vector<string>)> IMap =
{
	{"addiu", [](vector<string> a){return (uint32_t)ITypeInstruction(9, stoi(a[2]), stoi(a[1]), stoi(a[3]));}},
	{"sw", [](vector<string> a){return (uint32_t)ITypeInstruction(43, stoi(a[3]), stoi(a[1]), stoi(a[2]));}},
	{"lui", [](vector<string> a){return (uint32_t)ITypeInstruction(15, 0, stoi(a[1]), stoi(a[2]));}},
	{"jal", [](vector<string> a){return (uint32_t)JTypeInstruction(3, 0);}},
	{"sll", [](vector<string> a){return (uint32_t)RTypeInstruction(0, 0, stoi(a[2]), stoi(a[1]), stoi(a[3]), 0);}},
	{"lw", [](vector<string> a){return (uint32_t)ITypeInstruction(35, stoi(a[3]), stoi(a[1]), stoi(a[2]));}},
	{"j", [](vector<string> a){return (uint32_t)JTypeInstruction(2, 0);}},
	{"jr", [](vector<string> a){return (uint32_t)RTypeInstruction(0, stoi(a[1]), 0, 0, 0, 8);}},
	{"syscall", [](vector<string> a){return (uint32_t)RTypeInstruction(0, 0, 0, 0, 0, 12);}},
	{"ori", [](vector<string> a){return (uint32_t)ITypeInstruction(13, stoi(a[2]), stoi(a[1]), stoi(a[3]));}},
	{"or", [](vector<string> a){return (uint32_t)RTypeInstruction(0, stoi(a[2]), stoi(a[3]), stoi(a[1]), 0, 37);}},
	{"beq", [](vector<string> a){return (uint32_t)ITypeInstruction(4, stoi(a[1]), stoi(a[2]), 0);}},
	{"slt", [](vector<string> a){return (uint32_t)RTypeInstruction(0, stoi(a[2]), stoi(a[3]), stoi(a[1]), 0, 42);}},
	{"slti", [](vector<string> a){return (uint32_t)ITypeInstruction(10, stoi(a[2]), stoi(a[1]), stoi(a[3]));}},
};

void assemble(string program, vector<uint8_t>& rv, vector<string>& mySym, vector<uint32_t>& mySymL, vector<string>& reqSym, vector<uint32_t>& reqSymL)
{
	vector<string> lines = split(program, '\n');
	for(uint32_t i = 0; i < lines.size(); i++) //remove whitespace
	{
		int j = 0, lastComma = -1, lastPStart = -1;
		while(j < lines[i].length()) //Replace this with a while loop sometime soon, the for loop isn't real
		{
			if(lines[i][j] == ',')
				lastComma = j;
			if(lines[i][j] == '\t' || lines[i][0] == ' ' || (lines[i][j - 1] == ' ' && lines[i][j] == ' ') ||\
			(lines[i][j] == ')' && lastComma < lastPStart) || lines[i][j] == '$' || (lines[i][j-1] == ',' && lines[i][j] == ' '))
			{
				for(uint32_t k = j; k < lines[i].length(); k++)
					lines[i][k] = lines[i][k + 1];
				lines[i].resize(lines[i].length() - 1);
			}
			else if(lines[i][j] == '(' && lastComma != -1)
			{
				lines[i][j] = ',';
				lastPStart = j;
				j++;
			}
			else if(lines[i][j] == '\\') //check for specialCharaters, leaves bytes of what special after char, sloppy
			{
				lines[i][j] = specialCharaters.at(lines[i][j + 1]); //failure if \ followed by new line
			}
			else
				j++;
		}
	}
	vector<vector<string>> tProgram;
	for(uint32_t i = 0; i < lines.size(); i++) //replace pseudo ops and tokenize
	{
		if(lines[i] == "") continue;
		vector<string> preTokens = split(lines[i], ' ', 1);
		vector<string> tokens = {preTokens[0]};
		if(preTokens.size() > 1)
		{
			if(preTokens[1].find('(') == -1) //in reality, should count number of commas before '(' and limit to that many splits
			{
				vector<string> args = split(preTokens[1], ','); //tokenize
				for(uint32_t j = 0; j < args.size(); j++)
					tokens.push_back(args[j]);
			}
			else
				tokens.push_back(preTokens[1]);
		}
		for(uint32_t j = 1; j < tokens.size(); j++)	
			for(uint32_t k = 0; k < registerNames.size(); k++) //replace literal register names with numbers
				if(tokens[j] == registerNames[k])
				{
					tokens[j] = to_string(k);
					break;
				}
		bool isPsudo = false;
		for(uint32_t j = 0; j < psudoNames.size(); j++)	//replace pseudo Instructions with true instructions
		{
			if(tokens[0] == psudoNames[j])
			{
				isPsudo = true;
				vector<vector<string>> trueIns = pseudoIMap[tokens[0]](tokens);
				for(uint32_t k = 0; k < trueIns.size(); k++)
					tProgram.push_back(trueIns[k]);
				break;
			}
		}
		if(!isPsudo)
			tProgram.push_back(tokens);
	}
	for(int i = 0; i < tProgram.size(); i++) print(tProgram[i]); //Display program breakdown
	for(uint32_t i = 0; i < tProgram.size(); i++) //Compilation section
	{
		string firstWord = tProgram[i][0];
		if(firstWord[firstWord.length() - 1] == ':') //If label
		{
			while(rv.size() % 4 != 0)
				rv.push_back(0);
			mySym.push_back(firstWord.substr(0, firstWord.length() - 1));
			mySymL.push_back(rv.size());
		}
		else if(firstWord[0] == '.') //if data, for future, add functions to handle different directives
		{
			if(firstWord == ".ascii") //if data of type ascii
				for(uint32_t j = 1; j < tProgram[i][1].length() - 1; j++)
					rv.push_back((uint8_t)tProgram[i][1][j]);
		}
		else //if instruction
		{
			for(uint8_t j = 1; j < tProgram[i].size(); j++) //for each operand
			{
				char fc = tProgram[i][j][0];
				if(!_numeric(fc)) //if label
				{
					if(fc == '%') //handle %hi and %lo
					{
						reqSym.push_back((tProgram[i][j][1] == 'h' ? "<" : ">") + tProgram[i][j + 1]);
						reqSymL.push_back(rv.size() + 2);
						tProgram[i].pop_back();
					}
					else if(firstWord[0] == 'b') //branch instructions
					{
						reqSym.push_back("#" + tProgram[i][j]);
						reqSymL.push_back(rv.size() + 2);
					}
					else
					{
						reqSym.push_back("=" + tProgram[i][j]);
						reqSymL.push_back(rv.size());
					}
					tProgram[i][j] = "0";
				}
			}
			uint32_t instruction;
			try
			{
				instruction = IMap.at(firstWord)(tProgram[i]);
			}
			catch(...)
			{
				throw "Unrecognized Instruction: " + firstWord;
			}
			push32(rv, instruction);
		}
	}
}

void link(vector<vector<uint8_t>> programs, vector<vector<string>> mySyms, vector<vector<uint32_t>> mySymLs,\
vector<vector<string>> reqSyms, vector<vector<uint32_t>> reqSymLs, vector<uint8_t>& program, vector<string>& mySym,\
vector<uint32_t>& mySymL, vector<string>& reqSym, vector<uint32_t>& reqSymL)
{
	for(int i = 0; i < programs.size(); i++) //for each program
	{
		for(int j = i + 1; j < programs.size(); j++) //for each program after current program
		{
			for(int k = 0; k < mySymLs[j].size(); k++) //for each of programs owned symbols
				mySymLs[j][k] += programs[i].size();
			for(int k = 0; k < reqSymLs[j].size(); k++) //for each of programs required symbols
				reqSymLs[j][k] += programs[i].size();
		}
		program = concat<uint8_t>(program, programs[i]);
	}
	for(int i = 0; i < mySyms.size(); i++) //for each of my symbol lists
	{
		mySym = concat<string>(mySym, mySyms[i]);
		mySymL = concat<uint32_t>(mySymL, mySymLs[i]);
	}
	for(int i = 0; i < reqSyms.size(); i++) //for each of the required symbol lists
	{
		reqSym = concat<string>(reqSym, reqSyms[i]);
		reqSymL = concat<uint32_t>(reqSymL, reqSymLs[i]);
	}
	for(int i = 0; i < mySym.size(); i++)
	{
		for(int j = 0; j < mySym.size(); j++)
			if(i != j && mySym[i] == mySym[j])
				throw "duplicate symbol: " + mySym[i];
		bool found = false;
		for(int j = 0; j < reqSym.size(); j++)
		{
			if(mySym[i] == reqSym[j].substr(1))
			{
				found = true;
				break;
			}
		}
		if(!found)
		{
			cout<<"NOTE: Unused Symbol Removed: "<<mySym[i]<<endl;
			int start = mySymL[i], end = (i + 1 >= mySym.size() ? mySym.size() : mySymL[i + 1]);
			program.erase(program.begin() + start, program.begin() + end);
			mySym.erase(mySym.begin() + i);
			mySymL.erase(mySymL.begin() + i);
			i--;
			for(int j = 0; j < mySymL.size(); j++) //for each of programs owned symbols
				mySymL[j] -= mySymL[j] > start ? end - start : 0;
			for(int j = 0; j < reqSymL.size(); j++) //for each of programs required symbols
				reqSymL[j] -= reqSymL[j] > start ? end - start : 0;
		}
	}
	for(int i = 0; i < reqSym.size(); i++)
		if(linearSearch(mySym, reqSym[i].substr(1)) == -1)
			throw "Symbol does not exist: " + reqSym[i].substr(1);
}


//Keep in mind the offset legally must me divisible by 4 because of the two byte jump shift. 
vector<uint8_t> load(vector<uint8_t> program, vector<string> mySym, vector<uint32_t> mySymL, vector<string> reqSym, vector<uint32_t> reqSymL, uint32_t base)
{
	for(int i = 0; i < reqSym.size(); i++) //for each symbol to replace
	{
		uint32_t address;
		switch(reqSym[i][0])
		{
			case '>':
			case '<':
				address = mySymL[linearSearch(mySym, reqSym[i].substr(1))] + base;
				if(reqSym[i][0] == '<')
				{
					program[reqSymL[i]] = address>>24&0xff;
					program[reqSymL[i] + 1] = address>>16&0xff;
				}
				if(reqSym[i][0] == '>')
				{
					program[reqSymL[i]] = address>>8&0xff;
					program[reqSymL[i] + 1] = address&0xff;
				}
				break;
			case '#': //branches
			{
				int myLoc = reqSymL[i] - 2;
				address = (mySymL[linearSearch(mySym, reqSym[i].substr(1))] - myLoc + base)>>2; //potential mishap is location is before current
				//cout<<"branch("<<reqSymL[i]<<") = "<<(int)address<<endl;
				program[reqSymL[i]] = address>>8&0xff;
				program[reqSymL[i] + 1] = address&0xff;
				break;
			}
			case '=': //jumps
			default:
				address = (mySymL[linearSearch(mySym, reqSym[i].substr(1))] + base)>>2;
				program[reqSymL[i]] |= address>>24&0xff;
				program[reqSymL[i] + 1] = address>>16&0xff;
				program[reqSymL[i] + 2] = address>>8&0xff;
				program[reqSymL[i] + 3] = address&0xff;
		}
	}
	return program;
}


void printVerboseInfo(vector<uint8_t> program, vector<string> mySym, vector<uint32_t> mySymL, vector<string> reqSym, vector<uint32_t> reqSymL)
{
	cout<<"Generated Binary Object:"<<endl;
	printBin32(program);
	cout<<"Generated Syntax Table:"<<endl;
	print(mySym);
	print(mySymL);
	cout<<"Generated Dependency List:"<<endl;
	print(reqSym);
	print(reqSymL);
}

vector<uint8_t> all(vector<string> files, uint32_t base)
{
	try
	{
		vector<vector<uint8_t>> programs;
		vector<vector<string>> mySyms, reqSyms;
		vector<vector<uint32_t>> mySymLs, reqSymLs;
		vector<uint8_t> program;
		vector<string> mySym, reqSym;
		vector<uint32_t> mySymL, reqSymL;
		for(int i = 0; i < files.size(); i++)
		{
			program.clear();
			mySym.clear();
			mySymL.clear();
			reqSym.clear();
			reqSymL.clear();
			cout<<endl<<"Assembling "<<files[i]<<":"<<endl;
			ifstream ifs(files[i]);
			assemble(string((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>())), program, mySym, mySymL, reqSym, reqSymL);
			programs.push_back(program);
			mySyms.push_back(mySym);
			mySymLs.push_back(mySymL);
			reqSyms.push_back(reqSym);
			reqSymLs.push_back(reqSymL);
			printVerboseInfo(program, mySym, mySymL, reqSym, reqSymL);
		}
		cout<<endl<<"Linking Objects:"<<endl;
		program.clear();
		mySym.clear();
		mySymL.clear();
		reqSym.clear();
		reqSymL.clear();
		link(programs, mySyms, mySymLs, reqSyms, reqSymLs, program, mySym, mySymL, reqSym, reqSymL);
		printVerboseInfo(program, mySym, mySymL, reqSym, reqSymL);
		cout<<endl<<"Loading Program:"<<endl;
		vector<uint8_t> rv = load(program, mySym, mySymL, reqSym, reqSymL, base);
		cout<<"Final Program:"<<endl;
		printBin32(rv);
		return rv;
	}
	catch(string e)
	{
		cout<<"Error: "<<e<<endl;
		system("pause");
		return vector<uint8_t>();
	}
}

/*int main()
{
	vector<string> files = {"asm/print.asm", "asm/helloWorld.asm"};
	vector<uint8_t> bin = all(files, 0);
	system("pause");
}*/