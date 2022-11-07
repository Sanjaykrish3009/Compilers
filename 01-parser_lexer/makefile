Exec : getExec

	mkdir outputReport

	for filename in tests/*.chim; do \
	out_file=`echo $$filename | cut -d "/" -f 2 | cut -d "." -f 1` ;\
	out_file="output_$$out_file.txt" ;\
	./exec "$$filename" > outputReport/$$out_file ;\
	done

getExec : parser lexer

lexer : lexer.l parser.tab.h
				flex lexer.l
				gcc -g lex.yy.c parser.tab.c -o exec

parser : parser.y
				 bison -d parser.y

clean :
	rm parser.tab.*
	rm lex.*
	rm exec
	rm -r "outputReport"
