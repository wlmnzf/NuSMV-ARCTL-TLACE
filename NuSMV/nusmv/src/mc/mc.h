/**CHeaderFile*****************************************************************

  FileName    [mc.h]

  PackageName [mc]

  Synopsis    [Fair CTL model checking algorithms. External header file.]

  Description [Fair CTL model checking algorithms. External header file.]

  Author      [Marco Roveri, Roberto Cavada]

  Revision    [$Id: mc.h,v 1.5.4.16 2004/04/22 14:33:29 nusmv Exp $]

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

******************************************************************************/

#ifndef __MC_H__
#define __MC_H__

#include "config.h"
#include "utils/utils.h"
#include "dd/dd.h"
#include "prop/prop.h"
#include "fsm/bdd/BddFsm.h"
#include "trace/Trace.h"
/* 02/08/11 sbusard */
#include "tlace/mcTlace.h"

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN void Mc_Init ARGS((void));
EXTERN void Mc_End ARGS((void));

EXTERN void Mc_CheckCTLSpec ARGS((Prop_ptr prop));
EXTERN void Mc_CheckAGOnlySpec ARGS((Prop_ptr prop));
EXTERN void Mc_CheckInvar ARGS((Prop_ptr prop));
EXTERN void Mc_CheckCompute ARGS((Prop_ptr prop));

EXTERN void print_spec  ARGS((FILE *file, node_ptr n));
EXTERN void print_invar ARGS((FILE *file, node_ptr n));
EXTERN void print_compute ARGS((FILE *, node_ptr));

EXTERN BddStates ex      ARGS((BddFsm_ptr, BddStates));
EXTERN BddStates ef      ARGS((BddFsm_ptr, BddStates));
EXTERN BddStates eg      ARGS((BddFsm_ptr, BddStates));
EXTERN BddStates eu      ARGS((BddFsm_ptr, BddStates, BddStates));
EXTERN BddStates au      ARGS((BddFsm_ptr, BddStates, BddStates));
/* 01/08/11 sbusard */
EXTERN BddStates ew      ARGS((BddFsm_ptr, BddStates, BddStates));
EXTERN BddStates aw      ARGS((BddFsm_ptr, BddStates, BddStates));
/* Franco (02/08/2004) */
EXTERN BddStates eax      ARGS((BddFsm_ptr, BddStates, BddStates));
/* sbusard 06/12/13 */
EXTERN BddStates aax      ARGS((BddFsm_ptr, BddStates, BddStates));
/* Franco (03/08/2004) */
EXTERN BddStates eau      ARGS((BddFsm_ptr, BddStates, BddStates, BddStates));
/* Franco (05/08/2004) */
EXTERN BddStates aau      ARGS((BddFsm_ptr, BddStates, BddStates, BddStates));
/* 04/08/11 sbusard */
EXTERN BddStates eaw      ARGS((BddFsm_ptr, BddStates, BddStates, BddStates));
EXTERN BddStates aaw      ARGS((BddFsm_ptr, BddStates, BddStates, BddStates));

EXTERN BddStates eaf      ARGS((BddFsm_ptr, BddStates, BddStates));
EXTERN BddStates eag      ARGS((BddFsm_ptr, BddStates, BddStates));
/* Franco (06/08/2004) */
EXTERN BddStates reachable ARGS((BddFsm_ptr, BddStates));

EXTERN BddStates ebu     ARGS((BddFsm_ptr, BddStates, BddStates, int, int));
EXTERN BddStates ebf     ARGS((BddFsm_ptr, BddStates, int, int));
EXTERN BddStates ebg     ARGS((BddFsm_ptr, BddStates, int, int));
EXTERN BddStates abu     ARGS((BddFsm_ptr, BddStates, BddStates, int, int));

EXTERN int       minu    ARGS((BddFsm_ptr, bdd_ptr, bdd_ptr));
EXTERN int       maxu    ARGS((BddFsm_ptr, bdd_ptr, bdd_ptr));

EXTERN node_ptr explain  ARGS((BddFsm_ptr, BddEnc_ptr, node_ptr, 
			       node_ptr, node_ptr));

EXTERN bdd_ptr  
eval_spec ARGS((BddFsm_ptr, BddEnc_ptr enc, node_ptr, node_ptr));

EXTERN node_ptr 
eval_formula_list ARGS((BddFsm_ptr, BddEnc_ptr enc, node_ptr, node_ptr));

EXTERN int 
eval_compute ARGS((BddFsm_ptr, BddEnc_ptr enc, node_ptr, node_ptr));

EXTERN Trace_ptr check_AG_only  ARGS((BddFsm_ptr, BddEnc_ptr, 
				      node_ptr, node_ptr));

EXTERN int      check_invariant_forward     ARGS((BddFsm_ptr, Prop_ptr));

EXTERN void     free_formula_list ARGS((DdManager *, node_ptr));

#endif /* __MC_H__ */
