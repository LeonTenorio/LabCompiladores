#include <string>
#include <iostream>
#include <bits/stdc++.h> 
#include "symtab.cpp"

#define TEMPREGISTERSAMOUNT 12
#define USETEMPREGISTERAMOUNT 8
#define USESTATICREGISTERAMOUNT 10

#define STACKSIZE 62209 //Tamanho da memoria RAM utilizada

//getRegisterLikeRead apenas usar um unico temporario - OK
//getRegisterLikeWrite apenas usar um unico temporario - OK
//storeStackElement usar mais um temporario para as operacoes - OK
//Tudo isso me resulta em um uso maximo de 4 temporarios para load e store

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
static map<string,int> labels_lines;

static string scope = " ";

static int globalVarScope = 0;

static int static_scope_register = 0;
static int temp_scope_register = 0;
vector<int> temp_mem_pos;//Use for all temporary index bigger than USETEMPREGISTERAMOUNT-1

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
    if(static_scope_register>=USESTATICREGISTERAMOUNT || (bucketElement->data_type==IntPointer && bucketElement->is_parameter==false)){
        //Alocacao em memoria principal
        bucketElement->value_in_register = false;
        bucketElement->mem_pos = bucketElement->mem_pos;
        if(bucketElement->mem_loc==0){
            bucketElement->mem_loc = 1;
            bucketElement->mem_pos = mem_pos;
            mem_pos++;
            assembly.push_back("ADDI $sp $sp 1");
        }
        else{
            bucketElement->mem_pos = mem_pos;
            mem_pos = mem_pos + bucketElement->mem_loc;
            assembly.push_back("ADDI $sp $sp "+to_string(bucketElement->mem_loc));
        }
    }
    else{
        //Alocacao em registradores
        bucketElement->value_in_register = true;
        bucketElement->loc_register = "$s"+to_string(static_scope_register);
        *scopeRegisterAmount = *scopeRegisterAmount + 1;
        static_scope_register++;
    }
}

string getTempRegister(string id, string scope, int *temp_use, bool *in_mem){
    if(temp_scope_register < USETEMPREGISTERAMOUNT){
        temp_scope_register++;
        return "$t"+to_string(temp_scope_register-1);
    }
    else{
        *in_mem = true;
        temp_mem_pos.push_back(mem_pos);
        *temp_use = *temp_use + 1;
        mem_pos++;
        assembly.push_back("ADDI $sp $sp 1");
        return "$t"+to_string(*temp_use-1);
    }
}

