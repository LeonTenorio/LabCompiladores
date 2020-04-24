#include <string>
#include <iostream>
#include <bits/stdc++.h> 
#include "symtab.cpp"

#define TEMPREGISTERSAMOUNT 12
#define USETEMPREGISTERAMOUNT 9
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
}

static vector<string> assembly;
static vector<string> labels;

static string scope = " ";

static int globalVarScope = 0;

static int static_scope_register = 0;
static int temp_scope_register = 0;
static int argument_pos = 0;
vector<int> temp_mem_pos;//Use for all temporary index bigger than USETEMPREGISTERAMOUNT-1

static int previous_mem_pos = 0;

static int params_amount = 0;

static int mem_pos = 0;

bool isNumber(const string &line) {
 if (line[0] == '0') return true;
 return (atoi(line.c_str()));
}

vector<string> parseVectorElements(string variable){
    vector<string> ret;
    string line_delimiter = "[";
    string line;
    size_t pos = 0;
    while ((pos = variable.find(line_delimiter)) != std::string::npos) {
        line = variable.substr(0, pos);
        ret.push_back(line);
        variable.erase(0, pos + line_delimiter.length());
    }
    line = variable;
    line.erase(line.length()-1, 1);
    ret.push_back(line);
    return ret;
}

void allocVarSpace(string id, string scope, int *scopeRegisterAmount){
    BucketList bucketElement = getBucketElement(id, scope);
    if(static_scope_register<USESTATICREGISTERAMOUNT){
        if(bucketElement->data_type==IntPointer){
            bucketElement->value_in_register = true;
            bucketElement->mem_pos = mem_pos;
            bucketElement->loc_register = "$s"+to_string(static_scope_register);
            assembly.push_back("MOV $sp "+bucketElement->loc_register);
            assembly.push_back("ADDI $sp $sp "+to_string(bucketElement->mem_loc));
            mem_pos = mem_pos + bucketElement->mem_loc;
        }
        else{
            bucketElement->value_in_register = true;
            bucketElement->loc_register = "$s"+to_string(static_scope_register);
            assembly.push_back("MOV $zero "+bucketElement->loc_register);
        }
        *scopeRegisterAmount = *scopeRegisterAmount + 1;
        static_scope_register++;
    }
    else{
        if(bucketElement->data_type==IntPointer){
            bucketElement->value_in_register = false;
            bucketElement->mem_pos = mem_pos;
            assembly.push_back("ADDI $sp $sp "+to_string(bucketElement->mem_loc));
            mem_pos = mem_pos + bucketElement->mem_loc;
        }
        else{
            bucketElement->value_in_register = false;
            bucketElement->mem_pos = mem_pos;
            bucketElement->mem_loc = 1;
            assembly.push_back("ADDI $sp $sp "+to_string(bucketElement->mem_loc));
            mem_pos++;
        }
    }
}

string allocTempSpace(string id, string scope, int *temp_use, bool *in_mem){
    if(temp_scope_register < USETEMPREGISTERAMOUNT){
        assembly.push_back("MOV $zero $t"+to_string(temp_scope_register));
        temp_scope_register++;
        return "$t"+to_string(temp_scope_register-1);
    }
    else{
        *in_mem = true;
        temp_mem_pos.push_back(mem_pos);
        *temp_use = *temp_use + 1;
        return "$t"+to_string(*temp_use-1);
    }
}

string getRegisterLikeWrite(string id, string scope, int *temp_use, bool *in_mem){
    BucketList bucketElement;
    if(id.compare(0, 2, "_t")==0){
        *in_mem = false;
        return allocTempSpace(id, scope, temp_use, in_mem);
    }
    else{
        vector<string> vector_acess = parseVectorElements(id);
        if(vector_acess.size()==2){
            *in_mem = true;
            *temp_use = *temp_use + 1;
            return "$t"+to_string(*temp_use-1);
        }
        else{
            bucketElement = getBucketElement(id, scope);
            if(bucketElement==NULL){
            }
            else if(bucketElement->value_in_register){
                *in_mem = false;
                return bucketElement->loc_register;
            }
            else{
                *in_mem = true;
                *temp_use = *temp_use + 1;
                return "$t"+to_string(*temp_use);
            }
        }
    }
}

