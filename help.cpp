#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

string gerarTracos(int quantidade){
    string ret = "";
    while(ret.length()<quantidade){
        ret = ret + "-";
    }
    return ret;
}

int main(){
    cout << gerarTracos(120) << endl;
    cout << endl << "Autor: Leon Tenório da Silva" << endl;
    cout << "Compilador desenvolvido na disciplina de Laboratório de Compiladores 1S2020 ICT UNIFESP" << endl << endl;
    cout << gerarTracos(120) << endl;
    cout << "Precisando de ajuda para executar o compilador?" << endl;
    cout << "OK, aqui vão algumas informações para te ajudar" << endl;
    cout << gerarTracos(120) << endl;
    cout << "Execute para compilar o compilador:" << endl;
    cout << "$sh script.sh$" << endl << endl;
    cout << "Depois de compilado você pode executar ele com: " << endl;
    cout << "$./exec$" << endl << endl;
    cout << "No comando anterior você pode acrescentar parâmetros: " << endl;
    cout << "$./exec <param1=valor1> <param2=valor2> ...$" << endl << endl;
    cout << gerarTracos(120) << endl;
    cout << "Esses parâmetros são: "<< endl << endl;
    cout << "inputName=<NOME DO ARQUIVO A SER COMPILADO>" << endl;
    cout << "outSufix=<SUFIXO UTILIZADO NOS ARQUIVOS DE SAÍDA NA PASTA OUTPUTS>" << endl;
    cout << "debug=true, para visualizar comentários de alguns aspectos da conversão do código intermediário para ASSEMBLY" << endl;
    cout << "debug=false, para não visualizar os comentários e habilitar a saída do código BINARY" << endl;
    cout << "quartus=true, para habilitar a saída com a formatação específica para colocar o código no processador implementado no quartus e alvo desse compilador" << endl;
    cout << gerarTracos(120) << endl;
    cout << "Acho que isso é tudo, qualquer coisa entre em contado com Leon Tenório da Silva" << endl;
    cout << gerarTracos(120) << endl;
    return 0;
}