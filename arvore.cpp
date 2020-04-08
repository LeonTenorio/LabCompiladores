#include <iostream>
#include <string.h>

#define TABSAPCES 6
#define TABTREE 5

using namespace std;

int tempIndex = 0, labelIndex = 0;
enum NodeKind{ConstK,IdK,IdArrayK,TypeK,FnK, OpK, ReturnK, LoopK, CondK, CallK, AtrK};

struct treeNode{
    treeNode *child[3], *sibling;
    NodeKind nodeKind;
    string name;
    int val;
};

treeNode *newNode(NodeKind kind){
    treeNode *node = (treeNode *) malloc(sizeof(treeNode));
    for(int i = 0; i < 3; i++) node->child[i] = NULL;
    node->sibling = NULL;
    node->nodeKind = kind;
    return node;
}

string tabTree(int treeLevel){
  string ret = "";
  for(int i=0;i<treeLevel*TABTREE;i++){
    ret = ret + " ";
  }
  return ret;
}

string showTree(treeNode *tree, bool is_brother, int treeLevel){
  if(tree == NULL) return "";
  if(treeLevel==0)
    return "{\n'tree:'" + showTree(tree, false, 1) + "\n}\n";
  string ret = "";
  if(tree->sibling!=NULL && is_brother==false)
    ret = "\n" + tabTree(treeLevel) + "[\n"+tabTree(treeLevel)+"{\n"+tabTree(treeLevel)+"'Node': ";
  else
    ret = "\n" + tabTree(treeLevel) + "{\n"+tabTree(treeLevel)+"'Node': ";
  switch(tree->nodeKind){
    case ConstK:
      ret = ret + to_string(tree->val);
    break;
    case FnK:
      ret = ret + "'FUNCTION', ";
    break;
    case ReturnK:
      ret = ret + "'return', ";
    break;
    case LoopK:
      ret = ret + "'while', ";
    break;
    case CondK:
      ret = ret + "'if', ";
    break;
    case CallK:
      ret = ret + "'CALL - " + tree->name + "',";
    break;
    case AtrK:
      ret = ret + "'"+tree->name + "', ";
    break;
    default:
      ret = ret + "'" + tree->name + "', ";
  }
  if(tree->child[0]!=NULL)
    ret = ret + "\n"+tabTree(treeLevel)+"'child[0]': " + showTree(tree->child[0], false, treeLevel+1);
  if(tree->child[1]!=NULL)
    ret = ret + "\n"+tabTree(treeLevel)+"'child[1]': " + showTree(tree->child[1], false, treeLevel+1);
  if(tree->child[2]!=NULL)
    ret = ret + "\n"+tabTree(treeLevel)+"'child[2]': " + showTree(tree->child[2], false, treeLevel+1);

  ret = ret + "\n"+tabTree(treeLevel)+"}";
  if(tree->sibling!=NULL && is_brother==false){
    ret = ret + ",";
    treeNode *aux = tree->sibling;
    while(aux!=NULL){
      ret = ret + showTree(aux, true, treeLevel);
      if(aux->sibling!=NULL)
        ret = ret + tabTree(treeLevel)+",";
      aux = aux->sibling;
    }
  }
  if(tree->sibling!=NULL && is_brother==false)
    string ret = "\n" + tabTree(treeLevel) + "]\n";
  return ret;
}

int countParams(treeNode *tree){
	int ret = 0;
	while(tree!=NULL){
		ret++;
		tree = tree->sibling;
	}
	return ret;
}

string tabGenerator(string first_word, bool is_label){
  string ret = "";
  int i;
  for(i=first_word.length(); i<=TABSAPCES;i++){
    ret = ret + " ";
  }
  if(is_label)
    return first_word+ret;
  else
    return ret+first_word;
}

static string quadCode = "(goto, main, , )\n";

void getParamsFunction(treeNode *node){
  if(node==NULL)
    return;
  else{
    getParamsFunction(node->sibling);
    quadCode = quadCode + "(pop_param, " + node->child[0]->name + ", , )\n";
  }
}

