flex compilador.l
bison -d compilador.y
gcc -c lex.yy.c
g++ -o exec lex.yy.o compilador.tab.c -lfl
