%{
    #include<stdio.h>
    #include<stdlib.h>
    #include<string.h>
    void yyerror();
    int yylex();
    int status = 1;
    FILE* yyin;
    char* fname;
%}


%union {
    int num;
    float decimal;
    char *string;
}

%start PROGRAM
%token SEMICOLON COLON PERIOD LPAREN RPAREN LBRAC RBRAC SQOPEN SQCLOSE COMMA 
%token PLUS MINUS MUL DIV MOD ERROR
%token IF ELIF ELSE FOR WHILE RET WRITE READ 
%token LAND LOR EQUAL NOTEQ LTHAN LEQ GTHAN GEQ ASSIGN
%token INT VOID DOUBLE STRING BOOL ELEMENT MOLECULE 
%token ATOMIC_WT ATOMIC_NO CHARGE MOLARMASS NAME CHEMNAME BALANCE EMP
%token TRUE FALSE FUNC INC_DEC
%token <string> IDENTIFIER STRING_CONST ELEMENT_NAME MOL_NAME
%token <num> INTEGER
%token <decimal> DECIMAL


%nonassoc IF
%right ASSIGN
%left PERIOD
%left LOR
%left LAND
%left EQUAL NOTEQ
%left LTHAN GTHAN LEQ GEQ
%left PLUS MINUS
%left MULT DIV MOD
%nonassoc "then"
%nonassoc ELIF
%nonassoc ELSE
%nonassoc LBRAC RBRAC

%%

PROGRAM                 :   EXTERNAL_DECL
                        |   PROGRAM EXTERNAL_DECL;

EXTERNAL_DECL           :   FUNCTION
                        |   DECLARATION;                        
                        
FUNCTION                :   FUNC DECL_SPECIFIER DECLARATOR DECLARATION_LIST COMPOUND_STMT
                        |   FUNC DECLARATOR DECLARATION_LIST COMPOUND_STMT
                        |   FUNC DECL_SPECIFIER DECLARATOR COMPOUND_STMT
                        |   FUNC DECLARATOR COMPOUND_STMT;

DECLARATION             :   DECL_SPECIFIER INIT_DECL_LIST SEMICOLON
                        |   DECL_SPECIFIER SEMICOLON
                        |   ELEMENT ELEMENT_DECL_LIST SEMICOLON
                        |   MOLECULE MOL_DECL_LIST SEMICOLON;
               

DECLARATION_LIST        :   DECLARATION
                        |   DECLARATION_LIST DECLARATION;

DECL_SPECIFIER          :   TYPE DECL_SPECIFIER
                        |   TYPE;

					 
TYPE                    :   INT 
                        |   DOUBLE 
                        |   VOID
                        |   STRING 
                        |   BOOL;

                        
INIT_DECL_LIST          :   INIT_DECLARATOR
                        |   INIT_DECL_LIST COMMA INIT_DECLARATOR;
                        
ELEMENT_DECL_LIST       :   ELEMENT_DECLARATOR
                        |   ELEMENT_DECL_LIST COMMA ELEMENT_DECLARATOR;

ELEMENT_DECLARATOR      :   ELEMENT_NAME
                        |   ELEMENT_NAME ASSIGN LBRAC ELEMENT_EXPR RBRAC;
                        
MOL_DECL_LIST           :   MOL_DECLARATOR
                        |   MOL_DECL_LIST COMMA MOL_DECLARATOR;

MOL_DECLARATOR          :   MOL_NAME
                        |   MOL_NAME ASSIGN SQOPEN MOL_EXPR SQCLOSE;

INIT_DECLARATOR         :   DECLARATOR
                        |   DECLARATOR ASSIGN INITIALIZER;
                        
DECLARATOR              :   IDENTIFIER
                        |   LBRAC DECLARATOR RBRAC
                        |   DECLARATOR SQOPEN CONSTANT_EXPR SQCLOSE
                        |   DECLARATOR SQOPEN SQCLOSE
                        |   DECLARATOR LBRAC PARAM_LIST RBRAC
                        |   DECLARATOR LBRAC IDENTIFIER_LIST RBRAC
                        |   DECLARATOR LBRAC RBRAC;

