/**CHeaderFile*****************************************************************

  FileName    [mcTlacenode.h]

  PackageName [mc.tlace]

  Synopsis    [The header file for the <tt>tlacenode</tt> data structure.]

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

#ifndef __TLACENODE_H__
#define __TLACENODE_H__

#include "node/node.h"
#include "utils/utils.h"
#include "dd/dd.h"
#include "mc/mcInt.h"

/*
 * A tlacenode respects the following grammar, where (a b c d) is a list of
 * nodes composed of a, b, c, and d, (a . l) is a list composed of a and
 * elements of l.
 *
 * tlacenode 	::= NODE
 * NODE			::= (type state ATOMICS EXISTENTIALS UNIVERSALS)
 * ATOMICS		::= Nil | (spec . ATOMICS)
 * EXISTENTIALS	::= Nil | (EXISTENTIAL . EXISTENTIALS)
 * UNIVERSALS	::= Nil | (spec . UNIVERSALS)
 * EXISTENTIAL	::= (spec) | (spec PATH)
 * PATH			::= (NODE) | (NODE input . PATH) | (NODE input LOOP)
 * LOOP			::= (type NODE)
 *
 * type is an int, state is a BDD representing a state, spec is a CTL formula
 * and input is a BDD representing inputs.
 *
 */

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

#define TLACENODE_NODETYPE 0
#define TLACENODE_LOOPTYPE 1

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

typedef node_ptr tlacenode_ptr;

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

#define tlacenode_get_type(_n_) (node_get_type((car((node_ptr) _n_))))
#define tlacenode_get_state(_n_) ((bdd_ptr) car(cdr((node_ptr) _n_)))
#define tlacenode_get_atomics(_n_) car(cdr(cdr((node_ptr) _n_)))
#define tlacenode_get_existentials(_n_) car(cdr(cdr(cdr((node_ptr) _n_))))
#define tlacenode_get_universals(_n_) car(cdr(cdr(cdr(cdr((node_ptr) _n_)))))

#define tlacenode_get_loop(_n_) ((tlacenode_ptr) car(cdr((node_ptr) _n_)))

#define tlacenode_is_loop(_n_) (tlacenode_get_type(_n_) == TLACENODE_LOOPTYPE)
#define tlacenode_is_node(_n_) (tlacenode_get_type(_n_) == TLACENODE_NODETYPE)

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN tlacenode_ptr 	tlacenode_new_node ARGS((	bdd_ptr state,
													node_ptr atomics,
													node_ptr existentials,
													node_ptr universals ));
EXTERN tlacenode_ptr 	tlacenode_new_loop ARGS(( tlacenode_ptr to ));
EXTERN void 			tlacenode_free_node ARGS(( tlacenode_ptr self ));
EXTERN void 			tlacenode_free_node_structure ARGS(( tlacenode_ptr self
														));
EXTERN void 			tlacenode_free_loop ARGS(( tlacenode_ptr self ));


#endif /* __TLACENODE_H__ */
