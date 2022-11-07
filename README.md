
# Chimico

Chimico is a language predominantly built for chemists. It is built to assist
the operations like stoichiometric calculations, Redox reactions and ElectroChemistry. It is a structural programming language with syntax similar to C. Also, It is a statically typed language, variables should be defined explicitly.

This is a group project of 7 members for building a compiler for chimico language.


####                        Required Installations

We have used flex as lexer for lexical analysis and used bison as parser for syntax analysis.
Further, we have used MIPS Assembly Emulator for running assembly code generated in codegeneration.
These are the commands required for installations:  
• $ sudo apt-get install flex   (Flex installation)  
• $ sudo apt-get install bison (Bison installation)  
• [MIPS Assembly Emulator](https://courses.missouristate.edu/kenvollmar/mars/MARS_4_5_Aug2014/Mars4_5.jar)    (MIPS Assembly Emulator installation)  

####                  Commands For Compilation And Running The Program

Commands for compilation:  
• $ bison -d parser.y  
• $ flex lexer.l  
• $ gcc -g -w lex.yy.c parser.tab.c -o exec  (exec is the executable)  

Commands to run the execuatble on any test file(should be present in 'tests' folder):  
• $ ./exec ./tests/<test_filename.chim>  (produces out.asm assembly code file)  
• $ java -jar Mars4_5.jar out.asm (output is produced in the terminal)  

