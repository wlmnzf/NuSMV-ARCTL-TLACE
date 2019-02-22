/**CFile***********************************************************************

  FileName [bmcBmcInc.c]

  PackageName [bmc]

  Synopsis [High level functionalities layer for BMC (incremental) algorithms]

  Description [User-commands directly use function defined in this module.]

  SeeAlso  []

  Author   [Roberto Cavada, Andrei Tchaltsev]

  Copyright [ This file is part of the ``bmc'' package of NuSMV
  version 2.  Copyright (C) 2004 by ITC-irst.

  NuSMV version 2 is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  as published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version.

  NuSMV version 2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  USA.

  For more information of NuSMV see <http://nusmv.irst.itc.it> or
  email to <nusmv-users@irst.itc.it>.  Please report bugs to
  <nusmv-users@irst.itc.it>.

  To contact the NuSMV development board, email to
  <nusmv@irst.itc.it>. ]

******************************************************************************/


#include "bmcBmc.h"
#include "bmcInt.h"
#include "bmcGen.h"
#include "bmcTableau.h"
#include "bmcConv.h"
#include "bmcDump.h"
#include "bmcModel.h"
#include "bmcVarsMgr.h"
#include "bmcWff.h"
#include "bmcUtils.h"

#include "dag/dag.h"

#include "node/node.h"
#include "be/be.h"
#include "mc/mc.h" /* for print_spec */

#include "sat/sat.h" /* for solver and result */
#include "sat/SatSolver.h"
#include "sat/SatIncSolver.h"

#include "enc/enc.h"

#ifdef BENCHMARKING
  #include <time.h>
  clock_t start_time;
#endif

static char rcsid[] UTIL_UNUSED = "$Id: bmcBmcInc.c,v 1.1.2.13 2004/08/04 16:32:43 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* The file is compiled only if there is at least one incremental SAT solver*/
/*---------------------------------------------------------------------------*/
#if HAVE_INCREMENTAL_SAT

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Functions                                                                 */
/*---------------------------------------------------------------------------*/
static Be_Cnf_ptr 
bmc_add_be_into_solver ARGS((SatSolver_ptr solver,
			     SatSolverGroup group,
			     be_ptr prob, BmcVarsMgr_ptr vars_mgr));

static void  
bmc_add_be_into_solver_positively ARGS((SatSolver_ptr solver,
					SatSolverGroup group,
					be_ptr prob, BmcVarsMgr_ptr vars_mgr));

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**AutomaticStart*************************************************************/


/**AutomaticEnd***************************************************************/



