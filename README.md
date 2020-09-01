-------------------------------------------------------------------------------------------------------------
                                            C- Compiler
                              Subject matter: Compiler LAB - 1S2020
                                  ICT UNIFESP - São José dos Campos

                                             Author:
                                       Leon Tenório da Silva

                                          Collaboration:
                                   Vlademir Celso dos Santos Júnior
                  (Base code developed together in previous subject matter Compiler)


-------------------------------------------------------------------------------------------------------------
                                    Usage:
- Compiler input: "entrada.txt" or argument inputName=NAME;
- Exec command: "./exec";
- Generate compiler command: "sh script.sh";
- Outputs: In folder "outputs" with sufix name specified in argument out outSufix=OUT SUFIX;
           With debug=false in arguments, the output Binary is enabled;
           With debug=false and quartus=true in arguments, the output Binary is formatted for my quartus processor;
- Outputs files: *syntactic tree "treePreOrder"+OUTSUFIX;
                 *Symbolic table quadruples code "symbTabQuadCode"+OUTSUFIX;
                 *intermediatte quadruples code "quadCode"+OUTSUFIX;
                 *Symbolic table assembly "symbTabAssembly"+OUTSUFIX;
                 *Binary code "binary"+OUTFIX.

- HELP PROGRAM: "./help"

-------------------------------------------------------------------------------------------------------------

                                            Compilador C-
                                   UC: LAB de Compiladores - 1S2020
                                  ICT UNIFESP - São José dos Campos

                                               Autor:
                                        Leon Tenório da Silva

                                            Contribuição:
                                  Vlademir Celso dos Santos Júnior
                (Código base desenvolvido em grupo na disciplina anterior UC Compiladores)

-------------------------------------------------------------------------------------------------------------
                                  Execução
- Entrada utilizada: "entrada.txt" ou argumento inputName=NAME;
- Modo de execução: "./exec";
- Modo de compilar o compilador: "sh script.sh";
- Saídas do compilador: presentes na pasta "outputs" com o sufixo passado no argumento outSufix=OUTSUFIX;
                        Com debug=false no argumentos a saída binary é habilitada;
                        Com debug=false e quartus=true nos argumentos, a saída é formatada para o meu processador implementado no quartus;
- Saídas presentes: *Arvore sintática "threePreOrder"+OUTFIX;
                    *Tabela de símbolos das quádruplas "symbTabQuadCode"+OUTSUFIX;
                    *Código intermediário por quádruplas "quadCode"+OUTSUFIX;
                    *Tabela de símbolos assembly "symbTabAssembly"+OUTFIX;
                    *Código binário "binary"+OUTSUFIX.

- PROGRAMA DE AJUDA: "./help"