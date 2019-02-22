/**CFile***********************************************************************

  FileName    [l2s.c]

  PackageName [ltl2smv]

  Synopsis    [The LTL to SMV Translator]

  Description [This file provides routines for reducing LTL model
  checking to CTL model checking. This work is absed on the work
  presented in \[1\]<br>

  <ul><li> O. Grumberg E. Clarke and K. Hamaguchi.  <cite>Another Look
          at LTL Model Checking</cite>. <em>Formal Methods in System
          Design, 10(1):57--71, February 1997.</li>
  </ul>]

  SeeAlso     []

  Author      [Adapted to NuSMV by Marco Roveri. 
               Extended to the past operators by Ariel Fuxman.]

  Copyright   [
  This file is part of the ``ltl2smv'' package of NuSMV version 2. 
  Copyright (C) 1998-2001 by CMU and ITC-irst. 

  NuSMV version 2 is free software; you can redistribute it and/or 
  modify it under the terms of the GNU Lesser General Public 
  License as published by the Free Software Foundation; either 
  version 2 of the License, or (at your option) any later version.

  NuSMV version 2 is distributed in the hope that it will be useful, 
  but WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public 
  License along with this library; if not, write to the Free Software 
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA.

  For more information of NuSMV see <http://nusmv.irst.itc.it>
  or email to <nusmv-users@irst.itc.it>.
  Please report bugs to <nusmv-users@irst.itc.it>.

  To contact the NuSMV development board, email to <nusmv@irst.itc.it>. ]

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "l2s.h"


/* To avoid the creatino of too long item the parser cannot handle: */
#undef ENABLE_COMMENTS_PRINT_OUT 

#define BWIDTH 1024
#define STRHASHSIZE 512
#define PRE_PREFIX "LTL_"
#define PREFIXNAME "_SPECF_"
#define MODULE_NAME "ltl_spec_"
extern char *mycpy();
extern FILE * yyin;

static long bit[33];

node *Specf;
static int Linenumber = 0;
static str_list **Str_hash;
static output_list *Define_list, *Next_list, *Fairness_list, 
                   *Initially_list, *Var_list;
static int Spec_Counter = 0;

static int get_digits(int n);

int get_linenumber(void)
{
  return Linenumber;
}

void inc_linenumber(void)
{
  Linenumber++;
}

char *Mymalloc(unsigned size)
{
  char *ret;

  ret = malloc(size);
  if (ret == NULL) {
    printf("malloc returns NULL\n");
    exit(1);
  }
  return(ret);
}


output_list *mymalloc_output_list(void)
{
  output_list *ret;
  ret = (output_list *) Mymalloc(sizeof(output_list));
  ret -> key = NULL;
  ret -> item = NULL;
  ret -> next = NULL;
  return (ret);
}


void initbit(void)
{
  int i;
  bit[0] = 1;
  for (i = 1; i < 33; i++) {
    bit[i] = bit[i-1]<<1;
  }
} 

void print_node(FILE * ofile, node * t)
{
  fprintf(ofile, " ----\n");
  fprintf(ofile, " pointer = %d\n", (int) t);
  if (t->op == 'l')
    fprintf(ofile, " leaf %s\n", t->str);
  else if (t->op != (char)NULL){
    fprintf(ofile, " op = %c\n", t->op);
    fprintf(ofile, " left pointer = %d\n", (int) t->left);
    fprintf(ofile, " right pointer = %d\n", (int) t->right);
    print_node(ofile, t->left);
    print_node(ofile, t->right);
  }
  else {
    fprintf(stderr, "t->op == NULL in print_tree\n");
    exit(1);
  }
  return;
}

void print_formula(FILE * ofile, node *t)
{
  fprintf(ofile, "(");
  if (t->op == 'l')
    fprintf(ofile, "%s", t->str);
  else if (t->op != (char)NULL){
    if (t->right == NULL) {
      fprintf(ofile, "%c", t->op);
      print_formula(ofile, t->left);
    }
    else {
      print_formula(ofile, t->left);
      fprintf(ofile, "%c", t->op);
      print_formula(ofile, t->right);
    }
  }
  else {
    fprintf(stderr, "t->op == NULL in print_formula\n");
    exit(1);
  }
  fprintf(ofile, ")");
  return;
}