/**Function********************************************************************

  Synopsis      [Solves LTL problem the same way as the original
  Bmc_GenSolveLtl but just adds BE representing the path incrementaly.]

  Description        []

  SideEffects        []

  SeeAlso            [Bmc_GenSolve_Action]

******************************************************************************/
int Bmc_GenSolveLtlInc(Prop_ptr ltlprop,
		       const int k, const int relative_loop,
		       const boolean must_inc_length)
{
  node_ptr bltlspec;  /* Its booleanization */
  Bmc_Fsm_ptr be_fsm; /* The corresponding be fsm */
  BmcVarsMgr_ptr vars_mgr;
  /* sat solver instance */
  SatIncSolver_ptr solver;
  be_ptr beProb; /* A problem in BE format */
  /* ---------------------------------------------------------------------- */
  /* Here a property was selected                                           */
  /* ---------------------------------------------------------------------- */
  int k_max = k;
  int k_min = 0;
  int increasingK;
  int previousIncreasingK = 0; /* used to create Be of execution from time 0 */
  boolean found_solution = 0;
 
  if (!must_inc_length) k_min = k_max;

  /* checks that a property was selected: */
  nusmv_assert(ltlprop != PROP(NULL));

  /* checks if it has already been checked: */
  if (Prop_get_status(ltlprop) != Prop_Unchecked) {
    /* aborts this check */
    return 0;
  }

  /* solver construction: */
  solver = Sat_CreateIncSolver(get_sat_solver(options));
  if (solver == SAT_INC_SOLVER(NULL)) {
    fprintf(nusmv_stderr, "Incremental sat solver '%s' is not available.\n", 
	    get_sat_solver(options));
    return 1;
  }


  { /* booleanized, negated and NNFed formula: */
    node_ptr fltlspec;  
    fltlspec = Compile_FlattenSexpExpandDefine(Enc_get_symb_encoding(), 
					       Prop_get_expr(ltlprop), 
					       Nil);

    bltlspec = Bmc_Wff_MkNnf(Bmc_Wff_MkNot(detexpr2bexpr(fltlspec)));
  }

  if (opt_cone_of_influence(options) == true) {
    Prop_apply_coi_for_bmc(ltlprop, global_fsm_builder);
  }

  be_fsm = Prop_get_be_fsm(ltlprop);
  if (be_fsm == (Bmc_Fsm_ptr) NULL) {
    Prop_set_fsm_to_master(ltlprop);
    be_fsm = Prop_get_be_fsm(ltlprop);
    nusmv_assert(be_fsm != (Bmc_Fsm_ptr) NULL);
  }
  
  vars_mgr = Bmc_Fsm_GetVarsManager(be_fsm);
  
  /* insert initial conditions into the sat solver permanently */
  beProb = Bmc_Model_GetInit0(be_fsm);
  bmc_add_be_into_solver_positively(SAT_SOLVER(solver),
			   SatSolver_get_permanent_group(SAT_SOLVER(solver)),
			   beProb,
			   vars_mgr);
  
  /* Start problems generations: */
  for (increasingK = k_min; (increasingK <= k_max && !found_solution);
       ++increasingK) {
    int l;
    char szLoop[16]; /* to keep loopback string */
     /* additional group in sat solver */
    SatSolverGroup additionalGroup = SatIncSolver_create_group(solver);
    SatSolverResult satResult;

    /* the loopback value could be depending on the length
       if it were relative: */
    l = Bmc_Utils_RelLoop2AbsLoop(relative_loop, increasingK);

    /* this is for verbose messages */
    Bmc_Utils_ConvertLoopFromInteger(relative_loop, szLoop, sizeof(szLoop));

    /* prints a verbose message: */
    if (opt_verbose_level_gt(options, 0)) {
      if (Bmc_Utils_IsNoLoopback(l)) {
        fprintf(nusmv_stderr,
                "\nGenerating problem with bound %d, no loopback...\n",
                increasingK);
      }
      else if (Bmc_Utils_IsAllLoopbacks(l)) {
        fprintf(nusmv_stderr,
                "\nGenerating problem with bound %d, all possible loopbacks...\n",
                increasingK);
      }
      else {
        /* l can be negative iff loopback from the user pov is < -length */
        if ((l < increasingK) && (l >= 0)) {
          fprintf(nusmv_stderr,
                  "\nGenerating problem with bound %d, loopback %s...\n",
                  increasingK, szLoop);
        }
      }
    } /* verbose message */

    /* checks for loopback vs k compatibility */
    if (Bmc_Utils_IsSingleLoopback(l) && ((l >= increasingK) || (l < 0))) {
      fprintf(nusmv_stderr,
              "\nWarning: problem with bound %d and loopback %s is not allowed: skipped\n",
              increasingK, szLoop);
      continue;
    }

    /* Unroll the transition relation to the fixed frame 0 */
    if(previousIncreasingK < increasingK);
    {
      beProb = Bmc_Model_GetUnrolling(be_fsm, 
				      previousIncreasingK,
				      increasingK);
      bmc_add_be_into_solver_positively(SAT_SOLVER(solver),
			 SatSolver_get_permanent_group(SAT_SOLVER(solver)),
					beProb,
					vars_mgr);	     
      previousIncreasingK = increasingK;
    }
    
    /* add LTL tableau to an additional group of a solver */
    beProb = Bmc_Tableau_GetLtlTableau(be_fsm, bltlspec, increasingK, l);
    bmc_add_be_into_solver_positively(SAT_SOLVER(solver),
				       additionalGroup,
				       beProb,
				       vars_mgr);	     
    
#ifdef BENCHMARKING
    fprintf(nusmv_stdout, ":START:benchmarking Solving\n");
    start_time = clock();
#endif  
    
    satResult = SatSolver_solve_all_groups(SAT_SOLVER(solver));

#ifdef BENCHMARKING
      fprintf(nusmv_stdout, ":UTIME = %.4f secs.\n",
	      ((double)(clock()-start_time))/CLOCKS_PER_SEC);
      fprintf(nusmv_stdout, ":STOP:benchmarking Solving\n");
#endif

      /* Processes the result: */
      switch (satResult) {

      case SAT_SOLVER_UNSATISFIABLE_PROBLEM:
	{
	  char szLoopMsg[16]; /* for loopback part of message */
	  memset(szLoopMsg, 0, sizeof(szLoopMsg));

	  if (Bmc_Utils_IsAllLoopbacks(l)) {
	    strncpy(szLoopMsg, "", sizeof(szLoopMsg)-1);
	  }
	  else if (Bmc_Utils_IsNoLoopback(l)) {
	    strncpy(szLoopMsg, " and no loop", sizeof(szLoopMsg)-1);
	  }
	  else {
	    /* loop is Natural: */
	    strncpy(szLoopMsg, " and loop at ", sizeof(szLoopMsg)-1);
	    strncat(szLoopMsg, szLoop, sizeof(szLoopMsg)-1-strlen(szLoopMsg));
	  }

	  fprintf(nusmv_stdout,
		  "-- no counterexample found with bound %d%s",
		  increasingK, szLoopMsg);
	  if (opt_verbose_level_gt(options, 2)) {
	    fprintf(nusmv_stdout, " for "); 		  
	    print_spec(nusmv_stdout, Prop_get_expr(ltlprop));
	  }
	  fprintf(nusmv_stdout, "\n");

	  break;
	}
      case SAT_SOLVER_SATISFIABLE_PROBLEM:
        fprintf(nusmv_stdout, "-- ");
        print_spec(nusmv_stdout, Prop_get_expr(ltlprop));
        fprintf(nusmv_stdout, "  is false\n");
        Prop_set_status(ltlprop, Prop_False);

	found_solution = true;

	if (opt_counter_examples(options)) {
	  Trace_ptr trace = 
	    Bmc_Utils_generate_and_print_cntexample(vars_mgr, 
						    SAT_SOLVER(solver), 
						    Enc_get_bdd_encoding(), 
						    beProb, 
						    increasingK, 
						    "BMC Counterexample");
	  Prop_set_trace(ltlprop, Trace_get_id(trace));
	}

        break;

      case SAT_SOLVER_INTERNAL_ERROR:
        internal_error("Sorry, solver answered with a fatal Internal " 
		       "Failure during problem solving.\n");
	break;

      case SAT_SOLVER_TIMEOUT:
      case SAT_SOLVER_MEMOUT:
        internal_error("Sorry, solver ran out of resources and aborted "
		       "the execution.\n");
	break;

      default:
	internal_error("Bmc_GenSolveLtl: Unexpected value in satResult");
	
      } /* switch */
      
      SatIncSolver_destroy_group(solver, additionalGroup);
  }

  /* destroy the sat solver instance */
  SatIncSolver_destroy(solver);
  return 0;
}

