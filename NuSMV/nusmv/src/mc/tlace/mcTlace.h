/**CHeaderFile*****************************************************************

  FileName    [mcTlace.h]

  PackageName [mc]

  Synopsis    [The header file for the TLACE package.]

  Description [The TLACE package contains the implementation of tree-like
  annotated counter-examples. This includes the data structure and the
  associated functions, their generation and XML export.]

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

  Version    [02/08/2011]

******************************************************************************/

#ifndef __MCTLACE_H__
#define __MCTLACE_H__

#include <stdio.h>
#include "mcTlacenode.h"
#include "utils/utils_io.h"

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN tlacenode_ptr tlaces_explain	ARGS((	BddFsm_ptr fsm,
											BddEnc_ptr enc,
											bdd_ptr state,
											node_ptr formula ));

EXTERN void tlaces_export 			ARGS((	BddFsm_ptr fsm,
											BddEnc_ptr enc,
											tlacenode_ptr node,
											node_ptr formula,
											FILE * output ));

#endif /* __MCTLACE_H__ */
