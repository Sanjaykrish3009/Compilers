
/* ---------------------NODE TYPES-------------------------- */

typedef enum Node_Type {
	BASIC_NODE,   // 0-no special usage (for roots only)
	DECLARATIONS, // 1-declarations
	DECL_NODE,    // 2-declaration
	CONST_NODE,   // 3-constant
	NUM_NODE,     // 4
	ELEMENT_NODE, // 5
	MOLECULE_NODE,// 6
	ATTR_NODE,    // 7
	ELE_NODE,     // 8
	STATEMENTS,   // 9 statements
	IF_NODE,      // 10 if statement
	ELSIF_NODE,   // 11 else if branch
	FOR_NODE,     // 12for statement
	WHILE_NODE,   // 13 while statement
	ASSIGN_NODE,  // 14 assigment
	SIMPLE_NODE,  // 15 continue or break statement
	INCR_NODE,    // 16 increment statement (non-expression one)
	FUNC_CALL,    // 17 function call
	INBUILT_FUNC, // 18
	BALANCE_FUNC, // 19
	CALL_PARAMS,  // 20 function call parameters
	ARITHM_NODE,  // 21 arithmetic expression
	BOOL_NODE,    // 22 boolean expression
	REL_NODE,     // 23 relational expression
	EQU_NODE,     // 24 equality expression
	REF_NODE,     // 25 identifier in expression
	FUNC_DECLS,   // 26 function declarations
	FUNC_DECL,    // 27 function declaration
	RET_TYPE,     // 28 function return type
	DECL_PARAMS,  // 29 function parameters
	RETURN_NODE,  // 30 return statement of functions
	MOL_NODE,
}Node_Type;

/* --------------------OPERATOR TYPES----------------------- */

typedef enum Arithm_op{
	ADD,  // + operator
	SUB,  // - operator
	MUL,  // * operator
	DIV , // / operator
	INC, // ++ operator
	DEC, // -- operator
}Arithm_op;

typedef enum Bool_op{
	OR,  // || operator
	AND, // && operator
	NOT  // ! operator
}Bool_op;

typedef enum Rel_op{
	GREATER,        // > operator
	LESS,           // < operator
	GREATER_EQUAL,  // >= operator
	LESS_EQUAL     // <= operator
}Rel_op;

typedef enum Equ_op{
	EQUAL,    // == operator
	NOT_EQUAL // != operator
}Equ_op;

/* -----------------------AST NODES------------------------- */

/* The basic node */
typedef struct AST_Node{
	enum Node_Type type; // node type
	
	struct AST_Node *left;  // left child
	struct AST_Node *right; // right child
	
	char* ele_name;
	int num;
}AST_Node;

/* Declarations */
typedef struct AST_Node_Declarations{
	enum Node_Type type; // node type
	
	// declarations
	struct AST_Node **declarations;
	int declaration_count;
}AST_Node_Declarations;


typedef struct AST_Node_Decl{
	enum Node_Type type; // node type
	
	// data type
	int data_type;
	
	// symbol table entries of the variables
	list_t **names;
	int names_count;
}AST_Node_Decl;

typedef struct AST_Node_Const{
	enum Node_Type type; // node type
	
	// data type
	int const_type;
	
	// constant value
	Value val;
}AST_Node_Const;

typedef struct AST_Node_Const_Element{
	enum Node_Type type; // node type
	
	// symbol table entry
	list_t *entry;
	
	
	int const_type;
}AST_Node_Const_Element;

typedef struct AST_Node_Num{
	enum Node_Type type; // node type
	
	// data type
	int const_type;
	
	// constant value
	float val;
}AST_Node_Num;


typedef struct AST_Node_Element{
	enum Node_Type type; // node type
	
	// data type
	
	int const_type;
	
	
	Value atm_no;
	Value atm_wt;
	float charge;
	
}AST_Node_Element;

typedef struct AST_Node_Molecule{
	enum Node_Type type; // node type
	
	// data type
	
	int const_type;
	struct AST_Node **declarations;
	int declaration_count;
	
	
}AST_Node_Molecule;


/* Statements */
typedef struct AST_Node_Statements{
	enum Node_Type type; // node type
	
	// statements
	struct AST_Node **statements;
	int statement_count;
}AST_Node_Statements;

typedef struct AST_Node_If{
	enum Node_Type type; // node type
	
	// condition
	struct AST_Node *condition;
	
	// if branch
	struct AST_Node *if_branch;
	
	// else if branches
	struct AST_Node **elsif_branches;
	int elseif_count;
	
	// else branch
	struct AST_Node *else_branch;
}AST_Node_If;

typedef struct AST_Node_Elsif{
	enum Node_Type type; // node type
	
	// condition
	struct AST_Node *condition;
	
	// branch
	struct AST_Node *elsif_branch;
}AST_Node_Elsif;

