#include "code_generation.h"
/* main assembly code generation function */
void generate_code(){	
	FILE *fp;
	fp = fopen("out.asm", "w");
	
	generate_data_declarations(fp);
	
	generate_statements(fp);
	
	fclose(fp);
}

/* data declaration assembly code */
void generate_data_declarations(FILE *fp){
	
	fprintf(fp, ".data\n");	
	fprintf(fp, "# variables\n");
	
	fprintf(fp,"temp: .double 0.000000\n");
	int i, j;
	for (i = 0; i < SIZE; i++){ 
		//* if hashtable list not empty 
		if (hash_table[i] != NULL){ 
			list_t *l = hash_table[i];
			// loop through list 
			while (l != NULL){ 
				// Simple Variables 
				if (l->st_type == INT_TYPE){
					fprintf(fp, "%s: .word %d\n", l->st_name, l->val.ival);
				}
				else if (l->st_type == REAL_TYPE){
					fprintf(fp, "%s: .double %f\n", l->st_name, l->val.fval);
				}
				else if (l->st_type == STR_TYPE){
					fprintf(fp, "%s: .asciiz %s\n", l->st_name, l->val.sval);
				}	
				else if (l->st_type == ELEMENT_TYPE){
					fprintf(fp, "%s.no: .word %d\n", l->st_name, l->no.ival);
					fprintf(fp, "%s.wt: .double %f\n", l->st_name, l->wt.fval);
					fprintf(fp, "%s.charge: .double %f\n", l->st_name, l->charge);
				}	
				else if (l->st_type == MOLECULE_TYPE){
				for(j=0;j<l->No;j++)
				{
				    fprintf(fp, "%s.num%d: .word %d\n", l->st_name,(j+1), l->num[j]);
					
				    list_t*element = lookup(l->ele_name[j]);
				    
				   fprintf(fp, "%s.wt%d: .double %f\n", l->st_name,(j+1), element->wt.fval);
				}	
				}
				else if (l->st_type == FUNCTION_TYPE){
					
				}
				l = l->next;
			}
		}
	}
	// loop through the string messages 
	fprintf(fp, "# messages\n");
	for(i = 0; i < num_of_msg; i++){
		fprintf(fp, "msg%d: .asciiz %s\n", (i + 1), str_messages[i]);
	}
}

// statements assembly code 
void generate_statements(FILE *fp){
	int i, j;
	
	// print .text 
	fprintf(fp, "\n.text\n");
	
	/* Main Function Register Allocation */
	initGraph();
	
	main_reg_allocation(main_decl_tree);
	main_reg_allocation(main_func_tree);
	
	/* add edges from all the non-temporary variables */
	for(i = 0; i < var_count - temp_count; i++){
		for(j = 1; j < var_count; j++){
			if(i < j){
				insertEdge(i, j);
				insertEdge(j, i);
			}
		}
	}	
	printVarArray();
	/* print main: */
	fprintf(fp, "main:\n");
	
	temp_count = 0;
	
	/* traverse main function tree */
	main_func_traversal(fp, main_func_tree);
}

/* get register name from color */
char * GetRegisterName(int color, int isFloat){
	char* regName;
	regName = (char*) malloc(5 * sizeof(char));
	if(isFloat == 0){
		switch(color){
		/* callee saved values */
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			sprintf(regName, "$s%d", color);
			break;
		/* caller saved temporaries */
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		
			sprintf(regName, "$t%d", color - 8);
			break;
		default:
			fprintf(stderr, "Too many GP registers!\n");
			exit(1);
		}
	}
	else{
		switch(color){
		/* callee saved values */
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
			sprintf(regName, "$f%d", color*2);
			break;
		default:
			fprintf(stderr, "Too many FP registers!\n");
			exit(1);
		}
	}	
	
	return regName;
}