string getRegisterLikeWrite(string id, string scope, int *temp_use, bool *in_mem){
    BucketList bucketElement;
    if(id.compare(0, 2, "_t")==0){
        *in_mem = false;
        return getTempRegister(id, scope, temp_use, in_mem);
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
                cout << "Erro, simbolo nao esta na tabela" << endl;
                exit(-1);
            }
            else if(bucketElement->value_in_register){
                *in_mem = false;
                return bucketElement->loc_register;
            }
            else{
                *in_mem = true;
                *temp_use = *temp_use + 1;
                return "$t"+to_string(*temp_use-1);
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
                assembly.push_back("LOAD $t"+to_string(*temp_use)+" $t"+to_string(*temp_use)+" 0");
                *temp_use = *temp_use + 1;
                return "$t"+to_string(*temp_use-1);
            }
        }
        else{
            BucketList bucketElement;
            vector<string> vector_acess = parseVectorElements(id);
            if(vector_acess.size()==2){
                bucketElement = getBucketElement(vector_acess[0], scope);
                int previousTempUse = *temp_use;
                string desloc = getRegisterLikeRead(vector_acess[1], scope, temp_use);
                if((bucketElement->is_parameter)&&(bucketElement->value_in_register)){
                    assembly.push_back("ADD $t"+to_string(*temp_use)+" "+bucketElement->loc_register+" "+desloc);
                    assembly.push_back("LOAD $t"+to_string(*temp_use)+" $t"+to_string(*temp_use)+" 0");
                    *temp_use = *temp_use + 1;
                    return "$t"+to_string(*temp_use-1);
                }
                else if(bucketElement->is_parameter){
                    assembly.push_back("LI $t"+to_string(*temp_use)+" "+to_string(bucketElement->mem_pos));
                    assembly.push_back("ADD $t"+to_string(*temp_use)+" $gp $t"+to_string(*temp_use));
                    assembly.push_back("LOAD $t"+to_string(*temp_use)+" $t"+to_string(*temp_use)+" 0");
                    assembly.push_back("ADD $t"+to_string(*temp_use)+" $t"+to_string(*temp_use)+" "+desloc);
                    assembly.push_back("LOAD $t"+to_string(*temp_use)+" $t"+to_string(*temp_use)+" 0");
                    *temp_use = *temp_use + 1;
                    return "$t"+to_string(*temp_use-1);
                }
                else{
                    assembly.push_back("LI $t"+to_string(*temp_use)+" "+to_string(bucketElement->mem_pos));
                    if(bucketElement->scope.compare(" ")!=0 && bucketElement->scope.compare("GLOBAL")!=0){
                        assembly.push_back("ADD $t"+to_string(*temp_use)+" $t"+to_string(*temp_use)+" $gp");
                    }
                    assembly.push_back("ADD $t"+to_string(*temp_use)+" $t"+to_string(*temp_use)+" "+desloc);
                    assembly.push_back("LOAD $t"+to_string(*temp_use)+" $t"+to_string(*temp_use)+" 0");
                    *temp_use = *temp_use + 1;
                    return "$t"+to_string(*temp_use-1);
                }
            }
            else{
                bucketElement = getBucketElement(id, scope);
                if(bucketElement->value_in_register){
                    return bucketElement->loc_register;
                }
                else{
                    assembly.push_back("LI $t"+to_string(*temp_use)+" "+to_string(bucketElement->mem_pos));
                    if(bucketElement->scope.compare(" ")!=0 && bucketElement->scope.compare("GLOBAL")!=0){
                        assembly.push_back("ADD $t"+to_string(*temp_use)+" $t"+to_string(*temp_use)+" $gp");
                    }
                    if((bucketElement->data_type!=IntPointer) || (bucketElement->data_type==IntPointer && bucketElement->is_parameter)){
                        assembly.push_back("LOAD $t"+to_string(*temp_use)+" $t"+to_string(*temp_use)+" 0");
                    }
                    *temp_use = *temp_use + 1;
                    return "$t"+to_string(*temp_use-1);
                }
            }
        }
    }
}

void writeDebugAssembly(string writeString, bool debug){
    if(debug){
        assembly.push_back("*"+writeString);
    }
}

void debugMessages(string message, bool debug){
    if(debug){
        cout << message << endl;
    }
}

void storeStackElement(string id, string scope, string loc_register, int *temp_use, bool debug){
    writeDebugAssembly("STORE STACK ELEMENT", debug);
    debugMessages("Store stack element " + id, debug);
    vector<string> vector_acess = parseVectorElements(id);
    BucketList bucketElement;
    if(id.compare(0,2, "_t")==0){
        int indexString = stoi(id.substr(2, id.length()-2));
            if(indexString<USETEMPREGISTERAMOUNT){
                cout << "aconteceu algo de errado, temporario nao era pra estar na memoria principal" << endl; exit(-1);
            }
            else{
                assembly.push_back("LI $t"+to_string(*temp_use)+" "+to_string(temp_mem_pos[indexString-USETEMPREGISTERAMOUNT]));
                assembly.push_back("ADD $t"+to_string(*temp_use)+" $t"+to_string(*temp_use)+" $gp");
                assembly.push_back("STORE $t"+to_string(*temp_use)+" "+loc_register+" 0");
                *temp_use = *temp_use + 1;
            }
    }
    else{
        if(vector_acess.size()==2){
            bucketElement = getBucketElement(vector_acess[0], scope);
            int previousTempUse = *temp_use;
            string base = getRegisterLikeRead(vector_acess[0], scope, temp_use);
            if(base.compare("$t"+to_string(previousTempUse))==0){
                *temp_use = previousTempUse + 1;
            }
            else{
                *temp_use = previousTempUse;
            }
            previousTempUse = *temp_use;
            string desloc = getRegisterLikeRead(vector_acess[1], scope, temp_use);
            *temp_use = previousTempUse;
            assembly.push_back("ADD $t"+to_string(*temp_use)+" "+base+" "+desloc);
            assembly.push_back("STORE $t"+to_string(*temp_use)+" "+loc_register+" 0");
        }
        else{
            bucketElement = getBucketElement(id, scope);
            assembly.push_back("LI $t"+to_string(*temp_use)+" "+to_string(bucketElement->mem_pos));
            if(bucketElement->scope.compare(" ")!=0 && bucketElement->scope.compare("GLOBAL")!=0){
                assembly.push_back("ADD $t"+to_string(*temp_use)+" $t"+to_string(*temp_use)+" $gp");
            }
            assembly.push_back("STORE $t"+to_string(*temp_use)+" "+loc_register+" 0");
        }
    }
}

