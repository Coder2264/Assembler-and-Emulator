/*------------------------------------------------------------------------------------------------
TITLE: Emulator								
AUTHOR: Md Kamran
ROLL NO.: 2201CS47
Declaration of Authorship
This C++ file, emu.cpp, is part of the assignment of CS210 at the 
department of Computer Science and Engg, IIT Patna . 
------------------------------------------------------------------------------------------------*/


#include "bits/stdc++.h"
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
using namespace std;
using namespace chrono;
using namespace __gnu_pbds;

//MACROS
#define ll long long
#define ull unsigned long long
#define ld long double
#define pb push_back
#define eb emplace_back
#define F first
#define S second
#define pll pair<ll,ll>
#define pii pair<int,int>
#define yes cout << "YES\n" 
#define	no cout << "NO\n"
#define all(x)	x.begin(),x.end()

//ALIASES
template <typename T> using oset = tree<T, null_type, less<T>, rb_tree_tag,tree_order_statistics_node_update>;
typedef vector<bool> vb;
typedef vector<vb> vvb;
typedef vector<int> vi;
typedef vector<vi> vvi;
typedef vector<ll> vl;
typedef vector<vl> vvl;
typedef vector<vvl> vvvl;

//LOOPS
#define fl(i,nx) for(ll i=0;i<nx;i++)
#define cfl(i,nx) for(ll i=1;i<=nx;i++)
#define rfl(i,nx)	for(ll i=nx-1;i>=0;i--)
#define rep(i,a,b)	for(ll i=a;i<=b;i++)

//CONSTANTS
const ll mod1=1000000007;
const ll mod2=998244353;
constexpr ld eps= 1e-16;

/*------------------------------------------------------------------------------------------------*/


std::map<int, std::string> op;	//data strcuture for opcodes

int memory[10000];				//memory
int A = 0, B = 0, PC = 0, SP = 9999;	//32-bit sized registers


std::string DectoHex(int number){
	//converts decimal number into equivalent hexadecimal of length len
	int len=8;
    std::bitset<32> b(number);
	
	ll y=b.to_ullong();
	std::stringstream ss;
	ss<<hex<<y;
	std::string s=ss.str();
    reverse(s.begin(),s.end());
    while((int)s.size()<len){
        s.push_back('0');
    }
    while((int)s.size()>len){
        s.pop_back();
    }
    reverse(s.begin(),s.end());
	return s;


    
    std::string res="";
    std::vector<char> v={'a','b','c','d','e','f'};
    while(number>0){
        int r=number%16;
        number/=16;
        if(r<10){
            res.push_back('0'+r);
        }
        else{
            res.push_back(v[r-10]);
        }
    }
    
    reverse(res.begin(),res.end());
    return res;
}

void ISA() {
	//Gives ISA of the machine
	vector<std::string> instructions = {
       "Opcode Mnemonic Operand",
       "      data       value",
       "0       ldc       value",
       "1       adc       value",
       "2       ldl       value",
       "3       stl       value",
       "4       ldnl      value",
       "5       stnl      value",
       "6       add",
       "7       sub",
       "8       shl",
       "9       shr",
       "10      adj       value",
       "11      a2sp",
       "12      sp2a",
       "13      call      offset",
       "14      return",
       "15      brz       offset",
       "16      brlz      offset",
       "17      br        offset",
       "18      halt",
       "      SET       value"
   };

   // Loop through the vector and output each std::string
   for (const std::string& instruction : instructions) {
       cout << instruction << '\n';
   }
}

void dumping(int PC, ofstream& traceFile){
	std::string s="\n\t\t---Dumping from memory---\t\t\n";
	cout<<s;
	traceFile<<s;
	
	for (int i=0; i<PC; i++){
		if (i % 4){
			// 1 2 3 as remainder
			s=(DectoHex(memory[i])+" ");
			cout<<s;
			traceFile<<s;
			
			
		} else{
			//4's mutiples
			s=("\n"+DectoHex(i)+"\t"+DectoHex(memory[i])+" ");
			cout<<s;
			traceFile<<s;
		}
	} cout<<"\n";
}