/* various generation functions */
void generate_arithm(FILE *fp, AST_Node_Arithm *node){
	
	int float_op = 0,const_op = 0,Result = 0,Operand1 =0,Operand2 = 0;
	
	AST_Node_Const *temp_const;
	/* operation */
	switch(node->op){
		case ADD:
			/* check left operand */
			if (expression_data_type(node->left) == REAL_TYPE){
				float_op = 1;
				if(node->left->type == CONST_NODE){
					const_op = 1;
					Operand1 = 3;
				}
				else{
					Operand1 = 1;
				}
			}
			else{
				if(node->left->type == CONST_NODE){
					const_op = 1;
					Operand1 = 2;
				}
				else{
					Operand1 = 0;
				}
			}
			/* check right operand */
			if(expression_data_type(node->right) == REAL_TYPE){
				float_op = 1;
				if(node->right->type == CONST_NODE){
					const_op = 1;
					Operand2 = 3;
				}
				else{
					Operand2 = 1;
				}
			}
			else{
				if(node->right->type == CONST_NODE){
					const_op = 1;
					Operand2 = 2;
				}
				else{
					Operand2 = 0;
				}
			}
			
			/* check result */
			if(node->data_type == REAL_TYPE){
				float_op = 1;
				Result = 1;
			}
			
			if(float_op == 1){
				if(const_op == 1){
					if(Operand1 == 2 || Operand1 == 3){
						temp_const = (AST_Node_Const *) node->left;
					}
					else{
						temp_const = (AST_Node_Const *) node->right;
					}
					
					/* floating-point constant */
					if(temp_const->const_type == REAL_TYPE){
						fprintf(fp, "LI.D $f28, %.2f\n", temp_const->val);
					}
					else{
						fprintf(fp, "LI.D $f28, %d.0\n", temp_const->val);
					}
				}
				
				/* operand needs conversion */
				if(Operand1 == 0){
					fprintf(fp, "MTC1.D %s, $f30\n", GetRegisterName(getGraphIndex(node->left) , 0));
					fprintf(fp, "CVT.D.W $f30, $f30\n");
				}
				else if(Operand2 == 0){
					fprintf(fp, "MTC1.D %s, $f30\n", GetRegisterName(getGraphIndex(node->right) , 0));
					fprintf(fp, "CVT.D.W $f30, $f30\n");
				}
				
				fprintf(fp, "ADD.D ");
				
				/* check if result needs conversion */
				if(Result == 0){
					fprintf(fp, "$f26, ");
				}
				else{
					fprintf(fp, "%s, ", GetRegisterName(node->g_index, 1));
				}
				
				switch(Operand1){
					case 0:
						fprintf(fp, "$f30 ");
						break;
					case 1:
						fprintf(fp, "%s ", GetRegisterName(getGraphIndex(node->left) , 1));
						break;
					case 2:
					case 3:
						fprintf(fp, "$f28 ");
				}
				
				switch(Operand2){
					case 0:
						fprintf(fp, "$f30 ");
						break;
					case 1:
						fprintf(fp, "%s ", GetRegisterName(getGraphIndex(node->right) , 1));
						break;
					case 2:
					case 3:
						fprintf(fp, "$f28 ");
				}
				fprintf(fp, "\n");
				
				/* result needs type-conversion */
				if(Result == 0){
					fprintf(fp, "CVT.W.D $f26, $f26\n");
					fprintf(fp, "MTC1 %s, $f26\n", GetRegisterName(node->g_index, 0));
				}
			}
			else if(const_op == 1){
				if(Operand1 != 0){
					temp_const = (AST_Node_Const *) node->left;
					fprintf(fp, "ADDI %s, %s, %d\n", GetRegisterName(node->g_index, 0), GetRegisterName(getGraphIndex(node->right), 0), temp_const->val);
				}
				if(Operand2 != 0){
					temp_const = (AST_Node_Const *) node->right;
					fprintf(fp, "ADDI %s, %s, %d\n", GetRegisterName(node->g_index, 0), GetRegisterName(getGraphIndex(node->left), 0), temp_const->val);
				}
			}
			else{
				fprintf(fp, "ADD %s, %s, %s\n", GetRegisterName(node->g_index, 0), GetRegisterName(getGraphIndex(node->left), 0), GetRegisterName(getGraphIndex(node->right), 0));
			}
			
			break;		
		case SUB:
		
			/* check left operand */
			if (expression_data_type(node->left) == REAL_TYPE){
				float_op = 1;
				if(node->left->type == CONST_NODE){
					const_op = 1;
					Operand1 = 3;
				}
				else{
					Operand1 = 1;
				}
			}
			else{
				if(node->left->type == CONST_NODE){
					const_op = 1;
					Operand1 = 2;
				}
				else{
					Operand1 = 0;
				}
			}
			
			/* check right operand */
			if(expression_data_type(node->right) == REAL_TYPE){
				float_op = 1;
				if(node->right->type == CONST_NODE){
					const_op = 1;
					Operand2 = 3;
				}
				else{
					Operand2 = 1;
				}
			}
			else{
				if(node->right->type == CONST_NODE){
					const_op = 1;
					Operand2 = 2;
				}
				else{
					Operand2 = 0;
				}
			}
			
			/* check result */
			if(node->data_type == REAL_TYPE){
				float_op = 1;
				Result = 1;
			}
			
			if(float_op == 1){
				if(const_op == 1){
					if(Operand1 == 2 || Operand1 == 3){
						temp_const = (AST_Node_Const *) node->left;
					}
					else{
						temp_const = (AST_Node_Const *) node->right;
					}
					if(temp_const->const_type == REAL_TYPE){
						fprintf(fp, "LI.D $f28, %.2f\n", temp_const->val);
					}
					else{
						fprintf(fp, "LI.D $f28, %d.0\n", temp_const->val);
					}
					
				}
				
				if(Operand1 == 0){
					fprintf(fp, "MTC1.D %s, $f30\n", GetRegisterName(getGraphIndex(node->left) , 0));
					fprintf(fp, "CVT.D.W $f30, $f30\n");
				}
				else if(Operand2 == 0){
					fprintf(fp, "MTC1.D %s, $f30\n", GetRegisterName(getGraphIndex(node->right) , 0));
					fprintf(fp, "CVT.D.W $f30, $f30\n");
				}
				
				fprintf(fp, "SUB.D ");
				
				if(Result == 0){
					fprintf(fp, "$f26, ");
				}
				else{
					fprintf(fp, "%s, ", GetRegisterName(node->g_index, 1));
				}
				
				switch(Operand1){
					case 0:
						fprintf(fp, "$f30 ");
						break;
					case 1:
						fprintf(fp, "%s ", GetRegisterName(getGraphIndex(node->left) , 1));
						break;
					case 2:
					case 3:
						fprintf(fp, "$f28 ");
				}
				
				switch(Operand2){
					case 0:
						fprintf(fp, "$f30 ");
						break;
					case 1:
						fprintf(fp, "%s ", GetRegisterName(getGraphIndex(node->right) , 1));
						break;
					case 2:
					case 3:
						fprintf(fp, "$f28 ");
				}
				fprintf(fp, "\n");
				
				if(Result == 0){
					fprintf(fp, "CVT.W.D $f26, $f26\n");
					fprintf(fp, "MTC1 %s, $f26\n", GetRegisterName(node->g_index, 0));
				}
			}
			else if(const_op == 1){
				if(Operand1 != 0){
					temp_const = (AST_Node_Const *) node->left;
					fprintf(fp, "SUBI %s, %s, %d\n", GetRegisterName(node->g_index, 0), GetRegisterName(getGraphIndex(node->right), 0), temp_const->val);
				}
				if(Operand2 != 0){
					temp_const = (AST_Node_Const *) node->right;
					fprintf(fp, "SUBI %s, %s, %d\n", GetRegisterName(node->g_index, 0), GetRegisterName(getGraphIndex(node->left), 0), temp_const->val);
				}
			}
			else{
				fprintf(fp, "SUB %s, %s, %s\n", GetRegisterName(node->g_index, 0), GetRegisterName(getGraphIndex(node->left), 0), GetRegisterName(getGraphIndex(node->right), 0));
			}
			break;
		case MUL:
		
			/* check left operand */
			if (expression_data_type(node->left) == REAL_TYPE){
				float_op = 1;
				if(node->left->type == CONST_NODE){
					const_op = 1;
					Operand1 = 3;
				}
				else{
					Operand1 = 1;
				}
			}
			else{
				if(node->left->type == CONST_NODE){
					const_op = 1;
					Operand1 = 2;
				}
				else{
					Operand1 = 0;
				}
			}
			
			/* check right operand */
			if(expression_data_type(node->right) == REAL_TYPE){
				float_op = 1;
				if(node->right->type == CONST_NODE){
					const_op = 1;
					Operand2 = 3;
				}
				else{
					Operand2 = 1;
				}
			}
			else{
				if(node->right->type == CONST_NODE){
					const_op = 1;
					Operand2 = 2;
				}
				else{
					Operand2 = 0;
				}
			}
			
			/* check result */
			if(node->data_type == REAL_TYPE){
				float_op = 1;
				Result = 1;
			}
			
			if(float_op == 1){
				if(const_op == 1){
					if(Operand1 == 2 || Operand1 == 3){
						temp_const = (AST_Node_Const *) node->left;
					}
					else{
						temp_const = (AST_Node_Const *) node->right;
					}
					if(temp_const->const_type == REAL_TYPE){
						fprintf(fp, "LI.D $f28, %.2f\n", temp_const->val);
					}
					else{
						fprintf(fp, "LI.D $f28, %d.0\n", temp_const->val);
					}
					
				}
				
				if(Operand1 == 0){
					fprintf(fp, "MTC1.D %s, $f30\n", GetRegisterName(getGraphIndex(node->left) , 0));
					fprintf(fp, "CVT.D.W $f30, $f30\n");
				}
				else if(Operand2 == 0){
					fprintf(fp, "MTC1.D %s, $f30\n", GetRegisterName(getGraphIndex(node->right) , 0));
					fprintf(fp, "CVT.D.W $f30, $f30\n");
				}
				
				fprintf(fp, "MUL.D ");
				
				if(Result == 0){
					fprintf(fp, "$f26, ");
				}
				else{
					fprintf(fp, "%s, ", GetRegisterName(node->g_index, 1));
				}
				
				switch(Operand1){
					case 0:
						fprintf(fp, "$f30 ");
						break;
					case 1:
						fprintf(fp, "%s ", GetRegisterName(getGraphIndex(node->left) , 1));
						break;
					case 2:
					case 3:
						fprintf(fp, "$f28 ");
				}
				
				switch(Operand2){
					case 0:
						fprintf(fp, "$f30 ");
						break;
					case 1:
						fprintf(fp, "%s ", GetRegisterName(getGraphIndex(node->right) , 1));
						break;
					case 2:
					case 3:
						fprintf(fp, "$f28 ");
				}
				fprintf(fp, "\n");
				
				if(Result == 0){
					fprintf(fp, "CVT.W.D $f26, $f26\n");
					fprintf(fp, "MTC1 %s, $f26\n", GetRegisterName(node->g_index, 0));
				}
			}
			else if(const_op == 1){
				if(Operand1 != 0){
					temp_const = (AST_Node_Const *) node->left;
					fprintf(fp, "MUL %s, %s, %d\n", GetRegisterName(node->g_index, 0), GetRegisterName(getGraphIndex(node->right), 0), temp_const->val);
				}
				if(Operand2 != 0){
					temp_const = (AST_Node_Const *) node->right;
					fprintf(fp, "MUL %s, %s, %d\n", GetRegisterName(node->g_index, 0), GetRegisterName(getGraphIndex(node->left), 0), temp_const->val);
				}
			}
			else{
				fprintf(fp, "MUL %s, %s, %s\n", GetRegisterName(node->g_index, 0), GetRegisterName(getGraphIndex(node->left), 0), GetRegisterName(getGraphIndex(node->right), 0));
			}
			break;
		case DIV:
		
			/* check left operand */
			if (expression_data_type(node->left) == REAL_TYPE){
				float_op = 1;
				if(node->left->type == CONST_NODE){
					const_op = 1;
					Operand1 = 3;
				}
				else{
					Operand1 = 1;
				}
			}
			else{
				if(node->left->type == CONST_NODE){
					const_op = 1;
					Operand1 = 2;
				}
				else{
					Operand1 = 0;
				}
			}
			
			/* check right operand */
			if(expression_data_type(node->right) == REAL_TYPE){
				float_op = 1;
				if(node->right->type == CONST_NODE){
					const_op = 1;
					Operand2 = 3;
				}
				else{
					Operand2 = 1;
				}
			}
			else{
				if(node->right->type == CONST_NODE){
					const_op = 1;
					Operand2 = 2;
				}
				else{
					Operand2 = 0;
				}
			}
			
			/* check result */
			if(node->data_type == REAL_TYPE){
				float_op = 1;
				Result = 1;
			}
			
			if(float_op == 1){
				if(const_op == 1){
					if(Operand1 == 2 || Operand1 == 3){
						temp_const = (AST_Node_Const *) node->left;
					}
					else{
						temp_const = (AST_Node_Const *) node->right;
					}
					if(temp_const->const_type == REAL_TYPE){
						fprintf(fp, "LI.D $f28, %.2f\n", temp_const->val);
					}
					else{
						fprintf(fp, "LI.D $f28, %d.0\n", temp_const->val);
					}
					
				}
				
				if(Operand1 == 0){
					fprintf(fp, "MTC1.D %s, $f30\n", GetRegisterName(getGraphIndex(node->left) , 0));
					fprintf(fp, "CVT.D.W $f30, $f30\n");
				}
				else if(Operand2 == 0){
					fprintf(fp, "MTC1.D %s, $f30\n", GetRegisterName(getGraphIndex(node->right) , 0));
					fprintf(fp, "CVT.D.W $f30, $f30\n");
				}
				
				fprintf(fp, "DIV.D ");
				
				if(Result == 0){
					fprintf(fp, "$f26, ");
				}
				else{
					fprintf(fp, "%s, ", GetRegisterName(node->g_index, 1));
				}
				
				switch(Operand1){
					case 0:
						fprintf(fp, "$f30 ");
						break;
					case 1:
						fprintf(fp, "%s ", GetRegisterName(getGraphIndex(node->left) , 1));
						break;
					case 2:
					case 3:
						fprintf(fp, "$f28 ");
				}
				
				switch(Operand2){
					case 0:
						fprintf(fp, "$f30 ");
						break;
					case 1:
						fprintf(fp, "%s ", GetRegisterName(getGraphIndex(node->right) , 1));
						break;
					case 2:
					case 3:
						fprintf(fp, "$f28 ");
				}
				fprintf(fp, "\n");
				
				if(Result == 0){
					fprintf(fp, "CVT.W.D $f26, $f26\n");
					fprintf(fp, "MTC1 %s, $f26\n", GetRegisterName(node->g_index, 0));
				}
			}
			else if(const_op == 1){
				if(Operand1 != 0){
					temp_const = (AST_Node_Const *) node->left;
					fprintf(fp, "DIVI %s, %s, %d\n", GetRegisterName(node->g_index, 0), GetRegisterName(getGraphIndex(node->right), 0), temp_const->val);
				}
				if(Operand2 != 0){
					temp_const = (AST_Node_Const *) node->right;
					fprintf(fp, "DIVI %s, %s, %d\n", GetRegisterName(node->g_index, 0), GetRegisterName(getGraphIndex(node->left), 0), temp_const->val);
				}
			}
			else{
				fprintf(fp, "DIV %s, %s, %s\n", GetRegisterName(node->g_index, 0), GetRegisterName(getGraphIndex(node->left), 0), GetRegisterName(getGraphIndex(node->right), 0));
			}
			break;
		case INC:
			/* check data type */
			if (node->data_type == REAL_TYPE){
				fprintf(fp, "LI.D $28, 1.0\n");
				fprintf(fp, "ADD.D %s, %s, $28\n", GetRegisterName(node->g_index, 1), GetRegisterName(node->g_index, 1));
			}
			else{
				fprintf(fp, "ADDI %s, %s, 1\n", GetRegisterName(node->g_index, 0), GetRegisterName(node->g_index, 0));
			}
			break;
		case DEC:
			/* check data type */
			if (node->data_type == REAL_TYPE){
				fprintf(fp, "LI.D $28, 1.0\n");
				fprintf(fp, "SUB.D %s, %s, $28\n", GetRegisterName(node->g_index, 1), GetRegisterName(node->g_index, 1));
			}
			else{
				fprintf(fp, "SUBI %s, %s, 1\n", GetRegisterName(node->g_index, 0), GetRegisterName(node->g_index, 0));
			}
			break;
		default:
			fprintf(stderr, "Error in OP selection!\n");
			exit(1);
	}	
}

