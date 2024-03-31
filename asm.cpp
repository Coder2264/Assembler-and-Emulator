/*****************************************************************************
TITLE: Assembler                                                                                                                             
AUTHOR: Md Kamran
ROLL NUMBER: 2201CS47
Declaration of Authorship
This C++ file, asm.cpp, is part of the assignment of CS210 at the 
department of Computer Science and Engineering, IIT Patna. 
*****************************************************************************/


#include <bits/stdc++.h>
using namespace std;

std::vector<std::pair<int,std::string>> errors;    //List of lines with errors

unordered_map<std::string, int> labels;        //stores at which value of PC we can find that label
unordered_set<std::string> inserted_label;     //for ascertaining whether label has been inserted or not
std::map<int, std::vector<std::string>> code;
std::map<int,int> ind;
std::vector<std::pair<std::string,std::string>> instructions;
std::vector<std::pair<int, int>> machine_codes;  //<opcode,operand> form



std::map<std::string, std::pair<int, int>> op; 


std::string remove_whitespace(std::string text){
    //Removes trailing and leading whitespaces in a given std::string
    //Caution-> Take care of both empty spaces as well as tabs(treated as separate character though appear like mutiple spaces )
    int sz=text.size();
	int start=0, end=sz-1;
	while(start<sz-1 && (text[start]==' '|| text[start]=='\t')){
		start++;
	}
	while(end>=0 && (text[end]==' '|| text[end]=='\t')){
		end--;
	}
	std::string ans=text.substr(start,end-start+1);
	return ans;
}