/**Function********************************************************************

  Synopsis          [Solve an INVARSPEC problems with algorithm 
  ZigZag]

  Description        [The function will run not more then max_k transitions,
  then if the problem is not proved the function just returns 0]

  SideEffects        []

  SeeAlso            []

******************************************************************************/

int Bmc_GenSolveInvarZigzag(Prop_ptr invarprop, const int max_k)
{
  node_ptr binvarspec;  /* Its booleanization */
  Bmc_Fsm_ptr be_fsm; /* The corresponding be fsm */
  BmcVarsMgr_ptr vars_mgr;
  Be_Manager_ptr be_mgr;

  /* outputs the name of the algorithm */
  if (opt_verbose_level_gt(options, 2)) {
    fprintf(nusmv_stderr,
	    "The invariant solving algorithm is ZigZag\n");
  }
  /* checks that a property was selected: */
  nusmv_assert(invarprop != PROP(NULL));

  /* checks that a property was selected: */
  nusmv_assert(invarprop != (Prop_ptr)NULL);


  /* checks if it has already been checked: */
  if (Prop_get_status(invarprop) != Prop_Unchecked) {
    /* aborts this check */
    return 0;
  }

  { /* booleanized, negated and NNFed formula: */
    node_ptr finvarspec;  
    finvarspec = Compile_FlattenSexpExpandDefine(Enc_get_symb_encoding(), 
						 Prop_get_expr(invarprop), 
						 Nil);
    binvarspec = Bmc_Wff_MkNnf(detexpr2bexpr(finvarspec));
  }

  if (opt_cone_of_influence(options) == true) {
    Prop_apply_coi_for_bmc(invarprop, global_fsm_builder);
  }

  be_fsm = Prop_get_be_fsm(invarprop); 
  if (be_fsm == (Bmc_Fsm_ptr) NULL) {
    Prop_set_fsm_to_master(invarprop);
    be_fsm = Prop_get_be_fsm(invarprop);
    nusmv_assert(be_fsm != (Bmc_Fsm_ptr) NULL);
  }

  vars_mgr = Bmc_Fsm_GetVarsManager(be_fsm);
  be_mgr = BmcVarsMgr_GetBeMgr(vars_mgr);

  
  /* begin the solving of the problem: */
  if (opt_verbose_level_gt(options, 0)) {
    fprintf(nusmv_stderr, "\nSolving invariant problem (ZigZag)\n");
  }

  /* start of ZifZag algorithm */
  {
    be_ptr be_invar;
    int stepN;
    SatIncSolver_ptr solver;
    SatSolverGroup group_init; /* a SAT group containing initial state CNF */
    lsList group_list_init; /* a list containing just the initial group */

    /* Initialiaze the incremental SAT solver */
    solver = Sat_CreateIncSolver(get_sat_solver(options));
    if (solver == SAT_INC_SOLVER(NULL)) {
      fprintf(nusmv_stderr, "Incremental sat solver '%s' is not available.\n", 
	      get_sat_solver(options));
      return 1;
    }

    be_invar = Bmc_Conv_Bexp2Be(vars_mgr, binvarspec);

    /* create a group for CNF of initial states and a list for this group */
    group_init = SatIncSolver_create_group(solver); 
    group_list_init = lsCreate();
    lsNewBegin(group_list_init, (lsGeneric)group_init, 0);
    
    /* insert initial state into a special group */
    bmc_add_be_into_solver_positively( SAT_SOLVER(solver),
				       group_init,
				       Bmc_Model_GetInit0(be_fsm),
				       vars_mgr);

    for (stepN=0; stepN <= max_k; ++stepN) {
      SatSolverResult satResult;
      SatSolverGroup additionalGroup;
      int l;
      
      be_ptr prob_k;
      Be_Cnf_ptr cnf_prob_k;

      if (opt_verbose_level_gt(options, 0)) {
	fprintf(nusmv_stderr, "\nExtending the step to k=%d\n", stepN);
      }

      /* create a new group in solver */
      additionalGroup = SatIncSolver_create_group(solver);
      /* Obtain BE of the invariant at time k */
      prob_k = BmcVarsMgr_ShiftCurrNext2Time(vars_mgr, be_invar, stepN);
      /* Insert the invariant at time k  but not set the polarity */
      cnf_prob_k = bmc_add_be_into_solver( SAT_SOLVER(solver),
			   SatSolver_get_permanent_group(SAT_SOLVER(solver)),
			   prob_k,
			   vars_mgr);
      /* Set the polarity at time k to flase in the additiona group */
      SatSolver_set_polarity(SAT_SOLVER(solver), cnf_prob_k, -1, 
			     additionalGroup);

      /* SOLVE (withtout the initial state) */
      satResult = SatIncSolver_solve_without_groups(solver, group_list_init);
	
      /* Result processing: */
      switch (satResult) {

      case SAT_SOLVER_UNSATISFIABLE_PROBLEM:
	fprintf(nusmv_stdout, "-- ");
	print_invar(nusmv_stdout, Prop_get_expr(invarprop));
	fprintf(nusmv_stdout, "  is true\n");
	Prop_set_status(invarprop, Prop_True);
	    
	lsDestroy(group_list_init, 0);
	Be_Cnf_Delete(cnf_prob_k);
	SatIncSolver_destroy(solver);
	return 0;

      case SAT_SOLVER_SATISFIABLE_PROBLEM:
	break;

      case SAT_SOLVER_INTERNAL_ERROR:
	internal_error("Sorry, solver answered with a fatal Internal "
		       "Failure during problem solving.\n");

      case SAT_SOLVER_TIMEOUT:
      case SAT_SOLVER_MEMOUT:
	internal_error("Sorry, solver ran out of resources and aborted "
		       "the execution.\n");

      default:
	internal_error("Bmc_GenSolveLtl: Unexpected value in satResult");
	  
      } /* switch */

      if (opt_verbose_level_gt(options, 0)) {
	fprintf(nusmv_stderr, "\nExtending the base to k=%d\n", stepN);
      }
	

      /* SOLVE with initial states CNF */
      satResult = SatSolver_solve_all_groups(SAT_SOLVER(solver));
	
      /* Processes the result: */
      switch (satResult) {
      
      case SAT_SOLVER_SATISFIABLE_PROBLEM:
	fprintf(nusmv_stdout, "-- ");
	print_invar(nusmv_stdout, Prop_get_expr(invarprop));
	fprintf(nusmv_stdout, "  is false\n");	    
	Prop_set_status(invarprop, Prop_False);

	if (opt_counter_examples(options)) {
	  Trace_ptr trace = 
	    Bmc_Utils_generate_and_print_cntexample(vars_mgr, SAT_SOLVER(solver),
						    Enc_get_bdd_encoding(), 
						    prob_k, stepN, 
						    "BMC Counterexample");
	  Prop_set_trace(invarprop, Trace_get_id(trace));
	} 
	    
	lsDestroy(group_list_init, 0);
	Be_Cnf_Delete(cnf_prob_k);
	SatIncSolver_destroy(solver);
	return 0;
	    
      case  SAT_SOLVER_UNSATISFIABLE_PROBLEM:
	if (opt_verbose_level_gt(options, 0)) {
	  fprintf(nusmv_stderr, "No counter-example path of length %d found\n", 
		  stepN);
	}
	break;
	  
      case SAT_SOLVER_INTERNAL_ERROR:
	internal_error("Sorry, solver answered with a fatal Internal "
		       "Failure during problem solving.\n");
	  
      case SAT_SOLVER_TIMEOUT:
      case SAT_SOLVER_MEMOUT:
	internal_error("Sorry, solver ran out of resources and aborted "
		       "the execution.\n");

      default:
	internal_error("Bmc_GenSolveLtl: Unexpected value in satResult");
	
      } /* switch */
       
      /* Destrioy existing additional group with polarity of prob_k */
      SatIncSolver_destroy_group(solver, additionalGroup);

      /* Set the polarity of prob_k as true permaently */
      SatSolver_set_polarity(SAT_SOLVER(solver), cnf_prob_k, 1,
			     SatSolver_get_permanent_group(SAT_SOLVER(solver)));
      Be_Cnf_Delete(cnf_prob_k);

      /* Insert transition relation (stepN,stepN+1) permanently */
      {
	be_ptr unrolling = Bmc_Model_GetUnrolling(be_fsm, stepN, stepN+1);
	bmc_add_be_into_solver_positively( SAT_SOLVER(solver),
			      SatSolver_get_permanent_group(SAT_SOLVER(solver)),
					   unrolling,
					   vars_mgr);
      }
	
      { /* Insert and force to true
	   not_equal(i,stepN) for each 0 <= i < stepN permanently */

	const int last_idx = BmcVarsMgr_GetFirstCurrStateVarIndex(vars_mgr) + 
	  BmcVarsMgr_GetStateVarsNum(vars_mgr);     /* just shortcuts */

	for (l = 0; l < stepN; ++l) {
	  int v;
	  be_ptr not_equal = Be_Falsity(be_mgr);
	  
	  for (v = BmcVarsMgr_GetFirstCurrStateVarIndex(vars_mgr); 
	       v < last_idx; ++v) {
	    be_ptr be_var = Be_Index2Var(be_mgr, v);
	    be_ptr be_xor =
	      Be_Xor(be_mgr,
		     BmcVarsMgr_ShiftCurrNext2Time(vars_mgr, be_var, l),
		     BmcVarsMgr_ShiftCurrNext2Time(vars_mgr, be_var, stepN));
	    not_equal = Be_Or(be_mgr, not_equal, be_xor);
	  }
	  
	  bmc_add_be_into_solver_positively( SAT_SOLVER(solver),
			     SatSolver_get_permanent_group(SAT_SOLVER(solver)),
			     not_equal, vars_mgr);
	} /* for loop */
      }
	
      /* Print out the current state of solving */
      fprintf(nusmv_stdout,
	      "-- no proof or counterexample found with bound %d", stepN);
      if (opt_verbose_level_gt(options, 2)) {
	fprintf(nusmv_stdout, " for "); 		  
	print_invar(nusmv_stdout, Prop_get_expr(invarprop));
      }
      fprintf(nusmv_stdout, "\n");
	
    } /* for loop on stepN */

    /* release the list with the group containing initial states CNF */
    lsDestroy(group_list_init, 0);
    /* Release the incremental sat solver */
    SatIncSolver_destroy(solver);
  } /* end of ZifZag algorithm */

  return 0;  
}
  