string getRegisterLikeRead(string id, string scope, int *temp_use){
    if(isNumber(id)){
        assembly.push_back("LI $t"+to_string(*temp_use)+" "+id);
        *temp_use = *temp_use + 1;
        return "$t"+to_string(*temp_use-1);
    }
    else{
        if(id.compare(0,2, "_t")==0){
            int indexString = stoi(id.substr(2, id.length()-2));
            if(indexString<USETEMPREGISTERAMOUNT){
                return "$t"+to_string(indexString);
            }
            else{
                assembly.push_back("LI $t"+to_string(*temp_use)+" "+to_string(temp_mem_pos[indexString-USETEMPREGISTERAMOUNT]));
                assembly.push_back("ADD $t"+to_string(*temp_use)+" $t"+to_string(*temp_use)+" $gp");
                assembly.push_back("LOAD $t"+to_string(*temp_use)+" $t"+to_string(*temp_use));
                *temp_use = *temp_use + 1;
                return "$t"+to_string(*temp_use-1);
            }
        }
        else{
            BucketList bucketElement;
            vector<string> vector_acess = parseVectorElements(id);
            if(vector_acess.size()==2){
                bucketElement = getBucketElement(vector_acess[0], scope);
            }
            else{
                bucketElement = getBucketElement(id, scope);
            }
            if(bucketElement==NULL){
                cout << "Aconteceu algo mt errado" << endl; exit(-1);
            }
            if(vector_acess.size()==2){
                string index = getRegisterLikeRead(vector_acess[1], scope, temp_use);
                *temp_use = *temp_use + 1;
                assembly.push_back("ADD $t"+to_string(*temp_use)+" "+index+" "+getRegisterLikeRead(vector_acess[0], scope, temp_use));
                assembly.push_back("LOAD $t"+to_string(*temp_use)+" $t"+to_string(*temp_use)+" 0");
                return "$t"+to_string(*temp_use);
            }
            else{
                if(bucketElement->value_in_register){
                    return bucketElement->loc_register;
                }
                else{
                    *temp_use = *temp_use + 1;
                    assembly.push_back("LOAD $sp $t"+to_string(*temp_use)+" "+to_string(bucketElement->mem_pos));
                    return "$t"+to_string(*temp_use);
                }
            }
        }
    }
}

void storeStackElement(string id, string scope, string loc_register){
    vector<string> vector_acess = parseVectorElements(id);
    if(vector_acess.size()==2){
        BucketList bucketElement = getBucketElement(vector_acess[0], scope);
        assembly.push_back("LI "+to_string(USETEMPREGISTERAMOUNT)+" "+to_string(bucketElement->mem_pos));
        assembly.push_back("ADD $t"+to_string(USETEMPREGISTERAMOUNT)+" $gp");
        assembly.push_back("LI $t"+to_string(USETEMPREGISTERAMOUNT+1)+" "+vector_acess[1]);
        assembly.push_back("ADD $t"+to_string(USETEMPREGISTERAMOUNT)+" $t"+to_string(USETEMPREGISTERAMOUNT+1));
        assembly.push_back("STORE $t"+to_string(USETEMPREGISTERAMOUNT)+" "+loc_register+" 0");
    }
    else{
        BucketList bucketElement = getBucketElement(id, scope);
        assembly.push_back("STORE $sp "+loc_register+" "+to_string(bucketElement->mem_pos)+" 0");
    }
}

