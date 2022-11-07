

%{
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include "symtab.h"
#include "parser.tab.h"
#include "semantics.h"
#include "ast.h"
int line_cnt=1;

//int yylval; // in general defined as an enum ;; for our purpose int is enough.

static void multiple_line_comments(void);

/*definitions*/
%}


%%
"if"         				                   	{return IF;}
"else" 				                       {return ELSE;}
"elseif" 				                      	{return ELIF;}
"while" 				                      	{return WHILE;}
"for" 					                      	{return FOR;}
"int" 					                      	{return INT;}
"double" 				                      	{return DOUBLE;}
"read" 					                        {return READ ;}
"write" 					                      {return WRITE ;}
"element" 				                    	{return ELEMENT;}
"molecule" 					                    {return MOLECULE;}
"string" 					                      {return STRING;}
"bool" 					                        {return BOOL;}
"true" 					                        {return TRUE;}
"false"					                        {return FALSE;}
"func" 					                        {return FUNC;}
"ret"  					                        {return RET;}
"void"                                                                {return VOID;}


"(" 						                        {return LBRAC;}
")" 						                        {return RBRAC;}
"{" 						                        {return LPAREN;}
"}" 						                        {return RPAREN;}
"[" 						                        {return SQOPEN;}
"]" 						                        {return SQCLOSE;}
"=" 						                        {return ASSIGN;}
"," 						                        {return COMMA;}
";" 						                        {return SEMICOLON;}

"<=" 					                         	{ return LEQ;}
"<" 					                        	{ yylval.val.ival = LESS;  return LTHAN;}
">=" 					                        	 {  yylval.val.ival = GREATER_EQUAL;return GEQ;}
">" 				                        		{  yylval.val.ival = GREATER;return GTHAN;}
"=="					                        	{  yylval.val.ival = EQUAL;return EQU;}
"!=" 					                        	{  yylval.val.ival = NOT_EQUAL; return NOTEQ;}
"||" 				                         		{ return LOR;}
"&&" 				                        		{return LAND;}
"+" 				                         		{ yylval.val.ival = ADD;return PLUS;}
"-" 					                        	{ yylval.val.ival = SUB; return MINUS;}
"*" 					                        	{  return MULT;}
"/" 						                        {return DIVS;}
"%" 					                        	{return MOD;}

"." 						                        { return PERIOD;}
":" 						                        { return COLON;}

"++"								    { return INCR;}
"--"                                                             {return DECR;}

"balance" 					                    { return BALANCE;}
"molar_mass" 					                  {return MOLARMASS;}
"chem_name" 					                  {return CHEMNAME;}
"name" 					                        { return NAME;}
"charge" 					                      {return CHARG;}
"emp_for" 					                    {return EMP;}

"atm_wt"                                { return ATOMIC_WT;}
"atm_no"                                {return ATOMIC_NO;}


[A-Z][a-z]?                             {insert(yytext, strlen(yytext), UNDEF, line_cnt);yylval.symtab_item = lookup(yytext); return ELEMENT_NAME;}
([A-Z][a-z]?[1-9]*)+                    {insert(yytext, strlen(yytext), UNDEF, line_cnt);yylval.symtab_item = lookup(yytext); return MOL_NAME;}
[a-z][a-zA-Z0-9_]* 				            {insert(yytext, strlen(yytext), UNDEF, line_cnt);yylval.symtab_item = lookup(yytext); return IDENTIFIER;}
[0-9]+ 					                        {yylval.val.ival  = atoi(yytext); return INTEGER;}
[0-9]+[.][0-9]* 				                {yylval.val.fval = atof(yytext); return DECIMAL;}

"##".* 					                        { }     //single line comments

"#*" 						                        { multiple_line_comments();}

\"([^"\\&&^\n]|\\['"?\\abfnrtv])*\n 		{yytext[yyleng-1]='\0';printf("ERROR: Unterminated string at line number %d\t",line_cnt+1);return ERROR;}

\"([^"\\&&^\n]|\\['"?\\abfnrtv])*\" 		{yylval.val.sval = malloc(yyleng * sizeof(char));strcpy(yylval.val.sval,yytext);printf("STRING_CONST\t"); return STRING_CONST;}


[ \t]  					                        {} /*blank,tab,whitespace ;; eat up the whitespace*/
[\n] 						                        {line_cnt++ ;}
. 						                          {printf("ERROR: Invalid character %s at line number %d\t",yytext,line_cnt+1); return ERROR;}
%%

static void multiple_line_comments(void)
{
  int ch;
  int line=line_cnt+1;
  for(;;)
  {
    int loop=0;
    switch(input())
    {
      //if character is EOF, then comment is unterminated
      case '\0':
        printf("ERROR: Unterminated comment at line number %d\t",line);
        exit(-1);
        loop=-1;
      break;

      //if character is '*' and following character is '#', then end of comments or else, check for next loop
      case '*':
        if((ch = input()) == '#')
        {
          loop=-1;
          //printf("MULTILINE COMMENTS END\n");
          break;
        }
        unput(ch);
        break;

      //if character is newline('\n'), increment line no by 1
      case '\n':
          line_cnt++;

      default:
        break;
    }

    if(loop==-1)
    {
    	break;
    }
  }
}


int yywrap(void ) {return 1;}
/*
int main(int argc,char** argv)
{

  if(argc<2)
  {
    printf("STDIN: Enter the string(Press CTRL D to exit) :\n");
    while(yylex()!=0)
    {
       printf("-\t\t%s",yytext);

       printf("\n");
    }
  }

  for(int i=1;i<argc;i++)
  {
    int index=0;
    while(argv[i][index]!='\0'){index++;}
    if(index<5 || argv[i][index-1]!='m' || argv[i][index-2]!='i' && argv[i][index-3]!='h' || argv[i][index-4]!='c' || argv[i][index-5]!='.')
    {
      printf("WARNING!: The File named %s does not match with chimico file extensions(.chim)\n",argv[i]);
    }
    line_cnt=1;
    if(!(yyin=fopen(argv[i],"r")))
    {
      printf("ERROR!: File named %s could not be found",argv[i]);
    }
    else
    {
      while(yylex()!=0)
      {
         printf("-\t\t%s",yytext);

         printf("\n");
      }
    }
    printf("\n------------------------------------------------------------------\n");
  }

  return 0;

}
*/