/**Function********************************************************************

  Synopsis      [Solve an INVARSPEC problems wiht algorithm Dual]

  Description        [The function tries to solve the problem
  with not more then max_k transitions. If the problem is not
  solved after max_k transition then the function returns 0.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
int Bmc_GenSolveInvarDual(Prop_ptr invarprop, const int max_k)
{
  node_ptr binvarspec;  /* Its booleanization */
  be_ptr be_invar;  /* Its BE representation */
  Bmc_Fsm_ptr be_fsm; /* The corresponding be fsm */

  BmcVarsMgr_ptr vars_mgr;
  Be_Manager_ptr be_mgr;


  /* outputs the name of the algorithm */
  if (opt_verbose_level_gt(options, 2)) {
    fprintf(nusmv_stderr,
	    "The invariant solving algorithm is Dual\n");
  }

  /* checks that a property was selected: */
  nusmv_assert(invarprop != PROP(NULL));

  /* checks that a property was selected: */
  nusmv_assert(invarprop != (Prop_ptr)NULL);


  /* checks if it has already been checked: */
  if (Prop_get_status(invarprop) != Prop_Unchecked) {
    /* aborts this check */
    return 0;
  }

  { /* booleanized, negated and NNFed formula: */
    node_ptr finvarspec;  
    finvarspec = Compile_FlattenSexpExpandDefine(Enc_get_symb_encoding(), 
						 Prop_get_expr(invarprop), 
						 Nil);
    binvarspec = Bmc_Wff_MkNnf(detexpr2bexpr(finvarspec));
  }

  if (opt_cone_of_influence(options) == true) {
    Prop_apply_coi_for_bmc(invarprop, global_fsm_builder);
  }

  be_fsm = Prop_get_be_fsm(invarprop); 
  if (be_fsm == (Bmc_Fsm_ptr) NULL) {
    Prop_set_fsm_to_master(invarprop);
    be_fsm = Prop_get_be_fsm(invarprop);
    nusmv_assert(be_fsm != (Bmc_Fsm_ptr) NULL);
  }

  vars_mgr = Bmc_Fsm_GetVarsManager(be_fsm);
  be_mgr = BmcVarsMgr_GetBeMgr(vars_mgr);

  /* checks that no input variables are in the fsm,
   since DUAL currently can not work when there are input vars.
   The reason: the implementation does not maintain 
   0 state with input variable, but DUAL creates transactions in
   opposite direction and so need to build a transaction FROM 
   state 1 TO state 0. So state 0 need input vars.
   */
  if ( BmcVarsMgr_GetInputVarsNum(vars_mgr) > 0) {
    fprintf (nusmv_stderr, 
	     "Currently Dual algorithm can not be used when the model "
	     "being checked \ncontains input variables. "
	     "Use ZigZag algorithm instead.\n");
    return 1;
  }

  /* begin the solving of the problem: */
  if (opt_verbose_level_gt(options, 0)) {
    fprintf(nusmv_stderr, "\nSolving invariant problem (Dual)\n");
  }
  
  /* start of Dual algorithm */
  {
    Be_Cnf_ptr cnf;
    SatIncSolver_ptr solver_base;
    SatIncSolver_ptr solver_step;
    int stepN;
    int l;

    /* Initialiaze two incremental SAT solvers */
    solver_base = Sat_CreateIncSolver(get_sat_solver(options));
    if (solver_base == SAT_INC_SOLVER(NULL)) {
      fprintf(nusmv_stderr, "Incremental sat solver '%s' is not available.\n", 
	      get_sat_solver(options));
      return 1;
    }

    solver_step = Sat_CreateIncSolver(get_sat_solver(options));
    if (solver_step == SAT_INC_SOLVER(NULL)) {
      fprintf(nusmv_stderr, "Incremental sat solver '%s' is not available.\n", 
	      get_sat_solver(options));
      return 1;
    }

    be_invar = Bmc_Conv_Bexp2Be(vars_mgr, binvarspec);

    /* Insert the initial states to 'base' solver */
    bmc_add_be_into_solver_positively( SAT_SOLVER(solver_base),
  		       SatSolver_get_permanent_group(SAT_SOLVER(solver_base)),
				       Bmc_Model_GetInit0(be_fsm),
				       vars_mgr);

    /* Insert the negative invariant property at time 0 to 'step' solver */
    cnf = bmc_add_be_into_solver( SAT_SOLVER(solver_step),
  		       SatSolver_get_permanent_group(SAT_SOLVER(solver_step)),
		       BmcVarsMgr_ShiftCurrNext2Time(vars_mgr, be_invar, 0),
				  vars_mgr);
    SatSolver_set_polarity(SAT_SOLVER(solver_step), cnf, -1,
		    SatSolver_get_permanent_group(SAT_SOLVER(solver_step)));

    Be_Cnf_Delete(cnf);
    
    for (stepN=0; stepN <= max_k; ++stepN) {
      be_ptr prob_k;
      SatSolverGroup additionalGroup;
      SatSolverResult satResult;
      /* ---------------------- */
      /* --- Extending base --- */
      /* ---------------------- */
   
      if (opt_verbose_level_gt(options, 0)) {
	fprintf(nusmv_stderr, "\nExtending the base to k=%d\n", stepN);
      }

      /* create a new group in solver */
      additionalGroup = SatIncSolver_create_group(solver_base);
      /* BE of the invariant at time k */
      prob_k = BmcVarsMgr_ShiftCurrNext2Time(vars_mgr, be_invar, stepN);
      
      /* Insert the invariant at time k  but not set the polarity */
      cnf =  bmc_add_be_into_solver( SAT_SOLVER(solver_base),
		       SatSolver_get_permanent_group(SAT_SOLVER(solver_step)),
				     prob_k,
				     vars_mgr);
      
      /* Set the polarity at time k to flase in the additiona group */
      SatSolver_set_polarity(SAT_SOLVER(solver_base), cnf, -1, additionalGroup);
      
      
       /* SOLVE (base) */
      satResult = SatSolver_solve_all_groups(SAT_SOLVER(solver_base));
         

      /* Result processing: */
      switch (satResult) {

      case SAT_SOLVER_SATISFIABLE_PROBLEM:
	fprintf(nusmv_stdout, "-- ");
	print_invar(nusmv_stdout, Prop_get_expr(invarprop));
	fprintf(nusmv_stdout, "  is false\n");
	Prop_set_status(invarprop, Prop_False);

	if (opt_counter_examples(options)) {
	  Trace_ptr trace = 
	    Bmc_Utils_generate_and_print_cntexample(vars_mgr, 
						    SAT_SOLVER(solver_base), 
						    Enc_get_bdd_encoding(), 
						    prob_k, stepN, 
						    "BMC Counterexample");
	  Prop_set_trace(invarprop, Trace_get_id(trace));
	}

	Be_Cnf_Delete(cnf);
	SatIncSolver_destroy(solver_step);
	SatIncSolver_destroy(solver_base);
	return 0;

      case  SAT_SOLVER_UNSATISFIABLE_PROBLEM:
	if (opt_verbose_level_gt(options, 0)) {
	  fprintf(nusmv_stderr, "No counter-example path of length %d found\n", 
		  stepN);
	}
	break;

      case SAT_SOLVER_INTERNAL_ERROR:
	internal_error("Sorry, solver answered with a fatal Internal "
		       "Failure during problem solving.\n");
	  
      case SAT_SOLVER_TIMEOUT:
      case SAT_SOLVER_MEMOUT:
	internal_error("Sorry, solver ran out of resources and aborted "
		       "the execution.\n");

      default:
	internal_error("Bmc_GenSolveLtl: Unexpected value in satResult");
	
      } /* switch */

      /* Destrioy existing additional group with polarity of prob_k */
      SatIncSolver_destroy_group(solver_base, additionalGroup);

       /* Set the polarity of prob_k as true permaently */
      SatSolver_set_polarity(SAT_SOLVER(solver_base), cnf, 1,
		       SatSolver_get_permanent_group(SAT_SOLVER(solver_base)));
      Be_Cnf_Delete(cnf);

      /* Insert transition relation (steapN,stepN+1) permanently */
      {
	be_ptr unrolling = Bmc_Model_GetUnrolling(be_fsm, stepN, stepN+1);
	bmc_add_be_into_solver_positively( SAT_SOLVER(solver_base),
			 SatSolver_get_permanent_group(SAT_SOLVER(solver_base)),
					   unrolling,
					   vars_mgr);
      }

      
      /* ---------------------- */
      /* --- Extending step --- */
      /* ---------------------- */

      /* Note: because the transpose of the transition relation is used,
	 the bad goal state is state 0 and the current "initial" state is
	 state stepN */
      
      if (opt_verbose_level_gt(options, 0)) {
	fprintf(nusmv_stderr, "\nExtending the step to k=%d\n", stepN);
      }
      
      /* SAT problem solving */
      satResult = SatSolver_solve_all_groups(SAT_SOLVER(solver_step));
 
      /* Processes the result: */
      switch (satResult) {
 	
      case SAT_SOLVER_SATISFIABLE_PROBLEM:
	break;
	
      case SAT_SOLVER_UNSATISFIABLE_PROBLEM:
	fprintf(nusmv_stdout, "-- ");
	print_invar(nusmv_stdout, Prop_get_expr(invarprop));
	fprintf(nusmv_stdout, "  is true\n");
	Prop_set_status(invarprop, Prop_True);

	SatIncSolver_destroy(solver_step);
	SatIncSolver_destroy(solver_base);
	return 0;

      case SAT_SOLVER_INTERNAL_ERROR:
	internal_error("Sorry, solver answered with a fatal Internal "
		       "Failure during problem solving.\n");
	  
      case SAT_SOLVER_TIMEOUT:
      case SAT_SOLVER_MEMOUT:
	internal_error("Sorry, solver ran out of resources and aborted "
		       "the execution.\n");

      default:
	internal_error("Bmc_GenSolveLtl: Unexpected value in satResult");
	
      } /* switch */

 
    
      /* Insert the transion (stepN+1,stepN) to 'step' solver */
      {

        /* below it the code code from Bmc_Model_GetUnrolling 
           (we cannot use it directly, 
	   because k must be greater then j, see Bmc_Model_GetUnrolling) */
	be_ptr invar;
	be_ptr trans;

	/* restore the code below */
	trans = BmcVarsMgr_ShiftCurrNext2Times(vars_mgr, 
					       Bmc_Fsm_GetTrans(be_fsm),
					       stepN+1, stepN);	
	/* invars at step stepN */
	invar = BmcVarsMgr_ShiftCurrNext2Time(vars_mgr,
					      Bmc_Fsm_GetInvar(be_fsm),
					      stepN);
	trans = Be_And(be_mgr, trans, invar);
	
	/* invars at step stepN+1 */
	invar = BmcVarsMgr_ShiftCurrNext2Time(vars_mgr,
					      Bmc_Fsm_GetInvar(be_fsm),
					      stepN+1);
	trans = Be_And(be_mgr, trans, invar);
	
	bmc_add_be_into_solver_positively( SAT_SOLVER(solver_step),
			  SatSolver_get_permanent_group(SAT_SOLVER(solver_step)),
					   trans,
					   vars_mgr);
      }
     
      /* Insert the invarian at time stepN+1 to 'step' solver */
      bmc_add_be_into_solver_positively(
                  SAT_SOLVER(solver_step),
		  SatSolver_get_permanent_group(SAT_SOLVER(solver_step)),
		  BmcVarsMgr_ShiftCurrNext2Time(vars_mgr, be_invar, stepN+1),
		  vars_mgr);

      { /* Insert and force to true
	 not_equal(i,stepN+1) for each 0 < i < stepN+1 to 'step solver
	 Note: we do not need to care about state 0 because it is bad, 
	 therefore can not be equal to all other states, which are good */

	const int last_idx = BmcVarsMgr_GetFirstCurrStateVarIndex(vars_mgr) + 
	  BmcVarsMgr_GetStateVarsNum(vars_mgr); 	/* just shortcuts */

	for (l = 1; l < stepN + 1; ++l) {
	  int v;
	  be_ptr not_equal = Be_Falsity(be_mgr);
		  
	  for (v = BmcVarsMgr_GetFirstCurrStateVarIndex(vars_mgr); 
	       v < last_idx; ++v) {
	    be_ptr be_var = Be_Index2Var(be_mgr, v);
	    be_ptr be_xor =
	      Be_Xor(be_mgr,
		     BmcVarsMgr_ShiftCurrNext2Time(vars_mgr, be_var, l),
		     BmcVarsMgr_ShiftCurrNext2Time(vars_mgr, be_var, stepN+1));
	    not_equal = Be_Or(be_mgr, not_equal, be_xor);
	  }

	  bmc_add_be_into_solver_positively (SAT_SOLVER(solver_step),
		    SatSolver_get_permanent_group(SAT_SOLVER(solver_step)),
		    not_equal, vars_mgr);
	} /* for loop */
      }
      
      /* Print out the current state of solving */
      fprintf(nusmv_stdout,
	      "-- no proof or counterexample found with bound %d", stepN);
      if (opt_verbose_level_gt(options, 2)) {
	fprintf(nusmv_stdout, " for "); 		  
	print_invar(nusmv_stdout, Prop_get_expr(invarprop));
      }
      fprintf(nusmv_stdout, "\n");

    } /* for loop on stepN */

    /* Release the incremental sat solvers */
    SatIncSolver_destroy(solver_step);
    SatIncSolver_destroy(solver_base);
  } /* end of Dual algorithm */

  return 0;
}




