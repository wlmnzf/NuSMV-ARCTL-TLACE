/**CFile***********************************************************************

  FileName    [nodeXMLPrint.c]

  PackageName [node]

  Synopsis    [Pretty printing of formulas represented using node struct,
  XML compliant.]

  Description [This file contains the code to perform pretty printing
  of a formula represented with a node struct, XML compliant.]

  SeeAlso     [node.c]

  Author      [Simon Busard]

  Copyright   [
  This file is part of the ``node'' package of NuSMV version 2.
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

#include "nodeInt.h"
#include "utils/ustring.h"

#define LINE_LENGTH 100000


/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/
static int print_node_recur ARGS((out_func_t out_func, void *stream, node_ptr n, int p, custom_print_node_t pnc));
static int print_case_body ARGS((out_func_t out_func, void *stream, node_ptr n, custom_print_node_t pnc));
static int print_case ARGS((out_func_t out_func, void *stream, node_ptr n, custom_print_node_t pnc));
static out_func_t util_streamprint;
static out_func_t util_stringprint;
/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Pretty print a formula on a file, XML compliant]

  Description        [Pretty print a formula on a file, XML compliant]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int print_node_XML(FILE *stream, node_ptr n)
{ return print_node_custom_XML(stream,n,(custom_print_node_t *)NULL); }

/**Function********************************************************************

  Synopsis           [Customizable version of print_node_XML]

  Description        [This functions allows handling of unknown node types
  (for example, SMV extensions). If an external recursion function is given 
  (of type custom_print_node_t), when an unknown node type is found this
  function is called and is expected to correctly handle the unknown node
  type: to print the node via out_func and to end recursion on terminal nodes,
  xor to return arity, bracketing and priority information to continue
  recursion on non-terminal ones.]

  SideEffects        []

  SeeAlso            [print_node_XML]

******************************************************************************/
int print_node_custom_XML(FILE *stream, node_ptr n, custom_print_node_t pnc)
{
  print_node_recur(util_streamprint, stream, n, 0, pnc);
}

/**Function********************************************************************

  Synopsis           [Pretty print a formula into a string, XML compliant]

  Description        [Pretty print a formula into a string, XML compliant]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
char *sprint_node_XML(node_ptr n)
{
  char * str = ALLOC(char,1);
  str[0] = '\0';
  print_node_recur(util_stringprint,&str,n,0,NULL); 
  return str;
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [print string on stream.]

  Description        [print string on stream.]

  SideEffects        []

******************************************************************************/
int util_streamprint(void *stream, char *s1)
{
  if (*s1){
    return(fprintf((FILE *)stream,s1));
  }
  else
    return 1;
}

/**Function********************************************************************

  Synopsis           [append string to string.]

  Description        [append string to string.]

  SideEffects        []

******************************************************************************/
static int util_stringprint(void *string, char *s1)
{
  char *s0 = *((char **)string);
  char *sr = ALLOC(char, strlen(s0)+strlen(s1)+1);
  
  strcpy(sr,s0);
  strcat(sr,s1);

  FREE(s0);
  *((char **)string) = sr;
  
  return(1);
}