void generate_load(FILE *fp, AST_Node_Ref *node){
	if(node->entry->st_type == REAL_TYPE){
		fprintf(fp, "L.D %s, %s($0)\n", GetRegisterName(node->entry->g_index, 1), node->entry->st_name);	
	}
	else if(node->entry->st_type == INT_TYPE)
	{
		fprintf(fp, "LW %s, %s($0)\n", GetRegisterName(node->entry->g_index, 0), node->entry->st_name);
	}
	else{
		fprintf(fp, "LA %s, %s($0)\n", GetRegisterName(node->entry->g_index, 0), node->entry->st_name);
		
	}
}

void generate_attr(FILE *fp, AST_Node_Attr *node){

	if(node->attr_type==0)
	{
		fprintf(fp, "LW %s, %s.no($0)\n", GetRegisterName(node->entry->g_index, 0), node->entry->st_name);	
	}
	else if(node->attr_type==1)
	{
		fprintf(fp, "L.D %s, %s.wt($0)\n", GetRegisterName(node->entry->g_index, 1), node->entry->st_name);	
	}
	else if(node->attr_type==2)
	{	
		fprintf(fp, "L.D %s, %s.charge($0)\n", GetRegisterName(node->entry->g_index, 1), node->entry->st_name);	
	}
}

