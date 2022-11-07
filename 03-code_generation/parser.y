%{
	#include "semantics.c"
	#include "symtab.c"
	#include "ast.h"
	#include "ast.c"
	#include "code_generation.c"
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	extern FILE *yyin;
	extern FILE *yyout;
	extern int line_cnt;
	extern int yylex();
	void yyerror();

	// for declarations
	void add_to_names(list_t *entry);
	list_t **names;
	int nc = 0;

	// for else ifs
	void add_elseif(AST_Node *elsif);
	AST_Node **elsifs;
	int elseif_count = 0;

	// for functions
	AST_Node_Func_Decl *temp_function;
%}

/* YYSTYPE union */
%union{
	// different types of values
	Value val;

	// structures
	list_t* symtab_item;
	struct AST_Node* node;

	// for declarations
	int data_type;
	int const_type;


	// for parameters
	Param par;
}

%token <val>SEMICOLON COLON PERIOD LPAREN RPAREN LBRAC RBRAC SQOPEN SQCLOSE COMMA 
%token <val>PLUS MINUS MULT DIVS MOD ERROR
%token <val> IF ELIF ELSE FOR WHILE RET
%token <val> LAND LOR EQU NOTEQ LTHAN LEQ GTHAN GEQ ASSIGN
%token <val> INT VOID DOUBLE STRING BOOL ELEMENT MOLECULE 
%token <val> ATOMIC_WT ATOMIC_NO CHARG NAME CHEMNAME BALANCE EMP
%token <val> TRUE FALSE FUNC INCR DECR
%token <val> STRING_CONST 
%token <symtab_item> IDENTIFIER ELEMENT_NAME MOL_NAME
%token <val> INTEGER
%token <val> DECIMAL


/* precedencies and associativities */
%left COMMA
%right ASSIGN
%left LOR 
%left LAND
%left EQU NOTEQ
%left LTHAN LEQ GTHAN GEQ
%left PLUS MINUS
%left MULT DIVS MOD
%right NOTOP INCR DECR 
%left LBRAC RBRAC SQOPEN SQCLOSE


/* rule (non-terminal) definitions */
%type <node> program
%type <node> declarations declaration
%type <data_type> type ATTRIBUTE
%type <symtab_item> variable
%type <symtab_item> init var_init ELEMENT_DECLARATOR MOL_DECLARATOR 
%type <node> constant ELEMENT_EXPR NUMBER CHARGE MOL_EXPR MOL_DECL_LIST  
%type <node> expression var_ref
%type <node> statement assigment
%type <node> statements tail
%type <node> if_statement else_if optional_else
%type <node> for_statement while_statement
%type <node> functions_optional functions function
%type <node> parameters_optional parameters
%type <par>  parameter
%type <node> return_type
%type <node> function_call call_params call_param MOL_LIST 


%start program

%%


program:  declarations {  main_decl_tree = $1; ast_traversal($1); }
          statements   {  main_func_tree = $3; ast_traversal($3); }
         
	  RET SEMICOLON functions_optional { ast_traversal($7);}
;


/* declarations */
declarations:
	declarations declaration
	{
		AST_Node_Declarations *temp = (AST_Node_Declarations*) $1;
		$$ = new_declarations_node(temp->declarations, temp->declaration_count, $2);
	}
	| declaration
	{
		$$ = new_declarations_node(NULL, 0, $1);
	}
;