char *shortitob(long num)
{
  char *b;
  int i;
  b = (char *)Mymalloc(sizeof(char)*(33));
  for (i = 0; i < 32; i++) {
    b[i] =  (((num & bit[i]) == 0)? '0':'1');
  }
  b[32] = '\0';
  return(b);
}

char *itob(int num)
{
   char *b;
   int i;

   b = (char *)Mymalloc(sizeof(char)*(BWIDTH+1));
   for (i = 0; i < BWIDTH; i++) {
     b[i] = '0';
   }
   b[BWIDTH] = '\0';

   for (i = 0; i < 32; i++)
     b[i] =  (((num & bit[i]) == 0)? '0':'1');

/*   printf("itob(%d) = %d\n", num, btoi(b)); */
   return(b);
}

node * expand_case_body(node * body) {
  assert(body != NULL);
  assert((body->op == 'c') || (body->op == 'l'));

  if (body->op == 'c') {
    node * res;
    node * cond      = (body->left)->left;
    node * cond_res  = (body->left)->right;
    node * case_rest = expand_case_body(body->right);

    assert((body->left)->op == ':');
    res = gen_node('|', gen_node('&', cond, cond_res),
                        gen_node('&', gen_node('!', cond, NULL), case_rest));
    return(res);
  }
  else {
    return(body);
  }
}
    

node *expand_formula(node *t)
{
  node *ret;
  node *body;
  node *or, *or1, *or2, *not, *not1, *not2, *not3, *not4, *tmp;

  if (t == NULL) return (NULL);

  switch (t->op) {
  case '|':
  case '!':
  case 'X':
  case 'U':
  case 'Y':
  case 'S':
    ret = t;
    ret->left = expand_formula(t->left);
    ret->right = expand_formula(t->right);
    break;
  case 'l':
    ret = t;
    break;
  case 'c': /* it is case expression */
    body = expand_case_body(t->left);
    ret = expand_formula(body);
    break;
  case '&':
    not1 = gen_node('!',expand_formula(t->left),NULL);
    not2 = gen_node('!',expand_formula(t->right),NULL);
    or  = gen_node('|',not1,not2);
    ret = gen_node('!',or,NULL);
    break;
  case '>':
    not = gen_node('!',expand_formula(t->left),NULL);
    ret = gen_node('|',not,expand_formula(t->right));
    break;
  case '=':
    /* a = b <-> (!(!a | !b) | !(a | b)) */
    not2 = gen_node('!',expand_formula(t->left),NULL);
    not3 = gen_node('!',expand_formula(t->right),NULL);
    or1 = gen_node('|', not2, not3);
    not1 = gen_node('!',or1, NULL);
    /* not{2,3}->left is the already exapnded version of the left
       right part of t */
    or2 = gen_node('|', not2->left, not3->left);
    not4 = gen_node('!', or2, NULL);
    ret = gen_node('|',not1, not4);
    break;
  case 'x': /* xor */
    /* a xor b <-> (!(!a | b) | !(a | !b)) */
    not1 = gen_node('!', expand_formula(t->left), NULL);
    not2 = gen_node('!', expand_formula(t->right), NULL);
    /* not{1,2}->left is the already exapnded version of the left
       right part of t */
    or1 = gen_node('|', not1, not2->left);
    or2 = gen_node('|', not1->left, not2);
    not3 = gen_node('!', or1, NULL);
    not4 = gen_node('!', or2, NULL);
    ret = gen_node('|', not3, not4);
    break;
  case 'n': /* xnor */
    /* a xnor b <-> a = b */
    not2 = gen_node('!',expand_formula(t->left),NULL);
    not3 = gen_node('!',expand_formula(t->right),NULL);
    or1 = gen_node('|', not2, not3);
    not1 = gen_node('!',or1, NULL);
    /* not{2,3}->left is the already exapnded version of the left
       right part of t */
    or2 = gen_node('|', not2->left, not3->left);
    not4 = gen_node('!', or2, NULL);
    ret = gen_node('|',not1, not4);
    break;
  case 'Z':
    tmp = gen_node('Y', gen_node('!',expand_formula(t->left),NULL), NULL);
    ret = gen_node('!', tmp, NULL);
    break;
  case 'F':
    ret = gen_node('U',gen_leaf(mycpy("1")),expand_formula(t->left));
    break;
  case 'G':
    tmp = gen_node('U', gen_leaf(mycpy("1")), 
                        gen_node('!',expand_formula(t->left),NULL));
    ret = gen_node('!', tmp, NULL);
    break;
  case 'V':
    tmp = gen_node('U', gen_node('!',expand_formula(t->left),NULL),
		        gen_node('!',expand_formula(t->right),NULL));
    ret = gen_node('!', tmp, NULL);
    break;
  case 'O':
    ret = gen_node('S',gen_leaf(mycpy("1")),expand_formula(t->left));
    break;
  case 'H':
    tmp = gen_node('S', gen_leaf(mycpy("1")), 
                        gen_node('!',expand_formula(t->left),NULL));
    ret = gen_node('!', tmp, NULL);
    break;
  case 'T':
    tmp = gen_node('S', gen_node('!',expand_formula(t->left),NULL),
		        gen_node('!',expand_formula(t->right),NULL));
    ret = gen_node('!', tmp, NULL);
    break;
  default:
    fprintf(stderr, "expand_formula: unknown operator \"%c\"\n", t->op);
    exit(1);
    break;
  }
  return(ret);
}