void generate_molecule(FILE *fp, AST_Node_Mol *node){

	if(node->entry->st_type==MOLECULE_TYPE)
	{    fprintf(fp,"L.D $f22,temp($0)\n");
	
				for(int j=0;j<node->entry->No;j++)
				{
				    fprintf(fp, "LW %s, %s.num%d($0)\n",  GetRegisterName(node->entry->g_index, 0),node->entry->st_name,(j+1));	
				    fprintf(fp, "L.D %s, %s.wt%d($0)\n",  GetRegisterName(node->entry->g_index, 1),node->entry->st_name,(j+1));
				    fprintf(fp, "MTC1.D %s, $f30\n", GetRegisterName(node->entry->g_index, 0));
				    fprintf(fp, "CVT.D.W $f30, $f30\n");
				    fprintf(fp, "MUL.D $f30,$f30,%s\n",GetRegisterName(node->entry->g_index, 1));
				    fprintf(fp, "ADD.D $f22,$f22,$f30\n");
				}
	}
}

void generate_func_call_res(FILE *fp, AST_Node_Func_Call *node){
	if(node->entry->inf_type == REAL_TYPE){
		fprintf(fp, "LI.D %s, 0.0\n", GetRegisterName(node->g_index, 1));
	}
	else{
		fprintf(fp, "LI %s, 0\n", GetRegisterName(node->g_index, 0));
	}	
}