void lineToAssembly(vector<string> params){
    if(params[0].compare("goto")==0){
        assembly.push_back("B ."+params[1]);
    }
    else if(params[0].compare("fun")==0){
        temp_scope_register = 0;
        static_scope_register = globalVarScope;
        assembly.push_back("FUN");
        previous_mem_pos = mem_pos;
        mem_pos = 0;
        params_amount = 0;
        assembly.push_back("."+params[1]);
        labels.push_back("."+params[1]);
        assembly.push_back("STORE $sp $ra 0");
        assembly.push_back("ADDI $sp $sp 1");
        mem_pos++;
        argument_pos = -3;//Foi empilhados antes, nesta ordem, $gp e $ra
        scope = params[1];
        BucketList bucketElement = getBucketElement(params[1], " ");
        int localRegisterAmount = 0;
        for(int i=0;i<bucketElement->variables.size();i++){
            allocVarSpace(bucketElement->variables[i], scope, &localRegisterAmount);
        }
    }
    else if(params[0].compare("end_fun")==0){
        assembly.push_back("ENDFUN");
        assembly.push_back("ADDI $sp $sp -"+to_string(mem_pos));
        assembly.push_back("LOAD $t"+to_string(USETEMPREGISTERAMOUNT)+" $sp 0");
        assembly.push_back("BR $t"+to_string(USETEMPREGISTERAMOUNT));
        mem_pos = previous_mem_pos;
    }
    else if(params[0].compare("pop_param")==0){
        assembly.push_back("POP_PARAM");
        int temp_use = USETEMPREGISTERAMOUNT;
        bool in_mem;
        int localRegisterAmount = 0;
        allocVarSpace(params[1], scope, &localRegisterAmount);
        string rd = getRegisterLikeWrite(params[1], scope, &temp_use, &in_mem);
        assembly.push_back("LOAD $sp "+rd+" "+to_string(argument_pos));
        if(in_mem){
            storeStackElement(params[1], scope, rd);
        }
        argument_pos--;
    }
    else if(params[0].compare("param")==0){
        assembly.push_back("PARAM");
        int temp_use = USETEMPREGISTERAMOUNT;
        string rs = getRegisterLikeRead(params[1], scope, &temp_use);
        assembly.push_back("STORE $sp "+rs+" 0");
        assembly.push_back("ADDI $sp $sp 1");
        params_amount++;
    }
    else if(params[0].compare("label")==0){
        assembly.push_back("LABEL");
        assembly.push_back("."+params[1]);
        labels.push_back("."+params[1]);
    }
    else if(params[0].compare("asn_ret")==0){
        assembly.push_back("ASN RET");
        int temp_use = USETEMPREGISTERAMOUNT;
        string rs = getRegisterLikeRead(params[1], scope, &temp_use);
        assembly.push_back("MOV "+rs+" $v0");
    }
    else if(params[0].compare("system_in")==0){
        assembly.push_back("SYSTEM IN");
        int temp_use = USETEMPREGISTERAMOUNT;
        bool in_mem;
        string rd = getRegisterLikeWrite(params[1], scope, &temp_use, &in_mem);
        assembly.push_back("IN "+rd+" 0");
        if(in_mem){
            storeStackElement(params[1], scope, rd);
        }
    }
    else if(params[0].compare("system_out")==0){
        assembly.push_back("SYSTEM OUT");
        int temp_use = USETEMPREGISTERAMOUNT;
        string rs = getRegisterLikeRead(params[1], scope, &temp_use);
        assembly.push_back("OUT "+rs+" 0");
        assembly.push_back("HALT");
    }
    else if(params[0].compare("catch_return")==0){
        assembly.push_back("CATCH RETURN");
        int temp_use = USETEMPREGISTERAMOUNT;
        bool in_mem;
        string rd = getRegisterLikeWrite(params[1], scope, &temp_use, &in_mem);
        assembly.push_back("MOV $v0 "+rd);
        if(in_mem){
            storeStackElement(params[1], scope, rd);
        }
    }
    else if(params[0].compare("asn")==0){
        assembly.push_back("ASN");
        int temp_use = USETEMPREGISTERAMOUNT;
        bool in_mem;
        string rs = getRegisterLikeRead(params[2], scope, &temp_use);
        string rd = getRegisterLikeWrite(params[1], scope, &temp_use, &in_mem);
        assembly.push_back("MOV "+rs+" "+rd);
        if(in_mem){
            storeStackElement(params[1], scope, rd);
        }
    }
    else if(params[0].compare("-")==0){
        assembly.push_back("SUB");
        int temp_use = USETEMPREGISTERAMOUNT;
        bool in_mem;
        string rd = getRegisterLikeWrite(params[1], scope, &temp_use, &in_mem);
        string rs = getRegisterLikeRead(params[2], scope, &temp_use);
        string rt = getRegisterLikeRead(params[3], scope, &temp_use);
        assembly.push_back("SUB "+rs+" "+rt+" "+rd);
        if(in_mem){
            storeStackElement(params[1], scope, rd);
        }
    }
    else if(params[0].compare("+")==0){
        assembly.push_back("ADD");
        int temp_use = USETEMPREGISTERAMOUNT;
        bool in_mem;
        string rd = getRegisterLikeWrite(params[1], scope, &temp_use, &in_mem);
        string rs = getRegisterLikeRead(params[2], scope, &temp_use);
        string rt = getRegisterLikeRead(params[3], scope, &temp_use);
        assembly.push_back("ADD "+rs+" "+rt+" "+rd);
        if(in_mem){
            storeStackElement(params[1], scope, rd);
        }
    }
    else if(params[0].compare("/")==0){
        assembly.push_back("DIV");
        int temp_use = USETEMPREGISTERAMOUNT;
        bool in_mem;
        string rd = getRegisterLikeWrite(params[1], scope, &temp_use, &in_mem);
        string rs = getRegisterLikeRead(params[2], scope, &temp_use);
        string rt = getRegisterLikeRead(params[3], scope, &temp_use);
        assembly.push_back("DIV "+rs+" "+rt);
        assembly.push_back("MFLO "+rd);
        if(in_mem){
            storeStackElement(params[1], scope, rd);
        }
    }
    else if(params[0].compare("*")==0){
        assembly.push_back("MULT");
        int temp_use = USETEMPREGISTERAMOUNT;
        bool in_mem;
        string rd = getRegisterLikeWrite(params[1], scope, &temp_use, &in_mem);
        string rs = getRegisterLikeRead(params[2], scope, &temp_use);
        string rt = getRegisterLikeRead(params[3], scope, &temp_use);
        assembly.push_back("MULT "+rs+" "+rt);
        assembly.push_back("MFLO "+rd);
        if(in_mem){
            storeStackElement(params[1], scope, rd);
        }
    }
    /*else if(params[0].compare("jal")==0){
        for(int i=0;i<static_scope_register;i++){
            assembly.push_back("STORE $sp $s"+to_string(i)+" "+to_string(i));
        }
        assembly.push_back("ADDI $sp $sp "+to_string(static_scope_register));
        for(int i=0;i<temp_scope_register;i++){
            assembly.push_back("STORE $sp $t"+to_string(i)+" "+to_string(i));
        }
        assembly.push_back("ADDI $sp $sp "+to_string(temp_scope_register));
        assembly.push_back("STORE $sp $gp 0");
        assembly.push_back("ADDI $sp $sp 1");
        assembly.push_back("MOV $sp $gp");
        assembly.push_back("BL ."+params[1]);
        assembly.push_back("ADDI $sp $sp -1");
        assembly.push_back("LOAD $sp $gp 0");//Pegar de volta o valor de gp
        //end_fun tem que deixar a pilha como encontrou antes da funcao
        assembly.push_back("ADDI $sp $sp -"+to_string(static_scope_register));
        for(int i=0;i<static_scope_register;i++){
            assembly.push_back("LOAD $sp $s"+to_string(i)+ " "+to_string(i));
        }
        assembly.push_back("ADDI $sp $sp -"+to_string(params_amount));//Some da pilha os argumentos de chamada de funcao
        mem_pos = mem_pos - params_amount;
        assembly.push_back("MOV $sp $gp");
        params_amount = 0;
    }
    else if(params[0].compare("if_le")==0){

    }
    else if(params[0].compare("if_l")==0){

    }
    else if(params[0].compare("if_g")==0){

    }
    else if(params[0].compare("if_ge")==0){

    }
    else if(params[0].compare("if_e")==0){

    }
    else if(params[0].compare("if_ne")==0){

    }
    else{
        cout << "Quadrupla faltante " << params[0] << endl;
    }*/
}

string generateAssembly(string quad){
    string assemblyString = "";
    parseQuadCode(quad);
    cout << lines.size() << " linhas de código intermediário" << endl;
    assembly.push_back("MOV $zero $sp");
    assembly.push_back("MOV $zero $gp");
    BucketList bucketElement = getBucketElement("GLOBAL", " ");
    
    for(int i=0;i<bucketElement->variables.size();i++){
        allocVarSpace(bucketElement->variables[i], " ", &globalVarScope);
    }
    for(int i=0;i<lines.size();i++){
        vector<string> params = getLineParams(lines[i]);
        lineToAssembly(params);
    }
    cout << assembly.size() << " de linhas assembly" << endl;
    for(int i=0;i<assembly.size();i++){
        assemblyString = assemblyString + assembly[i] + "\n";
    }
    return assemblyString;
}