typedef struct AST_Node_For{
	enum Node_Type type; // node type
	
	// initialization
	struct AST_Node *initialize;
	
	// condition
	struct AST_Node *condition;
	
	// incrementation
	struct AST_Node *increment;
	
	// branch
	struct AST_Node *for_branch;
	
	// loop counter
	list_t *counter;
}AST_Node_For;

typedef struct AST_Node_While{
	enum Node_Type type; // node type
	
	// condition
	struct AST_Node *condition;
	
	// branch
	struct AST_Node *while_branch;
}AST_Node_While;

typedef struct AST_Node_Assign{
	enum Node_Type type; // node type
	
	// symbol table entry
	list_t *entry;
	
	// reference or not
	int ref; // 0: not reference, 1: reference
	
	// assignment value
	struct AST_Node *assign_val;
}AST_Node_Assign;

typedef struct AST_Node_Simple{
	enum Node_Type type; // node type
	
	// continue: '0', break: '1'
	int statement_type;
}AST_Node_Simple;

typedef struct AST_Node_Incr{
	enum Node_Type type; // node type
	
	// identifier
	list_t *entry;
	
	// increment or decrement
	int incr_type; // 0: increment, 1: decrement
	
	// post- or prefix
	int pf_type; // 0: postfix, 1: prefix
}AST_Node_Incr;

typedef struct AST_Node_Attr{
	enum Node_Type type; // node type
	
	// identifier
	list_t *entry;
	
	
	int attr_type; // 0: atm_no, 1: atm_wt, 2:charge
	
	/* register assignment stuff */
	int g_index;
	
	
}AST_Node_Attr;


typedef struct AST_Node_Func_Call{
	enum Node_Type type; // node type
	
	// function identifier
	list_t *entry;
	
	/* register assignment stuff */
	int g_index;
	
	
	// call parameters
	AST_Node **params;
	int num_of_pars;
}AST_Node_Func_Call;

typedef struct AST_Node_Inbuilt_Func_Call{
	enum Node_Type type; // node type
	
	// function identifier
	int func;
	
	/* register assignment stuff */
	int g_index;
	
	
	// call parameters
	AST_Node **params;
	int num_of_pars;
}AST_Node_Inbuilt_Func_Call;

typedef struct AST_Node_Balance_Func_Call{
	enum Node_Type type; // node type
	
	AST_Node **left_params;
	AST_Node **right_params;
	// function identifier
	int func;
	
	// call parameters
	
	int left_num_of_pars;
	
	int right_num_of_pars;
}AST_Node_Balance_Func_Call;

typedef struct AST_Node_Call_Params{
	enum Node_Type type; // node type
	
	// call parameters
	AST_Node **params;
	int num_of_pars;
}AST_Node_Call_Params;

/* Expressions */
typedef struct AST_Node_Arithm{
	enum Node_Type type; // node type
	
	
	struct AST_Node *left;  // left child
	struct AST_Node *right; // right child
	
	// data type of result
	int data_type;
	
	// operator
	enum Arithm_op op;
	
	/* register assignment stuff */
	int g_index;
}AST_Node_Arithm;

typedef struct AST_Node_Bool{
	enum Node_Type type; // node type
	
	struct AST_Node *left;  // left child
	struct AST_Node *right; // right child
	
	// data type of result
	int data_type;
	
	// operator
	enum Bool_op op;
	
	
	/* register assignment stuff */
	int g_index;
	
	
}AST_Node_Bool;

typedef struct AST_Node_Rel{
	enum Node_Type type; // node type
	
	
	struct AST_Node *left;  // left child
	struct AST_Node *right; // right child
	
	
	// data type of result
	int data_type;
	
	// operator
	enum Rel_op op;
	
	/* register assignment stuff */
	int g_index;
}AST_Node_Rel;

typedef struct AST_Node_Equ{
	enum Node_Type type; // node type
	
	
	struct AST_Node *left;  // left child
	struct AST_Node *right; // right child
	
	// data type of result
	int data_type;
	
	// operator
	enum Equ_op op;
	
	/* register assignment stuff */
	int g_index;
	
}AST_Node_Equ;

typedef struct AST_Node_Ref{
	enum Node_Type type; // node type
	
	// symbol table entry
	list_t *entry;
	
	// reference or not
	int ref; // 0: not reference, 1: reference
}AST_Node_Ref;

typedef struct AST_Node_Mol{
	enum Node_Type type; // node type
	
	// symbol table entry
	list_t *entry;
	
}AST_Node_Mol;

/* Functions */
typedef struct AST_Node_Func_Declarations{
	enum Node_Type type; // node type
	
	// declarations
	struct AST_Node **func_declarations;
	int func_declaration_count;
}AST_Node_Func_Declarations;

