#include <string>
#include <iostream>
#include <bits/stdc++.h> 
#include <map>

//Falta o tratamento para as labels

using namespace std;

static vector<string> binaryCode;

vector<string> getAssemblyLineParams(string line){
    vector<string> params;
    if(line.find("*")==string::npos){
        string delimiter = " ";
        string param;
        size_t pos = 0;
        while ((pos = line.find(delimiter)) != std::string::npos) {
            param = line.substr(0, pos);
            param.erase(std::remove(param.begin(), param.end(), ' '), param.end());
            params.push_back(param);
            line.erase(0, pos + delimiter.length());
        }
        line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
        params.push_back(line);
    }
    return params;
}

string convertNumberToBinary4Size(int number){
    return std::bitset<4>(number).to_string();
}

string convertNumberToBinary5Size(int number){
    return std::bitset<5>(number).to_string();
}

string convertNumberToBinary9Size(int number){
    return std::bitset<9>(number).to_string();
}

string convertNumberToBinary14Size(int number){
    return std::bitset<14>(number).to_string();
}

string convertNumberToBinary23Size(int number){
    return std::bitset<23>(number).to_string();
}

string convertNumberToBinary24Size(int number){
    return std::bitset<24>(number).to_string();
}

int _getRegister(string loc_register){/*
    $zero = [0] $at = [1]   $s0 = [2]  $s1 = [3]  $s2 = [4]  $s3 = [5]  $s4 = [6]  $s5 = [7]  $s6 = [8]  $s7 = [9]  $s8 = [10] 
    $s9 = [11]  $t0 = [12]  $t1 = [13] $t2 = [14] $t3 = [15] $t4 = [16] $t5 = [17] $t6 = [18] $t7 = [19] $t8 = [20] $t9 = [21]
    $t10 = [22] $t11 = [23] $v0 = [24] $sp = [25] $gp = [26] $sa = [27] $k0 = [28] $k1 = [29] $fp = [30] $ra = [31]*/
    if(loc_register.compare("$zero")==0)  return 0;
    else if(loc_register.compare("$at")==0)     return 1;
    else if(loc_register.compare("$s0")==0)     return 2;
    else if(loc_register.compare("$s1")==0)     return 3;
    else if(loc_register.compare("$s2")==0)     return 4;
    else if(loc_register.compare("$s3")==0)     return 5;
    else if(loc_register.compare("$s4")==0)     return 6;
    else if(loc_register.compare("$s5")==0)     return 7;
    else if(loc_register.compare("$s6")==0)     return 8;
    else if(loc_register.compare("$s7")==0)     return 9;
    else if(loc_register.compare("$s8")==0)     return 10;
    else if(loc_register.compare("$s9")==0)     return 11;
    else if(loc_register.compare("$t0")==0)     return 12;
    else if(loc_register.compare("$t1")==0)     return 13;
    else if(loc_register.compare("$t2")==0)     return 14;
    else if(loc_register.compare("$t3")==0)     return 15;
    else if(loc_register.compare("$t4")==0)     return 16;
    else if(loc_register.compare("$t5")==0)     return 17;
    else if(loc_register.compare("$t6")==0)     return 18;
    else if(loc_register.compare("$t7")==0)     return 19;
    else if(loc_register.compare("$t8")==0)     return 20;
    else if(loc_register.compare("$t9")==0)     return 21;
    else if(loc_register.compare("$t10")==0)     return 22;
    else if(loc_register.compare("$t11")==0)     return 23;
    else if(loc_register.compare("$v0")==0)     return 24;
    else if(loc_register.compare("$sp")==0)     return 25;
    else if(loc_register.compare("$gp")==0)     return 26;
    else if(loc_register.compare("$sa")==0)     return 27;
    else if(loc_register.compare("$k0")==0)     return 28;
    else if(loc_register.compare("$k1")==0)     return 29;
    else if(loc_register.compare("$fp")==0)     return 30;
    else if(loc_register.compare("$ra")==0)     return 31;
    else{
        cout << "Erro, registrador nao especificado " << loc_register << endl;
        exit (-1);
        return (-1);
    }
}