char *op_string(char *op, char *str1, char *str2)
{
  char *ret;
  int l1,l2,l3,l;

  if (str2 == NULL) {
    l1 = strlen(op);
    l2 = strlen(str1);
    l = l1 + l2 + 1 + 2;
    ret = (char *)Mymalloc(sizeof(char)*l);
    strcpy(ret, "(");
    strcat(ret,op);
    strcat(ret,str1);
    strcat(ret, ")");
  }
  else {
    l1 = strlen(op);
    l2 = strlen(str1);
    l3 = strlen(str2);
    l = l1 + l2 + l3 +1 + 2;
    ret = (char *)Mymalloc(sizeof(char)*l);
    strcpy(ret, "(");
    strcat(ret,str1);
    strcat(ret,op);
    strcat(ret,str2);
    strcat(ret, ")");
  }
  return(ret);
}

/* returned string must be freed */
char* gen_name_number(int n)
{
  char* ret;
  size_t len;

  len = strlen(PRE_PREFIX) + strlen(PREFIXNAME) +
    get_digits(Spec_Counter) + get_digits(n) + 1;
  
  ret = (char*) Mymalloc(sizeof(char) * len);
  
  snprintf(ret, len, "%s%d%s%d", PRE_PREFIX, Spec_Counter, PREFIXNAME, n);
  return ret;
}


int str_hash_func(char *s)
{
  int i,l,a;
  l = strlen(s);
  for (a = i = 0; i<l; i++) {
    a += (int) s[i];
  }
  a = a % STRHASHSIZE;
  return (a);
}

str_list *mymalloc_str_list(void)
{
  str_list *ret;
  ret =(str_list *)Mymalloc(sizeof(str_list));
  ret -> name = NULL;
  ret -> str = NULL;
  ret -> next = NULL;
  return (ret);
}

void add_var_list(char *key, char *item)
{
  output_list *m;
 
  m = Var_list;
  while(m != NULL) {
    if ((strcmp(m->key, key) == 0) && (strcmp(m->item, item) == 0)) return;
    m = m->next;
  }
  m = mymalloc_output_list();
  m -> key = key;
  m -> item = item;
  m -> next = Var_list;
  Var_list = m;
}

/* Creates a variable for the expression and returns it */
char *reg_var(char *str)
{
  static int counter = 0;
  int v;
  str_list *l;

  v = str_hash_func(str);
  for (l = Str_hash[v]; l != NULL; l = l->next) {
    if (strcmp(l->str, str) == 0) {
      return(l->name);
    }
  }
  l = mymalloc_str_list();
  l -> str = str;
  l->next = Str_hash[v];
  Str_hash[v] = l;
  l->name =  gen_name_number(counter++);

  return(l->name);
}