declaration: type { declare = 1; } names { declare = 0; } SEMICOLON
	{
		int i;
		$$ = new_ast_decl_node($1, names, nc);
		nc = 0;

		AST_Node_Decl *temp = (AST_Node_Decl*) $$;

		// declare types of the names
		for(i=0; i < temp->names_count; i++){
			// variable
			if(temp->names[i]->st_type == UNDEF){
				set_type(temp->names[i]->st_name, temp->data_type, UNDEF);
			}
		
		}
	}
	| ELEMENT { declare = 1;}  ELEMENT_DECL_LIST { declare = 0;} SEMICOLON
	{
		int i;
		$$ = new_ast_decl_node(ELEMENT_TYPE, names, nc);
		nc = 0;
		
		AST_Node_Decl *temp = (AST_Node_Decl*) $$;
		// declare types of the names
		for(i=0; i < temp->names_count; i++){
			// variable
			if(temp->names[i]->st_type == UNDEF){
				set_type(temp->names[i]->st_name, temp->data_type, UNDEF);
			}
			
		}
	
	}
	| MOLECULE MOL_DECL_LIST SEMICOLON
	 {
		int i;
		$$ = new_ast_decl_node(MOLECULE_TYPE, names, nc);
		nc = 0;
		
		AST_Node_Decl *temp = (AST_Node_Decl*) $$;
		
		// declare types of the names
		for(i=0; i < temp->names_count; i++){
			// variable
			if(temp->names[i]->st_type == UNDEF){
				set_type(temp->names[i]->st_name, temp->data_type, UNDEF);
				
			}
			
		}
		
	}
;



names: names COMMA variable
	{
		add_to_names($3);
	}
	| names COMMA init
	{
		add_to_names($3);
	}
	| variable
	{
		add_to_names($1);
	}
	| init
	{
		add_to_names($1);
	}
;


ELEMENT_DECL_LIST       :   ELEMENT_DECLARATOR
			    {add_to_names($1);}
                            
                        |   ELEMENT_DECL_LIST COMMA ELEMENT_DECLARATOR  {add_to_names($3);};
                        




MOL_DECL_LIST           :  { declare = 1;} MOL_DECLARATOR 
                            {add_to_names($2);}
                        |  MOL_DECL_LIST  COMMA  { declare = 1;} MOL_DECLARATOR {add_to_names($4);}; 
                      
                            


init:
	var_init { $$ = $1; }
	
;

var_init : IDENTIFIER ASSIGN constant
{
	AST_Node_Const *temp = (AST_Node_Const*) $3;
	$1->val = temp->val;
	$1->st_type = temp->const_type;
	$$ = $1;
}
;

constant:
	INTEGER   { $$ = new_ast_const_node(INT_TYPE, $1);  }
	| DECIMAL { $$ = new_ast_const_node(REAL_TYPE, $1); }
	| STRING_CONST { $$ = new_ast_const_node(STR_TYPE, $1); }
	
;
                 

ELEMENT_DECLARATOR      :   ELEMENT_NAME ASSIGN LBRAC ELEMENT_EXPR RBRAC
                            {
                            AST_Node_Element *temp = (AST_Node_Element*) $4;
                            
                            $1->no=temp->atm_no;
                            $1->wt=temp->atm_wt;
                            $1->charge=temp->charge;
                            $1->st_type=temp->const_type;
                            
			       $$ = $1;
			       
			      
			    };
			                              


ELEMENT_EXPR            :   NUMBER COMMA NUMBER COMMA PLUS CHARGE
                            {
                               $$ = new_ast_element_node(ELEMENT_TYPE, $1,$3,ADD,$6);  }
                             
                          |    NUMBER COMMA NUMBER COMMA MINUS CHARGE
                            {
                               
                               $$ = new_ast_element_node(ELEMENT_TYPE, $1,$3,SUB,$6);  
                              }
                        |   NUMBER COMMA NUMBER COMMA CHARGE
                            { $$ = new_ast_element_node(ELEMENT_TYPE, $1,$3,ADD,$5);  };

NUMBER                  :   INTEGER { $$ = new_ast_const_node(INT_TYPE, $1);  }
                        |   DECIMAL  { $$ = new_ast_const_node(REAL_TYPE, $1);  };            
                        
CHARGE                 :    DECIMAL  { $$ = new_ast_num_node(REAL_TYPE, $1.fval);  };                    
                        
