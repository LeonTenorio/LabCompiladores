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
    cout << gerarTracos(150) << endl;
    cout << endl << "Autor: Leon Tenório da Silva" << endl;
    cout << "Compilador desenvolvido na disciplina de Laboratório de Compiladores 1S2020 ICT UNIFESP" << endl << endl;
    cout << gerarTracos(150) << endl;
    cout << "Precisando de ajuda para executar o compilador?" << endl;
    cout << "OK, aqui vão algumas informações para te ajudar" << endl;
    cout << gerarTracos(150) << endl;
    cout << "Execute para compilar o compilador:" << endl;
    cout << "$sh script.sh$" << endl << endl;
    cout << "Depois de compilado você pode executar ele com: " << endl;
    cout << "$./exec$" << endl << endl;
    cout << "No comando anterior você pode acrescentar parâmetros: " << endl;
    cout << "$./exec <param1=valor1> <param2=valor2> ...$" << endl << endl;
    cout << gerarTracos(150) << endl;
    cout << "Esses parâmetros são: "<< endl << endl;
    cout << "inputName=<NOME DO ARQUIVO A SER COMPILADO>" << endl;
    cout << "outSufix=<SUFIXO UTILIZADO NOS ARQUIVOS DE SAÍDA NA PASTA OUTPUTS>" << endl;
    //cout << "debug=false, para não visualizar os comentários e habilitar a saída do código BINARY" << endl;
    cout << "quartus=true, para habilitar a saída com a formatação específica para colocar o código no processador implementado no quartus e alvo desse compilador" << endl;
    cout << "showBinary=true, para sair no terminal o código assembly junto do código binário" << endl;
    cout << gerarTracos(150) << endl << endl;
    cout << "Para a explicação da execução deste compilador temos alguns diagramas que podem ser vistos nessa pasta" << endl;
    cout << "Diagrama de atividade - Análise: Explica as sequência que ocorre na fase de análise do compilador" << endl;
    cout << "Diagrama de blocos - Análise: Explica as estruturas e suas correções na etapa de análise" << endl;
    cout << "Diagrama de Atividades - Quádruplas: Explica a sequência de execução para a síntese do código intermediário em quádruplas" << endl;
    cout << "Diagrama de blocos - Quádruplas: Explica a estrutura das minhas quádruplas e agrupa algumas de acordo com seu uso" << endl;
    cout << "Diagrama de atividades - Codigo assembly: Explica todo o procedimento de síntese do código ASSEMBLY de uma forma bem próxima a implementação realizada" << endl;
    cout << "Diagrama de atividades - Alocação de variáveis: Explica o processo de alocação de variáveis (subdiagrama do diagrama anterior)" << endl;
    cout << endl << gerarTracos(150) << endl;
    cout << "Acho que isso é tudo, qualquer coisa entre em contado com Leon Tenório da Silva" << endl;
    cout << gerarTracos(150) << endl;
    return 0;
}