string quadCodeGenerator(treeNode *node){
  if(node==NULL)
    return "";
  else{
    switch (node->nodeKind) {
      case ConstK:{
        return to_string(node->val);
        break;
      }
      case IdK:{
        return node->name;
        break;
      }
      case IdArrayK:{
        return node->name + "[" + quadCodeGenerator(node->child[0]) + "]";
        break;
      }
      case TypeK:{
        string aux = quadCodeGenerator(node->child[0]);
        aux = quadCodeGenerator(node->child[1]);
        aux = quadCodeGenerator(node->child[2]);
        aux = quadCodeGenerator(node->sibling);
        return "";
        break;
      }
      case FnK:{
        if(node->name=="input"){
          quadCode = quadCode + "(fun, " + node->name + ", , )\n";
          quadCode = quadCode + "(system_in, _t"+to_string(tempIndex)+", , )\n";
          quadCode = quadCode + "(return, _t"+to_string(tempIndex) + ", , )\n";
          tempIndex++;
          quadCode = quadCode + "(end_fun, , , )\n";
        }
        else if(node->name=="output"){
          quadCode = quadCode + "(fun, " + node->name + ", , )\n";
          quadCode = quadCode + "(pop_param, val, , )\n";
          quadCode = quadCode + "(system_out, val, , )\n";
          quadCode = quadCode + "(end_fun, , , )\n";
        }
        else{
          quadCode = quadCode + "(fun, " + node->name + ", , )\n";
          getParamsFunction(node->child[0]);
          quadCodeGenerator(node->child[1]);
          quadCode = quadCode + "(end_fun, , , )\n";
        }
        quadCodeGenerator(node->sibling);
        return "";
        break;
      }
      case OpK:{
        string left = quadCodeGenerator(node->child[0]);
        if(node->child[0]->nodeKind==CallK){
          quadCode = quadCode + "(catch_return, _t"+to_string(tempIndex) + ", , )\n";
          left = "_t" + to_string(tempIndex);
          tempIndex++;
        }
        string right = quadCodeGenerator(node->child[1]);
        if(node->child[1]->nodeKind==CallK){
          quadCode = quadCode + "(catch_return, _t"+to_string(tempIndex) + ", , )\n";
          right = "_t" + to_string(tempIndex);
          tempIndex++;
        }
        quadCode = quadCode + "("+node->name+", _t" + to_string(tempIndex) + ", " + left + ", " + right + ")\n";
        tempIndex++;
        return "_t" + to_string(tempIndex-1);
      }
      case ReturnK:{
        string left = quadCodeGenerator(node->child[0]);
        if(node->child[0]->nodeKind==CallK){
          left = "_t"+to_string(tempIndex);
          quadCode = quadCode + "(catch_return," + left + ", , )\n";
          tempIndex++;
        }
        quadCode = quadCode + "(return, "+ left + ", , )\n";
        string aux = quadCodeGenerator(node->sibling);
        return "";
        break;
      }
      case LoopK:{
        string compareLabel = "_l" + to_string(labelIndex);
        labelIndex++;
        string loopLabel = "_l" + to_string(labelIndex);
        labelIndex++;
        string outLabel = "_l" + to_string(labelIndex);
        labelIndex++;
        string compare = "";
        quadCode = quadCode + "(label, " + compareLabel + ", , )\n";
        if(node->child[0]->name=="<="){
          compare = "(if_le, "+quadCodeGenerator(node->child[0]->child[0]) + ", " + quadCodeGenerator(node->child[0]->child[1]) + ", " + loopLabel + ")";
        }
        else if(node->child[0]->name=="<"){
          compare = "(if_l, "+quadCodeGenerator(node->child[0]->child[0]) + ", " + quadCodeGenerator(node->child[0]->child[1]) + ", " + loopLabel + ")";
        }
        else if(node->child[0]->name==">"){
          compare = "(if_g, "+quadCodeGenerator(node->child[0]->child[0]) + ", " + quadCodeGenerator(node->child[0]->child[1]) + ", " + loopLabel + ")";
        }
        else if(node->child[0]->name==">="){
          compare = "(if_e, "+quadCodeGenerator(node->child[0]->child[0]) + ", " + quadCodeGenerator(node->child[0]->child[1]) + ", " + loopLabel + ")";
        }
        else if(node->child[0]->name=="=="){
          compare = "(if_e, "+quadCodeGenerator(node->child[0]->child[0]) + ", " + quadCodeGenerator(node->child[0]->child[1]) + ", " + loopLabel + ")";
        }
        else if(node->child[0]->name=="!="){
          compare = "(if_ne, "+quadCodeGenerator(node->child[0]->child[0]) + ", " + quadCodeGenerator(node->child[0]->child[1]) + ", " + loopLabel + ")";
        }
        quadCode = quadCode + compare + "\n";
        quadCode = quadCode + "(goto, " + outLabel + ", , )\n";

        quadCode = quadCode + "(label, " + loopLabel + ", , )\n";

        string aux = quadCodeGenerator(node->child[1]);

        quadCode = quadCode + "(goto, " + compareLabel + ", , )\n";
        quadCode = quadCode + "(label, "+ outLabel + ", , )\n";

        aux = quadCodeGenerator(node->sibling);
        return "";
        break;
      }
      case CondK:{
        string trueLabel = "_l" + to_string(labelIndex);
        labelIndex++;
        string falseLabel = "_l" + to_string(labelIndex);
        labelIndex++;
        string compare = "";
        if(node->child[0]->name=="<="){
          compare = "(if_le, "+quadCodeGenerator(node->child[0]->child[0]) + ", " + quadCodeGenerator(node->child[0]->child[1]) + ", " + trueLabel + ")";
        }
        else if(node->child[0]->name=="<"){
          compare = "(if_l, "+quadCodeGenerator(node->child[0]->child[0]) + ", " + quadCodeGenerator(node->child[0]->child[1]) + ", " + trueLabel + ")";
        }
        else if(node->child[0]->name==">"){
          compare = "(if_g, "+quadCodeGenerator(node->child[0]->child[0]) + ", " + quadCodeGenerator(node->child[0]->child[1]) + ", " + trueLabel + ")";
        }
        else if(node->child[0]->name==">="){
          compare = "(if_ge, "+quadCodeGenerator(node->child[0]->child[0]) + ", " + quadCodeGenerator(node->child[0]->child[1]) + ", " + trueLabel + ")";
        }
        else if(node->child[0]->name=="=="){
          compare = "(if_e, "+quadCodeGenerator(node->child[0]->child[0]) + ", " + quadCodeGenerator(node->child[0]->child[1]) + ", " + trueLabel + ")";
        }
        else if(node->child[0]->name=="!="){
          compare = "(if_ne, "+quadCodeGenerator(node->child[0]->child[0]) + ", " + quadCodeGenerator(node->child[0]->child[1]) + ", " + trueLabel + ")";
        }
        quadCode = quadCode + compare + "\n";
        quadCode = quadCode + "(goto, " + falseLabel + ", , )\n";
        quadCode = quadCode + "(label, " + trueLabel + ", , )\n";

        string aux = quadCodeGenerator(node->child[1]);
        
        if(node->child[2]!=NULL){
          string outLabel = "_l"+to_string(labelIndex);
          labelIndex++;

          quadCode = quadCode + "(goto, " + outLabel + ", , )\n";
          quadCode = quadCode + "(label, " + falseLabel + ", , )\n";

          aux = quadCodeGenerator(node->child[2]);

          quadCode = quadCode + "(label, "+outLabel + ", , )\n";
        }
        else{
          quadCode = quadCode + "(label, " + falseLabel + ", , )\n";
        }

        aux = quadCodeGenerator(node->sibling);
        return "";
        break;
      }
      case CallK:{
        treeNode *sibling = node->child[0];
        while(sibling!=NULL){
          string siblingString = quadCodeGenerator(sibling);
          if(sibling->nodeKind==CallK){
            quadCode = quadCode + "(catch_return, _t"+to_string(tempIndex)+", , )\n";
            siblingString = "_t"+to_string(tempIndex);
            tempIndex++;
          }
          quadCode = quadCode + "(param, " + siblingString + ", , )\n";
          sibling = sibling->sibling;
        }
        quadCode = quadCode + "(jal, " + node->name + ", , )\n";

        string aux = quadCodeGenerator(node->sibling);

        return "";
        break;
      }
      case AtrK:{
        string left = quadCodeGenerator(node->child[0]);
        if(node->child[1]->nodeKind==OpK && node->child[1]->child[0]->nodeKind!=OpK && node->child[1]->child[0]->nodeKind!=OpK){
          quadCode = quadCode + "(" + node->child[1]->name + ", " + left + ", " + quadCodeGenerator(node->child[1]->child[0]) + ", " + quadCodeGenerator(node->child[1]->child[1]) + ")\n";
        }
        else if(node->child[1]->nodeKind==CallK){
          quadCodeGenerator(node->child[1]);
          quadCode = quadCode + "(catch_return, " + left + ", , )\n";
        }
        else{
          if(node->child[0]->nodeKind==OpK)
            tempIndex++;
          if(node->child[1]->nodeKind==OpK)
            tempIndex++;
          string right = quadCodeGenerator(node->child[1]);
          quadCode = quadCode + "(asn, " + left + ", " + right + ", )\n";
        }
        quadCodeGenerator(node->sibling);
        return "";
        break;
      }
    }
  }
}