PARAM_LIST              :   PARAM_DECL
                        |   PARAM_LIST COMMA PARAM_DECL;

PARAM_DECL              :   DECL_SPECIFIER DECLARATOR
                        |   ELEMENT ELEMENT_NAME
                        |   MOLECULE MOL_NAME;

IDENTIFIER_LIST         :   IDENTIFIER
                        |   IDENTIFIER_LIST COMMA IDENTIFIER
                        |   ELEMENT_NAME
                        |   IDENTIFIER_LIST COMMA ELEMENT_NAME
                        |   MOL_NAME
                        |   IDENTIFIER_LIST COMMA MOL_NAME;

INITIALIZER             :   ASGN_EXPR
                        |   LPAREN INITIALIZER_LIST RPAREN ;

INITIALIZER_LIST        :   INITIALIZER
                        |   INITIALIZER_LIST COMMA INITIALIZER;

STATEMENT               :   EXPRESSION_STMT
                        |   COMPOUND_STMT
                        |   IFELSE_STMT
                        |   LOOP_STMT
                        |   WRITE_STMT
                        |   READ_STMT
                        |   RETURN_STMT;
                       
WRITE_STMT              :   WRITE LBRAC EXPRESSION RBRAC SEMICOLON;

READ_STMT               :   READ LBRAC EXPRESSION RBRAC SEMICOLON;
                        
RETURN_STMT             :   RET EXPRESSION SEMICOLON
                        |   RET SEMICOLON;
                        

EXPRESSION_STMT         :   EXPRESSION SEMICOLON
                        |   SEMICOLON ;

COMPOUND_STMT           :   LPAREN COMPSTMTS RPAREN
                        |   LPAREN RPAREN;

COMPSTMTS               :   DECLARATION
                        |   STATEMENT
                        |   COMPSTMTS DECLARATION
                        |   COMPSTMTS STATEMENT;

IFELSE_STMT             :   IF LBRAC EXPRESSION RBRAC STATEMENT ELSE STATEMENT     
                        |   IF LBRAC EXPRESSION RBRAC STATEMENT ELIF_STMT ELSE STATEMENT
                        |   IF LBRAC EXPRESSION RBRAC STATEMENT ELIF_STMT   %prec "then"
                        |   IF LBRAC EXPRESSION RBRAC STATEMENT      %prec "then"; 
                        
ELIF_STMT               :   ELIF LBRAC EXPRESSION RBRAC STATEMENT
                        |   ELIF_STMT ELIF LBRAC EXPRESSION RBRAC STATEMENT;                   

LOOP_STMT               :   WHILE LBRAC EXPRESSION RBRAC STATEMENT
                        |   FOR LBRAC FOREXPRESSION SEMICOLON EXPRESSION SEMICOLON EXPRESSION RBRAC STATEMENT
                        |   FOR LBRAC SEMICOLON EXPRESSION SEMICOLON EXPRESSION RBRAC STATEMENT
                        |   FOR LBRAC FOREXPRESSION SEMICOLON SEMICOLON EXPRESSION RBRAC STATEMENT
                        |   FOR LBRAC FOREXPRESSION SEMICOLON EXPRESSION SEMICOLON RBRAC STATEMENT
                        |   FOR LBRAC SEMICOLON SEMICOLON EXPRESSION RBRAC STATEMENT
                        |   FOR LBRAC SEMICOLON EXPRESSION SEMICOLON RBRAC STATEMENT
                        |   FOR LBRAC FOREXPRESSION SEMICOLON SEMICOLON RBRAC STATEMENT
                        |   FOR LBRAC SEMICOLON SEMICOLON RBRAC STATEMENT;

FOREXPRESSION           :   TYPE EXPRESSION
                        |   EXPRESSION;

EXPRESSION              :   ASGN_EXPR
                        |   EXPRESSION COMMA ASGN_EXPR;

ASGN_EXPR               :   LOGIC_OR_EXPR
                        |   UNARY_EXPR ASSIGN ASGN_EXPR;

CONSTANT_EXPR           :   LOGIC_OR_EXPR;