/* Given a node, produces the string of its corresponding formula */
char *formula_string(node *t)
{
  char *ret, *t1, *t2;
  switch(t->op) {
  case '|':
    ret=op_string("|", t1 = formula_string(t->left), 
                       t2 = formula_string(t->right));
    free(t1); free(t2);
    break;
  case '!':
    ret = op_string("!", t1 = formula_string(t->left), NULL);
    free(t1);
    break;
  case 'X':
    ret = op_string("X", t1 = formula_string(t->left), NULL);
    free(t1);
    break;
  case 'Y':
    ret = op_string("Y", t1 = formula_string(t->left), NULL);
    free(t1);
    break;
  case 'U':
    ret=op_string("U", t1 = formula_string(t->left), 
                       t2 = formula_string(t->right));
    free(t1); free(t2);
    break;
  case 'S':
    ret=op_string("S", t1 = formula_string(t->left), 
                       t2 = formula_string(t->right));
    free(t1); free(t2);
    break;
  case 'l':
    ret = mycpy(t->str);
    break;
  default:
    ;
  }
  return (ret);
}

char *gen_next_term(char *s)
{
  int l;
  char *ret;
  if (strcmp(s,"1") == 0) {
    ret = Mymalloc(sizeof(char)*2);
    strcpy(ret,"1");
  }
  else if (strcmp(s,"0") == 0) {
    ret = Mymalloc(sizeof(char)*2);
    strcpy(ret,"0");
  }
  else {
    l = strlen(s);
    l += 7;
    ret = Mymalloc(sizeof(char)*l);
    strcpy(ret,"next(");
    strcat(ret, s);
    strcat(ret,")");
  }
  return(ret);
}

output_list *search_list(output_list *list, char *s)
{
  output_list *l;
  for (l = list; l != NULL; l = l->next) {
    if (strcmp(l->key, s) == 0) {
      return(l);
    }
  }
  return(NULL);
}

/* returns whether the string s is in the Define_List */
int define_check(char *s)
{
  if (search_list(Define_list, s)!=NULL) return(1);
  return(0);
}

void add_define_list(char *lit, char *s)
{
  output_list *l;
  l = mymalloc_output_list();
  l -> key = lit;
  l -> item = s;
  l -> next = Define_list;
  Define_list = l;
}  

int next_check(char *s)
{
  if (search_list(Next_list, s)!=NULL) return(1);
  return(0);
}

void add_next_list(char *lit, char *s)
{
  output_list *l;
  l = mymalloc_output_list();
  l -> key = lit;
  l -> item = s;
  l -> next = Next_list;
  Next_list = l;
}  

int fairness_check(char *s)
{
  if (search_list(Fairness_list, s)!=NULL) return(1);
  return(0);
}

void add_fairness_list(char *lit, char *s)
{
  output_list *l;
  l = mymalloc_output_list();
  l -> key = lit;
  l -> item = s;
  l -> next = Fairness_list;
  Fairness_list = l;
}  

int initially_check(char *s)
{
  if (search_list(Initially_list, s)!=NULL) return(1);
  return(0);
}

void add_initially_list(char *lit, char *s)
{
  output_list *l;
  l = mymalloc_output_list();
  l -> key = lit;
  l -> item = s;
  l -> next = Initially_list;
  Initially_list = l;
}  

/* return the name of the variable that represents the expression */
char *get_plit(node *t)
{
  char *ret;
  ret = ((t->pel != NULL) ? t->pel : t->el);
  return(ret);
}