static string func_name = "";

void lineToAssembly(vector<string> params, bool debug){
    if(params[0].compare("goto")==0){
        assembly.push_back("B ."+params[1]);
    }
    else if(params[0].compare("fun")==0){
        temp_mem_pos.clear();
        func_name = params[1];
        temp_scope_register = 0;
        static_scope_register = globalVarScope;
        writeDebugAssembly("FUN", debug);
        mem_pos = 0;
        labels_lines["."+params[1]] = assembly.size() - labels.size();
        assembly.push_back("."+params[1]);
        labels.push_back("."+params[1]);
        if(params[1].compare("main")!=0){
            assembly.push_back("STORE $sp $sp 0");
            assembly.push_back("STORE $sp $ra 1");
            assembly.push_back("ADDI $sp $sp 2");
        }
        assembly.push_back("MOV $sp $gp");
        scope = params[1];
        BucketList bucketElement = getBucketElement(params[1], " ");
        int localRegisterAmount = 0;
        for(int i=0;i<bucketElement->variables.size();i++){
            allocVarSpace(bucketElement->variables[i], scope, &localRegisterAmount);
        }
    }
    else if(params[0].compare("end_fun")==0){
        writeDebugAssembly("ENDFUN", debug);
        if(func_name.compare("main")!=0){
            assembly.push_back("B .ENDFUN");
        }
    }
    else if(params[0].compare("pop_param")==0){
        writeDebugAssembly("POP_PARAM", debug);
        int temp_use = USETEMPREGISTERAMOUNT;
        bool in_mem;
        int localRegisterAmount = 0;
        allocVarSpace(params[1], scope, &localRegisterAmount);
        string rd = getRegisterLikeWrite(params[1], scope, &temp_use, &in_mem);
        assembly.push_back("ADDI $sa $sa 1");
        assembly.push_back("LOAD "+rd+" $sa 0");
        if(in_mem){
            storeStackElement(params[1], scope, rd, &temp_use, debug);
        }
    }
    else if(params[0].compare("param")==0){
        writeDebugAssembly("PARAM", debug);
        int temp_use = USETEMPREGISTERAMOUNT;
        string rs = getRegisterLikeRead(params[1], scope, &temp_use);
        assembly.push_back("STORE $sa "+rs+" 0");
        assembly.push_back("ADDI $sa $sa -1");
    }
    else if(params[0].compare("label")==0){
        writeDebugAssembly("LABEL", debug);
        labels_lines["."+params[1]] = assembly.size() - labels.size();
        assembly.push_back("."+params[1]);
        labels.push_back("."+params[1]);
    }
    else if(params[0].compare("asn_ret")==0){
        writeDebugAssembly("ASN RET", debug);
        cout << "AQUI " << params.size() << endl;
        if(params.size()>1 && params[1].length()>0){
            int temp_use = USETEMPREGISTERAMOUNT;
            string rs = getRegisterLikeRead(params[1], scope, &temp_use);
            assembly.push_back("MOV "+rs+" $v0");
        }
        writeDebugAssembly("ENDFUN RETURN", debug);
        assembly.push_back("B .ENDFUN");
    }
    else if(params[0].compare("system_in")==0){
        writeDebugAssembly("SYSTEM IN", debug);
        int temp_use = USETEMPREGISTERAMOUNT;
        bool in_mem;
        string rd = getRegisterLikeWrite(params[1], scope, &temp_use, &in_mem);
        assembly.push_back("IN "+rd+" 0");
        if(in_mem){
            storeStackElement(params[1], scope, rd, &temp_use, debug);
        }
    }
    else if(params[0].compare("system_out")==0){
        writeDebugAssembly("SYSTEM OUT", debug);
        int temp_use = USETEMPREGISTERAMOUNT;
        string rs = getRegisterLikeRead(params[1], scope, &temp_use);
        assembly.push_back("OUT "+rs+" 0");
        //assembly.push_back("HALT");
    }
    else if(params[0].compare("catch_return")==0){
        writeDebugAssembly("CATCH RETURN", debug);
        int temp_use = USETEMPREGISTERAMOUNT;
        bool in_mem;
        string rd = getRegisterLikeWrite(params[1], scope, &temp_use, &in_mem);
        assembly.push_back("MOV $v0 "+rd);
        if(in_mem){
            storeStackElement(params[1], scope, rd, &temp_use, debug);
        }
    }
    else if(params[0].compare("asn")==0){
        writeDebugAssembly("ASN "+params[1]+" "+params[2], debug);
        int temp_use = USETEMPREGISTERAMOUNT;
        bool in_mem;
        string rs = getRegisterLikeRead(params[2], scope, &temp_use);
        string rd = getRegisterLikeWrite(params[1], scope, &temp_use, &in_mem);
        debugMessages("ASN "+params[1]+" "+rd+" "+rs+" "+to_string(assembly.size()-1), debug);
        assembly.push_back("MOV "+rs+" "+rd);
        if(in_mem){
            storeStackElement(params[1], scope, rd, &temp_use, debug);
        }
    }
    else if(params[0].compare("-")==0){
        writeDebugAssembly("SUB", debug);
        int temp_use = USETEMPREGISTERAMOUNT;
        bool in_mem;
        string rd = getRegisterLikeWrite(params[1], scope, &temp_use, &in_mem);
        string rs = getRegisterLikeRead(params[2], scope, &temp_use);
        string rt = getRegisterLikeRead(params[3], scope, &temp_use);
        assembly.push_back("SUB "+rd+" "+rs+" "+rt);
        if(in_mem){
            storeStackElement(params[1], scope, rd, &temp_use, debug);
        }
    }
    else if(params[0].compare("+")==0){
        writeDebugAssembly("ADD", debug);
        int temp_use = USETEMPREGISTERAMOUNT;
        bool in_mem;
        string rd = getRegisterLikeWrite(params[1], scope, &temp_use, &in_mem);
        string rs = getRegisterLikeRead(params[2], scope, &temp_use);
        string rt = getRegisterLikeRead(params[3], scope, &temp_use);
        assembly.push_back("ADD "+rd+" "+rs+" "+rt);
        if(in_mem){
            storeStackElement(params[1], scope, rd, &temp_use, debug);
        }
    }
    else if(params[0].compare("/")==0){
        writeDebugAssembly("DIV", debug);
        int temp_use = USETEMPREGISTERAMOUNT;
        bool in_mem;
        string rd = getRegisterLikeWrite(params[1], scope, &temp_use, &in_mem);
        string rs = getRegisterLikeRead(params[2], scope, &temp_use);
        string rt = getRegisterLikeRead(params[3], scope, &temp_use);
        assembly.push_back("DIV "+rs+" "+rt);
        assembly.push_back("MFLO "+rd);
        if(in_mem){
            storeStackElement(params[1], scope, rd, &temp_use, debug);
        }
    }
    else if(params[0].compare("*")==0){
        writeDebugAssembly("MULT", debug);
        int temp_use = USETEMPREGISTERAMOUNT;
        bool in_mem;
        string rd = getRegisterLikeWrite(params[1], scope, &temp_use, &in_mem);
        string rs = getRegisterLikeRead(params[2], scope, &temp_use);
        string rt = getRegisterLikeRead(params[3], scope, &temp_use);
        assembly.push_back("MULT "+rs+" "+rt);
        assembly.push_back("MFLO "+rd);
        if(in_mem){
            storeStackElement(params[1], scope, rd, &temp_use, debug);
        }
    }
    else if(params[0].compare("if_le")==0){
        writeDebugAssembly("BLE", debug);
        int temp_use = USETEMPREGISTERAMOUNT;
        string rs = getRegisterLikeRead(params[1], scope, &temp_use);
        string rt = getRegisterLikeRead(params[2], scope, &temp_use);
        assembly.push_back("BLE "+rs+" "+rt+" ."+params[3]);
    }
    else if(params[0].compare("if_l")==0){
        writeDebugAssembly("BLT", debug);
        int temp_use = USETEMPREGISTERAMOUNT;
        string rs = getRegisterLikeRead(params[1], scope, &temp_use);
        string rt = getRegisterLikeRead(params[2], scope, &temp_use);
        assembly.push_back("BLT "+rs+" "+rt+" ."+params[3]);
    }
    else if(params[0].compare("if_g")==0){
        writeDebugAssembly("BGT", debug);
        int temp_use = USETEMPREGISTERAMOUNT;
        string rs = getRegisterLikeRead(params[1], scope, &temp_use);
        string rt = getRegisterLikeRead(params[2], scope, &temp_use);
        assembly.push_back("BGT "+rs+" "+rt+" ."+params[3]);
    }
    else if(params[0].compare("if_ge")==0){
        writeDebugAssembly("BGE", debug);
        int temp_use = USETEMPREGISTERAMOUNT;
        string rs = getRegisterLikeRead(params[1], scope, &temp_use);
        string rt = getRegisterLikeRead(params[2], scope, &temp_use);
        assembly.push_back("BGE "+rs+" "+rt+" ."+params[3]);
    }
    else if(params[0].compare("if_e")==0){
        writeDebugAssembly("BEQ", debug);
        int temp_use = USETEMPREGISTERAMOUNT;
        string rs = getRegisterLikeRead(params[1], scope, &temp_use);
        string rt = getRegisterLikeRead(params[2], scope, &temp_use);
        assembly.push_back("BEQ "+rs+" "+rt+" ."+params[3]);
    }
    else if(params[0].compare("if_ne")==0){
        writeDebugAssembly("BNE", debug);
        int temp_use = USETEMPREGISTERAMOUNT;
        string rs = getRegisterLikeRead(params[1], scope, &temp_use);
        string rt = getRegisterLikeRead(params[2], scope, &temp_use);
        assembly.push_back("BNE "+rs+" "+rt+" ."+params[3]);
    }
    else if(params[0].compare("jal")==0){
        writeDebugAssembly("JAL", debug);
        if(static_scope_register>globalVarScope){
            for(int i=globalVarScope;i<static_scope_register;i++){
                assembly.push_back("STORE $sp $s"+to_string(i)+" "+to_string(i-globalVarScope));
            }
            assembly.push_back("ADDI $sp $sp "+to_string(static_scope_register-globalVarScope));
        }
        if(temp_scope_register>0){
            for(int i=0;i<temp_scope_register;i++){
                assembly.push_back("STORE $sp $t"+to_string(i)+" "+to_string(i));
            }
            assembly.push_back("ADDI $sp $sp "+to_string(temp_scope_register));
        }
        assembly.push_back("STORE $sp $gp 0");
        assembly.push_back("ADDI $sp $sp 1");
        assembly.push_back("BL ."+params[1]);
        assembly.push_back("ADDI $sp $sp -1");
        assembly.push_back("LOAD $gp $sp 0");
        if(temp_scope_register>0){
            assembly.push_back("ADDI $sp $sp -"+to_string(temp_scope_register));
            for(int i=0;i<temp_scope_register;i++){
                assembly.push_back("LOAD $t"+to_string(i)+" $sp "+to_string(i));
            }
        }
        if(static_scope_register>globalVarScope){
            assembly.push_back("ADDI $sp $sp -"+to_string(static_scope_register-globalVarScope));
            for(int i=globalVarScope;i<static_scope_register;i++){
                assembly.push_back("LOAD $s"+to_string(i)+" $sp "+to_string(i-globalVarScope));
            }
        }
    }
    else if(params[0].compare("end")==0){
        assembly.push_back("HALT");
    }
    else{
        if(params[1].compare("main")!=0)
            cout << "Quadrupla faltante " << params[0] << endl;
    }
}

