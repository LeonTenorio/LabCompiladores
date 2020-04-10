#include <string>
#include <iostream>
#include <bits/stdc++.h> 
#include "symtab.cpp"

#define TEMPREGISTERSAMOUNT 12
#define USETEMPREGISTERAMOUNT 10
#define STATICREGISTERSAMOUNT 10
#define USESTATICREGISTERAMOUNT 8

using namespace std;

static vector<string> lines;

vector<string> getLineParams(string line){
    vector<string> params;
    string delimiter = ",";
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
    return params;
}

void parseQuadCode(string quad){
    string line_delimiter = "\n";
    string line;
    size_t pos = 0;
    while ((pos = quad.find(line_delimiter)) != std::string::npos) {
        line = quad.substr(0, pos);
        line.erase (0,1);
        line.erase(line.length()-1, 1);
        lines.push_back(line);
        quad.erase(0, pos + line_delimiter.length());
    }
    line = quad;
    line.erase (0,1);
    line.erase(line.length()-1, 1);
    lines.push_back(line);
    cout << to_string(lines.size()) << " linhas no código intermediário" << endl;
}

static vector<string> assembly;

static int scope_mem_pos = 0;
static int local_mem_pos = 0;

static int perm_register_index = 0;

static string scope = " ";

void lineQuadCodeToAssembly(vector<string> params){
    if(params[0].compare("goto")==0){
        assembly.push_back("B ."+params[1]);
    }
    else if(params[0].compare("pop_param")==0){
        
    }
    else if(params[0].compare("fun")==0){
        
    }
    else if(params[0].compare("system_in")==0){
        
    }
    else if(params[0].compare("asn_ret")==0){
        
    }
    else if(params[0].compare("end_fun")==0){

    }
    else if(params[0].compare("system_out")==0){

    }
    else if(params[0].compare("catch_return")==0){

    }
    else if(params[0].compare("asn_ret")==0){

    }
    else if(params[0].compare("label")==0){

    }
    else if(params[0].compare("if_le")==0){

    }
    else if(params[0].compare("if_l")==0){

    }
    else if(params[0].compare("if_g")==0){

    }
    else if(params[0].compare("if_e")==0){

    }
    else if(params[0].compare("if_ge")==0){

    }
    else if(params[0].compare("if_e")==0){

    }
    else if(params[0].compare("if_ne")==0){

    }
    else if(params[0].compare("jal")==0){

    }
    else if(params[0].compare("asn")==0){

    }
    else if(params[0].compare("+")==0){

    }
    else if(params[0].compare("-")==0){

    }
    else if(params[0].compare("*")==0){
        
    }
    else if(params[0].compare("/")==0){

    }
}