MOL_DECLARATOR          :    MOL_NAME  { declare = 0;} ASSIGN SQOPEN MOL_EXPR SQCLOSE
                            {
                                AST_Node_Molecule *temp = (AST_Node_Molecule*) $5;
                                $1->st_type=temp->const_type;
                                $1->ele_name=(char**)malloc(sizeof(char*)*temp->declaration_count);
                                $1->num=(int*)malloc(sizeof(int)*temp->declaration_count);
                                $1->No=temp->declaration_count;
                                for(int i=0; i < temp->declaration_count; i++){
			// variable
			      $1->ele_name[i]=temp->declarations[i]->ele_name;
			      $1->num[i]=temp->declarations[i]->num;
			
		}
                                
                                $$=$1;
                             
                                
			     };

			     
MOL_EXPR                :   ELEMENT_NAME COLON INTEGER
                           {  //add_to_names($1);
                           
                             $$=new_ast_molecule_node(MOLECULE_TYPE,NULL,0,$1->st_name,$3.ival);
                           
                           }
                         |   MOL_EXPR COMMA ELEMENT_NAME COLON INTEGER
                            {
                               //add_to_names($3);
                               
                               AST_Node_Molecule*temp=(AST_Node_Molecule*)$1;
                               
                               $$= new_ast_molecule_node(MOLECULE_TYPE,temp->declarations, temp->declaration_count,$3->st_name,$5.ival);
                            };           
   

       

                        
type: INT  		{ $$ = INT_TYPE;   }
	| DOUBLE 	{ $$ = REAL_TYPE;  }
	| VOID 		{ $$ = VOID_TYPE;  }
	| STRING        {$$ = STR_TYPE;}
;


variable: IDENTIFIER { $$ = $1; }
	
;

/* statements */
statements:
	statements statement
	{
		AST_Node_Statements *temp = (AST_Node_Statements*) $1;
		$$ = new_statements_node(temp->statements, temp->statement_count, $2);
	}
	| statement
	{
		$$ = new_statements_node(NULL, 0, $1);
	}
;

statement:
	if_statement
	{
		$$ = $1; /* just pass information */
	}
	| for_statement
	{
		$$ = $1; /* just pass information */
	}
	| while_statement
	{
		$$ = $1; /* just pass information */
	}
	| assigment SEMICOLON
	{
		$$ = $1; /* just pass information */
	}
	| function_call SEMICOLON
	{
		$$ = $1; /* just pass information */
	}
	| IDENTIFIER INCR SEMICOLON
	{
		/* increment */
		if($2.ival == INC){
			$$ = new_ast_incr_node($1, 0, 0);
		}
		else{
			$$ = new_ast_incr_node($1, 1, 0);
		}
		
	}
	
	| INCR IDENTIFIER SEMICOLON
	{
		/* increment */
		if($1.ival == INC){
			$$ = new_ast_incr_node($2, 0, 1);
		}
		else{
			$$ = new_ast_incr_node($2, 1, 1);
		}
		
	}
;

if_statement:
	IF LBRAC expression RBRAC tail else_if optional_else
	{
		$$ = new_ast_if_node($3, $5, elsifs, elseif_count, $7);
		elseif_count = 0;
		elsifs = NULL;
	}
	| IF LBRAC expression RBRAC tail optional_else
	{
		$$ = new_ast_if_node($3, $5, NULL, 0, $6);
	}
;

else_if:
	else_if ELSE IF LBRAC expression RBRAC tail
	{
		AST_Node *temp = new_ast_elsif_node($5, $7);
		add_elseif(temp);
	}
	| ELSE IF LBRAC expression RBRAC tail
	{
		AST_Node *temp = new_ast_elsif_node($4, $6);
		add_elseif(temp);
	}
;

optional_else:
	ELSE tail
	{
		/* else exists */
		$$ = $2;
	}
	| /* empty */
	{
		/* no else */
		$$ = NULL;
	}
;

for_statement: FOR LBRAC assigment SEMICOLON expression SEMICOLON IDENTIFIER INCR RBRAC tail
{
	/* create increment node*/
	AST_Node *incr_node;
	if($8.ival == INC){ /* increment */
		incr_node = new_ast_incr_node($7, 0, 0);
	}
	else{
		incr_node = new_ast_incr_node($7, 1, 0);
	}

	$$ = new_ast_for_node($3, $5, incr_node, $10);
	set_loop_counter($$);
}
;