void generate_simple(FILE *fp, char* Label){
	fprintf(fp, "J %s\n", Label);
}

void generate_assignment(FILE *fp, AST_Node_Assign *node){
	AST_Node_Const *temp_const;
	AST_Node_Attr *temp_attr;
	AST_Node_Mol *temp_mol;
	
	switch (node->entry->st_type){
		case INT_TYPE:
		
			if(expression_data_type(node->assign_val) == INT_TYPE){
				/* constant */
				if(node->assign_val->type == CONST_NODE){
					temp_const = (AST_Node_Const *) node->assign_val;
					fprintf(fp, "LI %s, %d\n",  GetRegisterName(node->entry->g_index, 0), temp_const->val);
					fprintf(fp, "SW %s, %s($0)\n",GetRegisterName(node->entry->g_index, 0),node->entry->st_name);
				}
				/* variable */
				else if(node->assign_val->type == ATTR_NODE)
				{
					temp_attr= (AST_Node_Attr *)node->assign_val;
					
					fprintf(fp, "MOVE %s, %s\n", GetRegisterName(node->entry->g_index, 0),GetRegisterName(temp_attr->entry->g_index, 0));
					fprintf(fp, "SW %s, %s($0)\n",GetRegisterName(node->entry->g_index, 0),node->entry->st_name);
					
				}
				
				else{
					fprintf(fp, "MOVE %s, %s\n", GetRegisterName(node->entry->g_index, 0), GetRegisterName(getGraphIndex(node->assign_val), 0));
					fprintf(fp, "SW %s, %s($0)\n",GetRegisterName(node->entry->g_index, 0),node->entry->st_name);
				}
			}
			else{
				if(node->assign_val->type == CONST_NODE){
					temp_const = (AST_Node_Const *) node->assign_val;					
					fprintf(fp, "LI %s, %d\n", GetRegisterName(node->entry->g_index, 0), temp_const->val);	
					fprintf(fp, "SW %s, %s($0)\n",GetRegisterName(node->entry->g_index, 0),node->entry->st_name);
				}
				/* variable */
				else{
					fprintf(fp, "CVT.W.D %s, %s\n", GetRegisterName(getGraphIndex(node->assign_val), 1), GetRegisterName(getGraphIndex(node->assign_val), 1));
					fprintf(fp, "MFC1 %s, %s\n", GetRegisterName(getGraphIndex(node->assign_val), 0), GetRegisterName(getGraphIndex(node->assign_val), 1));
					fprintf(fp, "MOVE %s, %s\n", GetRegisterName(node->entry->g_index, 0), GetRegisterName(getGraphIndex(node->assign_val), 0));	
					fprintf(fp, "SW %s, %s($0)\n",GetRegisterName(node->entry->g_index, 0),node->entry->st_name);
				}
			}
			break;
		case REAL_TYPE:
			if(expression_data_type(node->assign_val) == REAL_TYPE){
				/* constant */
				if(node->assign_val->type == CONST_NODE){
					temp_const = (AST_Node_Const *) node->assign_val;					
					fprintf(fp, "LI.D %s, %.2f\n", GetRegisterName(node->entry->g_index, 1), temp_const->val);	
					fprintf(fp, "S.D %s, %s($0)\n",GetRegisterName(node->entry->g_index, 1),node->entry->st_name);
				}
				else if(node->assign_val->type == ATTR_NODE)
				{
					temp_attr= (AST_Node_Attr *)node->assign_val;
					
					fprintf(fp, "MOV.D %s, %s\n", GetRegisterName(node->entry->g_index, 1),GetRegisterName(temp_attr->entry->g_index, 1));
					fprintf(fp, "S.D %s, %s($0)\n",GetRegisterName(node->entry->g_index, 1),node->entry->st_name);
					
				}
				else if(node->assign_val->type == FUNC_CALL)
				{
					
					
					fprintf(fp, "MOV.D %s, $f22\n", GetRegisterName(node->entry->g_index, 1));
					fprintf(fp, "S.D %s, %s($0)\n",GetRegisterName(node->entry->g_index, 1),node->entry->st_name);
					
				}
				/* variable*/
				else{
					fprintf(fp, "MOV.D %s, %s\n", GetRegisterName(node->entry->g_index, 1), GetRegisterName(getGraphIndex(node->assign_val), 1));
					fprintf(fp, "S.D %s, %s($0)\n",GetRegisterName(node->entry->g_index, 1),node->entry->st_name);
				}
			}
			else{
				/* constant */
				if(node->assign_val->type == CONST_NODE){
					temp_const = (AST_Node_Const *) node->assign_val;					
					fprintf(fp, "LI.D %s, %d.0\n", GetRegisterName(node->entry->g_index, 1), temp_const->val);	
					fprintf(fp, "S.D %s, %s($0)\n",GetRegisterName(node->entry->g_index, 1),node->entry->st_name);
				}
				/* variable*/
				else{
					fprintf(fp, "MTC1 %s, %s\n", GetRegisterName(getGraphIndex(node->assign_val), 0), GetRegisterName(getGraphIndex(node->assign_val), 1));
					fprintf(fp, "CVT.D.W %s, %s\n", GetRegisterName(getGraphIndex(node->assign_val), 1), GetRegisterName(getGraphIndex(node->assign_val), 1));
					fprintf(fp, "MOVE %s, %s\n", GetRegisterName(node->entry->g_index, 1), GetRegisterName(getGraphIndex(node->assign_val), 1));
					fprintf(fp, "SW %s, %s($0)\n",GetRegisterName(node->entry->g_index, 1),node->entry->st_name);
				}
			}
			break;
	
		default:
			fprintf(stderr, "Error in case selection!\n");
			exit(1);
	}
}