string getRegister(string loc_register){
    return convertNumberToBinary5Size(_getRegister(loc_register));
}

string toFullBinaryInst(string instruction){
    while(instruction.length()<32){
        instruction = instruction + "X";
    }
    return instruction;
}

string lineToBinary(vector<string> params, vector<string> labels, map<string, int> labels_lines, bool showBinary){
    if(params[0].find(".")==0){//Ignorar essa linha
        if(showBinary)
            cout << params[0] << endl;
        //cout << params[0] << " virou linha " << labels_lines[params[0]] << endl;
        return "";
    }
    else if(params[0].compare("HALT")==0){
        return convertNumberToBinary4Size(0)+convertNumberToBinary4Size(1)+convertNumberToBinary24Size(0);
    }
    else if(params[0].compare("ADD")==0){
        return convertNumberToBinary4Size(1)+convertNumberToBinary4Size(4)+getRegister(params[2])+getRegister(params[3])+getRegister(params[1]);
    }
    else if(params[0].compare("ADDI")==0){
        return convertNumberToBinary4Size(1)+convertNumberToBinary4Size(5)+getRegister(params[2])+getRegister(params[1])+convertNumberToBinary14Size(stoi(params[3]));
    }
    else if(params[0].compare("SUB")==0){
        return convertNumberToBinary4Size(1)+convertNumberToBinary4Size(6)+getRegister(params[2])+getRegister(params[3])+getRegister(params[1]);
    }
    else if(params[0].compare("MULT")==0){
        return convertNumberToBinary4Size(2)+"XXXX"+getRegister(params[1])+getRegister(params[2]);
    }
    else if(params[0].compare("DIV")==0){
        return convertNumberToBinary4Size(3)+"XXXX"+getRegister(params[1])+getRegister(params[2]);
    }
    else if(params[0].compare("B")==0){
        // << params[1] << " indo para a linha " << labels_lines[params[1]] << endl;
        return convertNumberToBinary4Size(4)+convertNumberToBinary4Size(0)+convertNumberToBinary24Size(labels_lines[params[1]]);
    }
    else if(params[0].compare("BL")==0){
        //cout << params[1] << " indo para a linha " << labels_lines[params[1]] << endl;
        return convertNumberToBinary4Size(4)+convertNumberToBinary4Size(1)+convertNumberToBinary24Size(labels_lines[params[1]]);
    }
    else if(params[0].compare("BR")==0){
        //cout << "BR linha: " << binaryCode.size() << endl;
        return convertNumberToBinary4Size(4)+convertNumberToBinary4Size(2)+getRegister(params[1]);
    }
    else if(params[0].compare("BEQ")==0){
        //cout << params[3] << " salto condicional para " << labels_lines[params[3]] << endl;
        return convertNumberToBinary4Size(4)+convertNumberToBinary4Size(3)+getRegister(params[1])+getRegister(params[2])+convertNumberToBinary14Size(labels_lines[params[3]]);
    }
    else if(params[0].compare("BNE")==0){
        //cout << params[3] << " salto condicional para " << labels_lines[params[3]] << endl;
        return convertNumberToBinary4Size(4)+convertNumberToBinary4Size(4)+getRegister(params[1])+getRegister(params[2])+convertNumberToBinary14Size(labels_lines[params[3]]);
    }
    else if(params[0].compare("BLT")==0){
        //cout << params[3] << " salto condicional para " << labels_lines[params[3]] << endl;
        return convertNumberToBinary4Size(4)+convertNumberToBinary4Size(5)+getRegister(params[1])+getRegister(params[2])+convertNumberToBinary14Size(labels_lines[params[3]]);
    }
    else if(params[0].compare("BLE")==0){
        //cout << params[3] << " salto condicional para " << labels_lines[params[3]] << endl;
        return convertNumberToBinary4Size(4)+convertNumberToBinary4Size(6)+getRegister(params[1])+getRegister(params[2])+convertNumberToBinary14Size(labels_lines[params[3]]);
    }
    else if(params[0].compare("BGT")==0){
        //cout << params[3] << " salto condicional para " << labels_lines[params[3]] << endl;
        return convertNumberToBinary4Size(4)+convertNumberToBinary4Size(7)+getRegister(params[1])+getRegister(params[2])+convertNumberToBinary14Size(labels_lines[params[3]]);
    }
    else if(params[0].compare("BGE")==0){
        //cout << params[3] << " salto condicional para " << labels_lines[params[3]] << endl;
        return convertNumberToBinary4Size(4)+convertNumberToBinary4Size(8)+getRegister(params[1])+getRegister(params[2])+convertNumberToBinary14Size(labels_lines[params[3]]);
    }
    else if(params[0].compare("STORE")==0){
        return convertNumberToBinary4Size(5)+"XXXX"+getRegister(params[1])+getRegister(params[2])+"XXXXX"+convertNumberToBinary9Size(stoi(params[3]));
    }
    else if(params[0].compare("LOAD")==0){
        //cout << params[0] +" " + params[1] +" "+params[2] + " "+params[3] << endl;
        return convertNumberToBinary4Size(6)+"XXXX"+getRegister(params[2])+"XXXXX"+getRegister(params[1])+convertNumberToBinary9Size(stoi(params[3]));
    }
    else if(params[0].compare("LI")==0){
        return convertNumberToBinary4Size(7)+getRegister(params[1])+convertNumberToBinary23Size(stoi(params[2]));
    }
    else if(params[0].compare("MOV")==0){
        return convertNumberToBinary4Size(8)+convertNumberToBinary4Size(0)+getRegister(params[1])+"XXXXX"+getRegister(params[2]);
    }
    else if(params[0].compare("MFHI")==0){
        return convertNumberToBinary4Size(8)+convertNumberToBinary4Size(1)+"XXXXXXXXXX"+getRegister(params[1]);
    }
    else if(params[0].compare("MFLO")==0){
        return convertNumberToBinary4Size(8)+convertNumberToBinary4Size(2)+"XXXXXXXXXX"+getRegister(params[1]);
    }
    else if(params[0].compare("IN")==0){
        return convertNumberToBinary4Size(9)+"XXXX"+getRegister(params[1]);
    }
    else if(params[0].compare("OUT")==0){
        return convertNumberToBinary4Size(10)+"XXXX"+getRegister(params[1]);
    }
    else{
        cout << "Erro, linha de assembly não reconhecida" << endl;
        return "";
    }
}