/* populates the lists (Var_List, Define_List, etc) from the parse tree */
void set_node(node *t)
{
  char *fs;

  if (t == NULL) return;
  switch(t->op) {
  case '|':
    t->pel = reg_var(formula_string(t));
    break;
  case '!':
    t->pel = reg_var(formula_string(t));
    break;
  case 'X':
    t->el = reg_var(fs = formula_string(t));
    add_var_list(fs, t->el);
    break;
  case 'Y':
    t->el = reg_var(fs = formula_string(t));
    add_var_list(fs, t->el);
    break;
  case 'U':
    t->pel = reg_var(formula_string(t));
    t->el = reg_var(fs = op_string("X",formula_string(t), NULL));
    add_var_list(fs, t->el);
    break;
  case 'S':
    t->pel = reg_var(formula_string(t));
    t->el = reg_var(fs = op_string("Y",formula_string(t), NULL));
    add_var_list(fs, t->el);
    break;
  case 'l':
    t->el = t->str;
    break;
  default:
    ;
  }
  set_node(t->left);
  set_node(t->right);
  switch(t->op) {
  case '|':
    if (define_check(t->pel) == 0) {
      add_define_list(t->pel, op_string(" := ",
				t->pel, 
				op_string("|",
					  get_plit(t->left), 
					  get_plit(t->right))));
    }
    break;
  case '!':
    if (define_check(t->pel) == 0) {
      add_define_list(t->pel, op_string(" := ",
				t->pel, 
				op_string("!",
					  get_plit(t->left), NULL)));
    }
    break;
  case 'X':
    if (next_check(t->el) == 0) {
      add_next_list(t->el, op_string(" = ",
				     gen_next_term(get_plit(t->left)),
				     t->el));
    }
    break;
  case 'Y':
    if (next_check(t->el) == 0) {
      /* Note that the index is t->el as with X, but the generated 
         string has the terms in a different order */
      add_next_list(t->el, op_string(" = ",
				     get_plit(t->left),
				     gen_next_term(t->el)));
    }
    if (initially_check(t->el) == 0) {
      add_initially_list(t->el, op_string(" = ",
                                          t->el,
                                          "0"));
    }
    break;
  case 'U':
    if (define_check(t->pel) == 0) {
      add_define_list(t->pel, op_string(" := ",
				t->pel, 
				op_string("|",
					  get_plit(t->right), 
					  op_string("&",
						    get_plit(t->left),
						    t->el))));
							    
    }
    if (next_check(t->el) == 0) {
      add_next_list(t->el, op_string(" = ",
				     gen_next_term(get_plit(t)),
				     t->el));
    }
    if (fairness_check(t->pel) == 0) {
      add_fairness_list(t->pel, op_string("|",
				 op_string("!",t->pel,NULL),
				 get_plit(t->right)));
    }
    break;
  case 'S':
    if (define_check(t->pel) == 0) {
      add_define_list(t->pel, op_string(" := ",
				t->pel, 
				op_string("|",
					  get_plit(t->right), 
					  op_string("&",
						    get_plit(t->left),
						    t->el))));
							    
    }
    if (next_check(t->el) == 0) {
      add_next_list(t->el, op_string(" = ",
				     get_plit(t),
				     gen_next_term(t->el)));
    }


    if (initially_check(t->el) == 0) {
      add_initially_list(t->el, op_string(" = ",
                                          t->el,
                                          "0"));
    }
    break;
  default:
    ;
  }    
}

void init_str_hash(void )
{
  int i;

  Str_hash = (str_list **) Mymalloc(sizeof(str_list *) * STRHASHSIZE);
  for (i = 0; i < STRHASHSIZE; i++) {
    Str_hash[i] = NULL;
  }
}

void remove_outer_par(char *s)
{
  int l;
  l = strlen(s);
  if (s[0] == '(') s[0] = ' ';
  if (s[l-1] == ')') s[l-1] = ' ';
  return;
}

void print_smv_format(FILE * ofile)
{
  output_list *l;

  fprintf(ofile, "MODULE %s%d\n", MODULE_NAME, Spec_Counter);
  if (Var_list != NULL) {
    fprintf(ofile, "VAR\n");
    for (l = Var_list; l != NULL; l = l ->next) {
      /* printf("%s : boolean; -- %s", l->item, l->key); */
      fprintf(ofile, "   %s : boolean; ", l->item);
      fprintf(ofile, "\n");
    }
  }
  if (Initially_list != NULL) {
    for (l = Initially_list; l != NULL; l = l ->next) {
      fprintf(ofile, "INIT\n");
      remove_outer_par(l->item);
      fprintf(ofile, "  %s",l->item);
      fprintf(ofile, "\n");
    }
  }
  if (Next_list != NULL) {
    for (l = Next_list; l != NULL; l = l ->next) {
      fprintf(ofile, "TRANS\n");
      remove_outer_par(l->item);
      fprintf(ofile, "  %s",l->item);
      fprintf(ofile, "\n");
    }
  }
  if (Define_list != NULL) {
    fprintf(ofile, "DEFINE\n");
    for (l = Define_list; l != NULL; l = l ->next) {
      remove_outer_par(l->item);
      fprintf(ofile, "  %s;",l->item);
      fprintf(ofile, "\n");
    }
  }
  if (Fairness_list != NULL) {
    for (l = Fairness_list; l != NULL; l = l ->next) {
      fprintf(ofile, "FAIRNESS\n");
      remove_outer_par(l->item);
      fprintf(ofile, "  %s",l->item);
      fprintf(ofile, "\n");
    }
  }
  fprintf(ofile, "INIT\n");
  fprintf(ofile, "   %s\n", get_plit(Specf));
}