std::string DectoHex(int number,int len){

    bitset<32> b(number);
	
	long long y=b.to_ullong();
	stringstream ss;
	ss<<hex<<y;
	string s=ss.str();
    reverse(s.begin(),s.end());
    while((int)s.size()<len){
        s.push_back('0');
    }
    while((int)s.size()>len){
        s.pop_back();
    }
    reverse(s.begin(),s.end());
	return s;


    //converts decimal number into equivalent hexadecimal of length len
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

int HexToDec(std::string s){
    int n=s.size();
    int res=0;
    //0x must be in beginning of std::string before call so not checking
    std::map<int,int> charval;
    charval['a']=10;        charval['A']=10;
    charval['b']=11;        charval['B']=11;
    charval['c']=12;        charval['C']=12;
    charval['d']=13;        charval['D']=13;
    charval['e']=14;        charval['E']=14;
    charval['f']=15;        charval['F']=15;
    for(int i=2;i<n;i++){
        res*=16;
        if(s[i]>='0' && s[i]<='9'){
            res+=s[i]-'0';
        }
        else{
            res+=charval[s[i]];
        }
    }
    return res;
}

int OctToDec(std::string s){
    //Assuming first character is 0
    int n=s.size();
    int res=0;
    for(int j=1;j<n;j++){
        res*=8;
        res+=s[j]-'0';
    }
    return res;
}

std::string DectoOct(int n){
    std::string res="";
    while(n>0){
        int rem=n%8;
        n/=8;
        res.push_back(rem+'0');
    }
    return res;
}

std::string operand_to_bin(int n){
    bitset<24> b(n);
    string s=b.to_string();
    return s;
}

std::string opcode_to_bin(int n){
    bitset<8> b(n);
    string s=b.to_string();
    return s;
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

bool isValidLabel(std::string label){
    //Label is alphanumeric and starts with an alphabet
    char c=label[0];
    if((c<'A'||c>'Z')&&(c<'a'||c>'z')){
        return 0;
    }
    for(char c:label){
        if((c<'A'||c>'Z')&&(c<'a'||c>'z')&&(c<'0'||c>'9')){
            return 0;
        }
    }
    return 1;
}

void insertLabel(std::string label,int PC,int line){
    if(!isValidLabel(label)){
        errors.push_back({line,"Bogus labelname: "+label});
    }
    else if(inserted_label.find(label)==inserted_label.end()){
		labels[label]=PC;
        inserted_label.insert(label);
	}
    else{
		errors.push_back({line,"Duplicate label definition: "+label});
	}
}


int decode(std::string operan, int line){
    //Gives the number represented by operand

    //If it is a Label
	if (isValidLabel(operan)){
		if (inserted_label.find(operan)==inserted_label.end()){
			errors.push_back({line, "No such label: " + operan});
		}
		return labels[operan];
	}
    //If it is a number
    int n=operan.size();
    int result=0;
    bool invalidnum=0;
    if(n<1){return 0;}
	if(n==1){
        //Only decimal is possible that too without sign
        if(operan[0]<'0'||operan[0]>'9'){
            invalidnum=1;
        }
        else{
            result=stoi(operan);
        }
    }
    else if(n==2){
        //Only octal or decimal
        if(operan[0]=='0'){
            if(operan[1]<'0'||operan[1]>'9'){
                invalidnum=1;
            }
            else{
                result=operan[1]-'0';
            }
        }
        else{
            for(int i=0;i<2;i++){
                if(operan[1]<'0'||operan[1]>'9'){
                    invalidnum=1;
                    break;
                }
            }
            if(!invalidnum){
                result=stoi(operan);
            }
        }
    }
    else{
        
        if(operan[0]=='0'){
            if(operan[1]=='b'){
                //Binary number
                
                for(int i=2;i<n;i++){
                    if(operan[i]!='0' && operan[i]!='1'){
                        invalidnum=1;
                        break;
                    }
                }
                if(!invalidnum){
                    bitset<32> b(operan.substr(2));
                    result=b.to_ullong();
                }
            }

            else if(operan[1]=='x'){
                //Hexadecimal number
                for(int i=2;i<n;i++){
                    if(operan[i]>='0' && operan[i]<='9'){
                        continue;
                    }
                    else{
                        int del=operan[i]-'a';
                        if(del>=0 && del<=5){
                            continue;
                        }
                        del=operan[i]-'A';
                        if(del>=0 && del<=5){
                            continue;
                        }
                        invalidnum=1;
                        break;
                    }
                }
                if(!invalidnum){
                    result=HexToDec(operan);
                }
            }
            else{
                //Octal Number
                for(int i=1;i<n;i++){
                    if(operan[i]<'0'|| operan[i]>'7'){
                        invalidnum=1;
                        break;
                    }
                }
                if(!invalidnum){
                    result=OctToDec(operan);
                }
            }   
        }
        else{
            //Decimal
            int begi=0;
            if(operan[0]=='+' || operan[0]=='-'){
                begi++;
            }
            for(int i=begi;i<n;i++){
                if(operan[i]<'0' || operan[i]>'9'){
                    invalidnum=1;
                    break;
                }
            }
            if(!invalidnum){
                result=stoi(operan);
            }
        }
    }

    if(invalidnum){
        errors.push_back({line,"Error! Not a Number "+operan});
    }

    return result;
}

void First_Pass(std::string file_name){
    ifstream content;
    content.open(file_name);
    std::string text;
    int PC=0;
    int line=0;

    std::pair<int,std::string> lastLabel;
    while(getline(content,text)){
        line++;
        text=remove_whitespace(text);
        code[PC].push_back(text);
        text=text.substr(0,text.find(';'));
        text=remove_whitespace(text);
        std::string label="",instr="",operan="";

        if(text.find(':')!=std::string::npos){
            int colon_pos=text.find(':');
            label = text.substr(0, colon_pos);
			label = remove_whitespace(label);
            if(label.size()>0){
                insertLabel(label,PC,line);
                lastLabel={PC,label};
            }
			text = text.substr(colon_pos+1);
			text = remove_whitespace(text);
        }

        if(text.find(' ')!=std::string::npos){
            int space_pos=text.find(' ');
            instr=text.substr(0, space_pos);
            instr=remove_whitespace(instr);
            operan=text.substr(space_pos+1); 
            operan=remove_whitespace(operan);
        }
        else{
            instr=remove_whitespace(text);
        }
        
        
        if(text.size()==0){     //Only label without instructions and operands
            continue;           //here line will be incremented but PC wont be
        }
        ind[PC]=line;
        

        if (op.find(instr)==op.end()){
			errors.push_back({line, "Bogus Mnemonic: " + instr});
		}

        
        if(instr=="SET"){
            if(lastLabel.first==PC){
                labels[label]=decode(operan,line);
            }
            else{
                errors.push_back({line,"Label doesn't exist: "+instr});
            }
        }



		if (operan.size()==0 && op[instr].first){
			errors.push_back({line, "Missing operand: "+instr});
		} else if (operan.size() && !op[instr].first){
			errors.push_back({line, "Unexpected operand: "+instr+" "+operan});
		}
		
		instructions.push_back({instr, operan});
        PC++;
    }
    std::cout<<"First Pass done!\n";
    
}


void SecondPass(ofstream &LogFile, ofstream &OutFile, ofstream &ObjFile){
    
    int till1=instructions.size();
    for(int i=0;i<till1;i++){
        int line = ind[i];
		std::string instr = instructions[i].first;
		std::string operan = instructions[i].second;

        int opCode = op[instr].second;

        
		int operand=op[instr].first?decode(operan,line):0;

        if(instr=="data"){
            //No opcode given for data in PDF
            opCode=operand & 0b11111111;        //bottom 8 bits for opcode
            operand>>=8;                        //Upper 24 bits for operand
        }

        machine_codes.push_back({opCode,operand});
    }
    if(errors.size()>0){
        std::cout<<"Error Found in Code! Please check the log file\n";
        sort(errors.begin(),errors.end());
        int esz=errors.size();
        for(int i=0;i<esz;i++){
            LogFile<<"Error in Line number: "<<errors[i].first<<" "<<errors[i].second<<"\n";
        }
        return;
    }
    


    
    int PC=0;
    std::map<int,int> offsets;
	offsets[13]=1;
    offsets[15]=1;
    offsets[16]=1;
    offsets[17]=1;
	for (int PC=0; PC<machine_codes.size(); PC++){
		OutFile << DectoHex(PC, 8) << " ";
		for (int i=0; i<code[PC].size()-1; i++){
			std::string s = code[PC][i];
			OutFile << "\t\t " << " " << s <<"\n";
			OutFile << DectoHex(PC, 8) << " ";
		}

		int opCode = machine_codes[PC].first;
		int operan = machine_codes[PC].second;

		if (offsets[opCode]==1){
			operan -= PC + 1;          //For branching, calculating actual address from relative
		} 
		OutFile << DectoHex(operan, 6) << DectoHex(opCode, 2) << " " << code[PC][code[PC].size()-1] << "\n";

        
        ObjFile<<operand_to_bin(operan)<<opcode_to_bin(opCode);
        
        
	}
    std::cout<<"Second Pass done!\n";
    
}


int main(int count,char **listo){

    if(count<2){
        std::cout<<"Error! Filename not given\n";
        return 0;
    }

    op["ldc"]={1, 0},
	op["adc"]={1, 1},
	op["ldl"]={1, 2},
	op["stl"]={1, 3},
	op["ldnl"]={1, 4},
	op["stnl"]={1, 5},
	op["add"]={0, 6},
	op["sub"]={0, 7},
	op["shl"]={0, 8},
	op["shr"]={0, 9},
	op["adj"]={1, 10},
	op["a2sp"]={0, 11},
	op["sp2a"]={0, 12},
	op["call"]={1, 13},
	op["return"]={0, 14},
	op["brz"]={1, 15},
	op["brlz"]={1 ,16},
	op["br"]={1, 17},
	op["HALT"]={0, 18},
	op["data"]={1, -1},
	op["SET"]={1, -2};


    
    std::string inputFile=std::string(listo[1]);
    First_Pass(inputFile);

    //Creating output files with same name
    std::string name=inputFile.substr(0,inputFile.find('.'));
    ofstream outputFile,logFile,objectFile;
    outputFile.open(name+".lst");     //LISTING FILE
    logFile.open(name+".log");      //LOG FILE
    objectFile.open(name+".o",ios::binary|ios::out);     //Object File

    SecondPass(logFile,outputFile,objectFile);

    

    return 0;
}

