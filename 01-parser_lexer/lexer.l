
%option yylineno
%{
#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"
int line_cnt=0;

//int yylval; // in general defined as an enum ;; for our purpose int is enough.

static void multiple_line_comments(void);

/*definitions*/
%}


%%
"if"         				                   	{printf("IF             \t"); return IF;}
"else" 				                         	{printf("ELSE           \t"); return ELSE;}
"elseif" 				                      	{printf("ELIF           \t"); return ELIF;}
"while" 				                      	{printf("WHILE          \t"); return WHILE;}
"for" 					                      	{printf("FOR            \t"); return FOR;}
"int" 					                      	{printf("INT            \t"); return INT;}
"double" 				                      	{printf("DOUBLE         \t"); return DOUBLE;}
"read" 					                        {printf("READ           \t"); return READ ;}
"write" 					                      {printf("WRITE          \t"); return WRITE ;}
"element" 				                    	{printf("ELEMENT        \t"); return ELEMENT;}
"molecule" 					                    {printf("MOLECULE       \t"); return MOLECULE;}
"string" 					                      {printf("STRING         \t"); return STRING;}
"bool" 					                        {printf("BOOL           \t"); return BOOL;}
"true" 					                        {printf("TRUE           \t"); return TRUE;}
"false"					                        {printf("FALSE          \t"); return FALSE;}
"func" 					                        {printf("FUNC           \t"); return FUNC;}
"ret"  					                        {printf("RET            \t"); return RET;}
"void"                                  {printf("VOID           \t"); return VOID;}

"(" 						                        {printf("LBRAC          \t"); return LBRAC;}
")" 						                        {printf("RBRAC          \t"); return RBRAC;}
"{" 						                        {printf("LPAREN         \t"); return LPAREN;}
"}" 						                        {printf("RPAREN         \t"); return RPAREN;}
"[" 						                        {printf("SQOPEN         \t"); return SQOPEN;}
"]" 						                        {printf("SQCLOSE        \t"); return SQCLOSE;}
"=" 						                        {printf("ASSIGN         \t"); return ASSIGN;}
"," 						                        {printf("COMMA          \t"); return COMMA;}
";" 						                        {printf("SEMICOLON      \t"); return SEMICOLON;}

"<=" 					                         	{printf("LEQ            \t"); return LEQ;}
"<" 					                        	{printf("LTHAN          \t"); return LTHAN;}
">=" 					                        	{printf("GEQ            \t"); return GEQ;}
">" 				                        		{printf("GTHAN          \t"); return GTHAN;}
"=="					                        	{printf("EQUAL          \t"); return EQUAL;}
"!=" 					                        	{printf("NOTEQ          \t"); return NOTEQ;}
"||" 				                         		{printf("LOR            \t"); return LOR;}
"&&" 				                        		{printf("LAND           \t"); return LAND;}
"+" 				                         		{printf("PLUS           \t"); return PLUS;}
"-" 					                        	{printf("MINUS          \t"); return MINUS;}
"*" 					                        	{printf("MULT           \t"); return MULT;}
"/" 						                        {printf("DIV            \t"); return DIV;}
"%" 					                        	{printf("MOD            \t"); return MOD;}

"." 						                        {printf("PEROD          \t"); return PERIOD;}
":" 						                        {printf("COLON          \t"); return COLON;}

"++"|"--"                               {printf("INC_DEC        \t"); return INC_DEC;}

"balance" 					                    {printf("BALANCE        \t"); return BALANCE;}
"molar_mass" 					                  {printf("MOLARMASS      \t"); return MOLARMASS;}
"chem_name" 					                  {printf("CHEMNAME       \t"); return CHEMNAME;}
"name" 					                        {printf("NAME           \t"); return NAME;}
"charge" 					                      {printf("CHARGE         \t"); return CHARGE;}
"emp_for" 					                    {printf("EMP            \t"); return EMP;}

"atm_wt"                                {printf("ATOMIC_WT      \t"); return ATOMIC_WT;}
"atm_no"                                {printf("ATOMIC_NO      \t"); return ATOMIC_NO;}


[A-Z][a-z]?                             {printf("ELEMENT_NAME   \t"); return ELEMENT_NAME;}
([A-Z][a-z]?[1-9]*)+                    {printf("MOL_NAME       \t"); return MOL_NAME;}
[a-z][a-zA-Z0-9_]* 				              {printf("IDENTIFIER     \t"); return IDENTIFIER;}
[0-9]+ 					                        {printf("INTEGER        \t"); return INTEGER;}
[0-9]+[.][0-9]* 				                {printf("DECIMAL        \t"); return DECIMAL;}

"##".* 					                        { }     //single line comments

"#*" 						                        {printf("MULTI LINE COMMENTS START\n"); multiple_line_comments();}

\"([^"\\&&^\n]|\\['"?\\abfnrtv])*\n 		{yytext[yyleng-1]='\0';printf("ERROR: Unterminated string at line number %d\t",line_cnt+1);return ERROR;}

\"([^"\\&&^\n]|\\['"?\\abfnrtv])*\" 		{printf("STRING_CONST\t"); return STRING_CONST;}


[ \t]  					                        {} /*blank,tab,whitespace ;; eat up the whitespace*/
[\n] 						                        {line_cnt++ ;printf("\n");}
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
          printf("MULTILINE COMMENTS END\n");
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