/**Function********************************************************************

  Synopsis           [Recursive step of print_node_atcol (XML compliant)]

  Description        [Recursive step of print_node_atcol (XML compliant)]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static int print_node_recur(out_func_t out_func, void * stream, node_ptr n, int p, custom_print_node_t pnc)
{
  char *op = 0;
  int priority = 0;
  int arity = 0;     /* 0: unary, 1: binary, 2: terciary, 3:quad */
  /* Franco (02/08/2004): I'm using arity 10 to denote arity = 2,
  post-fix (for EAX (act) (formula) and others), and arity 11 for EAU
  and AAU  */
  int brckts = 0;
  if(n == Nil) return(1);
  if(n == (node_ptr)(-1)) return(out_func(stream, "*no value*"));

  switch (node_get_type(n)) {
  case TRUEEXP:
    return(out_func(stream, "TRUE"));
  case FALSEEXP:
    return(out_func(stream, "FALSE"));
  case SELF:
    return(out_func(stream, "self"));
  case START:
    return(out_func(stream, "START"));
  case ATOM:
    if(!out_func(stream, get_text((string_ptr)car(n))))return(0);
    if(cdr(n)){
      char buf[20];

      sprintf(buf, "_%d", (int)cdr(n));
      return(out_func(stream, buf));
    }
    return(1);
  case NUMBER:
    {
      char buf[20];

      sprintf(buf,"%d", (int)car(n));
      return(out_func(stream, buf));
    }
  case DOT:
    if(car(n) == Nil) return(print_node_recur(out_func, stream, cdr(n), 0, pnc));
    return(print_node_recur(out_func, stream, car(n), 0, pnc)
     && out_func(stream, ".")
     && print_node_recur(out_func, stream, cdr(n),0,pnc));
  case BIT:
    {
      char buf[30];
      sprintf(buf,".%d", (int)cdr(n));
      return(print_node(stream, car(n)) && util_streamprint(stream, buf));
    }
  case CONTEXT:
    return (print_node_recur(out_func, stream, cdr(n), 0, pnc) && 
	    (car(n) != Nil) && out_func(stream, " IN ") && 
	    print_node_recur(out_func, stream, car(n), 0, pnc));
      
  case CONS:
    return(print_node_recur(out_func, stream, car(n),0, pnc)
     && ((cdr(n) == Nil)
         || (out_func(stream, " , ")
       && print_node_recur(out_func, stream, cdr(n), 0, pnc))));
  case CASE:
    return(print_case(out_func, stream, n, pnc));
  case ARRAY:
    return(print_node_recur(out_func, stream, car(n), 0, pnc)
     && out_func(stream, "[")
     && print_node_recur(out_func, stream, cdr(n), 0, pnc)
     && out_func(stream, "]"));
  case TWODOTS: op = ".."; priority = 3; arity = 1; break;
  case IMPLIES: op = "-&gt;"; priority = 4; arity = 1; p = 5; break;
  case IFF: op = "&lt;-&gt;"; priority = 4; arity = 1; p = 5; break;
  case OR: op = "|"; priority = 5; arity = 1;  p = 6; break;
  case AND: op = "&amp;"; priority = 6; arity = 1; p = 7; break;
  case XOR: op = "xor"; priority = 4; arity = 1; p = 5; break;
  case XNOR: op = "xnor"; priority = 4; arity = 1; p = 5; break;
  case NOT: op = "!"; priority = 7; arity = 0; break;
  case VAR: op = "VAR "; priority = 8 ; arity = 0; break;
  case IVAR: op = "IVAR "; priority = 8 ; arity = 0; break;
  case EX: op = "EX "; priority = 8; arity = 0; break;
  case AX: op = "AX "; priority = 8; arity = 0; break;
  case EF: op = "EF "; priority = 8; arity = 0; break;
  case AF: op = "AF "; priority = 8; arity = 0; break;
  case EG: op = "EG "; priority = 8; arity = 0; break;
  case AG: op = "AG "; priority = 8; arity = 0; break;
  case OP_NEXT: op = " X "; priority = 8; arity = 0; break;
  case OP_PREC: op = " Y "; priority = 8; arity = 0; break;
  case OP_NOTPRECNOT: op = " Z "; priority = 8; arity = 0; break;
  case OP_GLOBAL: op = " G "; priority = 8; arity = 0; break;
  case OP_HISTORICAL: op = " H "; priority = 8; arity = 0; break;
  case OP_FUTURE: op = " F "; priority = 8; arity = 0; break;
  case OP_ONCE: op = " O "; priority = 8; arity = 0; break;
  case UNTIL: op = "U"; priority = 8; p = 9; arity = 1; break;
  case SINCE: op = "S"; priority = 8; p = 9; arity = 1; break;
  case RELEASES: op = "V"; priority = 8; p = 9; arity = 1; break;
  case TRIGGERED: op = "T"; priority = 8; p = 9; arity = 1; break;
  case EU:
    if(!out_func(stream, "E")) return(0);
    op = "U"; priority = 8; p = 9; arity = 1; brckts = 1; break;
  case AU:
    if(!out_func(stream, "A")) return(0);
    op = "U"; priority = 8; p = 9; arity = 1; brckts = 1; break;
  /* 01/08/11 sbusard */
  case EW:
    if(!out_func(stream, "E")) return(0);
    op = "W"; priority = 8; p = 9; arity = 1; brckts = 1; break;
  case AW:
    if(!out_func(stream, "A")) return(0);
    op = "W"; priority = 8; p = 9; arity = 1; brckts = 1; break;
    /* Franco (02/08/2004) added the following three lines */
  case EAX:
    op = "EAX"; priority = 8; p = 9; arity = 10; brckts = 1; break;
    /* Franco (02/08/2004) added the following three lines */
  case EAU:
    op = "EA"; priority = 8; p = 9; arity = 11; brckts = 1; break;
  /* 04/08/11 sbusard */
  case EAW:
      op = "EA"; priority = 8; p = 9; arity = 12; brckts = 1; break;
    /* Franco (05/08/2004): using form similar to EAX for AAX and EAF*/ 
  case AAX:
    op = "AAX"; priority = 8; p = 9; arity = 10; brckts = 1; break;
  case EAF:
    op = "EAF"; priority = 8; p = 9; arity = 10; brckts = 1; break;
  case AAF:
    op = "AAF"; priority = 8; p = 9; arity = 10; brckts = 1; break;
  case EAG:
    op = "EAG"; priority = 8; p = 9; arity = 10; brckts = 1; break;
  case AAG:
    op = "AAG"; priority = 8; p = 9; arity = 10; brckts = 1; break;
    /* Franco (05/08/2004) Support for AAU */
  case AAU:
    op = "AA"; priority = 8; p = 9; arity = 11; brckts = 1; break;
  /* 04/08/11 sbusard */
  case AAW:
      op = "AA"; priority = 8; p = 9; arity = 12; brckts = 1; break;
    /* Franco: (06/08/2004): REACHABLE similar to AX etc. */
  case REACHABLE: op = "REACHABLE "; priority = 8; arity = 0; break;

  case EBU:
    if(!out_func(stream, "E")) return(0);
    op = "BU"; priority = 8; p = 9; arity = 3; brckts = 1; break;
  case ABU:
    if(!out_func(stream, "A")) return(0);
    op = "BU"; priority = 8; p = 9; arity = 3; brckts = 1; break;
  case EBF: op = "EBF "; priority = 8; arity = 2; break;
  case ABF: op = "ABF "; priority = 8; arity = 2; break;
  case EBG: op = "EBG "; priority = 8; arity = 2; break;
  case ABG: op = "ABG "; priority = 8; arity = 2; break;
  case MINU:
    if(!out_func(stream, "MIN")) return(0);
    op = ","; priority = 8; p = 9; arity = 1; brckts = 1; break;
  case MAXU:
    if(!out_func(stream, "MAX")) return(0);
    op = ","; priority = 8; p = 9; arity = 1; brckts = 1; break;
  case EQUAL: op = "="; priority = 9; arity = 1; break;
  case NOTEQUAL: op = "!="; priority = 9; arity = 1; break;
  case LT:    op = "&lt;"; priority = 9; arity = 1; break;
  case GT:    op = "&gt;"; priority = 9; arity = 1; break;
  case LE:    op = "&lt;="; priority = 9; arity = 1; break;
  case GE:    op = "&gt;="; priority = 9; arity = 1; break;
  case UNION: op = "union"; priority = 10; arity = 1; break;
  case SETIN: op = "in"; priority = 11; arity = 1; break;
  case MOD:   op = "mod"; priority = 12; arity = 1; break;
  case PLUS:  op = "+"; priority = 13; arity = 1; break;
  case MINUS: op = "-"; priority = 13; arity = 1; break;
  case TIMES: op = "*"; priority = 14; arity = 1; break;
  case DIVIDE: op = "/"; priority = 14; arity = 1; break;
  case EQDEF: op = ":="; priority = 1; arity = 1; break;
  case NEXT:
    if (!out_func(stream, "next")) return(0);
    op = ""; priority = 0; p = 1; arity = 0; break;
  case SMALLINIT:
    if (!out_func(stream, "init")) return(0);
    op = ""; priority = 0; p = 1; arity = 0; break;
  default:
    if(pnc==(custom_print_node_t *)NULL || node_get_type(n)<=TOKEN_NUSMV_MAX){
      internal_error("fprint_node_recur: type = %d", node_get_type(n));
    } else {
      int retval;
      retval=pnc(out_func,stream,n,&p,&op,&priority,&arity,&brckts);
      /* External function evals to -1 iff it has reached terminal nodes
	 (external recursion termination). */
      if(retval==-1) return retval;
    }

  }


  if (brckts == 1 && priority < p && !out_func(stream, " [ ")) return(0);
  if (brckts == 0 && priority < p && !out_func(stream, "(")) return(0);
  switch(arity){
  case 0:
    if (!out_func(stream, op))return(0);
    if (!print_node_recur(out_func, stream, car(n), priority, pnc)) return(0);
    break;
  case 1:
    if (!print_node_recur(out_func, stream, car(n), priority, pnc)) return(0);
    if (!out_func(stream, " ")) return(0);
    if (!out_func(stream, op)) return(0);
    if (!out_func(stream, " ")) return(0);
    if (!print_node_recur(out_func, stream, cdr(n), priority, pnc))return(0);
    break;
  case 2:
    /* EF a..b f */
    if (!out_func(stream, op)) return(0);                /* EF */
    if (!print_node_recur(out_func, stream, car(cdr(n)), priority, pnc))
                                       return(0);         /* a */
    if (!out_func(stream, "..")) return(0);
    if (!print_node_recur(out_func, stream, cdr(cdr(n)), priority, pnc))
                                       return(0);         /* b */
    if (!out_func(stream, " ")) return(0);
    if (!print_node_recur(out_func, stream, car(n), priority, pnc)) return(0); /* f */
    break;
  case 3:
    /* E[f BU a..b g] */
    if (!print_node_recur(out_func, stream, car(car(n)), priority, pnc))
                                       return(0);         /* f */
    if (!out_func(stream, " ")) return(0);
    if (!out_func(stream, op)) return(0);                /* BU */
    if (!out_func(stream, " ")) return(0);
    if (!print_node_recur(out_func, stream, car(cdr(n)), priority, pnc))
                                       return(0);         /* a */
    if (!out_func(stream, "..")) return(0);
    if (!print_node_recur(out_func, stream, cdr(cdr(n)), priority, pnc))
                                       return(0);         /* b */
    if (!out_func(stream, " ")) return(0);
    if (!print_node_recur(out_func, stream, cdr(car(n)), priority, pnc))
                                       return(0);         /* g */
    break;
  case 10:
    /* Franco (02/08/2004) */
    /* EAX act f */
    if (!out_func(stream, op)) return(0);
    if (!out_func(stream, " ")) return(0);
    if (!print_node_recur(out_func, stream, car(n), priority, pnc)) return(0);
    if (!out_func(stream, " ")) return(0);
    if (!print_node_recur(out_func, stream, cdr(n), priority, pnc))return(0);
    break;
  case 11:
    /* Franco (03/08/2004) */
    /* EAU act f g*/
    if (!out_func(stream, op)) return(0);
    if (!out_func(stream, " ")) return(0);
    if (!print_node_recur(out_func, stream, car(n), priority, pnc)) return(0);
    if (!out_func(stream, " ")) return(0);
    if (!print_node_recur(out_func, stream, car(cdr(n)), priority,
  pnc))return(0);
    if (!out_func(stream, " U ")) return(0);
    if (!print_node_recur(out_func, stream, cdr(cdr(n)), priority, pnc))return(0);
    break;
  /* 04/08/11 sbusard */
  case 12:
      if (!out_func(stream, op)) return(0);
      if (!out_func(stream, " ")) return(0);
      if (!print_node_recur(out_func, stream, car(n), priority, pnc)) return(0);
      if (!out_func(stream, " ")) return(0);
      if (!print_node_recur(out_func, stream, car(cdr(n)), priority,
    pnc))return(0);
      if (!out_func(stream, " W ")) return(0);
      if (!print_node_recur(out_func, stream, cdr(cdr(n)), priority, pnc))return(0);
      break;
  }
  if (brckts == 0 && priority < p && !out_func(stream, ")")) return(0);
  if (brckts == 1 && priority < p && !out_func(stream, " ] ")) return(0);
  return(1);
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int print_case(out_func_t out_func, void *stream, node_ptr n, custom_print_node_t pnc){
  return(out_func(stream, "case\n") &&
         print_case_body(out_func, stream, n, pnc) &&
         out_func(stream, "esac"));
}

/**Function********************************************************************

  Synopsis           [required]

  Description        [optional]

  SideEffects        [required]

  SeeAlso            [optional]

******************************************************************************/
static int print_case_body(out_func_t out_func, void *stream, node_ptr n, custom_print_node_t pnc){
  int res = 0;

  nusmv_assert(n != Nil);
  res = print_node_recur(out_func, stream, car(car(n)), 0, pnc) &&
        out_func(stream, " : ")      &&
        print_node_recur(out_func, stream, cdr(car(n)), 0, pnc) &&
        out_func(stream, ";\n");
  if (res != 0) {
    /* This case has been inserted to keep track of BDD printing */
    if ((cdr(n) != Nil) && (node_get_type(cdr(n)) == FALSEEXP)) {
      res = out_func(stream, " 1 : ") &&
            print_node_recur(out_func, stream, cdr(n), 0, pnc)   &&
            out_func(stream, " ;\n");
    }
    else {
      if ((cdr(n) != Nil) && (node_get_type(cdr(n)) != TRUEEXP)) {
        res = print_case_body(out_func, stream, cdr(n), pnc);
      }
    }
  }
  return(res);
}

