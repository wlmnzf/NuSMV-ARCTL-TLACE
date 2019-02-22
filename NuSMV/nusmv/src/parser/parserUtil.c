/**CFile***********************************************************************

  FileName    [parserUtil.c]

  PackageName [parser]

  Synopsis    [Parser utilities]

  Description [This file contains some parser utilities that allows
  for example to parse from a string, instead that from a file.]

  SeeAlso     []

  Author      [Marco Roveri]

  Copyright   [
  This file is part of the ``parser'' package of NuSMV version 2. 
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

#include "parserInt.h" 
#include "utils/error.h"

static char rcsid[] UTIL_UNUSED = "$Id: parserUtil.c,v 1.7.4.6 2004/05/13 12:30:54 nusmv Exp $";


/*---------------------------------------------------------------------------*/
/* Macro definitions                                                         */
/*---------------------------------------------------------------------------*/

#define YY_BUF_SIZE 16384


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

static char* tmpfname1;
static char* tmpfname2;

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Open a file and inform the parser to read from it]

  Description        [Open a file and inform the parser to start
  reading tokens from this file. If no input file is provided, then it
  inform the parser to start reading tokens from the standard input.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void Parser_OpenInput(const char *filename)
{
  if (filename != (char *)NULL) {
    if(!(yyin = fopen(filename,"r")))
      rpterr("cannot open %s for input",filename);
    yylineno = 1;
  }
  if (yyin == NULL) yyin = stdin;
  /* Flushes the current input buffer */
  (void) yy_switch_to_buffer(yy_create_buffer(yyin, 16384));
  /*                                                ^^^^^ */
  /* Taken from the input.c flex generated file.          */
  (void) yyrestart(yyin);
}

/**Function********************************************************************

  Synopsis           [Open a file and inform the parser to read from it]

  Description        [Open a file, pre-process it, and inform the parser to
start reading tokens from this file. The directory in which the original file
resides is used to store the temporary files. This is so that any #includes
statements used by the second or later pre-processor work properly.]

  SideEffects        [Creates temporary files which are subsequently deleted.]

  SeeAlso            []

******************************************************************************/
void Parser_OpenInputPP(const char* filename)
{
# define STDIN_OPT_NAME " -"

  char c;

  char* pp_curr;
  char* pp_next;
  char* pp_cmd;
  char* pp_exec;

  char* var;
  char* tmp_dir;
  char* tmp_file;
  char* path;

  const char* tmp1;
  int tmp_len;

  FILE* tempstream1;
  FILE* tempstream2;

  char* pp_list = get_pp_list(options);
  char pp_list_copy[strlen(pp_list) + 1];

  /* Set tmpfname1 to initial file to use as input */
  if (filename != (char*) NULL) {
    tmpfname1 = ALLOC(char, strlen(filename) + 1);
    strcpy(tmpfname1, filename);
  }

  /* Get path to initial file */
  tmp_len = strlen(filename) - strlen(Utils_StripPath(filename));
  path = ALLOC(char, tmp_len + 1);
  strncpy(path, filename, tmp_len);
  path[tmp_len] = '\0';

  strcpy(pp_list_copy, pp_list);

  pp_curr = strtok(pp_list_copy, " ");
  pp_next = strtok(NULL, " ");

  /* Need to indicate that tmpfname1 refers to the original input file
     so that it does not get deleted when Parser_ClosePP() is called.
     This is done by making tmpfname2 not NULL */
  if (pp_next == NULL) {
    tmpfname2 = ALLOC(char, 2);
    strcpy(tmpfname2, "x");
  }

  while (pp_next != NULL) {
    if (tmpfname1 != (char*) NULL) {
      /* Test whether it is possible to open the file */
      if(!(tempstream1 = fopen(tmpfname1,"r")))
        rpterr("cannot open %s for input",tmpfname1);
     (void) fclose(tempstream1);
    }

    if (opt_verbose_level_gt(options, 1)) {
      fprintf(nusmv_stdout, "Calling %s preprocessor\n", pp_curr);
    }
    pp_exec = get_preprocessor_call(pp_curr);

    if (tmpfname1 != (char*)NULL) {
      pp_cmd = ALLOC(char, strlen(pp_exec) + strlen(tmpfname1) + 2);
    }
    else {
      pp_cmd = ALLOC(char, strlen(pp_exec) + strlen(STDIN_OPT_NAME) + 1);
    }

    if (pp_cmd == NIL(char)) {
      fprintf(nusmv_stderr, "Parser_OpenInputPP: unable to allocate memory\n");
      nusmv_exit(1);
    }

    strcpy(pp_cmd, pp_exec);
    if (tmpfname1 != (char*)NULL) {
      strcat(pp_cmd, " ");
      strcat(pp_cmd, tmpfname1);
    }
    else { strcat(pp_cmd, STDIN_OPT_NAME); }
    yylineno = 1;

    if (opt_verbose_level_gt(options, 2)) {  
      fprintf(nusmv_stderr, "\nInvoking the command: '%s'...\n", pp_cmd);
    }
    /* Invoke command */
    if ( !(tempstream1 = popen(pp_cmd, "r")) ) {
      rpterr("error executing command \"%s\"", pp_cmd);
    }

    /* Get unique temporary filename */
    tmp_file = Utils_get_temp_filename_in_dir(path, "NuSMVXXXXXX");
    if (tmp_file == NULL) {
      rpterr("Unable to generate unique temporary file. (Previously generated temporary file: %s)\n", tmpfname1);
    }

    tmpfname2 = ALLOC(char, strlen(path) + 12);
    strcpy(tmpfname2, path);
    strcat(tmpfname2, tmp_file);
    FREE(tmp_file);

    tempstream2 = fopen(tmpfname2, "w");

    while (feof(tempstream1) == 0) {
      c = getc(tempstream1);
      if (c != EOF) {putc(c, tempstream2);}
    }
    (void) pclose(tempstream1);
    (void) fclose(tempstream2);

    if ((strcmp(tmpfname1, filename) != 0) && (tempstream1 = fopen(tmpfname1,"r"))) {
      if (remove(tmpfname1) == -1) {
        rpterr("error deleting temporary file \"%s\"", tmpfname1);
      }
      (void) fclose(tempstream1);
    }

    FREE(tmpfname1);
    tmpfname1 = ALLOC(char, strlen(tmpfname2) + 1);
    strcpy(tmpfname1, tmpfname2);
    FREE(tmpfname2);

    FREE(pp_cmd);

    pp_curr = pp_next;
    pp_next = strtok(NULL, " ");
  }

  pp_exec = get_preprocessor_call(pp_curr);

  /* Set yyin to result of running last pre-processor */
  if (filename != (char*) NULL) {
    pp_cmd = ALLOC(char, strlen(pp_exec) + strlen(tmpfname1) + 2);
    strcpy(pp_cmd, pp_exec);
    strcat(pp_cmd, " ");
    strcat(pp_cmd, tmpfname1);
  }
  else {
    pp_cmd = ALLOC(char, strlen(STDIN_OPT_NAME) + 1);
    strcpy(pp_cmd, pp_exec);
    strcat(pp_cmd, STDIN_OPT_NAME);
  }

  if (opt_verbose_level_gt(options, 1)) {
    fprintf(nusmv_stdout, "Calling %s preprocessor\n", pp_curr);
  }
  if (opt_verbose_level_gt(options, 2)) {  
    fprintf(nusmv_stderr, "\nInvoking the command: '%s'...\n", pp_cmd);
  }
  if( !(yyin = popen(pp_cmd, "r")) ) {
    rpterr("error executing command \"%s\"", pp_cmd);
  }

  FREE(pp_cmd);

  /* Flushes the current input buffer */
  (void) yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
  (void) yyrestart(yyin);
}

/**Function********************************************************************

  Synopsis           [Close the input file]

  Description        [Closes the input file used from parser to read tokens.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
void Parser_CloseInput()
{
  (void) fclose(yyin);
}

/**Function********************************************************************

  Synopsis           [Close the input file]

  Description        [Closes the input file used from parser to read tokens.]

  SideEffects        [Deletes any temporary files created by Parser_OpenInputPP.]

  SeeAlso            []

******************************************************************************/
void Parser_CloseInputPP()
{
  FILE* stream;

  if (tmpfname2 != NULL) {
    /* tmpfname refers to original input file */
    FREE(tmpfname2);
  }
  else {
    if (stream = fopen(tmpfname1,"r")) {
      if (remove(tmpfname1) == -1) {
	rpterr("error deleting file \"%s\"", tmpfname1);
      }
      (void) fclose(stream);
    }
  }

  FREE(tmpfname1);

  (void) pclose(yyin);
}

/**Function********************************************************************

  Synopsis           [Parse SMV code from a given file.]

  Description        [Parse SMV code from a given file. If 
  no file is provided, parse from stdin. If a parsing error occurs then
  return 1, else return 0. The result of parsing is stored in
  the global variable <tt>parse_tree</tt> to be used from the caller.]

  SideEffects        []

  SeeAlso            []

*****************************************************************************/
int Parser_ReadSMVFromFile(const char *filename)
{
  int retval = 0;
  if (strcmp(get_pp_list(options), "") != 0) {
    Parser_OpenInputPP(filename);
  } 
  else {
    Parser_OpenInput(filename);
  }

  parse_tree = Nil;
  parse_command_flag = 0;

  if (yyparse()) {
    retval = 1;
  } 
  else {
    yylineno = 0;
    parse_tree = reverse(parse_tree);
  }

  if (strcmp(get_pp_list(options), "") != 0) {
    Parser_CloseInputPP(filename);
  } 
  else {
    Parser_CloseInput();
  }

  return retval;
}

/**Function********************************************************************

  Synopsis           [Parse a comand from a given string.]

  Description        [Create a string for a command, and then call
  <tt>yyparse</tt> to read from the created string. 
  If a parsing error occurs than return 1, else return 0. 
  The result of parsing is stored in <tt>pc</tt> to be used from the caller.]

  SideEffects        []

  SeeAlso            []

*****************************************************************************/
int Parser_ReadCmdFromString(int argc, char **argv, char * head, char *tail, node_ptr *pc)
{
  int i;
  char * old_input_file;
  int l = strlen(head);
  int status = 0;
  char * cmd = NIL(char);
  
  for (i = 1; i < argc; i++) l += strlen(argv[i]) + 1;
  l += strlen(tail) +1;
  cmd = ALLOC(char, l);
  if (cmd == NIL(char)) {
    fprintf(nusmv_stderr, "Parser_ReadFromString: unable to allocate \"cmd\"\n");
    nusmv_exit(1);
  }
  sprintf(cmd, "%s", head);
  for (i = 1; i < argc; i++) sprintf(cmd, "%s%s ", cmd, argv[i]);
  sprintf(cmd, "%s%s", cmd, tail);
  if (opt_verbose_level_gt(options, 3)) fprintf(nusmv_stderr, "%s\n", cmd);

  old_input_file = get_input_file(options);
  set_input_file(options, "<command-line>");

  parse_tree_int = Nil;
  parse_command_flag = 1;
  yy_scan_string(cmd);
  if (yyparse() == 0) {
    status = 0;
  } else {
    status = 1;
  }
  
  FREE(cmd); /* this deallocation must be checked yet */

  /* We need to reset the input buffer */
  yyrestart(yyin);
  set_input_file(options, old_input_file);
  *pc = parse_tree_int;
  return(status);
}