/* Graph Functions */
void initGraph(){
	g = (AdjGraph*) malloc(sizeof(AdjGraph));
	g->adj = NULL;
	g->vertexCount = 0;
}

void insertEdge(int i, int j){
	
	AdjList *l;
	
	// check if less than zero (constant in operation)
	if(i < 0 || j < 0){
		return;
	}
	
	/* find max */
	int max;
	if(i > j){
		max = i;
	}
	else{
		max = j;
	}

	/* variable count is max + 1 */
	max = max + 1;

	/* manage space */
	if (g->vertexCount == 0){ /* first allocation */
		g->adj = (AdjList**) malloc(max * sizeof(AdjList*));
		g->vertexCount = max;
	}
	else if(g->vertexCount < max){ /* allocate space for more variables */
		g->adj = (AdjList**) realloc(g->adj, max * sizeof(AdjList*));
		g->vertexCount = max;
	}
	
	/* insert edge i-j */
	l = g->adj[i];
	
	while ((l != NULL) && (l->index != j)) l = l->next;
	
	if (l == NULL){
		l = (AdjList*) malloc(sizeof(AdjList));
		l->index = j;
		l->next = g->adj[i];
		g->adj[i] = l;
	}
	
	/* insert edge j-i */
	l = g->adj[j];
	
	while ((l != NULL) && (l->index != i)) l = l->next;
	
	if (l == NULL){
		l = (AdjList*) malloc(sizeof(AdjList));
		l->index = i;
		l->next = g->adj[j];
		g->adj[j] = l;
	}
}

int *greedyColoring(){
	
	AdjList *l;
	int i;
	
	
	int V = g->vertexCount;
	
	// color array
	int *colors;
	colors = (int*) malloc(V * sizeof(int));

	// initialize all vertices to '-1', which means unassigned
	for(i = 0; i < V; i++){
		colors[i] = -1;
	}

	// assign first color (0) to first vertex
	colors[0] = 0;
	
	

	// boolean array that shows us which colors are still available
	int *available;
	available = (int*) malloc(V * sizeof(int));

	// starting off, all colors are available
	for(i = 0; i < V; i++){
		available[i] = 1;
	}

	// assign colors to the remaining V-1 vertices
	int u;
	for (u = 1; u < V; u++) {
		// process adjacent vertices and flag their colors as unavailable
		l = g->adj[u];
		while (l != NULL) {
			i = l->index;
			if (colors[i] != -1) {
				available[colors[i]] = 0;
			}
			l = l->next;
		}

		// find the first avaiable color
		int cr;
		for (cr = 0; cr < V; cr++) {
			if (available[cr] == 1){
				break;
			}
		}

		// assign the first avaiable color
		colors[u] = cr;

		// reset values back to true for the next iteration
		for(i = 0; i < V; i++){
			available[i] = 1;
		}
	}
	
	return colors;
}

/* Variable Array Functions */

void insertVar(char *name){
	/* first insertion */
	if(var_count == 0){
		var_name = (char**) malloc(1 * sizeof(char*));
		var_name[0] = (char*) malloc((strlen(name) + 1) * sizeof(char));		
		strcpy(var_name[0], name);
		
		var_count++;
	}
	else{
		/* check if variable already exists */
		int flag = 0;
		int i;
		for(i = 0; i < var_count; i++){
			if(strcmp(var_name[i], name) == 0){
				flag = 1;
				break;
			}
		}
		
		/* not inserted yet */
		if(flag == 0){
			var_name = (char**) realloc(var_name, (var_count + 1) * sizeof(char*));
			var_name[var_count] = (char*) malloc((strlen(name) + 1) * sizeof(char));		
			strcpy(var_name[var_count], name);
			
			var_count++;
		}
	}
}

int getVarIndex(char *name){
	int index = -1,i;
	for(i = 0; i < var_count; i++){
		if(strcmp(var_name[i], name) == 0){
			index = i;
			break;
		}
	}	
	return index;
}

void printVarArray(){
	int i;
	printf("VarArray:\n");
	for(i = 0 ; i < var_count; i++){
		printf("%d: %s\n", i, var_name[i]);
	}
	printf("\n");
}

