/**CFile***********************************************************************

  FileName    [mcTlaceExport.c]

  PackageName [mc.tlace]

  Synopsis    [Tree-like annotated counter-examples XML export.]

  Description [This file contains the code to print XML version of TLACEs.]

  SeeAlso     []

  Author      [Simon Busard]

  Copyright   [
  This file is part of the ``mc'' package of NuSMV version 2.
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

  Version	  [02/08/2011]

******************************************************************************/

#include "mcTlacenode.h"

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

int id_node;

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void tlaces_export_node	ARGS((	BddFsm_ptr fsm,
										BddEnc_ptr enc,
										tlacenode_ptr node,
										FILE * output ));
static void tlaces_export_path	ARGS((	BddFsm_ptr fsm,
										BddEnc_ptr enc,
										node_ptr path,
										FILE * output ));
static void tlaces_export_state	ARGS((	BddFsm_ptr fsm,
										BddEnc_ptr enc,
										bdd_ptr state,
										FILE * output ));
static void tlaces_export_inputs_combinatorials
								ARGS((	BddFsm_ptr fsm,
										BddEnc_ptr enc,
										bdd_ptr input,
										bdd_ptr state,
										FILE * output ));
/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Export a tree-like annotated counter-example for formula
  into output.]

  Description        [Prints the TLACE node for formula into output. It is
  exported into XML format.]

  SideEffects        []