/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis      [Converts Be into CNF, and adds it into a group of a solver.]

  Description        [Outputs into nusmv_stdout the total time 
  of conversion and adding BE to solver. It is resposibility of the invoker
  to destroy returned CNF (with Be_Cnf_Delete)]

  SideEffects        [creates an instance of CNF formula. (do not forget to
  delete it)]

  SeeAlso            []

******************************************************************************/
static Be_Cnf_ptr bmc_add_be_into_solver(SatSolver_ptr solver,
					 SatSolverGroup group,
					 be_ptr prob,
					 BmcVarsMgr_ptr vars_mgr)
{
  Be_Cnf_ptr cnf;
#ifdef BENCHMARKING
  fprintf(nusmv_stdout, ":START:benchmarking CNF-ization\n");
  start_time = clock();
#endif   

  cnf = Be_ConvertToCnf(BmcVarsMgr_GetBeMgr(vars_mgr), prob); 
  SatSolver_add(solver, cnf, group);

#ifdef BENCHMARKING
  fprintf(nusmv_stdout, ":UTIME = %.4f secs.\n",
	  ((double)(clock()-start_time))/CLOCKS_PER_SEC);
  fprintf(nusmv_stdout, ":STOP:benchmarking  CNF-ization\n");
#endif

  return cnf;
}

