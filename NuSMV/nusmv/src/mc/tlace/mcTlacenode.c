/**CFile***********************************************************************

  FileName    [mcTlacenode.c]

  PackageName [mc.tlace]

  Synopsis    [The main routines of the <tt>tlacenode</tt> data structure.]

  Description [The <tt>tlacenode</tt> implements a TLACE node. This node
  contains a BDD representing a state, atomic properties, existential
  properties and their eventual explanation, and universal properties.
  A TLACE node also has a type.]

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

  Version    [29/07/2011]

******************************************************************************/

#include "mcTlacenode.h"
#include "mc/mcInt.h"

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static void tlaces_free_existential ARGS((node_ptr existentials));

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis           [Creates a new TLACE node of type NODE, with state,
  atomics, existentials and universals.]

  Description        [This function creates a TLACE node of type NODE,
  containing a state, a set of atomics properties, a set of annotated paths of
  nodes representing the explanations of existential temporal formulas and a set
  of universal temporal formulas. The returned data structure is a LISP-like
  tree.]

  SideEffects        []

******************************************************************************/
tlacenode_ptr tlacenode_new_node(	bdd_ptr state,
									node_ptr atomics,
									node_ptr existentials,
									node_ptr universals)
{
	tlacenode_ptr node =	cons(new_node(TLACENODE_NODETYPE, Nil, Nil),
							cons((node_ptr) state,
							cons(atomics,
							cons(existentials,
							cons(universals, Nil
							)))));
	return node;
}

/**Function********************************************************************

  Synopsis           [Creates a new TLACE node of type LOOP, with to as the
  looping node.]

  Description        [This function creates a TLACE node of type LOOP, looping
  to to. The returned data structure is a LISP-like tree.]

  SideEffects        []

******************************************************************************/
tlacenode_ptr tlacenode_new_loop(tlacenode_ptr to)
{
	tlacenode_ptr loop =	cons(new_node(TLACENODE_LOOPTYPE, Nil, Nil),
							cons((node_ptr) to, Nil
							));
	return loop;
}

/**Function********************************************************************

  Synopsis           [Free a given TLACE node.]

  Description        [This function frees the memory allocated to a given TLACE
  node of type NODE. It uses the functionalities given by the node package.]

  SideEffects        [Frees the state contained in the TLACE node.]

******************************************************************************/
void tlacenode_free_node(tlacenode_ptr self)
{
	// Free universals (no need to free formulas)
	free_node(cdr(cdr(cdr(cdr(self)))));

	// Free existentials
	walk(tlaces_free_existential, tlacenode_get_existentials(self));
	free_node(cdr(cdr(cdr(self))));

	// Free atomics (no need to free formulas)
	free_node(cdr(cdr(self)));

	// Free the state
	bdd_free(dd_manager, (bdd_ptr) car(cdr(self)));
	free_node(cdr(self));

	free_node(self);
}

/**Function********************************************************************

  Synopsis           [Free a given TLACE node.]

  Description        [This function frees the memory allocated to a given TLACE
  node of type NODE. It uses the functionalities given by the node package.
  This function doesn't free all the content of the TLACE node but only its
  structure. It is useful when we want to free a node, but its state, atomics,
  existentials and universals are shared with another node.]

  SideEffects        [Frees the state contained in the TLACE node.]

******************************************************************************/
void tlacenode_free_node_structure(tlacenode_ptr self)
{
	// Free universals (no need to free formulas)
	free_node(cdr(cdr(cdr(cdr(self)))));

	// Free existentials
	free_node(cdr(cdr(cdr(self))));

	// Free atomics (no need to free formulas)
	free_node(cdr(cdr(self)));

	// Free the state
	free_node(cdr(self));

	free_node(self);
}

/**Function********************************************************************

  Synopsis           [Free a given TLACE node.]

  Description        [This function frees the memory allocated to a given TLACE
  node of type LOOP. It uses the functionalities given by the node package.]

  SideEffects        []

******************************************************************************/
void tlacenode_free_loop(tlacenode_ptr self)
{
	// No need to free the looping node twice! It has been freed before.

	free_node(cdr(self));
	free_node(self);
}

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

static void tlaces_free_existential(node_ptr existential)
{
	// Free path if present
	if(llength(existential) > 1)
	{
		node_ptr current = car(cdr(existential));
		node_ptr next = Nil;
		while(cdr(current) != Nil)
		{
			// Free the node
			tlacenode_free_node(car(current));
			next = cdr(current);
			free_node(current);

			current = next;

			// Free the input
			// FIXME when checking several specs (with EU ops, apparently),
			// this freeing causes a bug with CUDD.
			//bdd_free(dd_manager, (bdd_ptr) car(current));
			next = cdr(current);
			free_node(current);

			current = next;
		}
		// Free the last node/loop
		if(tlacenode_is_node(car(current)))
		{
			tlacenode_free_node(car(current));
		}
		else // The last is a loop
		{
			tlacenode_free_loop(car(current));
		}
		free_node(current);

		free_node(cdr(existential));
	}

	// No need to free the specification

	free_node(existential);
}