/* main function register allocation */
void main_reg_allocation(AST_Node *node){
	
	AST_Node_Declarations *temp_declarations;
	AST_Node_Decl *temp_decl;
	AST_Node_Arithm *temp_arithm;
	AST_Node_Bool *temp_bool;
	AST_Node_Statements *temp_statements;
	AST_Node_If *temp_if;
	AST_Node_Elsif *temp_elsif;
	AST_Node_For *temp_for;
	AST_Node_While *temp_while;
	AST_Node_Incr *temp_incr;
	AST_Node_Assign *temp_assign;
	AST_Node_Func_Call *temp_func_call;
	AST_Node_Call_Params *temp_call_params;
	
	/* temp variable name */
	char name[MAXTOKENLEN];
	int i;
	/* check if empty */
	if(node == NULL){
	
		return;
	}
	switch(node->type){
		/* declarations case */
		case DECLARATIONS:
		
			temp_declarations = (struct AST_Node_Declarations *) node;	
			for(i = 0; i < temp_declarations->declaration_count; i++){
				main_reg_allocation(temp_declarations->declarations[i]);
			}
			break;
		/* declaration case */
		case DECL_NODE:
			temp_decl = (struct AST_Node_Decl *) node;
			for(i = 0; i < temp_decl->names_count; i++){
				insertVar(temp_decl->names[i]->st_name);
				
				/* graph index */
				temp_decl->names[i]->g_index = getVarIndex(temp_decl->names[i]->st_name);
			}
			break;
		/* left and right child cases */
		case BASIC_NODE:
			main_reg_allocation(node->left);
			main_reg_allocation(node->right);
			break;
		case ARITHM_NODE:
			temp_arithm = (struct AST_Node_Arithm *) node;
			
			main_reg_allocation(node->left);
			main_reg_allocation(node->right);
		
			/* insert temporary */
			sprintf(name, "_temp%d", temp_count);
			insertVar(name);
			temp_count++;
			
			printf("Inserting %s for node:\n", name);
			ast_print_node(node);
			printf("\n");
			
			declare = 1;
			insert(name, strlen(name), temp_arithm->data_type, -1);
			declare = 0;
			
			/* graph index */
			temp_arithm->g_index = var_count - 1;
			
			/* manage graph */
			if(temp_arithm->op != INC && temp_arithm->op != DEC){
				insertEdge(temp_arithm->g_index, getGraphIndex(temp_arithm->left));
				insertEdge(temp_arithm->g_index, getGraphIndex(temp_arithm->right));
				insertEdge(getGraphIndex(temp_arithm->left), getGraphIndex(temp_arithm->right));
			}
			else{
				insertEdge(temp_arithm->g_index, getGraphIndex(temp_arithm->left));
			}
			
			break;
		case BOOL_NODE:
			temp_bool = (struct AST_Node_Bool *) node;
			
			main_reg_allocation(node->left);
			main_reg_allocation(node->right);
		
			/* insert temporary */
			sprintf(name, "_temp%d", temp_count);
			insertVar(name);
			temp_count++;
			
			printf("Inserting %s for node:\n", name);
			ast_print_node(node);
			printf("\n");
			
			declare = 1;
			insert(name, strlen(name), temp_bool->data_type, -1);
			declare = 0;
			
			/* graph index */
			temp_bool->g_index = var_count - 1;
			
			/* manage graph */
			if(temp_bool->op != NOT){
				insertEdge(temp_bool->g_index, getGraphIndex(temp_bool->left));
				insertEdge(temp_bool->g_index, getGraphIndex(temp_bool->right));	
				insertEdge(getGraphIndex(temp_bool->left), getGraphIndex(temp_bool->right));
			}
			else{
				insertEdge(temp_bool->g_index, getGraphIndex(temp_bool->left));
			}
			
			break;
		case REL_NODE:			
			/* used in branch and loop conditions - not stored */
			break;
		case EQU_NODE:		
			/* used in branch and loop conditions - not stored */
			break;
		/* reference case */
		case REF_NODE:
			/* all the entries are already being managed by the Decl case */
			break;
		case MOL_NODE:
			break;
		/* constant case */
		case ATTR_NODE:
			break;
		case CONST_NODE:
			/* already managed in getGraphIndex */
			break;
		/* statements case */
		case STATEMENTS:
			temp_statements = (struct AST_Node_Statements *) node;	
			for(i = 0; i < temp_statements->statement_count; i++){
				main_reg_allocation(temp_statements->statements[i]);
			}
			break;
		/* the if case */
		case IF_NODE: //have to be done
			break;
		/* the else if case */
		case ELSIF_NODE: //have to be done	
			break;
		/* for case */
		case FOR_NODE: //have to be done	
			
			break;
		/* while case */
		case WHILE_NODE:  //have to be done
			break;
		/* assign case */
		case ASSIGN_NODE:
			temp_assign = (struct AST_Node_Assign *) node;
			/* manage graph */
			insertEdge(temp_assign->entry->g_index, getGraphIndex(temp_assign->assign_val));
			main_reg_allocation(temp_assign->assign_val);
			break;
		/* simple case */
		case SIMPLE_NODE:
			break;
		/* increment statement */
		case INCR_NODE:
			temp_incr = (AST_Node_Incr*) node;
			
			break;
		/* function call case */
		case FUNC_CALL:
			temp_func_call = (struct AST_Node_Func_Call *) node;
			if(temp_func_call->num_of_pars != 0){
				for(i = 0; i < temp_func_call->num_of_pars; i++){
					main_reg_allocation(temp_func_call->params[i]);
				}
			}

			/* insert temporary when function non-void */
			if(temp_func_call->entry->inf_type != VOID_TYPE){
				sprintf(name, "_temp%d", temp_count);
				insertVar(name);
				temp_count++;
				
				printf("Inserting %s for node:\n", name);
				ast_print_node(node);
				printf("\n");
				
				declare = 1;
				insert(name, strlen(name), temp_func_call->entry->inf_type, -1);
				declare = 0;
				
				/* graph index */
				temp_func_call->g_index = var_count - 1;
			}
			break;
		case CALL_PARAMS:
			temp_call_params = (struct AST_Node_Call_Params*) node;
			if(temp_call_params->num_of_pars > 0){
				for(i = 0; i < temp_call_params->num_of_pars; i++){
					main_reg_allocation(temp_call_params->params[i]);
				}
			}
			break;
		/* function declaration stuff */
		case FUNC_DECLS:
		case FUNC_DECL:
		case RET_TYPE:
		case DECL_PARAMS:
		case RETURN_NODE:
			/* can't occur in main */
			break;
		default: /* wrong choice case */
			fprintf(stderr, "Error in node selections!\n");
			exit(1);
	}
}

