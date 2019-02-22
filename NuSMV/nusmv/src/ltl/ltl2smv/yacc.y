/**CFile***********************************************************************

  FileName    [yacc.y]

  PackageName [ltl2smv]

  Synopsis    [The LTL to SMV Translator -- LTL parser]

  Description [This file provides routines for the parsing of LTL formulas.]

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
%{
#include <stdio.h>
#include "l2s.h"

extern node *Specf;      /* spec formula */
extern char yytext[];

%}
%expect 123
%start ltlformula

%token OR AND IMPLY EQUIV NEG TWODOTS
%token FUTURE GLOBAL UNTIL RELEASES NEXT 
%token ONCE HISTORY SINCE TRIGGERED PREV NOTPREVNOT
%token PLUS MINUS TIMES DIVIDE MOD EQUAL NOTEQUAL
%token LE GE LT GT UNION SETIN UMINUS
%token ATOM NUMBER CASE ESAC

%right IMPLY
%left  EQUIV
%left  OR 
%left  XOR 
%left  XNOR 
%left  AND
%left  NEG 
%left  NEXT FUTURE GLOBAL UNTIL RELEASES
%left  UNION
%left  SETIN
%left  MOD
%left  PLUS MINUS
%left  TIMES DIVIDE
%left  UMINUS		/* supplies precedence for unary minus */
%left  CASE ESAC

%union  {
  node *node_ptr;
  char *str_ptr;
      }
%type  <node_ptr>  ltlf orexpr andexpr atomexpr untilexpr case_body
%type  <str_ptr>   term number constant neatomset smvexpr ATOM NUMBER

%%

ltlformula : ltlf {Specf = $1;}
           ;
ltlf       : orexpr {$$ = $1;}
           | orexpr IMPLY ltlf {$$ = gen_node('>',$1,$3);}
           | ltlf EQUIV orexpr {$$ = gen_node('=',$1,$3);}
           | ltlf XOR orexpr   {$$ = gen_node('x',$1,$3);}
           | ltlf XNOR orexpr  {$$ = gen_node('n',$1,$3);}
           ;
case_body  : {$$ = gen_leaf(mycpy("1"));}
           | ltlf ':' ltlf ';' case_body
             { $$ = gen_node('c', gen_node(':', $1, $3), $5); }
           ;
orexpr     : andexpr {$$ = $1;}
           | orexpr OR andexpr {$$ = gen_node('|',$1,$3);}
           ;
andexpr    : untilexpr {$$ = $1;}
           | andexpr AND untilexpr {$$ = gen_node('&',$1,$3);}
           ;
untilexpr  : atomexpr {$$ = $1;}
           | untilexpr UNTIL atomexpr {$$ = gen_node('U',$1,$3);}
           | untilexpr SINCE atomexpr {$$ = gen_node('S',$1,$3);}
           | untilexpr RELEASES atomexpr {$$ = gen_node('V',$1,$3);}
           | untilexpr TRIGGERED atomexpr {$$ = gen_node('T',$1,$3);}
           ;
atomexpr   : NEG atomexpr {$$ = gen_node('!',$2,NULL);}
           | NEXT atomexpr {$$ = gen_node('X',$2,NULL);}
           | GLOBAL atomexpr {$$ = gen_node('G',$2,NULL);}
           | FUTURE atomexpr {$$ = gen_node('F',$2,NULL);}
           | PREV atomexpr {$$ = gen_node('Y',$2,NULL);}
           | NOTPREVNOT atomexpr {$$ = gen_node('Z',$2,NULL);}
           | HISTORY atomexpr {$$ = gen_node('H',$2,NULL);}
           | ONCE atomexpr {$$ = gen_node('O',$2,NULL);}
           | '(' ltlf ')' {$$ = $2;}
           | CASE case_body ESAC {$$ = gen_node('c',$2,NULL);}
           | smvexpr {$$ = gen_leaf($1);}
           ;

term       : ATOM {$$ = $1;}
           | term '[' smvexpr ']' {$$ = mystrconcat4($1,"[",$3,"]");}
           | term '.' term {$$ = mystrconcat3ns($1,".",$3);}
           | term '.' number {$$ = mystrconcat3ns($1,".",$3);}
           ;

number     : NUMBER {$$ = $1;}
	   | PLUS NUMBER %prec UMINUS
	     { $$ = mystrconcat2("+",$2);}
           | MINUS NUMBER %prec UMINUS
	     { $$ = mystrconcat2("-",$2);}
           ;

constant   : ATOM {$$ = $1;}
           | number {$$ = $1;}
           ;

