%{

#include <iostream>
#include <fstream>
#include <string.h>
#include <stack>
#include "arvore.cpp"
#include "symtab.cpp"
#include "utils.c"
#define YYSTYPE treeNode *
#define SIZE 523

using namespace std;

static treeNode *savedTree;
int savedInt;
extern char currentToken[];
extern BucketList hashTable[SIZE];

extern "C"
{
  int yylex(void);
  void abrirArq();
}
extern char* yytext;
extern int yylineno;
int nLine;
string currentFunction = " ";

void yyerror(char *);

stack<string> savedIDs;

%}

%start program
%token INT VOID IF ELSE WHILE RETURN
%token NUM ID
%left ADD SUB
%left MUL DIV
%token PRTO PRTC SBTO SBTC SCL COM KEYO KEYC ATR LT LTE GT GTE EQ NEQ
%token ERR

%%

program:
  declaration_list
    {
      savedTree = (treeNode *) malloc(sizeof(treeNode));
      savedTree->nodeKind = TypeK;
      savedTree->name = "int";
      savedTree->child[0] = (treeNode *) malloc(sizeof(treeNode));
      savedTree->child[1] = NULL;
      savedTree->child[2] = NULL;
      savedTree->child[0]->nodeKind = FnK;
      savedTree->child[0]->name = "input";
      for(int i = 0; i < 3; i ++)
        savedTree->child[0]->child[i] = NULL;
      savedTree->sibling = (treeNode *) malloc(sizeof(treeNode));
      savedTree->sibling->nodeKind = TypeK;
      savedTree->sibling->name = "output";
      savedTree->sibling->child[0] = (treeNode *) malloc(sizeof(treeNode));
      savedTree->sibling->child[1] = NULL;
      savedTree->sibling->child[2] = NULL;
      savedTree->sibling->child[0]->nodeKind = FnK;
      savedTree->sibling->child[0]->name = "output";
      for(int i = 0; i < 3; i ++)
        savedTree->sibling->child[0]->child[i] = NULL;
      savedTree->sibling->sibling = $1;
    }
  ;

declaration_list:
  declaration_list declaration
    {
      YYSTYPE t = $1;
      if(t!=NULL){//This preserves order of declarations
        while(t->sibling != NULL) t = t->sibling;
        t->sibling = $2;
        $$ = $1;
      }else{
        $$ = $2;
      }
    }
  |declaration
    {$$ = $1;}
  ;

declaration:
  var_declaration
    {$$ = $1;}
  |fun_declaration
    {$$  = $1;}
  ;

