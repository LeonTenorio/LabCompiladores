#include <string>
#include <iostream>
#include <bits/stdc++.h> 

using namespace std;

static int dontShowLines = 0;

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

string getRegister(string loc_register){
/*
$zero = [0] $at = [1]   $s0 = [2]  $s1 = [3]  $s2 = [4]  $s3 = [5]  $s4 = [6]  $s5 = [7]  $s6 = [8]  $s7 = [9]  $s8 = [10] 
$s9 = [11]  $t0 = [12]  $t1 = [13] $t2 = [14] $t3 = [15] $t4 = [16] $t5 = [17] $t6 = [18] $t7 = [19] $t8 = [20] $t9 = [21]
$t10 = [22] $t11 = [23] $v0 = [24] $sp = [25] $gp = [26] $sa = [27] $k0 = [28] $k1 = [29] $fp = [30] $ra = [31]
*/
    if(loc_register.compare("$zero")==0)  return "0";
    else if(loc_register.compare("$at")==0)     return "1";
    else if(loc_register.compare("$s0")==0)     return "2";
    else if(loc_register.compare("$s1")==0)     return "3";
    else if(loc_register.compare("$s2")==0)     return "4";
    else if(loc_register.compare("$s3")==0)     return "5";
    else if(loc_register.compare("$s4")==0)     return "6";
    else if(loc_register.compare("$s5")==0)     return "7";
    else if(loc_register.compare("$s6")==0)     return "8";
    else if(loc_register.compare("$s7")==0)     return "9";
    else if(loc_register.compare("$s8")==0)     return "10";
    else if(loc_register.compare("$s9")==0)     return "11";
    else if(loc_register.compare("$t0")==0)     return "12";
    else if(loc_register.compare("$t1")==0)     return "13";
    else if(loc_register.compare("$t2")==0)     return "14";
    else if(loc_register.compare("$t3")==0)     return "15";
    else if(loc_register.compare("$t4")==0)     return "16";
    else if(loc_register.compare("$t5")==0)     return "17";
    else if(loc_register.compare("$t6")==0)     return "18";
    else if(loc_register.compare("$t7")==0)     return "19";
    else if(loc_register.compare("$t8")==0)     return "20";
    else if(loc_register.compare("$t9")==0)     return "21";
    else if(loc_register.compare("$t10")==0)     return "22";
    else if(loc_register.compare("$t11")==0)     return "23";
    else if(loc_register.compare("$v0")==0)     return "24";
    else if(loc_register.compare("$sp")==0)     return "25";
    else if(loc_register.compare("$gp")==0)     return "26";
    else if(loc_register.compare("$sa")==0)     return "27";
    else if(loc_register.compare("$k0")==0)     return "28";
    else if(loc_register.compare("$k1")==0)     return "29";
    else if(loc_register.compare("$fp")==0)     return "30";
    else if(loc_register.compare("$ra")==0)     return "31";
    else{
        cout << "Erro, registrador nao especificado" << endl;
        return "";
    }
}

void lineToBinary(vector<string> params, vector<int> labels){
    
}