string generateBinary(vector<string> assembly_lines, vector<string> labels, map<string, int> labels_lines, bool binaryToQuartus, bool showBinary){
    string assemblyString = "";
    for(int i=0;i<assembly_lines.size();i++){
        vector<string> params = getAssemblyLineParams(assembly_lines[i]);
        string line = lineToBinary(params, labels, labels_lines, showBinary);
        if(line.length()>0){
            line = toFullBinaryInst(line);
            string lineParams = "";
            for(int i=0;i<params.size();i++){
                lineParams = lineParams + params[i] + " ";
            }
            while(lineParams.length()<20){
                lineParams = lineParams + " ";
            }
            if(showBinary){
                cout << lineParams << " ";
                cout << " - "<< line << " - " << binaryCode.size() << endl;
            }
            binaryCode.push_back(line);
        }
        if(line.length()>32){
            cout << "error " << assembly_lines[i] << " " <<  line << endl;
        }
    }
    cout << binaryCode.size() << " de linhas código binário" << endl;
    for(int i=0;i<binaryCode.size();i++){
        if(binaryToQuartus){
            assemblyString = assemblyString + "registers["+to_string(i)+"] = {32'b" + binaryCode[i] + "};\n";
        }
        else{
            assemblyString = assemblyString + binaryCode[i] + "\n";
        }
    }
    return assemblyString;
}