typedef struct AST_Node_Func_Decl{
	enum Node_Type type; // node type
	
	// return type
	int ret_type;
	
	// symbol table entry
	list_t *entry;
	
	// declarations, statements and return
	struct AST_Node *declarations;
	struct AST_Node *statements;
	struct AST_Node *return_node;
}AST_Node_Func_Decl;

typedef struct AST_Node_Ret_Type{
	enum Node_Type type; // node type
	
	// return type
	int ret_type;
	
}AST_Node_Ret_Type;

typedef struct AST_Node_Decl_Params{
	enum Node_Type type; // node type
	
	// parameters
	Param *parameters;
	int num_of_pars;
}AST_Node_Decl_Params;

typedef struct AST_Node_Return{
	enum Node_Type type; // node type
	
	// return type
	int ret_type;
	
	// return value
	struct AST_Node *ret_val;
}AST_Node_Return;

/* Static AST Structures */
static AST_Node* main_decl_tree; /* main function's declarations AST Tree */
static AST_Node* main_func_tree; /* main function's statements AST Tree */
static AST_Node* opt_func_tree; /* optional functions AST Tree */

/* ------------------AST NODE MANAGEMENT-------------------- */

/* The basic node */
AST_Node *new_ast_node(Node_Type type, AST_Node *left, AST_Node *right); 	 // simple nodes

/* Declarations */
AST_Node *new_declarations_node(AST_Node **declarations, int declaration_count, AST_Node *declaration);
AST_Node *new_ast_decl_node(int data_type, list_t **names, int names_count); // declaration
AST_Node *new_ast_const_node(int const_type, Value val);		
AST_Node *new_ast_num_node(int const_type, float val);			 // constant
AST_Node *new_ast_element_node(int const_type,AST_Node*atm_no,AST_Node*atm_wt,enum Arithm_op op,AST_Node*charge);
AST_Node *new_ast_molecule_node(int const_type,AST_Node **declarations, int declaration_count,char*name,int num);

/* Statements */
AST_Node *new_statements_node(AST_Node **statements, int statement_count, AST_Node *statement);
AST_Node *new_ast_if_node(AST_Node *condition, AST_Node *if_branch, AST_Node **elsif_branches, 
	int elseif_count, AST_Node *else_branch);
AST_Node *new_ast_elsif_node(AST_Node *condition, AST_Node *elsif_branch);
AST_Node *new_ast_for_node(AST_Node *initialize, AST_Node *condition, AST_Node *increment, AST_Node *for_branch);
void set_loop_counter(AST_Node *node);
AST_Node *new_ast_while_node(AST_Node *condition, AST_Node *while_branch);
AST_Node *new_ast_assign_node(list_t *entry, int ref, AST_Node *assign_val);
AST_Node *new_ast_simple_node(int statement_type);							 // continue or break
AST_Node *new_ast_incr_node(list_t *entry, int incr_type, int pf_type);      // increment decrement
AST_Node *new_ast_func_call_node(list_t *entry, AST_Node **params, int num_of_pars); // function call
AST_Node *new_ast_call_params_node(AST_Node **params, int num_of_pars, AST_Node *param);
AST_Node *new_ast_const_element_node(int const_type, list_t*entry);
AST_Node *new_ast_inbuilt_func_call_node(int func,AST_Node **params, int num_of_pars);
AST_Node *new_ast_balance_func_call_node(int func,AST_Node **left_params, int left_num_of_pars,AST_Node **right_params, int right_num_of_pars);

/* Expressions */
AST_Node *new_ast_arithm_node(enum Arithm_op op, AST_Node *left, AST_Node *right);
AST_Node *new_ast_bool_node(enum Bool_op op, AST_Node *left, AST_Node *right);
AST_Node *new_ast_attribute_node(list_t *entry,int type);
AST_Node *new_ast_rel_node(enum Rel_op op, AST_Node *left, AST_Node *right);
AST_Node *new_ast_equ_node(enum Equ_op op, AST_Node *left, AST_Node *right);
AST_Node *new_ast_ref_node(list_t *entry, int ref);
AST_Node *new_ast_mol_node(list_t *entry);
int expression_data_type(AST_Node *node);                                    // returns the data type of an expression
int getGraphIndex(AST_Node *node);

/* Functions */
AST_Node *new_func_declarations_node(AST_Node **func_declarations, int func_declaration_count, AST_Node *func_declaration);
AST_Node *new_ast_func_decl_node(int ret_type, list_t *entry);
AST_Node *new_ast_ret_type_node(int ret_type);                  // function return type
AST_Node *new_ast_decl_params_node(Param *parameters, int num_of_pars, Param param);
AST_Node *new_ast_return_node(int ret_type, AST_Node *ret_val);				 // function return

/* Tree Traversal */
void ast_print_node(AST_Node *node);	// print information of node
void ast_traversal(AST_Node *node);		// tree traversal (for testing right now)