// Function to trace individual instructions
void trace(int PC, ofstream& traceFile){
	
	std::string s="\n\t\t---Tracing instructions---\t\t\n\n";
	cout<<s;
	traceFile<<s;

	std::map<int,int> offsets;
	offsets[13]=1;
	offsets[15]=1;
	offsets[16]=1;
	offsets[17]=1;

	int line = 0;

	// Loop till stop is true
	bool stop = false;
	while(true){
		int instr = memory[PC];

		// for calculation of negative numbers in hexadecimal
		ll hex_value = stoll("ffffffff", nullptr, 16);
		int all_max = static_cast<int>(hex_value);

		int max_opCode = stoi("ff", nullptr, 16);

		int tmpCode = instr & 0b11111111;
		std::string tmp = DectoHex(tmpCode);
		tmp = tmp.substr(6, 2);

		int opCode = stoi(tmp, nullptr, 16);
		if (tmp[0] >= '8'){
			opCode = -(max_opCode - opCode + 1);
		} 

		int opr = instr & 0xffffff00;
		if (opr & (1<<31)){				//if operand is negative
			opr = -(all_max - opr + 1);
		}
		opr >>= 8;

		if (op.count(opCode)){
			stringstream ss;

			ss<< "PC: " << DectoHex(PC) << "\tSP: " << DectoHex(SP) << "\tA: " 
			<< DectoHex(A) << "\tB: " << DectoHex(B) << "\t" << op[opCode] 
			<< " " << opr << endl << endl;

			string s=ss.str();
			cout<<s;

			traceFile << s;
		} else{
			stop = true;
		}
		
		switch(opCode){
		case 1:
			A += opr;
			break;
		case 0:
			B = A;
			A = opr;
			break;
		case 3:
			memory[SP+opr] = A;
			A = B;
			break;
		case 6:
			A += B;
			break;
		case 2:
			B = A;
			A = memory[SP+opr];
			break;
		case 10:
			SP += opr;
			break;
		case 8:
			A = B << A;
			break;
		case 4:
			A = memory[A+opr];
			break;
		case 5:
			memory[A+opr] = B;
			break;
		
		case 7:
			A = B - A;
			break;
		case 13:
			B = A;
			A = PC;
			PC += opr;
			break;
		case 9:
			A = B >> A;
			break;
		
		case 11:
			SP = A;
			A = B;
			break;
		case 15:
			if(A == 0){
				PC += opr;
			}
			break;
		case 12:
			B = A;
			A = SP;
			break;
		
		case 14:
			if (PC == A && A == B) stop = true;
			PC = A;
			A = B;
			break;
		case 18:
			stop = true;
			break;
		
		case 16:
			if(A < 0){
				PC += opr;
			}
			break;
		case 17:
			PC += opr;
			break;
		}
		if (SP >= 10000){
			cout<<"SP exceeding the memory at PC: "<<PC<<"\n";
			stop = true;
		}
		if (offsets[opCode] && opr == -1){
			cout<<"Infinite loop detected"<<"\n";
			stop = true;
		} 
		if (stop) break;
		PC++;
		line++;
	}
	cout<<line<<" number of instructions executed!\n";
}

void throw_error(){
	cout<<"Error! wrong usage of command\n";
	cout<<"Correct Format: ./emu [task] filename.o\n";
	cout<<"Where [task] can be from below:\n";
	cout<<"\t-Trace : Display executed instruction's .\n";
	cout<<"\t-Before : Display memory dump before the execution of program .\n";
	cout<<"\t-After : Display memory dump after the execution of program .\n";
	cout << "\t-ISA\tdisplay the ISA of Machine\n";
}

int twos_complement(ll upp,ll num){
	ll nn=-(upp-num+ 1);
	return nn;
}

int BintoDec(std::string s){
    int n=s.size();
    int ans=0;
    ans-=s[0]-'0';
    for(int i=1;i<n;i++){
    	ans*=2;
        ans+=s[i]-'0';
        
    }
    return ans;
}

int main(int argc, char* argv[]){
	

	if (argc < 3){
		throw_error();
		return 0;
	}

	
	
	//this is opcode assignment
	op[1] = "adc";
	op[0] = "ldc";
	op[7] = "sub";
	op[14] = "return";
	op[3] = "stl";
	op[5] = "stnl";
	op[9] = "shr";
	
	op[11] = "a2sp";
	op[6] = "add";
	op[13] = "call";
	op[15] = "brz";
	op[17] = "br";
	op[10] = "adj";
	op[18] = "halt";
	op[-1] = "data";
	op[-2] = "SET";
	op[4] = "ldnl";
	op[8] = "shl";
	op[12] = "sp2a";
	op[16] = "brlz";
	op[2] = "ldl";

	std::string inputFile = std::string(argv[2]);
	
	std::string traceFile = inputFile.substr(0, inputFile.find('.')) + ".trace";

	ifstream input(inputFile, ios::out | ios::binary);
	ofstream output(traceFile);

	std::string text;
	getline(input, text);
	if (text.size()==0) return 0;

	int line = 0;

	// for calculation of negative numbers in hexadecimal
	int max_operand = stoi("ffffff", nullptr, 16);
	int max_opCode = stoi("ff", nullptr, 16);

	for (int start = 0; start < text.size(); start += 32){
		std::string machineCode = text.substr(start, 32);
		ull binary_value = stoul(machineCode, nullptr, 2);
		int instr = static_cast<int>(binary_value);

		
		std::string tmp = machineCode.substr(24);		//string storing binary opcode
		
		string operand=machineCode.substr(0, 24);
		int opr =BintoDec(operand);
		int opcode=BintoDec(tmp);
		if(opcode<0){
			// negative opcode-> can be data or set 
			memory[line]=opr;
		}
		else{
			memory[line]=instr;
		}
		line++;
	}

	// Dumping before execution
	if(std::string(argv[1]) == "-Before"){
		dumping(line, output);
	}

	// Tracing each instruction
	else if(std::string(argv[1]) == "-Trace"){
		trace(PC, output);
	}

	// Dumping after execution
	else if(std::string(argv[1]) == "-After"){
		trace(PC, output);
		dumping(line, output);
	}

	else if(std::string(argv[1]) == "-ISA"){
		ISA();
	}
	else{
		throw_error();
		return 0;
	}
	return 0;
}