neatomset  : constant
           | neatomset ',' constant {$$ = mystrconcat3($1,",",$3);}
           ; 

smvexpr    : term {$$ = $1;}
           | number {$$ = $1;}
           | '(' smvexpr ')' {$$ = mystrconcat3("(",$2,")");}
           | smvexpr PLUS smvexpr { $$ = mystrconcat3($1,"+",$3); }
           | smvexpr MINUS smvexpr { $$ = mystrconcat3($1,"-",$3); }
           | smvexpr TIMES smvexpr { $$ = mystrconcat3($1,"*",$3); }
           | smvexpr DIVIDE smvexpr { $$ = mystrconcat3($1,"/",$3); }
           | smvexpr MOD smvexpr { $$ = mystrconcat3($1,"mod",$3); }
           | smvexpr EQUAL smvexpr { $$ = mystrconcat3($1,"=",$3); }
           | smvexpr NOTEQUAL smvexpr { $$ = mystrconcat3($1,"!=",$3); }
           | smvexpr LT smvexpr { $$ = mystrconcat3($1,"<",$3); }
           | smvexpr GT smvexpr { $$ = mystrconcat3($1,">",$3); }
           | smvexpr LE smvexpr { $$ = mystrconcat3($1,"<=",$3); }
           | smvexpr GE smvexpr { $$ = mystrconcat3($1,">=",$3); }
           | '{' neatomset '}' { $$ = mystrconcat3("{",$2,"}"); } 
           | number TWODOTS number { $$ = mystrconcat3($1,"..",$3); }
           | smvexpr UNION smvexpr { $$ = mystrconcat3($1,"union",$3); }
           | smvexpr SETIN smvexpr { $$ = mystrconcat3($1,"in",$3); }
           ;

%%
#if 0
#include "lex.yy.c"
#endif
char *mystrconcat2(char *s1, char *s2)
{
  char *ret;
  int len;
  len = strlen(s1);
  len += strlen(s2);
  ret = (char *)malloc(sizeof(char)*(len+1));
  strcpy(ret,s1);
  strcat(ret,s2);
  return(ret);
}

char *mystrconcat3(char *s1, char *s2, char *s3)
{
  char *ret;
  int len;
  len = strlen(s1);
  len += strlen(s2);
  len += strlen(s3);
  ret = (char*)malloc(sizeof(char)*(len+3));
  strcpy(ret,s1);
  strcat(ret," ");
  strcat(ret,s2);
  strcat(ret," ");
  strcat(ret,s3);
  return(ret);
}

char *mystrconcat3ns(char *s1, char *s2, char *s3)
{
  char *ret;
  int len;
  len = strlen(s1);
  len += strlen(s2);
  len += strlen(s3);
  ret = (char*)malloc(sizeof(char)*(len+1));
  strcpy(ret,s1);
  strcat(ret,s2);
  strcat(ret,s3);
  return(ret);
}

char *mystrconcat4(char *s1, char *s2, char *s3, char *s4)
{
  char *ret;
  int len;
  len = strlen(s1);
  len += strlen(s2);
  len += strlen(s3);
  len += strlen(s4);
  ret = (char*)malloc(sizeof(char)*(len+1));
  strcpy(ret,s1);
  strcat(ret,s2);
  strcat(ret,s3);
  strcat(ret,s4);
  return(ret);
}

node *mymalloc_node(void)
{
  node *ret;
  ret = (node *)Mymalloc(sizeof(node));
  ret -> str = (char *)NULL;
  ret -> op = (char)NULL;
  ret -> el = (char *)NULL;
  ret -> pel = (char *)NULL;
  ret -> flag = 0;
  ret -> left = (node *)NULL;
  ret -> right = (node *)NULL;
  return(ret);
}

node *gen_node(char op, node *left, node *right)
{
  node *ret;
  ret = mymalloc_node();
  ret -> op = op;
  ret -> left = left;
  ret -> right = right;
  return(ret);
}

node *gen_leaf(char *str)
{
  node *ret;
  ret = mymalloc_node();
  ret -> op = 'l';
  ret -> str = mystrconcat3("(",str,")");
  return (ret);
}

char *mycpy(char *text)
{
  char *cptr;
  cptr = (char *) Mymalloc(strlen(text) + 1);
  /*  printf("string = %s, length = %d\n", text,strlen(text)); */
  strcpy(cptr, text);
  return(cptr);
}

void yyerror(char *msg)
{
  (void) fprintf(stderr, "%s\n", msg);
  (void) fprintf(stderr, "%s : Line %d\n", yytext, get_linenumber());
  /* exit(1); */
}

int yywrap(void)
{
  return(1);
}