while_statement: WHILE LBRAC expression RBRAC tail
{
	$$ = new_ast_while_node($3, $5);
}
;

tail: LPAREN statements RPAREN
{
	$$ = $2; /* just pass information */
}
;

expression:
    expression PLUS expression
	{
	    $$ = new_ast_arithm_node(ADD, $1, $3);
	}
	| expression MINUS expression
	{
	    $$ = new_ast_arithm_node(SUB, $1, $3);
	}
	| expression MULT expression
	{
	    $$ = new_ast_arithm_node(MUL, $1, $3);
	}
	| expression DIVS expression
	{
		$$ = new_ast_arithm_node(DIV, $1, $3);
	}
	| IDENTIFIER INCR
	{
		/* increment */
		if($2.ival == INC){
			$$ = new_ast_incr_node($1, 0, 0);
		}
		else{
			$$ = new_ast_incr_node($1, 1, 0);
		}
		
	}
	| INCR IDENTIFIER
	{
		/* increment */
		if($1.ival == INC){
			$$ = new_ast_incr_node($2, 0, 1);
		}
		else{
			$$ = new_ast_incr_node($2, 1, 1);
		}
		
		
	}
	| ELEMENT_NAME PERIOD ATTRIBUTE
	{
		$$ = new_ast_attribute_node($1,$3);
		
	}
	| expression LOR expression
	{
		$$ = new_ast_bool_node(OR, $1, $3);
	}
	| expression LAND expression
	{
		$$ = new_ast_bool_node(AND, $1, $3);
	}
	| NOTOP expression
	{
	    $$ = new_ast_bool_node(NOT, $2, NULL);
	}
	| expression EQU expression
	{
		$$ = new_ast_equ_node(EQUAL, $1, $3);
	}
	| expression NOTEQ expression
	{
		$$ = new_ast_equ_node(NOT_EQUAL, $1, $3);
	}
	| expression LEQ expression
	{
		$$ = new_ast_rel_node(LESS_EQUAL, $1, $3);
	}
	| expression LTHAN expression
	{
		$$ = new_ast_rel_node(LESS, $1, $3);
	}
	| expression GEQ expression
	{
		$$ = new_ast_rel_node(GREATER_EQUAL, $1, $3);
	}
	| expression GTHAN expression
	{
		$$ = new_ast_rel_node(GREATER, $1, $3);
	}
	| LBRAC expression RBRAC
	{
		$$ = $2; /* just pass information */
	}
	| var_ref
	{
		$$ = $1; /* just pass information */
	}
	| constant
	{
		$$ = $1; /* no sign */
	}
	| MOL_NAME
	{
		$$ = new_ast_mol_node($1); /* no sign */
	}
	| function_call
	{
		$$ = $1; /* just pass information */
	}
;


ATTRIBUTE : ATOMIC_NO {$$=0;}
   	  | ATOMIC_WT {$$=1;}
   	  | CHARG{$$=2;};
assigment: var_ref ASSIGN expression
{
	AST_Node_Ref *temp = (AST_Node_Ref*) $1;
	$$ = new_ast_assign_node(temp->entry, temp->ref, $3);

	/* find datatypes */
	int type1 = get_type(temp->entry->st_name);
	int type2 = expression_data_type($3);

	/* the last function will give us information about revisits */

	/* contains revisit => add assignment-check to revisit queue */
	if(cont_revisit == 1){
		/* search if entry exists */
		revisit_queue *q = search_queue(temp->entry->st_name);
		if(q == NULL){
			add_to_queue(temp->entry, temp->entry->st_name, ASSIGN_CHECK);
			q = search_queue(temp->entry->st_name);
		}

		/* setup structures */
		if(q->num_of_assigns == 0){ /* first node */
			q->nodes = (void**) malloc(sizeof(void*));
		}
		else{ /* general case */
			q->nodes = (void**) realloc(q->nodes, (q->num_of_assigns + 1) * sizeof(void*));
		}

		/* add info of assignment */
		q->nodes[q->num_of_assigns] = (void*) $3;

		/* increment number of assignments */
		q->num_of_assigns++;

		/* reset revisit flag */
		cont_revisit = 0;

		printf("Assignment revisit for %s at line %d\n", temp->entry->st_name, line_cnt);
	}
	else{ /* no revisit */
		/* check assignment semantics */
		get_result_type(
			type1,       /*  variable datatype  */
			type2,       /* expression datatype */
			NONE  /* checking compatibility only (no operator) */
		);
	}
}
;