/**Function********************************************************************

  Synopsis      [Converts Be into CNF, and adds it into a group of a solver,
  sets polarity to 1, and then destroys the CNF.]

  Description        [Outputs into nusmv_stdout the total time 
  of conversion, adding, setting polarity and destroying BE. ]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
static void  bmc_add_be_into_solver_positively(SatSolver_ptr solver,
					       SatSolverGroup group,
					       be_ptr prob,
					       BmcVarsMgr_ptr vars_mgr)
{
  Be_Cnf_ptr cnf;
  
#ifdef BENCHMARKING
  fprintf(nusmv_stdout, ":START:benchmarking initial state CNF-ization\n");
  start_time = clock();
#endif   
  
  cnf = Be_ConvertToCnf(BmcVarsMgr_GetBeMgr(vars_mgr), prob);
  SatSolver_add(solver, cnf, group);
  SatSolver_set_polarity(solver, cnf, 1, group);
  Be_Cnf_Delete(cnf);
    
#ifdef BENCHMARKING
  fprintf(nusmv_stdout, ":UTIME = %.4f secs.\n",
	  ((double)(clock()-start_time))/CLOCKS_PER_SEC);
  fprintf(nusmv_stdout, ":STOP:benchmarking initial state CNF-ization\n");
#endif
}

#endif 
/* HAVE_INCREMENTAL_SAT */