var_declaration:
  type_specifier erro ID {savedIDs.push(copyString(currentToken));} SCL
    {
      $$ = $1;
      $$->child[0] = newNode(IdK);
      $$->child[0]->name = savedIDs.top();
      if(currentFunction != " "){
        if(existID(savedIDs.top()," ")) {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 1"; exit(-1);}
        if(savedIDs.top().compare(currentFunction) == 0) {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 2"; exit(-1);}
        if($$->name=="int"){
          if(!insertSymTab(savedIDs.top(),VarType,currentFunction,Int,yylineno)) {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 3"; exit(-1);}
        }else {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno;; exit(-1);}
      }else{
        if(existIdEveryScope(savedIDs.top())) {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 4"; exit(-1);}
        insertSymTab(savedIDs.top(),VarType,currentFunction,Int,yylineno);
      }
      savedIDs.pop();
    }
  |type_specifier erro ID {savedIDs.push(copyString(currentToken));} SBTO NUM {savedInt = atoi(copyString(currentToken));} SBTC SCL
    {
      $$ = $1;
      $$->child[0] = newNode(IdArrayK);
      $$->child[0]->name = savedIDs.top();
      if(currentFunction != " "){
        if(existID(savedIDs.top()," ")) {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno; exit(-1);}
        if(savedIDs.top().compare(currentFunction) == 0) {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 5"; exit(-1);}
        if($$->name=="int"){
          if(!insertSymTab(savedIDs.top(),VarType,currentFunction,Int,yylineno)) {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 6"; exit(-1);}
        }else {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 7";; exit(-1);}
      }else{
        if(existIdEveryScope(savedIDs.top())) {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 8"; exit(-1);}
        insertSymTab(savedIDs.top(),VarType,currentFunction,Int,yylineno);
      }
      savedIDs.pop();
      $$->child[0]->child[0] = newNode(ConstK);
      $$->child[0]->child[0]->val = savedInt;
    }
  ;

type_specifier:
  INT
    {
      $$ = newNode(TypeK);
      $$->name = "int";
    }
  |VOID
    {
      $$ = newNode(TypeK);
      $$->name = "void";
    }
  ;

fun_declaration:
  type_specifier erro ID {
      savedIDs.push(copyString(currentToken)); currentFunction = copyString(currentToken);
      $$ = $1;
      if($$->name=="int"){
        insertSymTab(savedIDs.top(),FuncType," ",Int,yylineno);
      }else {
        !insertSymTab(savedIDs.top(),FuncType," ",Void,yylineno);
      }
    } PRTO params PRTC compound_stmt{
      if(existIdEveryScope(savedIDs.top())) {
        if(currentFunction!=savedIDs.top()){
          cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 9"; exit(-1);
        }
      }
      $$->child[0] = newNode(FnK);
      $$->child[0]->name = savedIDs.top();
      savedIDs.pop();
      $$->child[0]->child[0] = $6;
      $$->child[0]->child[1] = $8;
      currentFunction = " ";
    }
  ;

compound_stmt:
  KEYO local_declarations statement_list KEYC{
    $$ = $2;
    if($$==NULL){
      $$ = $3;
    }
    else{
      YYSTYPE t = $$;
      while(t->sibling != NULL) t = t->sibling;
      t->sibling = $3;
    }
  }
  ;

statement_list:
  statement_list statement {
    YYSTYPE t = $1;
    if(t!=NULL){
      while(t->sibling != NULL) t = t->sibling;
      t->sibling = $2;
      $$ = $1;
    }else{
      $$ = $2;
    }
  }
  | /* empty */{
    $$ = NULL;
  }
  ;

statement:
  expression_stmt{
    $$ = $1;
  }
  |selection_stmt{
    $$ = $1;
  }
  |compound_stmt{
    $$ = $1;
  }
  |iteration_stmt{
    $$ = $1;
  }
  |return_stmt{
    $$ = $1;
  }
  ;

expression_stmt:
  expression SCL{
    $$ = $1;
  }
  | SCL{
    $$ = NULL;
  }
  ;

selection_stmt:
  IF PRTO expression PRTC statement selection_else_stmt{
    if(checkAtr($3)) {cout <<"Erro semântico na atribuicao na linha " << yylineno; exit(-1);}
    $$ = newNode(CondK);
    $$->name = "if";
    $$->child[0] = $3;
    $$->child[1] = $5;
    $$->child[2] = $6;
  }
  ;

selection_else_stmt:
  ELSE statement{
    $$ = $2;
  }
  |  /**/{
    $$ = NULL;
  }
  ;


iteration_stmt:
  WHILE PRTO expression PRTC statement{
    if(checkAtr($3)) {cout <<"Erro semântico na atribuição na linha " << yylineno; exit(-1);}
    $$ = newNode(LoopK);
    $$->name = "while";
    $$->child[0] = $3;
    $$->child[1] = $5;
  }
  ;

return_stmt:
  RETURN  SCL{
    $$ = newNode(ReturnK);
    $$->name = "return";
  }
  | RETURN expression SCL{
    $$ = newNode(ReturnK);
    $$->child[0] = $2;
  }
  ;

expression:
  var ATR expression{
    if(checkVoid($3)) {cout <<"Erro semântico no ID: " << $3->name << " na linha " << yylineno << ": Erro 10"; exit(-1);}
    $$ = newNode(AtrK);
    $$->name = "=";
    $$->child[0] = $1;
    $$->child[1] = $3;
  }
  | simple_expression{
    $$ = $1;
  }
  ;

var:
  erro ID {savedIDs.push(copyString(currentToken));}{
    if(!existID(savedIDs.top(),currentFunction)) {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 11"; exit(-1);}
    if(getTypeID(savedIDs.top(),currentFunction) != VarType) {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 12"; exit(-1);}
    $$ = newNode(IdK);
    $$->name = savedIDs.top();
    insertLineID(savedIDs.top(), currentFunction, yylineno);
    savedIDs.pop();
  }
  | erro ID {savedIDs.push(copyString(currentToken));} SBTO expression SBTC{
    if(!existID(savedIDs.top(),currentFunction)) {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 13"; exit(-1);}
    if(getTypeID(savedIDs.top(),currentFunction) != VarType) {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 14"; exit(-1);}
    if(checkAtr($5)) {cout <<"Erro semântico na atribuicao na linha " << yylineno; exit(-1);}
    $$ = newNode(IdArrayK);
    $$->name = savedIDs.top();
    insertLineID(savedIDs.top(), currentFunction, yylineno);
    savedIDs.pop();
    $$->child[0] = $5;
  }
  ;

relop:
  LTE{
    $$ = newNode(OpK);
    $$->name = "<=";
  }
  | LT{
    $$ = newNode(OpK);
    $$->name = "<";
  }
  | GT{
    $$ = newNode(OpK);
    $$->name = ">";
  }
  | GTE{
    $$ = newNode(OpK);
    $$->name = ">=";
  }
  | EQ{
    $$ = newNode(OpK);
    $$->name = "==";
  }
  | NEQ{
    $$ = newNode(OpK);
    $$->name = "!=";
  }
  ;

additive_expression:
  additive_expression adop term{
    if(checkVoid($3)) {cout <<"Erro semântico no ID: " << $3->name << " na linha " << yylineno << ": Erro 15"; exit(-1);}
    $$ = $2;
    $$->child[0] = $1;
    $$->child[1] = $3;
  }
  | term{
    $$ = $1;
  }
  ;

adop:
  ADD{
    $$ = newNode(OpK);
    $$->name = "+";
  }
  |  SUB{
    $$ = newNode(OpK);
    $$->name = "-";
  }
  ;

term:
  factor mulop term{
    if(checkVoid($3)) {cout <<"Erro semântico no ID: " << $3->name << " na linha " << yylineno << ": Erro 16"; exit(-1);}
    $$ = $2;
    $$->child[0] = $1;
    $$->child[1] = $3;
  }
  | factor{
    $$ = $1;
  }
  ;

mulop:
  MUL{
    $$ = newNode(OpK);
    $$->name = "*";
  }
  | DIV{
    $$ = newNode(OpK);
    $$->name = "/";
  }
  ;

factor:
  PRTO expression PRTC{
    $$ = $2;
  }
  | var{
    $$ = $1;
  }
  | NUM {savedInt = atoi(copyString(currentToken));}{
    $$ = newNode(ConstK);
    $$->val = savedInt;
  }
  | call{
    $$ = $1;
  }
  ;

call:
  erro ID {savedIDs.push(copyString(currentToken));} PRTO args PRTC{
    if(!existID(savedIDs.top()," ")) {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << " função não declarada"; exit(-1);}
    $$ = newNode(CallK);
    $$->name = savedIDs.top();
    insertLineIDGlobal(savedIDs.top(), yylineno);
    savedIDs.pop();
    $$->child[0] = $5;
  }
  ;

args:
  arg_list{
    $$ = $1;
  }
  |/* empty */{
    $$ = NULL;
  }
  ;

arg_list:
  arg_list COM expression{
    YYSTYPE t = $1;
    if(t!=NULL){
      while(t->sibling != NULL) t = t->sibling;
      t->sibling = $3;
      $$ = $1;
    }else{
      $$ = $3;
    }
  }
  | expression{
    if(checkVoid($1)) {cout <<"Erro semântico no ID: " << $1->name << " na linha " << yylineno << ": Erro 17"; exit(-1);}
    $$ = $1;
  }
  ;

simple_expression:
    additive_expression relop additive_expression{
      $$ = $2;
      $$->child[0] = $1;
      $$->child[1] = $3;
    }
  | additive_expression{
    $$ = $1;
  }
  ;

local_declarations:
  local_declarations var_declaration{
    YYSTYPE t = $1;
    if(t!=NULL){
      while(t->sibling != NULL) t = t->sibling;
      t->sibling = $2;
      $$ = $1;
    }else{
      $$ = $2;
    }
  }
  | /* empty */{
    $$ = NULL;
  }
  ;

params:
      param_list{
        $$ = $1;
      }
  |   VOID {
        $$ = NULL;
      }
  ;

param_list:
  param_list COM param{
    YYSTYPE t = $1;
    if(t!=NULL){
      while(t->sibling != NULL) t = t->sibling;
      t->sibling = $3;
      $$ = $1;
    }else{
      $$ = $3;
    }
  }
  |param{
    $$ = $1;
  }
  ;

param:
  type_specifier erro ID{
    savedIDs.push(copyString(currentToken));
    if(existID(savedIDs.top()," ")) {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 18";; exit(-1);}
    if($$->name=="int"){
      if(!insertSymTab(savedIDs.top(),VarType,currentFunction,Int,yylineno)) {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 19"; exit(-1);}
    }else {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 20";; exit(-1);}
    $$ = $1;
    $$->child[0] = newNode(IdK);
    $$->child[0]->name = savedIDs.top();
    savedIDs.pop();
  }
  |type_specifier erro ID {savedIDs.push(copyString(currentToken));} SBTO SBTC{
    if(existID(savedIDs.top()," ")) {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 21";; exit(-1);}
    if($$->name=="int"){
      if(!insertSymTab(savedIDs.top(),VarType,currentFunction,Int,yylineno)) {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 22"; exit(-1);}
    }else {cout <<"Erro semântico no ID: " << savedIDs.top() << " na linha " << yylineno << ": Erro 23";; exit(-1);}
    $$ = $1;
    $$->child[0] = newNode(IdK);
    $$->child[0]->name = savedIDs.top();
    savedIDs.pop();
  }
  ;

erro:
  ERR {yyerror("lex error"); exit(-1);}
  |/* empty */
  ;

%%

int main()
{
  cout << "\nParser em execução...\n";
  abrirArq();
  insertSymTab("input",FuncType," ",Int,0);
  insertSymTab("output",FuncType," ",Void,0);
  yyparse();
  if(!checkMain()) {cout <<"Nao foi declarada uma funcao main"; exit(-1);}
  else{
    showSymbTab();
    ofstream symbTabFile;
    symbTabFile.open("./outputs/symbTab");
    symbTabFile << symbTabString;
    symbTabFile.close();

    string treePreOrder = showTree(savedTree, false, 0);
    ofstream treePreOrderFile;
    treePreOrderFile.open("./outputs/treePreOrder");
    treePreOrderFile << treePreOrder;
    treePreOrderFile.close();

    quadCodeGenerator(savedTree);
    ofstream quadCodeFile;
    quadCodeFile.open("./outputs/quadCode");
    quadCode = quadCode + "(end, , , )";
    quadCodeFile << quadCode;
    quadCodeFile.close();

    cout << "Parser executado" << endl;
    return 0;
  }
}

void yyerror(char * msg)
{
  cout << msg << ": erro no token \"" << yytext << "\" na linha " << yylineno << endl;
}