******************************************************************************/
void tlaces_export(	BddFsm_ptr fsm, BddEnc_ptr enc, tlacenode_ptr node,
					node_ptr formula, FILE * output )
{
	reset_indent_size();

	// Print header
	indent(output);
	fprintf(output, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

	// Open counterexample
	indent(output);
	fprintf(output, "<counterexample specification=\"");
	print_node_XML(output, formula);
	fprintf(output, "\">\n");

	inc_indent_size();

	id_node = 0;

	// Print the TLACE
	tlaces_export_node(fsm, enc, node, output);

	dec_indent_size();

	// Close counterexample
	indent(output);
	fprintf(output, "</counterexample>\n");
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Export a TLACE node into the specified output.]

  Description        [Prints an XML version of node in output.]

  SideEffects        []

******************************************************************************/
static void tlaces_export_node( BddFsm_ptr fsm, BddEnc_ptr enc,
								tlacenode_ptr node, FILE * output )
{
	// Open node
	indent(output);
	fprintf(output, "<node id=\"%d\">\n", id_node++);

	inc_indent_size();

	// Print state
	tlaces_export_state(fsm, enc, tlacenode_get_state(node), output);

	// Print atomics
	node_ptr atomics = tlacenode_get_atomics(node);
	while(atomics != Nil)
	{
		indent(output);
		fprintf(output, "<atomic specification=\"");
		print_node_XML(output, car(atomics));
		fprintf(output, "\" />\n");

		atomics = cdr(atomics);
	}

	// Print existentials
	node_ptr existentials = tlacenode_get_existentials(node);
	while(existentials != Nil)
	{
		node_ptr existential = car(existentials);

		indent(output);
		fprintf(output, "<existential specification=\"");
		print_node_XML(output, car(existential));
		fprintf(output, "\"");

		// Print path if needed
		if(llength(existential) > 1)
		{
			fprintf(output, " explained=\"true\">\n");
			inc_indent_size();

			// Print path
			tlaces_export_path(fsm, enc, car(cdr(existential)), output);

			dec_indent_size();
			indent(output);
			fprintf(output, "</existential>\n");
		} else
		{
			fprintf(output, " explained=\"false\" />\n");
		}

		existentials = cdr(existentials);
	}

	// Print universals
	node_ptr universals = tlacenode_get_universals(node);
	while(universals != Nil)
	{
		indent(output);
		fprintf(output, "<universal specification=\"");
		print_node_XML(output, car(universals));
		fprintf(output, "\" />\n");

		universals = cdr(universals);
	}

	dec_indent_size();

	// Close node
	indent(output);
	fprintf(output, "</node>\n");
}

/**Function********************************************************************

  Synopsis           [Export a TLACE path into the specified output.]

  Description        [Prints an XML version of path in output. Path is assumed
  to have at least one node.]

  SideEffects        []

******************************************************************************/
static void tlaces_export_path(	BddFsm_ptr fsm, BddEnc_ptr enc, node_ptr path,
								FILE * output )
{
	int loop;
	bdd_ptr ii;
	node_ptr ni, tmp_path;
	tlacenode_ptr last_node;

	loop = -1;

	// Print first node
	tlaces_export_node(fsm, enc, car(path), output);

	tmp_path = path;
	while(cdr(tmp_path) != Nil)
	{
		tmp_path = cdr(tmp_path);
	}
	last_node = car(tmp_path);

	// Check if loop to first node
	if (	tlacenode_is_loop(last_node) &&
			car(path) == tlacenode_get_loop(last_node)) {
		loop = id_node;
	}

	path = cdr(path);

	while(path != Nil)
	{
		ii = (bdd_ptr) car(path);
		path = cdr(path);
		ni = car(path);

		// Print inputs and combinatorials
		if(tlacenode_is_node(ni))
		{
			tlaces_export_inputs_combinatorials(fsm, enc, ii,
				tlacenode_get_state(ni), output);
		} else
		{
			tlaces_export_inputs_combinatorials(fsm, enc, ii,
				tlacenode_get_state(tlacenode_get_loop(ni)), output);
		}

		if(tlacenode_is_loop(ni))
		{
			indent(output);
			fprintf(output, "<loop to=\"%d\" />\n", loop);
		} else
		{
			if (	tlacenode_is_loop(last_node) &&
					ni == tlacenode_get_loop(last_node)) {
				loop = id_node;
			}
			tlaces_export_node(fsm, enc, ni, output);
		}

		path = cdr(path);
	}
}

/**Function********************************************************************

  Synopsis           [Export a state into the specified output.]

  Description        [Prints an XML version of state in output.]

  SideEffects        []

******************************************************************************/
static void tlaces_export_state(	BddFsm_ptr fsm, BddEnc_ptr enc,
									bdd_ptr state, FILE * output )
{
	ListIter_ptr los;
	NodeList_ptr list;
	add_ptr add_state, cur_sym_vals, tmp;
	node_ptr cur_sym_value, cur_sym;
	Encoding_ptr senc = BddEnc_get_symbolic_encoding(enc);

	indent(output);
	fprintf(output, "<state>\n");
	inc_indent_size();

	// Print values
	add_state = bdd_to_add(dd_manager, (BddStates) state);
	list = Encoding_get_model_state_symbols_list(senc);
	los = NodeList_get_first_iter(list);
	while (!ListIter_is_end(los))
	{
		cur_sym = NodeList_get_value_at(list, los);
		cur_sym_vals = BddEnc_expr_to_add(enc, cur_sym, Nil);
		tmp = add_if_then(dd_manager, add_state, cur_sym_vals);

		cur_sym_value = add_value(dd_manager, tmp);

		add_free(dd_manager, tmp);
		add_free(dd_manager, cur_sym_vals);

		indent(output);
		fprintf(output,	"<value variable=\"%s\">%s</value>\n",
				sprint_node_XML(cur_sym), sprint_node_XML(cur_sym_value));

		los = ListIter_get_next(los);
	}

	dec_indent_size();
	indent(output);
	fprintf(output, "</state>\n");
}

static void tlaces_export_inputs_combinatorials(
										BddFsm_ptr fsm, BddEnc_ptr enc,
										bdd_ptr input, bdd_ptr state,
										FILE * output)
{
	// Print inputs
	ListIter_ptr los;
	NodeList_ptr list;

	node_ptr cur_sym_value, cur_sym;
	add_ptr cur_sym_vals, tmp;

	BddInputs curr_input = (BddInputs) input;
	add_ptr add_input, add_comb;
	bdd_ptr bdd_comb;

	Encoding_ptr senc = BddEnc_get_symbolic_encoding(enc);

	bdd_comb = bdd_and(dd_manager, state, curr_input);
	add_comb = bdd_to_add(dd_manager, bdd_comb);
	bdd_free(dd_manager, bdd_comb);

	indent(output);
	fprintf(output,"<input>\n");
	inc_indent_size();

	add_input = bdd_to_add(dd_manager, curr_input);
	list = Encoding_get_model_input_symbols_list(senc);
	los = NodeList_get_first_iter(list);
	while (! ListIter_is_end(los)) {
		cur_sym = NodeList_get_value_at(list, los);
		cur_sym_vals = BddEnc_expr_to_add(enc, cur_sym, Nil);
		tmp = add_if_then(dd_manager, add_input, cur_sym_vals);

		cur_sym_value = add_value(dd_manager, tmp);

		add_free(dd_manager, tmp);
		add_free(dd_manager, cur_sym_vals);

		indent(output);
		fprintf(output,
				"<value variable=\"%s\">%s</value>\n",
				sprint_node_XML(cur_sym),
				sprint_node_XML(cur_sym_value));

		los = ListIter_get_next(los);
	}

	dec_indent_size();
	indent(output);
	fprintf(output,"</input>\n");


	// Print combinatorials
	curr_input = (BddInputs) input;

	indent(output);
	fprintf(output,"<combinatorial>\n");
	inc_indent_size();

	list = Encoding_get_model_state_input_symbols_list(senc);
	los = NodeList_get_first_iter(list);
	while (! ListIter_is_end(los)) {
		cur_sym = NodeList_get_value_at(list, los);
		cur_sym_vals = BddEnc_expr_to_add(enc, cur_sym, Nil);
		tmp = add_if_then(dd_manager, add_comb, cur_sym_vals);

		cur_sym_value = add_value(dd_manager, tmp);

		add_free(dd_manager, tmp);
		add_free(dd_manager, cur_sym_vals);

		indent(output);
		fprintf(output,
			"<value variable=\"%s\">%s</value>\n",
			sprint_node_XML(cur_sym),
			sprint_node_XML(cur_sym_value));

		los = ListIter_get_next(los);
	}
	add_free(dd_manager, add_comb);

	dec_indent_size();
	indent(output);
	fprintf(output,"</combinatorial>\n");
}
