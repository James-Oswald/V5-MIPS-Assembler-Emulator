
#include<vector>
#include<bitset>
#include<iostream>

using namespace std;

//Printing Utilities
template<typename T>
void print(vector<T> v) //handy vector printer
{
	cout<<'{';
	for(int i = 0; i < v.size(); i++)
	{
		cout<<v[i]<<", ";
	}
	cout<<(v.size()==0?"}":"\b\b}")<<endl;
}

void print(string s)
{
	//cout<<s<<'\n';
}

inline void printBin(vector<uint8_t> v) //bin printer
{
	for(int i = 0; i < v.size(); i++)
	{
		bitset<8> b = v[i];
		cout<<b<<" ";
		if(i % 4 == 0 && i != 0) cout<<"\n";
	}
}

inline void printBin32(vector<uint8_t> v) //bin printer
{
	for(int i = 0; i < v.size(); i += 4)
	{
		bitset<32> b = v[i]<<24|v[i+1]<<16|v[i+2]<<8|v[i+3];
		cout<<i<<":"<<b<<" : ";
		for(int j = i; j < i + 4; j++)
			cout<<(v[j] <= 0xf ? "0" : "")<<hex<<(int)v[j]<<" ";
		cout<<dec<<"\n";
	}
	cout<<dec<<endl;
}

inline void printBin(uint32_t v) //bin printer
{
	bitset<32> b = v;
	cout<<b<<endl;
}

inline void printHex(vector<uint8_t> v) //hex printer
{
	for(int i = 0; i < v.size(); i++)
	{
		cout<<(v[i] < 0xf ? "0" : "")<<hex<<(int)v[i]<<" ";
	}
	cout<<endl;
}