var_ref: variable
	{
		$$ = new_ast_ref_node($1, 0); /* no reference */
	}
	
;

function_call: IDENTIFIER LBRAC call_params RBRAC
{
	AST_Node_Call_Params *temp = (AST_Node_Call_Params*) $3;
	$$ = new_ast_func_call_node($1, temp->params, temp->num_of_pars);

	/* add information to revisit queue entry (if one exists) */
	revisit_queue *q = search_queue($1->st_name);
	if(q != NULL){
		/* setup structures */
		if(q->num_of_calls == 0){ /* first call */
			q->par_types = (int**) malloc(sizeof(int*));
			q->num_of_pars = (int*) malloc(sizeof(int));
		}
		else{ /* general case */
			q->par_types = (int**) realloc(q->par_types, (q->num_of_calls + 1) * sizeof(int*));
			q->num_of_pars = (int*) realloc(q->num_of_pars, (q->num_of_calls + 1) * sizeof(int));
		}

		/* add info of function call */
		q->num_of_pars[q->num_of_calls] = temp->num_of_pars;
		q->par_types[q->num_of_calls] = (int*) malloc(temp->num_of_pars * sizeof(int));
		/* get the types of the parameters */
		int i;
		for(i = 0; i < temp->num_of_pars; i++){
			/* get datatype of parameter-expression */
			q->par_types[q->num_of_calls][i] = expression_data_type(temp->params[i]);
		}

		/* increment number of calls */
		q->num_of_calls++;
	}
	else{
		/* function declared before call */
		if($1->st_type == FUNCTION_TYPE){
			/* check number of parameters */
			if($1->num_of_pars != temp->num_of_pars){
				fprintf(stderr, "Function call of %s has wrong num of parameters!\n", $1->st_name);
				exit(1);
			}
			/* check if parameters are compatible */
			int i;
			for(i = 0; i < temp->num_of_pars; i++){
				/* type of parameter in function declaration */
				int type_1 = expression_data_type(temp->params[i]);

				/* type of parameter in function call*/
				int type_2 = $1->parameters[i].par_type;

				/* check compatibility for function call */
				get_result_type(type_1, type_2, NONE);
				/* error occurs automatically in the function */
			}
		}
	}
}
|   NAME LBRAC ELEMENT_NAME RBRAC
{
	AST_Node *temp = new_ast_const_element_node(ELEMENT_TYPE, $3);
	
			

AST_Node_Call_Params *temp1 = (AST_Node_Call_Params *)new_ast_call_params_node(NULL, 0, temp);
	$$ = new_ast_inbuilt_func_call_node(0, temp1->params, temp1->num_of_pars);
	
}

| EMP LBRAC MOL_NAME RBRAC
{
	AST_Node *temp = new_ast_const_element_node(MOLECULE_TYPE, $3);
	AST_Node_Call_Params *temp1 = (AST_Node_Call_Params *)new_ast_call_params_node(NULL, 0, temp);
	$$ = new_ast_inbuilt_func_call_node(2, temp1->params, temp1->num_of_pars);
}
| BALANCE LBRAC MOL_LIST MINUS MOL_LIST RBRAC
{
        AST_Node_Call_Params *temp1 = (AST_Node_Call_Params*) $3;
        AST_Node_Call_Params *temp2 = (AST_Node_Call_Params*) $5;
        
        
	$$ = new_ast_balance_func_call_node(3, temp1->params, temp1->num_of_pars,temp2->params, temp2->num_of_pars);
}