string generateAssembly(string quad, bool debug){
    string assemblyString = "";
    parseQuadCode(quad);
    cout << lines.size() << " linhas de código intermediário" << endl << endl;
    assembly.push_back("LI $zero 0");
    assembly.push_back("MOV $zero $sp");
    assembly.push_back("MOV $zero $gp");
    assembly.push_back("LI $sa "+to_string(STACKSIZE-1));
    BucketList bucketElement = getBucketElement("GLOBAL", " ");
    
    for(int i=0;i<bucketElement->variables.size();i++){
        allocVarSpace(bucketElement->variables[i], " ", &globalVarScope);
    }
    for(int i=0;i<lines.size();i++){
        vector<string> params = getLineParams(lines[i]);
        lineToAssembly(params, debug);
        if(params[0].compare("goto")==0 && params[1].compare("main")==0){
            labels_lines[".ENDFUN"] = assembly.size() - labels.size();
            assembly.push_back(".ENDFUN");
            labels.push_back(".ENDFUN");
            assembly.push_back("LOAD $sp $gp -2");
            assembly.push_back("LOAD $t8 $gp -1");
            assembly.push_back("BR $t8");
        }
    }
    cout << assembly.size() << " de linhas assembly" << endl;
    for(int i=0;i<assembly.size();i++){
        assemblyString = assemblyString + assembly[i] + "\n";
    }
    return assemblyString;
}