LOGIC_OR_EXPR           :   LOGIC_AND_EXPR
                        |   LOGIC_OR_EXPR LOR LOGIC_AND_EXPR;

LOGIC_AND_EXPR          :   EQUALITY_CHECK
                        |   LOGIC_AND_EXPR LAND EQUALITY_CHECK;

EQUALITY_CHECK          :   COMPARISION_EXPR
                        |   EQUALITY_CHECK EQUAL COMPARISION_EXPR
                        |   EQUALITY_CHECK NOTEQ COMPARISION_EXPR;

COMPARISION_EXPR        :   ADDITION_EXPR
                        |   COMPARISION_EXPR LTHAN ADDITION_EXPR
                        |   COMPARISION_EXPR GTHAN ADDITION_EXPR
                        |   COMPARISION_EXPR LEQ ADDITION_EXPR
                        |   COMPARISION_EXPR GEQ ADDITION_EXPR;
                        
                        
ADDITION_EXPR           :   MULTIP_EXPR
                        |   ADDITION_EXPR PLUS MULTIP_EXPR
                        |   ADDITION_EXPR MINUS MULTIP_EXPR;

MULTIP_EXPR             :   CAST_EXPR
                        |   MULTIP_EXPR MULT CAST_EXPR
                        |   MULTIP_EXPR DIV CAST_EXPR
                        |   MULTIP_EXPR MOD CAST_EXPR;

CAST_EXPR               :   UNARY_EXPR
                        |   LBRAC TYPE RBRAC CAST_EXPR;

UNARY_EXPR              :   POSTFIX_EXPR
                        |   INC_DEC UNARY_EXPR;
                       
                               
POSTFIX_EXPR            :   PRIMARY_EXPR
                        |   POSTFIX_EXPR SQOPEN EXPRESSION SQCLOSE
                        |   POSTFIX_EXPR LBRAC ARG_EXPR_LIST RBRAC
                        |   POSTFIX_EXPR LBRAC RBRAC
                        |   POSTFIX_EXPR PERIOD IDENTIFIER
                        |   POSTFIX_EXPR PERIOD ATTRIBUTE
                        |   POSTFIX_EXPR INC_DEC;
                        
ATTRIBUTE               :   ATOMIC_WT
                        |   ATOMIC_NO
                        |   CHARGE;
                        
PRIMARY_EXPR            :   IDENTIFIER
                        |   ELEMENT_NAME
                        |   MOL_NAME
                        |   CONSTANT
                        |   STRING_CONST
                        |   BALANCE LBRAC MOL_LIST MINUS MOL_LIST RBRAC
                        |   NAME LBRAC ELEMENT_NAME RBRAC
                        |   MOLARMASS LBRAC MOL_NAME RBRAC
                        |   EMP LBRAC MOL_NAME RBRAC
                        |   CHEMNAME LBRAC MOL_NAME RBRAC;
                       
                        
MOL_LIST                :   MOL_NAME
                        |   MOL_LIST COMMA MOL_NAME;  
                       
ELEMENT_EXPR            :   NUMBER COMMA NUMBER COMMA PLUS NUMBER
                        |   NUMBER COMMA NUMBER COMMA MINUS NUMBER
                        |   NUMBER COMMA NUMBER COMMA NUMBER;

NUMBER                  :   INTEGER
                        |   DECIMAL;                    
                        
MOL_EXPR                :   ELEMENT_NAME COLON INTEGER
                        |   MOL_EXPR COMMA ELEMENT_NAME COLON INTEGER;

ARG_EXPR_LIST           :   ASGN_EXPR
                        |   ARG_EXPR_LIST COMMA ASGN_EXPR;

CONSTANT                :   INTEGER
                        |   DECIMAL
                        |   TRUE
                        |   FALSE;

%%

int main(int argc, char** argv) {

    for (int i = 1; i < argc; ++i) {
        fname = argv[i];
        yyin = fopen(fname, "r");

        yyparse();  

        fclose(yyin);
    }

    if (status==1)
        printf("\n******Parsing Successful!******\n");
    
}

void yyerror () {
    extern int yylineno;
	fprintf(stderr, "Parsing failed at Line %d\nSyntax Error!\n",yylineno);
    status = 0;
} 


