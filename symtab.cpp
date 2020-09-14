#include <string>
#include <iostream>
#include <bits/stdc++.h> 
#pragma once

#define SIZE 523
#define SHIFT 4

using namespace std;

enum TypeID{VarType, FuncType};
enum DataType{Int, IntPointer, Void};

typedef struct LineListRec{
  int lineno;
  LineListRec* next;
}*LineList;

typedef struct BucketListRec{
  string id;
  TypeID type_id;
  string scope;
  DataType data_type;
  LineList lines;
  BucketListRec* next;
  int mem_loc;
  int mem_pos;
  int var_amount;
  vector<string> variables; 

  bool value_in_register;
  bool is_parameter;
  string loc_register;
}*BucketList;

static BucketList hashTable[SIZE];

int hashTab(string key){
  int temp = 0;
  int i = 0;
  for(i = 0; i < key.length(); i++)
  {
    temp = ((temp << SHIFT) + (int)key[i]) % SIZE;
    ++i;
  }
  return temp;
}

void insertAllocMemScope(string scope, int mem_loc){
  int pos = hashTab(scope+" ");
  BucketList l = hashTable[pos];
  while(l!=NULL){
    if(scope.compare(l->id)==0) break;
    l = l->next;
  }
  l->mem_loc = l->mem_loc + mem_loc;
}

void insertVarInScope(string scope, string id){
  int pos = hashTab(scope+" ");
  BucketList l = hashTable[pos];
  while(l!=NULL){
    if(scope.compare(l->id)==0) break;
    l = l->next;
  }
  l->var_amount = l->var_amount + 1;
  l->variables.push_back(id);
}

bool insertSymTab(string id, TypeID type_id, string scope, DataType data_type, int lineno, int mem_loc, bool is_param){
  int pos = hashTab(id+scope);
  BucketList l = hashTable[pos];
  while(l!=NULL){
    if(id.compare(l->id) == 0 && scope.compare(l->scope) == 0 ) return false;
    l = l->next;
  }
  BucketList newElm = (BucketList) malloc(sizeof(struct BucketListRec));
  newElm->id = id;
  newElm->type_id = type_id;
  newElm->scope = scope;
  newElm->data_type = data_type;
  newElm->lines = (LineList) malloc(sizeof(struct LineListRec));
  newElm->lines->lineno = lineno;
  newElm->lines->next = NULL;
  newElm->next = hashTable[pos];
  newElm->mem_loc = mem_loc;
  newElm->mem_pos = -1;
  newElm->var_amount = 0;
  newElm->loc_register = " ";
  newElm->value_in_register = false;
  newElm->is_parameter = is_param;
  if(scope!=" "){
    insertAllocMemScope(scope, mem_loc);
  }
  else if(mem_loc!=0){
    insertAllocMemScope("GLOBAL", mem_loc);
  }
  hashTable[pos] = newElm;
  return true;
}

bool existID(string id, string scope){
  int pos = hashTab(id+scope);
  BucketList l = hashTable[pos];
  while(l!=NULL){
    if(id.compare(l->id) == 0 && scope.compare(l->scope) == 0 ) return true;
    l = l->next;
  }
  pos = hashTab(id+" ");
  l = hashTable[pos];
  while(l!=NULL){
    if(id.compare(l->id) == 0) return true;
    l = l->next;
  }
  return false;
}

bool existIdEveryScope(string id){
  for(int i = 0; i < SIZE; i++){
    BucketList l = hashTable[i];
    while(l!=NULL){
      if(l->id.compare(id) == 0) return true;
      l = l->next;
    }
  }
  return false;
}

string showLines(LineList l){
  string ret = "";
  while(l!=NULL){
    ret = ret + to_string(l->lineno);
    if(l->next!=NULL)
      ret = ret + " ";
    l = l ->next;
  }
  return ret;
}

string stringDataType(DataType t){
  if(t == Int) return "int";
  else if(t== IntPointer) return "int*";
  else return "void";
}

string stringTypeID(TypeID t){
  if(t == VarType) return "var";
  else return "func";
}

string vectorString(vector<string> vectorString){
  string ret = "";
  for(int i=0;i<vectorString.size(); i++){
    if(i==0)
      ret = vectorString[i];
    else
      ret = ret + ", " + vectorString[i];
  }
  return ret;
}

static string symbTabString = "";

string formatTabCeil(string info, int index){
  int sizes[9] = {13, 15, 13, 16, 14, 40, 15, 15, 15};
  if(index<9){
    while(info.size()<sizes[index]){
      info = info + " ";
    }
  }
  return info;
}