;

call_params:
	call_param
	{
		$$ = $1;
	}
	| STRING
	{
		AST_Node *temp = new_ast_const_node(STR_TYPE, $1);
		$$ = new_ast_call_params_node(NULL, 0, temp);
	}
	| /* empty */
	{
		AST_Node_Call_Params *temp = malloc (sizeof (AST_Node_Call_Params));
		temp->type = CALL_PARAMS;
		temp->params = NULL;
		temp->num_of_pars = 0;
		$$ = (AST_Node*)temp;
	}
;

call_param:
	call_param COMMA expression
	{
		AST_Node_Call_Params *temp = (AST_Node_Call_Params*) $1;
		$$ = new_ast_call_params_node(temp->params, temp->num_of_pars, $3);
	}
	| expression
	{
		$$ = new_ast_call_params_node(NULL, 0, $1);
	}
;

MOL_LIST                :   MOL_NAME
			    {
			    AST_Node *temp = new_ast_const_element_node(MOLECULE_TYPE, $1);
			    $$=new_ast_call_params_node(NULL, 0, temp);
	                   }
                        |   MOL_LIST COMMA MOL_NAME
                           { AST_Node_Call_Params *temp = (AST_Node_Call_Params*) $1;
                             AST_Node *temp1 = new_ast_const_element_node(MOLECULE_TYPE, $3);
		$$ = new_ast_call_params_node(temp->params, temp->num_of_pars, temp1);};  
       

/* functions */
functions_optional:
	functions
	{
		$$ = $1;
	}
	| /* empty */
	{
		$$ = NULL;
	}
;

functions:
	functions function
	{
		AST_Node_Func_Declarations *temp = (AST_Node_Func_Declarations*) $1;
		$$ = new_func_declarations_node(temp->func_declarations, temp->func_declaration_count, $2);
	}
	| function
	{
		$$ = new_func_declarations_node(NULL, 0, $1);
	}
;

function: FUNC{ incr_scope(); } function_head function_tail
{
	/* perform revisit */
	revisit(temp_function->entry->st_name);

	hide_scope();
	$$ = (AST_Node *) temp_function;
}
;

function_head: { function_decl = 1; } return_type IDENTIFIER LBRAC
	{
		function_decl = 0;

		AST_Node_Ret_Type *temp = (AST_Node_Ret_Type *) $2;
		temp_function = (AST_Node_Func_Decl *) new_ast_func_decl_node(temp->ret_type, $3);
		temp_function->entry->st_type = FUNCTION_TYPE;
		temp_function->entry->inf_type = temp->ret_type;
	}
	parameters_optional RBRAC
	{
		if($6 != NULL){
			AST_Node_Decl_Params *temp = (AST_Node_Decl_Params *) $6;

			temp_function->entry->parameters = temp->parameters;
			temp_function->entry->num_of_pars = temp->num_of_pars;
		}
		else{
			temp_function->entry->parameters = NULL;
			temp_function->entry->num_of_pars = 0;
		}
	}
;

return_type:
	type
	{
		$$ = new_ast_ret_type_node($1);
	}
	
;

parameters_optional:
	parameters
	{
		$$ = $1;
	}
	| /* empty */
	{
		$$ = NULL;
	}
;

parameters:
	parameters COMMA parameter
	{
		AST_Node_Decl_Params *temp = (AST_Node_Decl_Params *) $1;
		$$ = new_ast_decl_params_node(temp->parameters, temp->num_of_pars, $3);
	}
	| parameter
	{
		$$ = new_ast_decl_params_node(NULL, 0, $1);
	}
;

parameter : { declare = 1; } type variable
{
	declare = 0;

	// set type of symbol table entry
	if($3->st_type == UNDEF){ /* "simple" type */
		set_type($3->st_name, $2, UNDEF);
	}

	/* define parameter */
	$$ = def_param($2, $3->st_name, 0);
}
;