void set_information(node *t)
{
  init_str_hash();
  set_node(t);
}

node *reduce_formula(node *t)
{
  if (t == NULL) return (NULL);
  if (t->op == 'l') return (t);
  if (t->op == '!') {
    if (t -> left != NULL) {
      if (t->left->op =='!') {
	t = t->left->left;
      }
    }
    else {
      printf("Something is wrong in reduce_formula\n");
      exit(1);
    }
  }
  t->left = reduce_formula(t->left);
  t->right = reduce_formula(t->right);
  return (t);
}

int main(int argc, char **argv)
{
  FILE * input_file;
  FILE * output_file;
  
  if ((argc > 4) || (argc < 3)) {
    (void) fprintf(stderr, "%s: Converts an LTL formula to a fragment of an SMV program.\n", argv[0]);
    (void) fprintf(stderr, "%s: %s # <ifile> [<ofile>]\n", argv[0], argv[0]);
    (void) fprintf(stderr, "Where:\n\t#\t is the counter to be used in _LTL#_SPECF_N_.\n");
    (void) fprintf(stderr, "\t<ifile>\t is the file from which the LTL Formula to be translated\n\t\t is read in.\n");
    (void) fprintf(stderr, "\t<ofile>\t is the file in which the SMV code corresponding to the\n\t\t tableau of LTL Formula is written in.\n\t\t If not specified than stdout is used.\n");
    exit(1);
  }
  
  {
    char *err_occ[1];
    err_occ[0] = "";
    Spec_Counter = (int)strtol(argv[1], err_occ, 10);
    if (strcmp(err_occ[0], "") != 0) {
      (void) fprintf(stderr, "Error: \"%s\" is not a natural number.\n", err_occ[0]);
      exit(1);
    }
    if (Spec_Counter < 0) {
      (void) fprintf(stderr, "Error: \"%d\" is not a natural number.\n", Spec_Counter);
      exit(1);
    }
  }

  /* opening the input file */
  input_file = fopen(argv[2], "r");
  if (input_file == (FILE *)NULL) {
    (void) fprintf(stderr, "Error: Unable to open file \"%s\" for reading.\n", argv[2]);
    exit(1);
  } else {
    yyin = input_file;
  }

  if (yyparse()) {
    (void) fclose(input_file);
    (void) fprintf(stderr, "Error: Parsing error\n");
    exit(1);
  }

  if (argc == 4) {
    output_file = fopen(argv[3], "w");
    if (output_file == (FILE *)NULL) {
      (void) fclose(input_file);
      (void) fprintf(stderr, "Error: Unable to open file \"%s\" for writing.\n", argv[3]);
      exit(1);
    }
  } else {
    output_file = stdout;
  }
  
  
/* BEGIN COMMENT */
#ifdef ENABLE_COMMENTS_PRINT_OUT
  fprintf(output_file, "-- LTLSPEC ");
  print_formula(output_file, Specf);
  fprintf(output_file, "\n");
#endif
/* END COMMENT */

  Specf = expand_formula(Specf);

/* BEGIN COMMENT */
#ifdef ENABLE_COMMENTS_PRINT_OUT
  fprintf(output_file, "-- (expanded) ");
  print_formula(output_file, Specf);
  fprintf(output_file, "\n");
#endif
/* END COMMENT */

  Specf = reduce_formula(Specf);

/* BEGIN COMMENT */
#ifdef ENABLE_COMMENTS_PRINT_OUT
  fprintf(output_file, "-- (reduced) ");
  print_formula(output_file, Specf);
  fprintf(output_file, "\n");
#endif
/* END COMMENT */


  set_information(Specf);
  print_smv_format(output_file);

  fclose(input_file);
  if (argc == 4) fclose(output_file);
  return 0;
}

/* returns the number of digits of n */
static int get_digits(int n)
{
  int d;
  for (d = 1; n != 0; ++d)  n = n/10;
  return d;
}
