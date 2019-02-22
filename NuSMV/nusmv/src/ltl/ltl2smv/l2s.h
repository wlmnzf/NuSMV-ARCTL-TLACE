/**CHeaderFile*****************************************************************

  FileName    [l2s.h]

  PackageName [ltl2smv]

  Synopsis    [The LTL to SMV Translator]

  Description [This file provides the header file fot routines for
  reducing LTL model checking to CTL model checking. This work is
  absed on the work presented in \[1\]<br>

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
#ifndef _l2s_h_
#define _l2s_h_

#if HAVE_CONFIG_H
#include "config.h"
#endif 

typedef struct node_struct {
  char *str;
  char op;
  char *el;
  char *pel;
  int flag; 
  struct node_struct *left;
  struct node_struct *right;
} node;

typedef struct str_list_struct{
  char *name;
  char *str;
  struct str_list_struct *next;
} str_list;

typedef struct output_list_struct{
  char *key;
  char *item;
  struct output_list_struct *next;
} output_list;
int get_linenumber(void);
void inc_linenumber(void);
char *Mymalloc(unsigned size);
output_list *mymalloc_output_list(void);
void initbit(void);
void print_node(FILE * ofile, node * t);
void print_formula(FILE * ofile, node *t);
char *shortitob(long num);
char *itob(int num);
node *expand_formula(node *t);
char *op_string(char *op, char *str1, char *str2);
char *gen_name_number(int n);
int str_hash_func(char *s);
str_list *mymalloc_str_list(void);
void add_var_list(char *key, char *item);
str_list *mymalloc_str_list(void);
void add_var_list(char *key, char *item);
char *reg_var(char *str);
char *formula_string(node *t);
char *gen_next_term(char *s);
output_list *search_list(output_list *list, char *s);
int define_check(char *s);
void add_define_list(char *lit, char *s);
int next_check(char *s);
void add_next_list(char *lit, char *s);
int fairness_check(char *s);
void add_fairness_list(char *lit, char *s);
int initially_check(char *s);
void add_initially_list(char *lit, char *s);
char *get_plit(node *t);
void set_node(node *t);
void init_str_hash(void );
void remove_outer_par(char *s);
void print_smv_format(FILE * ofile);
void set_information(node *t);
node *reduce_formula(node *t);

int yywrap(void);
void yyerror(char *msg);
char *mycpy(char *text);
node *gen_leaf(char *str);
node *gen_node(char op, node *left, node *right);
node *mymalloc_node(void);
char *mystrconcat4(char *s1, char *s2, char *s3, char *s4);
char *mystrconcat3(char *s1, char *s2, char *s3);
char *mystrconcat2(char *s1, char *s2);
char *mystrconcat3ns(char *s1, char *s2, char *s3);

#endif