function_tail: LPAREN declarations_optional statements_optional return_optional RPAREN ;

declarations_optional:
	declarations
	{
		temp_function->declarations = $1;
	}
	| /* empty */
	{
		temp_function->declarations = NULL;
	}
;

statements_optional:
	statements
	{
		temp_function->statements = $1;
	}
	| /* empty */
	{
		temp_function->statements = NULL;
	}
;

return_optional:
	RET expression SEMICOLON
	{
		temp_function->return_node = new_ast_return_node(temp_function->ret_type, $2);
	}
	| /* empty */
	{
		temp_function->return_node = NULL;
	}
;

%%

void yyerror ()
{
  fprintf(stderr, "Syntax error at line %d\n", line_cnt);
  exit(1);
}

void add_to_names(list_t *entry){
	// first entry
	if(nc == 0){
		nc = 1;
		names = (list_t **) malloc( 1 * sizeof(list_t *));
		names[0] = entry;
	}
	// general case
	else{
		nc++;
		names = (list_t **) realloc(names, nc * sizeof(list_t *));
		names[nc - 1] = entry;
	}
}

void add_elseif(AST_Node *elsif){
	// first entry
	if(elseif_count == 0){
		elseif_count = 1;
		elsifs = (AST_Node **) malloc(1 * sizeof(AST_Node));
		elsifs[0] = elsif;
	}
	// general case
	else{
		elseif_count++;
		elsifs = (AST_Node **) realloc(elsifs, elseif_count * sizeof(AST_Node));
		elsifs[elseif_count - 1] = elsif;
	}
}

int main (int argc, char *argv[]){

	// initialize symbol table
	init_hash_table();

	// initialize revisit queue
	queue = NULL;

	// parsing
	int flag;
	yyin = fopen(argv[1], "r");
	flag = yyparse();
	fclose(yyin);

	printf("Parsing finished!\n");
	

	// remove print from revisit queue 
	revisit_queue *q = search_prev_queue("print");
	if(q == NULL){  //special case: first entry
		if(queue != NULL){ // check if queue not empty
			queue = queue->next;
		}
	}
	else{
		q->next = q->next->next;
	}
	
	// remove print from revisit queue 
	q = search_prev_queue("mol_mass");
	if(q == NULL){  //special case: first entry
		if(queue != NULL){ // check if queue not empty
			queue = queue->next;
		}
	}
	else{
		q->next = q->next->next;
	}
	

	// perform the remaining checks (ASSIGNMENTS)
	if(queue != NULL){
		revisit_queue *cur;
		cur = queue;
		while(cur != NULL){
			if(cur->revisit_type == ASSIGN_CHECK){
				revisit(cur->st_name);
			}
			cur = cur->next;
		}
	}
	
	// perform the remaining checks (undeclared)
	if(queue != NULL){
		revisit_queue *cur;
		cur = queue;
		while(cur != NULL){
			if(cur->revisit_type == PARAM_CHECK){
				printf("ERROR: %s is undeclared variable\n",cur->st_name);
				exit(1);
			}
			cur = cur->next;
		}
		
	}
	
 
	/* if still not empty => warning */
	if(queue != NULL){
		printf("Warning! Something in the revisit queue has not been checked yet!\n");
	}

	/* declare function type of "print" */
	func_declare("print", VOID_TYPE, 1, NULL);
	func_declare("mol_mass", REAL_TYPE, 1, NULL);
	
	//func_declare("name", STR_TYPE, 1, NULL);

	// symbol table dump
	yyout = fopen("symtab_dump.out", "w");
	symtab_dump(yyout);
	fclose(yyout);

	// revisit queue dump
	yyout = fopen("revisit_dump.out", "w");
	revisit_dump(yyout);
	fclose(yyout);
	
	// code generation
	printf("\nGenerating code...\n");
	generate_code();

	return flag;
}
