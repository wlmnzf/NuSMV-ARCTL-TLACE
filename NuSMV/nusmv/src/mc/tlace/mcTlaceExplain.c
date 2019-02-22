/**CFile***********************************************************************

  FileName    [mcTlaceExplain.c]

  PackageName [mc.tlace]

  Synopsis    [TLACE nodes generation for CTL and ARCTL specifications.]

  Description [This file contains the code to find tree-like annotated
  counterexamples for ARCTL and CTL specifications.]

  SeeAlso     [mcExplain.c]

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

  Version	  [29/07/2011]

******************************************************************************/

#include "mcTlacenode.h"
#include "mcTlace.h"
#include "parser/symbols.h"
#include "dd/dd.h"

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static tlacenode_ptr tlaces_cntex_explain ARGS((BddFsm_ptr fsm, BddEnc_ptr enc,
		bdd_ptr state, node_ptr formula, node_ptr context, int depth));
static tlacenode_ptr tlaces_witness_explain ARGS((BddFsm_ptr fsm,
		BddEnc_ptr enc, bdd_ptr state, node_ptr formula, node_ptr context,
		int depth));
static node_ptr tlaces_path_explain ARGS((BddFsm_ptr fsm, BddEnc_ptr enc,
		bdd_ptr state, node_ptr formula, node_ptr context, int depth));

static boolean tlaces_state_satisfies ARGS((BddFsm_ptr fsm, BddEnc_ptr enc,
		bdd_ptr state, node_ptr formula, node_ptr context));