void showSymbTab(){
  symbTabString = "";
  bool hasMoreThanOne;
  for(int i = 0; i < SIZE; i++){
    BucketList l = hashTable[i];
    while(l!=NULL){
      symbTabString = symbTabString + formatTabCeil("ID: "+l->id, 0);
      symbTabString = symbTabString + formatTabCeil("SCOPE: "+l->scope, 1);
      if(l->is_parameter){
        symbTabString = symbTabString + formatTabCeil("PARAM: true", 2);
      }
      else{
        symbTabString = symbTabString + formatTabCeil("PARAM: false", 2);
      }
      symbTabString = symbTabString + formatTabCeil("DATA TYPE: "+stringDataType(l->data_type), 3);
      symbTabString = symbTabString + formatTabCeil("TYPE ID: "+stringTypeID(l->type_id), 4);
      symbTabString = symbTabString + formatTabCeil("LINES: "+showLines(l->lines), 5);
      symbTabString = symbTabString + formatTabCeil("MEMLOC: "+to_string(l->mem_loc), 6);
      symbTabString = symbTabString + formatTabCeil("MEMPOS: "+to_string(l->mem_pos), 7);
      symbTabString = symbTabString + formatTabCeil("VARAMOUNT: "+to_string(l->var_amount), 8);
      if(l->value_in_register && stringTypeID(l->type_id)=="var")
        symbTabString = symbTabString + formatTabCeil("REG: "+l->loc_register, 9);
      else if(stringTypeID(l->type_id)=="var")
        symbTabString = symbTabString + formatTabCeil("REG: mem["+l->loc_register+"]", 9);
      if(stringTypeID(l->type_id)=="func")
        symbTabString = symbTabString + formatTabCeil("VARS: ["+vectorString(l->variables)+"]", 9);
      symbTabString = symbTabString + "\n";
      l = l->next;
    }
  }
}

bool checkMain(){
  for(int i = 0; i < SIZE; i++){
    BucketList l = hashTable[i];
    while(l!=NULL){
      if(l->id.compare("main") == 0 && l->type_id == FuncType) return true;
      l = l->next;
    }
  }
  return false;
}

DataType getDataType(string id, string scope){
  int pos = hashTab(id+scope);
  BucketList l = hashTable[pos];
  while(l->id != id) l = l ->next;
  return l->data_type;
}

TypeID getTypeID(string id, string scope){
  int pos = hashTab(id+scope);
  BucketList l = hashTable[pos];
  while(l!=NULL){
    if(id.compare(l->id) == 0 && scope.compare(l->scope) == 0 ) return l->type_id;
    l = l->next;
  }
  pos = hashTab(id+" ");
  l = hashTable[pos];
  while(l!=NULL){
    if(id.compare(l->id) == 0) return l->type_id;
    l = l->next;
  }
}

bool variableIsArray(string id, string scope){
  int pos = hashTab(id+scope);
  BucketList l = hashTable[pos];
  while(l!=NULL){
    if(id.compare(l->id) == 0 && scope.compare(l->scope) == 0 ){
      if(l->data_type==IntPointer)
        return true;
      else
        return false;
    }
    l = l->next;
  }
  pos = hashTab(id+" ");
  l = hashTable[pos];
  while(l!=NULL){
    if(id.compare(l->id) == 0){
      if(l->data_type==IntPointer)
        return true;
      else 
        return false;
    }
    l = l->next;
  }
  return false;
}

bool checkVoid(treeNode *tree){
  if(tree->nodeKind == CallK){
    if(getDataType(tree->name," ") == Void) return true;
  }
  return false;
}

bool checkAtr(treeNode *tree){
  if(tree->nodeKind == OpK){
    if(tree->name.compare("=") == 0) return true;
  }
  return false;
}

void insertLineID(string id, string scope, int lineno){
  int pos = hashTab(id+scope);
  BucketList l = hashTable[pos];
  while(l!=NULL){
    if(id.compare(l->id) == 0 && scope.compare(l->scope) == 0 ) break;
    l = l->next;
  }
  if(l!=NULL){
    LineList newLine = (LineList) malloc(sizeof(struct LineListRec));
    newLine->lineno = lineno;
    newLine->next = l->lines;
    l->lines = newLine;
  }
  pos = hashTab(id+" ");
  l = hashTable[pos];
  while(l!=NULL){
    if(id.compare(l->id) == 0 && scope.compare(l->scope) == 0 ) break;
    l = l->next;
  }
  if(l!=NULL){
    LineList newLine = (LineList) malloc(sizeof(struct LineListRec));
    newLine->lineno = lineno;
    newLine->next = l->lines;
    l->lines = newLine;
  }
}

void insertLineIDGlobal(string id, int lineno){
  int pos = hashTab(id+" ");
  BucketList l = hashTable[pos];
  while(l!=NULL){
    if(id.compare(l->id) == 0) break;
    l = l->next;
  }
  if(l!=NULL){
    LineList newLine = (LineList) malloc(sizeof(struct LineListRec));
    newLine->lineno = lineno;
    newLine->next = l->lines;
    l->lines = newLine;
  }
}

string getDataTypeElement(string id, string scope){
  int pos = hashTab(id+scope);
  BucketList l = hashTable[pos];
  while(l!=NULL){
    if(id.compare(l->id) == 0 && scope.compare(l->scope) == 0 ){
      break;
    }
    l = l->next;
  }
  return stringTypeID(l->type_id);
}

BucketList getBucketElement(string id, string scope){
  int pos = hashTab(id+scope);
  BucketList l = hashTable[pos];
  while(l!=NULL){
    if(id.compare(l->id) == 0 && scope.compare(l->scope) == 0 ){
      return l;
    }
    l = l->next;
  }
  pos = hashTab(id+" ");
  l = hashTable[pos];
  string globalScope = " ";
  while(l!=NULL){
    if(id.compare(l->id) == 0 && globalScope.compare(l->scope)==0 ){
      return l;
    }
    l = l->next;
  }
  cout << "elemento não encontrado" << " "+  id + " " << scope << endl;
  return l;
}