/* main function tree traversal */
void main_func_traversal(FILE *fp, AST_Node *node){
	int i,j;
	
	AST_Node_Declarations *temp_declarations;
	AST_Node_Decl *temp_decl;
	AST_Node_Arithm *temp_arithm;
	AST_Node_Bool *temp_bool;
	AST_Node_Rel *temp_rel;
	AST_Node_Equ *temp_equ;
	AST_Node_Ref *temp_ref;
	AST_Node_Attr *temp_attr;
	AST_Node_Statements *temp_statements;
	AST_Node_If *temp_if;
	AST_Node_Elsif *temp_elsif;
	AST_Node_For *temp_for;
	AST_Node_While *temp_while;
	AST_Node_Incr *temp_incr;
	AST_Node_Assign *temp_assign;
	AST_Node_Func_Call *temp_func_call;
	AST_Node_Call_Params *temp_call_params;
	AST_Node_Mol *temp_mol;
	
	/* temp variable ST entry */
	list_t *entry;
	
	/* check if empty */
	if(node == NULL){
		return;
	}	   
	switch(node->type){
		/* declarations case */
		case DECLARATIONS:
			/* nothing */
			break;
		/* declaration case */
		case DECL_NODE:
			/* nothing */
			break;
		/* left and right child cases */
		case BASIC_NODE:
			main_func_traversal(fp, node->left);
			main_func_traversal(fp, node->right);
			break;
		case ARITHM_NODE:
			temp_arithm = (struct AST_Node_Arithm *) node;
			
			main_func_traversal(fp, node->left);
			main_func_traversal(fp, node->right);
			
			generate_arithm(fp, temp_arithm);
			break;
		case BOOL_NODE: //not defined yet	
			break;
		case REL_NODE:  //not defined yet	
			break;
		case EQU_NODE:  //not defined yet	
			break;
		// reference case
		case REF_NODE:
			temp_ref = (struct AST_Node_Ref *) node;
			/* load value from memory to register */
			generate_load(fp, temp_ref);
			break;
		case MOL_NODE:
			temp_mol = (struct AST_Node_Mol *) node;
			
			/* load value from memory to register */
			generate_molecule(fp, temp_mol);
			break;
		case ATTR_NODE:
			temp_attr = (struct AST_Node_ATTR *) node;
			/* load value from memory to register */
			generate_attr(fp, temp_attr);
			break;
		/* constant case */
		case CONST_NODE:
			/* already managed in the various generation functions */
			break;
		/* statements case */
		case STATEMENTS:
			temp_statements = (struct AST_Node_Statements *) node;	
			for(i = 0; i < temp_statements->statement_count; i++){
				main_func_traversal(fp, temp_statements->statements[i]);
			}
			break;
		/* the if case */
		case IF_NODE: //have to be done
			break;
		/* the else if case */
		case ELSIF_NODE: //have to be done
			break;
		/* for case */
		case FOR_NODE: //have to be done
			break;
		/* while case */
		case WHILE_NODE: //have to be done
			break;
		/* assign case */
		case ASSIGN_NODE:
			temp_assign = (struct AST_Node_Assign *) node;
			main_func_traversal(fp, temp_assign->assign_val);
			generate_assignment(fp, temp_assign);
			break;
		/* increment statement */
		case INCR_NODE: 
			break;
		/* function call case */
		case FUNC_CALL:
		      
			temp_func_call = (struct AST_Node_Func_Call *) node;
			if(temp_func_call->num_of_pars != 0){
				for(i = 0; i < temp_func_call->num_of_pars; i++){
					main_func_traversal(fp, temp_func_call->params[i]);
				}
			}
	
		        if(strcmp(temp_func_call->entry->st_name,"print")==0)
		        {       
		          temp_ref=(struct AST_Node_Ref *)temp_func_call->params[0];
		          AST_Node_Const *temp_const = (AST_Node_Const *) temp_func_call->params[0];
		          
		          if(temp_ref->entry->st_type == REAL_TYPE){
		              fprintf(fp, "LI $v0, 3\n");
		             fprintf(fp, "MOV.D $f12, %s\n", GetRegisterName(temp_ref->entry->g_index, 1));      
	                  }
	                  else   if(temp_ref->entry->st_type == INT_TYPE){
	                      fprintf(fp, "LI $v0, 1\n");
		             fprintf(fp, "MOVE $a0, %s\n", GetRegisterName(temp_ref->entry->g_index, 0)); 
	                  }
	                  else 
	                  {  fprintf(fp, "LI $v0, 4\n");
	                  
	                     switch(temp_func_call->params[0]->type)
	                     {
	                     	case REF_NODE:  fprintf(fp, "MOVE $a0, %s\n", GetRegisterName(temp_ref->entry->g_index, 0));
	                     			break;
	                     	case CONST_NODE:  
	                     	for(i = 0; i < num_of_msg; i++){
				if(!strcmp(str_messages[i],(temp_const->val).sval))
				{
					
		  			fprintf(fp,"LA $a0, msg%d\n",(i+1));
		                }
	                        }
	                   	      break;
	                     }
	                  }
		            fprintf(fp, "SYSCALL\n");
		        }	
		        else if(strcmp(temp_func_call->entry->st_name,"mol_mass")==0)
		        {
		                   //do nothing 
		         }
		         else
		         {	/* when function non-void */
			if(temp_func_call->entry->inf_type != VOID_TYPE){
				
				generate_func_call_res(fp, temp_func_call);				
			}	
			}		
			break;
		case CALL_PARAMS:
			temp_call_params = (struct AST_Node_Call_Params*) node;
			/* parameters will be covered in another article */
			break;
		/* function declaration stuff */
		case FUNC_DECLS:
		case FUNC_DECL:
		case RET_TYPE:
		case DECL_PARAMS:
		case RETURN_NODE:
			/* can't occur in main */
			break;
		default: /* wrong choice case */
			fprintf(stderr, "Error in node selection!\n");
			exit(1);
	}	
}