static void free_path ARGS((node_ptr path));

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Tree-like annotated counter-examples generator.]

  Description        [This function generates a new TLACE to explain why state
  violates formula in fsm. It uses options defined in the options structure.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
tlacenode_ptr tlaces_explain(BddFsm_ptr fsm, BddEnc_ptr enc, bdd_ptr state,
						node_ptr formula)
{
	return tlaces_cntex_explain(fsm, enc, state, formula, Nil,
									get_tlaces_depth(options));
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Tree-like annotated counter-examples generator.]

  Description        [This function generates a new TLACE to explain why state
  violates formula in fsm. It uses options defined in the options structure.
  depth gives the maximum depth of temporal operators to explain.]

  SideEffects        []

  SeeAlso            [tlaces_explain]

******************************************************************************/
static tlacenode_ptr tlaces_cntex_explain(BddFsm_ptr fsm,
		BddEnc_ptr enc, bdd_ptr state, node_ptr formula, node_ptr context,
		int depth)
{
	node_ptr new_formula, act, f, g;

	switch(node_get_type(formula))
	{
	case CONTEXT:	return tlaces_cntex_explain(fsm, enc, state, cdr(formula),
						car(formula), depth);
	case FALSEEXP:	new_formula = new_node(TRUEEXP, Nil, Nil); break;
	case NOT:		new_formula = car(formula); break;
	case OR:		new_formula = find_node(AND,
										find_node(NOT, car(formula), Nil),
										find_node(NOT, cdr(formula), Nil)
									); break;
	case AND:		new_formula = find_node(OR,
										find_node(NOT, car(formula), Nil),
										find_node(NOT, cdr(formula), Nil)
									); break;
	case IMPLIES:	new_formula = find_node(AND,
										car(formula),
										find_node(NOT, cdr(formula), Nil)
									); break;
	case IFF:		new_formula = find_node(OR,
										find_node(AND,
											car(formula),
											find_node(NOT, cdr(formula), Nil)),
										find_node(AND,
											find_node(NOT, car(formula), Nil),
											cdr(formula))
									); break;
	case EX:		new_formula = find_node(AX, find_node(NOT,
													car(formula), Nil),
										Nil); break;
	case EF:		new_formula = find_node(AG, find_node(NOT,
													car(formula), Nil),
										Nil); break;
	case EG:		new_formula = find_node(AF, find_node(NOT,
													car(formula), Nil),
										Nil); break;
	case EU:		new_formula = find_node
						(AW, find_node(
							NOT, cdr(formula), Nil), find_node(
							AND, find_node(
								NOT, car(formula),Nil), find_node(
								NOT, cdr(formula), Nil)
							)
						); break;
	case EW:		new_formula = find_node
						(AU, find_node(
							NOT, cdr(formula), Nil), find_node(
							AND, find_node(
								NOT, car(formula),Nil), find_node(
								NOT, cdr(formula), Nil)
							)
						); break;
	case AX:		new_formula = find_node(EX, find_node(NOT,
													car(formula), Nil),
										Nil); break;
	case AF:		new_formula = find_node(EG, find_node(NOT,
													car(formula), Nil),
										Nil); break;
	case AG:		new_formula = find_node(EF, find_node(NOT,
													car(formula), Nil),
										Nil); break;
	case AU:		new_formula = find_node
						(EW, find_node(
							NOT, cdr(formula), Nil), find_node(
							AND, find_node(
								NOT, car(formula),Nil), find_node(
								NOT, cdr(formula), Nil)
							)
						); break;
	case AW:		new_formula = find_node
						(EU, find_node(
							NOT, cdr(formula), Nil), find_node(
							AND, find_node(
								NOT, car(formula),Nil), find_node(
								NOT, cdr(formula), Nil)
							)
						); break;

	// ARCTL operators
	case EAX:		new_formula = find_node
						(AAX,
							car(formula), find_node(
								NOT,
								cdr(formula), Nil)
						); break;
	case EAF:		new_formula = find_node
						(AAG,
							car(formula), find_node(
								NOT,
								cdr(formula), Nil)
						); break;
	case EAG:		new_formula = find_node
						(AAF,
							car(formula), find_node(
								NOT,
								cdr(formula), Nil)
						); break;
	case EAU:
		act = car(formula);
		f = car(cdr(formula));
		g = cdr(cdr(formula));
		new_formula = find_node(AAW, act,
						find_node(AW,
							find_node(NOT, g, Nil),
							find_node(AND,
								find_node(NOT, f, Nil),
								find_node(NOT, g, Nil))));
		break;
	case EAW:
		act = car(formula);
		f = car(cdr(formula));
		g = cdr(cdr(formula));
		new_formula = find_node(AAU, act,
						find_node(AU,
							find_node(NOT, g, Nil),
							find_node(AND,
								find_node(NOT, f, Nil),
								find_node(NOT, g, Nil))));
		break;
	case AAX:		new_formula = find_node
						(EAX,
							car(formula), find_node(
								NOT,
								cdr(formula), Nil)
						); break;
	case AAF:		new_formula = find_node
						(EAG,
							car(formula), find_node(
								NOT,
								cdr(formula), Nil)
						); break;
	case AAG:		new_formula = find_node
						(EAF,
							car(formula), find_node(
								NOT,
								cdr(formula), Nil)
						); break;
	case AAU:
		act = car(formula);
		f = car(cdr(formula));
		g = cdr(cdr(formula));
		new_formula = find_node(EAW, act,
						find_node(EW,
							find_node(NOT, g, Nil),
							find_node(AND,
								find_node(NOT, f, Nil),
								find_node(NOT, g, Nil))));
		break;
	case AAW:
		act = car(formula);
		f = car(cdr(formula));
		g = cdr(cdr(formula));
		new_formula = find_node(EAU, act,
						find_node(EU,
							find_node(NOT, g, Nil),
							find_node(AND,
								find_node(NOT, f, Nil),
								find_node(NOT, g, Nil))));
		break;

	// The default case: deal with unmanaged formulas by returning a single
	// TLACE node. This includes the case of atomic propositions.
	// All unrecognized operators are considered as atomics
	default:
		if(node_get_type(formula) == NOT)
		{
			new_formula = car(formula);
		} else
		{
			new_formula = find_node(NOT, formula, Nil);
		}
		return tlacenode_new_node(	bdd_dup(state), cons(new_formula, Nil),
									Nil, Nil);
	}

	// Returns a witness for the new formula
	return tlaces_witness_explain(fsm, enc, state, new_formula, context, depth);
}

/**Function********************************************************************

  Synopsis           [Tree-like annotated counter-examples witnesses generator.]

  Description        [This function generates a new TLACE to explain why state
  satisfies formula in fsm. It uses options defined in the options structure.
  depth gives the maximum depth of temporal operators to explain.]

  SideEffects        []

  SeeAlso            [tlaces_explain]

******************************************************************************/
static tlacenode_ptr tlaces_witness_explain(BddFsm_ptr fsm,
		BddEnc_ptr enc, bdd_ptr state, node_ptr formula, node_ptr context,
		int depth)
{
	tlacenode_ptr n1, n2, res;
	int type, nd = (depth > 0) ? depth - 1 : depth;

	switch(node_get_type(formula))
	{
	case CONTEXT: return tlaces_witness_explain(fsm, enc, state, cdr(formula),
							car(formula), depth);
	case TRUEEXP: return tlacenode_new_node(bdd_dup(state), Nil, Nil, Nil);
	case NOT: return tlaces_cntex_explain(fsm, enc, state, car(formula),
							context, depth);
	case OR:
		if(tlaces_state_satisfies(fsm, enc, state, car(formula), context))
		{
			return tlaces_witness_explain(fsm, enc, state, car(formula),
						context, depth);
		} else
		{
			return tlaces_witness_explain(fsm, enc, state, cdr(formula),
						context, depth);
		}
	case AND:
		n1 = tlaces_witness_explain(fsm, enc, state,
				car(formula), context, depth);
		n2 = tlaces_witness_explain(fsm, enc, state,
				cdr(formula), context, depth);
		res = tlacenode_new_node(
				bdd_dup(state),
				append(tlacenode_get_atomics(n1), tlacenode_get_atomics(n2)),
				append(tlacenode_get_existentials(n1), tlacenode_get_existentials(n2)),
				append(tlacenode_get_universals(n1), tlacenode_get_universals(n2))
				);

		// Free temp nodes partially (free states but not other parts)
		bdd_free(dd_manager, tlacenode_get_state(n1));
		tlacenode_free_node_structure(n1);
		bdd_free(dd_manager, tlacenode_get_state(n2));
		tlacenode_free_node_structure(n2);

		return res;
	case IMPLIES:
		return tlaces_witness_explain(fsm, enc, state,
				find_node(OR, find_node(
							NOT, car(formula), Nil),
							cdr(formula)), context, depth);
	case IFF:
		return tlaces_witness_explain(fsm, enc, state,
				find_node(OR, find_node(
							AND,
								car(formula),
								cdr(formula)), find_node(
							AND, find_node(
								NOT, car(formula), Nil), find_node(
								NOT, cdr(formula), Nil))), context, depth);
	case AX:
	case AF:
	case AG:
	case AU:
	case AW:
	case AAX:
	case AAF:
	case AAG:
	case AAU:
	case AAW:
		return tlacenode_new_node(bdd_dup(state), Nil, Nil, cons(formula, Nil));
	case EX:
	case EF:
	case EG:
	case EU:
	case EW:
	case EAX:
	case EAF:
	case EAG:
	case EAU:
	case EAW:
		type = node_get_type(formula);
		if(((type == EX && opt_tlaces_explain_level_ex(options)) |
			(type == EF && opt_tlaces_explain_level_eu(options)) |
			(type == EG && opt_tlaces_explain_level_eg(options)) |
			(type == EU && opt_tlaces_explain_level_eu(options)) |
			(type == EW && opt_tlaces_explain_level_eu(options)) |
			(type == EAX && opt_tlaces_explain_level_ex(options)) |
			(type == EAF && opt_tlaces_explain_level_eu(options)) |
			(type == EAG && opt_tlaces_explain_level_eg(options)) |
			(type == EAU && opt_tlaces_explain_level_eu(options)) |
			(type == EAW && opt_tlaces_explain_level_eu(options)))
			&& depth != 0)
		{
			return tlacenode_new_node(bdd_dup(state), Nil,
					cons(	cons(	formula,
									cons(
											tlaces_path_explain(fsm, enc, state,
														formula, context, nd),
											Nil)),
							Nil),
					Nil);
		} else
		{
			return tlacenode_new_node(bdd_dup(state), Nil,
					cons(cons(formula, Nil), Nil), Nil);
		}

	case REACHABLE:
		return tlacenode_new_node(bdd_dup(state), Nil,
					cons(	cons(	formula,
									cons(
											tlaces_path_explain(fsm, enc, state,
														formula, context, nd),
											Nil)),
							Nil),
					Nil);

	// The default case: deal with unmanaged formulas by returning a single
	// TLACE node. This includes the case of atomic propositions.
	// All unrecognized operators are considered as atomics
	default:
		return tlacenode_new_node(bdd_dup(state), cons(formula, Nil), Nil, Nil);
	}
}

/**Function********************************************************************

  Synopsis           [Tree-like annotated counter-examples paths generator.]

  Description        [This function generates a new TLACE path to explain why
  state is the source of a path in fsm that satisfies formula. For technical
  reasons, formula is not a path formula but an existential temporal formula.
  This function uses options defined in the options structure.]

  SideEffects        []

  SeeAlso            [tlaces_explain]

******************************************************************************/
static node_ptr tlaces_path_explain(BddFsm_ptr fsm,
		BddEnc_ptr enc, bdd_ptr state, node_ptr formula, node_ptr context,
		int depth)
{
	bdd_ptr a1, a2, a3, ii, si;
	node_ptr tmp_path, new_path, result, path, current;
	tlacenode_ptr loop, ni;

	switch(node_get_type(formula))
	{
	case EX:
		// Get the path
		a1 = (bdd_ptr) eval_spec(fsm, enc, car(formula), context);
		tmp_path = cons((node_ptr) bdd_dup(state), Nil);
		new_path = ex_explain(fsm, enc, tmp_path, a1);
		bdd_free(dd_manager, a1);

		// Create the TLACE path
		result = cons(
				tlacenode_new_node(bdd_dup(state), Nil, Nil, Nil),cons(
				(node_ptr) bdd_dup((bdd_ptr) car(cdr(new_path))),cons(
				tlaces_witness_explain(fsm, enc,
											bdd_dup((bdd_ptr) car(new_path)),
											car(formula), context, depth),
											Nil)));

		free_path(new_path);

		return result;
	case EF:
		return tlaces_path_explain(fsm, enc, state,
				find_node(EU, new_node(TRUEEXP, Nil, Nil), car(formula)),
					context, depth);
	case EG:
		// Get the path
		a1 = (bdd_ptr) eval_spec(fsm, enc, car(formula), context);
		tmp_path = cons((node_ptr) bdd_dup(state), Nil);
		new_path = eg_explain(fsm, enc, tmp_path, a1);
		bdd_free(dd_manager, a1);

		// Create the TLACE path
		path = Nil; loop = Nil;
		current = cdr(new_path);
		while(current != Nil)
		{
			// Get input and state
			ii = (bdd_ptr) car(current);
			current = cdr(current);
			si = (bdd_ptr) car(current);

			// Computes the node
			ni = tlaces_witness_explain(fsm, enc, si, car(formula), context,
					depth);

			// Computes loop if needed
			if(si == (bdd_ptr) car(new_path))
			{
				loop = tlacenode_new_loop(ni);
			}

			// Create the path
			// Warning: new_path is reversed, the returned path will be in the
			// correct order
			path = cons((node_ptr) ni, cons((node_ptr) ii, path));
			current = cdr(current);
		}

		// Free and return
		free_path(new_path);

		// Append loop
		new_path = path;
		while(cdr(new_path) != Nil)
		{
			new_path = cdr(new_path);
		}
		node_node_setcdr(new_path, cons(loop, Nil));

		return path;
	case EU:
		// Get the path
		a1 = (bdd_ptr) eval_spec(fsm, enc, car(formula), context);
		a2 = (bdd_ptr) eval_spec(fsm, enc, cdr(formula), context);
		tmp_path = cons((node_ptr) bdd_dup(state), Nil);
		new_path = eu_explain(fsm, enc, tmp_path, a1, a2);
		bdd_free(dd_manager, a1);
		bdd_free(dd_manager, a2);

		// Create the TLACE path
		// Create the last node
		si = (bdd_ptr) car(new_path);
		ni = tlaces_witness_explain(fsm, enc, si, cdr(formula), context, depth);

		path = cons(ni, Nil);
		current = cdr(new_path);
		while(current != Nil)
		{
			// Get input and state
			ii = (bdd_ptr) car(current);
			current = cdr(current);
			si = (bdd_ptr) car(current);

			// Computes the node
			ni = tlaces_witness_explain(fsm, enc, si, car(formula), context,
					depth);

			// Create the path
			// Warning: new_path is reversed, the returned path will be in the
			// correct order
			path = cons((node_ptr) ni, cons((node_ptr) ii, path));
			current = cdr(current);
		}

		// Free and return
		free_path(new_path);
		return path;
	case EW:
		if(tlaces_state_satisfies(fsm, enc, state,
				find_node(EU, car(formula), cdr(formula)), context))
		{
			return tlaces_path_explain(fsm, enc, state,
					find_node(EU, car(formula), cdr(formula)), context, depth);
		} else
		{
			return tlaces_path_explain(fsm, enc, state,
					find_node(EG, car(formula), Nil), context, depth);
		}

	case EAX:
		// Get the path
		a1 = (bdd_ptr) eval_spec(fsm, enc, car(formula), context);
		a2 = (bdd_ptr) eval_spec(fsm, enc, cdr(formula), context);
		tmp_path = cons((node_ptr) bdd_dup(state), Nil);
		new_path = (node_ptr) eax_explain(fsm, enc, tmp_path, a2, a1);
		bdd_free(dd_manager, a1);
		bdd_free(dd_manager, a2);

		// Create the TLACE path
		result = cons(
				tlacenode_new_node(bdd_dup(state), Nil, Nil, Nil),cons(
				(node_ptr) bdd_dup((bdd_ptr) car(cdr(new_path))),cons(
				tlaces_witness_explain(fsm, enc,
											bdd_dup((bdd_ptr) car(new_path)),
											cdr(formula), context, depth),
											Nil)));

		free_path(new_path);

		return result;
	case EAF:
		return tlaces_path_explain(fsm, enc, state,
				find_node(EAU,
					car(formula),
					find_node(EU,
						new_node(TRUEEXP, Nil, Nil),
						cdr(formula))),
					context, depth);
	case EAG:
		// Get the path
		a1 = (bdd_ptr) eval_spec(fsm, enc, car(formula), context);
		a2 = (bdd_ptr) eval_spec(fsm, enc, cdr(formula), context);
		tmp_path = cons((node_ptr) bdd_dup(state), Nil);
		new_path = (node_ptr) eag_explain(fsm, enc, tmp_path, a2, a1);
		bdd_free(dd_manager, a2);
		bdd_free(dd_manager, a1);

		// Create the TLACE path
		path = Nil; loop = Nil;
		current = cdr(new_path);
		while(current != Nil)
		{
			// Get input and state
			ii = (bdd_ptr) car(current);
			current = cdr(current);
			si = (bdd_ptr) car(current);

			// Computes the node
			ni = tlaces_witness_explain(fsm, enc, si, cdr(formula), context,
					depth);

			// Computes loop if needed
			if(si == (bdd_ptr) car(new_path))
			{
				loop = tlacenode_new_loop(ni);
			}

			// Create the path
			// Warning: new_path is reversed, the returned path will be in the
			// correct order
			path = cons((node_ptr) ni, cons((node_ptr) ii, path));
			current = cdr(current);
		}

		// Free and return
		free_path(new_path);

		// Append loop
		new_path = path;
		while(cdr(new_path) != Nil)
		{
			new_path = cdr(new_path);
		}
		node_node_setcdr(new_path, cons(loop, Nil));

		return path;
	case EAU:
		// Get the path
		a1 = (bdd_ptr) eval_spec(fsm, enc, car(formula), context);
		a2 = (bdd_ptr) eval_spec(fsm, enc, car(cdr(formula)), context);
		a3 = (bdd_ptr) eval_spec(fsm, enc, cdr(cdr(formula)), context);
		tmp_path = cons((node_ptr) bdd_dup(state), Nil);
		new_path = (node_ptr) eau_explain(fsm, enc, tmp_path, a2, a3, a1);
		bdd_free(dd_manager, a3);
		bdd_free(dd_manager, a2);
		bdd_free(dd_manager, a1);

		// Create the TLACE path
		// Create the last node
		si = (bdd_ptr) car(new_path);
		ni = tlaces_witness_explain(fsm, enc, si, cdr(cdr(formula)), context, depth);

		path = cons(ni, Nil);
		current = cdr(new_path);
		while(current != Nil)
		{
			// Get input and state
			ii = (bdd_ptr) car(current);
			current = cdr(current);
			si = (bdd_ptr) car(current);

			// Computes the node
			ni = tlaces_witness_explain(fsm, enc, si, car(cdr(formula)), context,
					depth);

			// Create the path
			// Warning: new_path is reversed, the returned path will be in the
			// correct order
			path = cons((node_ptr) ni, cons((node_ptr) ii, path));
			current = cdr(current);
		}

		// Free and return
		free_path(new_path);
		return path;
	case EAW:
		if(tlaces_state_satisfies(fsm, enc, state,
				find_node(EAU,
					car(formula),
					find_node(EU,
						car(cdr(formula)),
						cdr(cdr(formula)))), context))
		{
			return tlaces_path_explain(fsm, enc, state,
					find_node(EAU,
						car(formula),
						find_node(EU,
							car(cdr(formula)),
							cdr(cdr(formula)))), context, depth);
		} else
		{
			return tlaces_path_explain(fsm, enc, state,
					find_node(EAG, car(formula), car(cdr(formula))),
					context, depth);
		}

	/* sbusard 25/11/11 */
	case REACHABLE:
		// Get a reachable witness
		a1 = eval_spec(fsm, enc, car(formula), context);
		tmp_path = cons((node_ptr) bdd_dup(state), Nil);
		new_path = (node_ptr) reachable_explain(fsm, enc, tmp_path, a1);
		bdd_free(dd_manager, a1);
		// Create the TLACE path
		// Create the last node
		si = (bdd_ptr) car(new_path);
		ni = tlacenode_new_node(bdd_dup(si), Nil, Nil, Nil);

		path = cons(ni, Nil);
		current = cdr(new_path);
		while(current != Nil)
		{
			// Get input and state
			ii = (bdd_ptr) car(current);
			current = cdr(current);
			si = (bdd_ptr) car(current);

			// Computes the node
			ni = tlacenode_new_node(bdd_dup(si), Nil, Nil, Nil);

			// Create the path
			// Warning: new_path is reversed, the returned path will be in the
			// correct order
			path = cons((node_ptr) ni, cons((node_ptr) ii, path));
			current = cdr(current);
		}

		// Free and return
		free_path(new_path);
		return path;

	default:
		// This cannot happen. Raises an error.
		fprintf(nusmv_stderr, "tlaces_path_explain: Undefined formula (%d)\n",
				node_get_type(formula));
		nusmv_exit(1);
		break;
	}
}

/**Function********************************************************************

  Synopsis           [Does the state satisfies the property?]

  Description        [This function returns TRUE the given state satisfies the
  given formula. FALSE otherwise.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static boolean tlaces_state_satisfies(BddFsm_ptr fsm, BddEnc_ptr enc,
		bdd_ptr state, node_ptr formula, node_ptr context)
{
	bdd_ptr s;
	boolean res = false;

	s = (bdd_ptr) eval_spec(fsm, enc, formula, context);
	bdd_and_accumulate(dd_manager, &s, state);

	if(bdd_isnot_zero(dd_manager, s))
		res = true;

	bdd_free(dd_manager, s);

	return res;
}

/**Function********************************************************************

  Synopsis           [Free the list and all its elements.]

  Description        [This function assumes the given node_ptr is a list of
  BDDs. These BDDs are freed and the list itself is freed.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void free_path(node_ptr path)
{
	node_ptr current = path, next;
	while(current != Nil)
	{
		bdd_free(dd_manager, (bdd_ptr) car(current));
		next = cdr(current);
		free_node(current);
		current = next;
	}
}
