/**CFile***********************************************************************

 FileName    [mcMc.c]

 PackageName [mc]

 Synopsis    [Fair CTL model checking routines.]

 Description [Fair CTL model checking routines.]

 SeeAlso     [mcExplain.c]

 Author      [Marco Roveri]

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

#include "mc.h"
#include "mcInt.h" 
#include "fsm/bdd/FairnessList.h"
#include "parser/symbols.h"
#include "utils/error.h"
#include "utils/utils_io.h"
#include "trace/Trace.h"
#include "trace/TraceManager.h"
#include "enc/enc.h"
#include "tlace/mcTlace.h"

static char rcsid[] UTIL_UNUSED = "$Id: mcMc.c,v 1.13.2.57.2.1 2004/07/19 10:13:45 nusmv Exp $";

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static BddStatesInputs
Mc_get_fair_si_subset ARGS((BddFsm_ptr fsm,
				BddStatesInputs si));

static BddStatesInputs
Mc_fair_si_iteration ARGS((BddFsm_ptr fsm,
				bdd_ptr states,
				bdd_ptr subspace));

/* sbusard 16/02/11 */
static BddStatesInputs
ex_si_aware ARGS((BddFsm_ptr fsm, BddStatesInputs si));

/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

 Synopsis    [Verifies that M,s0 |= alpha ]

 Description [Verifies that M,s0 |= alpha using the fair CTL model checking.]

 SideEffects []

 SeeAlso     []

 ******************************************************************************/
void Mc_CheckCTLSpec(Prop_ptr prop) {
	tlacenode_ptr exp;
	Trace_ptr trace;
	bdd_ptr s0, tmp_1, tmp_2;
	BddFsm_ptr fsm;
	Expr_ptr spec = Prop_get_expr(prop);
	BddEnc_ptr enc = Enc_get_bdd_encoding();

	if (opt_verbose_level_gt(options, 0)) {
		fprintf(nusmv_stderr, "evaluating ");
		print_spec(nusmv_stderr, spec);
		fprintf(nusmv_stderr, "\n");
	}

	if (opt_cone_of_influence(options) == true) {
		Prop_apply_coi_for_bdd(prop, global_fsm_builder);
	}

	fsm = Prop_get_bdd_fsm(prop);

	if (fsm == BDD_FSM(NULL)) {
		Prop_set_fsm_to_master(prop);
		fsm = Prop_get_bdd_fsm(prop);
		BDD_FSM_CHECK_INSTANCE(fsm);
	}

	/* Evaluates the spec */
	s0 = eval_spec(fsm, enc, spec, Nil);

	tmp_1 = bdd_not(dd_manager, s0);
	tmp_2 = BddFsm_get_state_constraints(fsm);
	bdd_and_accumulate(dd_manager, &tmp_2, tmp_1);
	bdd_free(dd_manager, tmp_1);
	tmp_1 = BddFsm_get_fair_states(fsm);
	bdd_and_accumulate(dd_manager, &tmp_2, tmp_1);
	bdd_free(dd_manager, tmp_1);
	bdd_free(dd_manager, s0);

	s0 = BddFsm_get_init(fsm);
	bdd_and_accumulate(dd_manager, &s0, tmp_2);
	bdd_free(dd_manager, tmp_2);

	/* Prints out the result, if not true explain. */
	fprintf(nusmv_stdout, "-- ");
	print_spec(nusmv_stdout, spec);

	if (bdd_is_zero(dd_manager, s0)) {
		fprintf(nusmv_stdout, "is true\n");
		Prop_set_status(prop, Prop_True);
	} else {
		int tr = 0;
		fprintf(nusmv_stdout, "is false\n");
		Prop_set_status(prop, Prop_False);

		if (opt_counter_examples(options)) {
			tmp_1 = BddEnc_pick_one_state(enc, s0);
			bdd_free(dd_manager, s0);
			s0 = bdd_dup(tmp_1);
			bdd_free(dd_manager, tmp_1);

			/* 27/04/10 sbusard */
			if (opt_tlaces_explain_level(options) > 0) {
				/* 03/08/11 sbusard */
				exp	= tlaces_explain(fsm, enc, s0, spec);

				fprintf(nusmv_stdout,
						"-- as demonstrated by the following execution sequence\n");

				// Display counter-example
				FILE* fileid;
				if (get_tlaces_output_file(options) != NIL(char)) {

					fileid = fopen(get_tlaces_output_file(options), "w");
					if (fileid == (FILE*) NULL) {
						fprintf(nusmv_stderr, "Unable to open file \"%s\".\n",
								get_tlaces_output_file(options));
						nusmv_exit(1);
					}
				} else
					fileid = nusmv_stdout;

				if (opt_verbose_level_gt(options, 0) && get_tlaces_output_file(options) != NIL(char)) {
					fprintf(nusmv_stderr,
							"Writing counter-example into file \"%s\"...",
							get_tlaces_output_file(options));
				} else if (opt_verbose_level_gt(options, 0)) {
					fprintf(nusmv_stderr,
							"Writing counter-example into standard output...");
				}

				tlaces_export(fsm, enc, exp, spec, fileid);

				if (opt_verbose_level_gt(options, 0))
					fprintf(nusmv_stderr, "... done.\n");

				if (get_tlaces_output_file(options) != NIL(char)) {
					fflush(fileid);
					fclose(fileid);
				}

				tlacenode_free_node(exp);
			} else {
				// OLD VERSION
				exp = reverse(explain(fsm, enc, cons((node_ptr) bdd_dup(s0),
						Nil), spec, Nil));

				if (exp == Nil) {
					// The counterexample consists of one initial state
					exp = cons((node_ptr) bdd_dup(s0), Nil);
				}

				trace = Trace_create_from_state_input_list(enc,
						"CTL Counterexample", TRACE_TYPE_CNTEXAMPLE, exp);

				tr = TraceManager_register_trace(global_trace_manager, trace);
				Prop_set_trace(prop, Trace_get_id(trace));

				fprintf(nusmv_stdout,
						"-- as demonstrated by the following execution sequence\n");

				TraceManager_execute_plugin(global_trace_manager,
						TraceManager_get_default_plugin(global_trace_manager),
						tr);

				walk_dd(dd_manager, bdd_free, exp);
				free_list(exp);
			}
		}
	}

	bdd_free(dd_manager, s0);
}

/**Function********************************************************************

 Synopsis    [Compute quantitative characteristics on the model.]

 Description [Compute the given quantitative characteristics on the model.]

 SideEffects []

 SeeAlso     []

 ******************************************************************************/
void Mc_CheckCompute(Prop_ptr prop) {
	int s0;
	Expr_ptr spec = Prop_get_expr(prop);
	BddFsm_ptr fsm;
	BddEnc_ptr enc = Enc_get_bdd_encoding();

	if (opt_verbose_level_gt(options, 0)) {
		fprintf(nusmv_stderr, "evaluating ");
		print_spec(nusmv_stderr, spec);
		fprintf(nusmv_stderr, "\n");
	}

	if (opt_cone_of_influence(options) == true) {
		Prop_apply_coi_for_bdd(prop, global_fsm_builder);
	}

	fsm = Prop_get_bdd_fsm(prop);

	if (fsm == BDD_FSM(NULL)) {
		Prop_set_fsm_to_master(prop);
		fsm = Prop_get_bdd_fsm(prop);
		BDD_FSM_CHECK_INSTANCE(fsm);
	}

	{
		/*
		 We force computation of reachable states, as the following
		 calls will be performed more efficiently since they are cached.
		 */
		bdd_ptr r = BddFsm_get_reachable_states(fsm);
		bdd_free(dd_manager, r);
	}

	s0 = eval_compute(fsm, enc, spec, Nil);

	fprintf(nusmv_stdout, "-- ");
	print_compute(nusmv_stdout, spec);

	if (s0 == -1) {
		fprintf(nusmv_stdout, "is infinity\n");
		Prop_set_number_infinite(prop);
		Prop_set_status(prop, Prop_Number);
	} else {
		fprintf(nusmv_stdout, "is %d\n", s0);
		Prop_set_number(prop, s0);
		Prop_set_status(prop, Prop_Number);
	}
}

/**Function********************************************************************

 Synopsis           [Set of states satisfying <i>EX(g)</i>.]

 Description        [Computes the set of states satisfying <i>EX(g)</i>.]

 SideEffects        []

 SeeAlso            [eu ef eg]

 ******************************************************************************/
BddStates ex(BddFsm_ptr fsm, BddStates g) {
	bdd_ptr result;
	bdd_ptr tmp = bdd_dup(g);

	{
		/*
		 The explicit restriction to fair states is required (it affects
		 the result from a logical point of view.)
		 */
		bdd_ptr fair_states_bdd = BddFsm_get_fair_states(fsm);

		bdd_and_accumulate(dd_manager, &tmp, fair_states_bdd);
		bdd_free(dd_manager, fair_states_bdd);
	}

	if (opt_use_reachable_states(options)) {
		bdd_ptr reachable_states_bdd = BddFsm_get_reachable_states(fsm);
		bdd_and_accumulate(dd_manager, &tmp, reachable_states_bdd);
		bdd_free(dd_manager, reachable_states_bdd);
	}

	result = BddFsm_get_backward_image(fsm, tmp);
	bdd_free(dd_manager, tmp);

	if (opt_use_reachable_states(options)) {
		bdd_ptr reachable_states_bdd = BddFsm_get_reachable_states(fsm);
		bdd_and_accumulate(dd_manager, &result, reachable_states_bdd);
		bdd_free(dd_manager, reachable_states_bdd);
	}

	return (result);
}

/* Franco (02/08/2004): eax added
 /**Function********************************************************************

 Synopsis           [Set of states satisfying <i>EAX\[ act_expr ctl_expr \]</i>.]

 Description        [Computes the set of states satisfying <i>EAX\[
 act_expr ctl_expr \]</i>.]

 SideEffects        []

 SeeAlso            [ex]

 ******************************************************************************/
BddStates eax(BddFsm_ptr fsm, BddStates f, BddStates g) {
	bdd_ptr tmp_1, result;

	tmp_1 = bdd_and(dd_manager, f, g);
	/* sbusard 16/02/11 */
	result = ex_si_aware(fsm, tmp_1);
	result = BddFsm_states_inputs_to_states(fsm, result);

	bdd_free(dd_manager, tmp_1);

	return (result);
}

/* sbusard 06/12/13 */
/**Function********************************************************************

Synopsis           [Set of states satisfying <i>AAX\[ act_expr ctl_expr \]</i>.]

Description        [Computes the set of states satisfying <i>AAX\[
act_expr ctl_expr \]</i>.]

SideEffects        []

SeeAlso            [ex]

******************************************************************************/
BddStates aax(BddFsm_ptr fsm, BddStates f, BddStates g) {
	bdd_ptr one, eaxt, np, eaxnp, neaxnp, result;

	one = bdd_one(dd_manager);
	eaxt = eax(fsm, f, one);

	np = bdd_not(dd_manager, g);
	eaxnp = eax(fsm, f, np);
	neaxnp = bdd_not(dd_manager, eaxnp);

	result = bdd_and(dd_manager, eaxt, neaxnp);

	bdd_free(dd_manager, one);
	bdd_free(dd_manager, eaxt);
	bdd_free(dd_manager, np);
	bdd_free(dd_manager, eaxnp);
	bdd_free(dd_manager, neaxnp);

	return (result);
}

/* Franco (03/08/2004): eau added
 /**Function********************************************************************

 Synopsis           [Set of states satisfying <i>EAU\[ act_expr ctl_expr ctl_expr\]</i>.]

 Description        [Computes the set of states satisfying <i>EAU\[
 act_expr ctl_expr ctl_expr\]</i>.]

 SideEffects        []

 SeeAlso            [ex, eu]

 ******************************************************************************/
BddStates eau(BddFsm_ptr fsm, BddStates act, BddStates f, BddStates g) {

	bdd_ptr tmp_1, tmp_2, tmp_3, result;

	tmp_1 = bdd_and(dd_manager, act, f);
	tmp_2 = bdd_and(dd_manager, act, g);

	/* sbusard 16/02/11 */
	tmp_3 = ex_si_aware(fsm, eu_si(fsm, tmp_1, tmp_2));

	result = bdd_or(dd_manager, g, bdd_and(dd_manager, f, tmp_3));
	result = BddFsm_states_inputs_to_states(fsm, result);

	bdd_free(dd_manager, tmp_1);
	bdd_free(dd_manager, tmp_2);
	bdd_free(dd_manager, tmp_3);

	return (result);

}

/* Franco (05/08/2004): aau added
 /**Function********************************************************************

 Synopsis           [Set of states satisfying <i>AAU\[ act_expr ctl_expr ctl_expr\]</i>.]

 Description        [Computes the set of states satisfying <i>AAU\[
 act_expr ctl_expr ctl_expr\]</i>.]

 SideEffects        []

 SeeAlso            [ex, eu]

 ******************************************************************************/
BddStates aau(BddFsm_ptr fsm, BddStates act, BddStates f, BddStates g) {
	bdd_ptr tmp_1, tmp_2, tmp_3, tmp_4, result;

	/* sbusard 16/02/11 */
	/* Aa[f U g] = !(Ea[!g U (!f & !g)] | EaG !g) */
	tmp_1 = bdd_not(dd_manager, g);
	tmp_2 = bdd_and(dd_manager, bdd_not(dd_manager, f), tmp_1);
	tmp_3 = eau(fsm, act, tmp_1, tmp_2);
	tmp_4 = eag(fsm, act, tmp_1);
	result = bdd_not(dd_manager, bdd_or(dd_manager, tmp_3, tmp_4));
	result = BddFsm_states_inputs_to_states(fsm, result);

	bdd_free(dd_manager, tmp_1);
	bdd_free(dd_manager, tmp_2);
	bdd_free(dd_manager, tmp_3);
	bdd_free(dd_manager, tmp_4);

	return (result);

}

/* ----- sbusard 04/08/11 --------------------------------------------------- */
 /**Function********************************************************************

 Synopsis           [Set of states satisfying <i>EAW\[ act_expr ctl_expr ctl_expr\]</i>.]

 Description        [Computes the set of states satisfying <i>EAW\[
 act_expr ctl_expr ctl_expr\]</i>.]

 SideEffects        []

 SeeAlso            []

 ******************************************************************************/
BddStates eaw(BddFsm_ptr fsm, BddStates act, BddStates f, BddStates g) {

	/* EA (act) [f W g] is equivalent to EA (act) [f U g] OR EA (act) G f */

	bdd_ptr tmp_1, tmp_2, result;

	tmp_1 = eau(fsm, act, f, g);
	tmp_2 = eag(fsm, act, f);
	result = bdd_or(dd_manager, tmp_1, tmp_2);
	result = BddFsm_states_inputs_to_states(fsm, result);

	bdd_free(dd_manager, tmp_1);
	bdd_free(dd_manager, tmp_2);

	return (result);

}

 /**Function********************************************************************

 Synopsis           [Set of states satisfying <i>AAW\[ act_expr ctl_expr ctl_expr\]</i>.]

 Description        [Computes the set of states satisfying <i>AAW\[
 act_expr ctl_expr ctl_expr\]</i>.]

 SideEffects        []

 SeeAlso            []

 ******************************************************************************/
BddStates aaw(BddFsm_ptr fsm, BddStates act, BddStates f, BddStates g) {
	bdd_ptr tmp_1, tmp_2, tmp_3, result;

	/* AA (act) [f W g] = !(EA (act) [!g U (!f & !g)]) */
	tmp_1 = bdd_not(dd_manager, g);
	tmp_2 = bdd_and(dd_manager, bdd_not(dd_manager, f), tmp_1);
	tmp_3 = eau(fsm, act, tmp_1, tmp_2);
	result = bdd_not(dd_manager, tmp_3);
	result = BddFsm_states_inputs_to_states(fsm, result);

	bdd_free(dd_manager, tmp_1);
	bdd_free(dd_manager, tmp_2);
	bdd_free(dd_manager, tmp_3);

	return (result);

}
/* ----- sbusard ------------------------------------------------------------ */

/* Franco (05/08/2004): eaf added
 /**Function********************************************************************

 Synopsis           [Set of states satisfying <i>EAF\[ act_expr ctl_expr \]</i>.]

 Description        [Computes the set of states satisfying <i>EAF\[
 act_expr ctl_expr \]</i>.]

 SideEffects        []

 SeeAlso            [ex]

 ******************************************************************************/
BddStates eaf(BddFsm_ptr fsm, BddStates f, BddStates g) {

	/* sbusard 06/12/13 */
	bdd_ptr one, result;

	one = bdd_one(dd_manager);
	result = eau(fsm, f, one, g);

	bdd_free(dd_manager, one);

	return result;
}

/* Franco (05/08/2004): eag added
 /**Function********************************************************************

 Synopsis           [Set of states satisfying <i>EAG\[ act_expr ctl_expr \]</i>.]

 Description        [Computes the set of states satisfying <i>EAG\[
 act_expr ctl_expr \]</i>.]

 SideEffects        []

 SeeAlso            [ex]

 ******************************************************************************/
BddStates eag(BddFsm_ptr fsm, BddStates f, BddStates g) {
	/* sbusard 06/12/13 */
	/* EAG[ act, g ] = EAU[ act, g, g & !EAX[ act, true]] | !EG_SI[act, g] */

	bdd_ptr tmp_1, tmp_2, tmp_3, tmp_4;
	bdd_ptr eaxt, neaxt, eaup, paneaxt, one, result;

	tmp_1 = bdd_and(dd_manager, f, g);

	/* sbusard 16/02/11 */
	tmp_2 = ex_si_aware(fsm, eg_si(fsm, tmp_1));

	tmp_3 = bdd_and(dd_manager, g, tmp_2);
	tmp_4 = BddFsm_states_inputs_to_states(fsm, tmp_3);

	/* sbusard 06/12/13 */
	one = bdd_one(dd_manager);
	eaxt = eax(fsm, f, one);
	neaxt = bdd_not(dd_manager, eaxt);
	paneaxt = bdd_and(dd_manager, neaxt, g);
	eaup = eau(fsm, f, g, paneaxt);

	result = bdd_or(dd_manager, eaup, tmp_4);

	bdd_free(dd_manager, tmp_1);
	bdd_free(dd_manager, tmp_2);
	bdd_free(dd_manager, tmp_3);
	bdd_free(dd_manager, tmp_4);
	bdd_free(dd_manager, one);
	bdd_free(dd_manager, eaxt);
	bdd_free(dd_manager, neaxt);
	bdd_free(dd_manager, paneaxt);
	bdd_free(dd_manager, eaup);

	return (result);
}

/* Franco (06/08/2004): reachable added */
 /**Function********************************************************************

 Synopsis           [Set of reachable states via a fixed IVAR label
 on transitions]

 Description        [Computes the set of reachable states from the
 initial states when a fixed IVAR labels transitions.

 SideEffects        []

 SeeAlso            [bdd_fsm_compute_reachable_states]

 ******************************************************************************/
BddStates reachable(BddFsm_ptr self, BddStates act) {
	/* this procedure is taken from bdd_fsm_compute_reachable_states,
	 where temporal relation is restricted to relations labelled with
	 act. Notice: the result is not stored
	 */

	bdd_ptr reachable_states_bdd;
	bdd_ptr from_lower_bound; /* the frontier */
	bdd_ptr invars, result;

	int diameter = 0;

	reachable_states_bdd = BddFsm_get_init(self);
	invars = BddFsm_get_state_constraints(self);
	bdd_and_accumulate(dd_manager, &reachable_states_bdd, invars);
	bdd_free(dd_manager, invars);

	from_lower_bound = BddFsm_get_init(self);

	if (opt_verbose_level_gt(options, 1)) {
		fprintf(nusmv_stderr,
				"\ncomputing reachable state space for some IVAR\n");
	}

	while (bdd_isnot_zero(dd_manager, from_lower_bound)) {

		bdd_ptr from_upper_bound = bdd_dup(reachable_states_bdd);

		{
			/* Franco (06/08/2004: changed this computation */
			BddStates img = BddFsm_get_forward_image(self, bdd_and(dd_manager,
					BDD_STATES(from_lower_bound), act));

			bdd_or_accumulate(dd_manager, &reachable_states_bdd, img);
			bdd_free(dd_manager, (bdd_ptr) img);
		}

		/* updates from_lower_bound */
		{
			bdd_ptr not_from_upper_bound =
					bdd_not(dd_manager, from_upper_bound);

			bdd_free(dd_manager, from_lower_bound);
			from_lower_bound = bdd_and(dd_manager, reachable_states_bdd,
					not_from_upper_bound);

			bdd_free(dd_manager, not_from_upper_bound);
		}

		bdd_free(dd_manager, from_upper_bound);
		++diameter;

	} /* while loop */

	bdd_free(dd_manager, from_lower_bound);

	result = bdd_dup(reachable_states_bdd);

	bdd_free(dd_manager, reachable_states_bdd);

	if (opt_verbose_level_gt(options, 1)) {
		fprintf(nusmv_stderr, "done\n");
	}

	return result;

}

/**Function********************************************************************

 Synopsis           [Set of states satisfying <i>E\[ f U g \]</i>.]

 Description        [Computes the set of states satisfying <i>E\[ f U g \]</i>.]

 SideEffects        []

 SeeAlso            [ebu]

 ******************************************************************************/
BddStates eu(BddFsm_ptr fsm, BddStates f, BddStates g) {
	BddEnc_ptr enc = Enc_get_bdd_encoding();
	bdd_ptr new, oldY;
	bdd_ptr Y = bdd_dup(g);
	int n = 1;

	/* The following simplification may be useful for efficiency since g
	 may be unreachable (but they are not fundamental for correctness
	 similar simplifications are applied in ex). */

	{
		bdd_ptr fair_states_bdd = BddFsm_get_fair_states(fsm);

		bdd_and_accumulate(dd_manager, &Y, fair_states_bdd);
		bdd_free(dd_manager, fair_states_bdd);
	}

	if (opt_use_reachable_states(options)) {
		bdd_ptr reachable_states_bdd = BddFsm_get_reachable_states(fsm);
		bdd_and_accumulate(dd_manager, &Y, reachable_states_bdd);
		bdd_free(dd_manager, reachable_states_bdd);
	}

	if (opt_verbose_level_gt(options, 1)) {
		indent_node(nusmv_stderr,
				"eu: computing fixed point approximations for ",
				get_the_node(), " ...\n");
	}

	oldY = bdd_dup(Y);
	new = bdd_dup(Y);
	while (bdd_isnot_zero(dd_manager, new)) {
		bdd_ptr tmp_1, tmp_2;

		if (opt_verbose_level_gt(options, 1)) {
			double states = BddEnc_count_states_of_bdd(enc, Y);
			int size = bdd_size(dd_manager, Y);

			indent(nusmv_stderr);
			fprintf(nusmv_stderr, "size of Y%d = %g states, %d BDD nodes\n",
					n++, states, size);

		}
		bdd_free(dd_manager, oldY);
		oldY = bdd_dup(Y);

		tmp_1 = ex(fsm, new);

		tmp_2 = bdd_and(dd_manager, f, tmp_1);

		bdd_free(dd_manager, tmp_1);
		bdd_or_accumulate(dd_manager, &Y, tmp_2);

		bdd_free(dd_manager, tmp_2);
		tmp_1 = bdd_not(dd_manager, oldY);

		bdd_free(dd_manager, new);
		new = bdd_and(dd_manager, Y, tmp_1);

		bdd_free(dd_manager, tmp_1);
	}
	bdd_free(dd_manager, new);
	bdd_free(dd_manager, oldY);

	return (Y);
}

/**Function********************************************************************

 Synopsis           [Set of states satisfying <i>EF(g)</i>.]

 Description        [Computes the set of states satisfying <i>EG(g)</i>.]

 SideEffects        []

 SeeAlso            [eu ex]

 ******************************************************************************/
BddStates eg(BddFsm_ptr fsm, BddStates g) {
	bdd_ptr fair_transitions;
	bdd_ptr fair_transitions_g;
	bdd_ptr res_si;
	bdd_ptr res;

	fair_transitions = BddFsm_get_fair_states_inputs(fsm);
	fair_transitions_g = bdd_and(dd_manager, fair_transitions, g);

	res_si = eg_si(fsm, fair_transitions_g);

	res = BddFsm_states_inputs_to_states(fsm, res_si);

	bdd_free(dd_manager, res_si);
	bdd_free(dd_manager, fair_transitions_g);
	bdd_free(dd_manager, fair_transitions);

	return (res);
}

/**Function********************************************************************

 Synopsis           [Set of states satisfying <i>EF(g)</i>.]

 Description        [Computes the set of states satisfying <i>EF(g)</i>.]

 SideEffects        []

 SeeAlso            [eu ex]

 ******************************************************************************/
BddStates ef(BddFsm_ptr fsm, BddStates g) {
	bdd_ptr result, one;

	one = bdd_one(dd_manager);
	result = eu(fsm, one, g);
	bdd_free(dd_manager, one);

	return (result);
}

/**Function********************************************************************

 Synopsis           [Set of states satisfying <i>A\[f U g\]</i>.]

 Description        [Computes the set of states satisfying <i>A\[f U g\]</i>.]

 SideEffects        []

 SeeAlso            [ax af ex ef]

 ******************************************************************************/
BddStates au(BddFsm_ptr fsm, BddStates f, BddStates g) {
	bdd_ptr result, tmp_1, tmp_2, tmp_3, tmp_4;

	tmp_1 = bdd_not(dd_manager, f);
	tmp_2 = bdd_not(dd_manager, g);
	tmp_3 = eg(fsm, tmp_2);
	tmp_4 = bdd_and(dd_manager, tmp_1, tmp_2);
	bdd_free(dd_manager, tmp_1);
	tmp_1 = eu(fsm, tmp_2, tmp_4);
	bdd_free(dd_manager, tmp_2);
	tmp_2 = bdd_or(dd_manager, tmp_1, tmp_3);
	result = bdd_not(dd_manager, tmp_2);

	bdd_free(dd_manager, tmp_2);
	bdd_free(dd_manager, tmp_1);
	bdd_free(dd_manager, tmp_4);
	bdd_free(dd_manager, tmp_3);

	return (result);
}

/* ----- 01/08/11 sbusard --------------------------------------------------- */
/**Function********************************************************************

 Synopsis           [Set of states satisfying <i>A\[f W g\]</i>.]

 Description        [Computes the set of states satisfying <i>A\[f W g\]</i>.]

 SideEffects        []

 SeeAlso            [ax af ex ef]

 ******************************************************************************/
BddStates aw(BddFsm_ptr fsm, BddStates f, BddStates g) {

	// A[f W g] = NOT E[ NOT g U NOT f AND NOT g]

	bdd_ptr result, tmp_1, tmp_2, tmp_4;

	tmp_1 = bdd_not(dd_manager, f);
	tmp_2 = bdd_not(dd_manager, g);
	tmp_4 = bdd_and(dd_manager, tmp_1, tmp_2);
	bdd_free(dd_manager, tmp_1);
	tmp_1 = eu(fsm, tmp_2, tmp_4);
	result = bdd_not(dd_manager, tmp_1);

	bdd_free(dd_manager, tmp_2);
	bdd_free(dd_manager, tmp_1);
	bdd_free(dd_manager, tmp_4);

	return (result);
}

/**Function********************************************************************

 Synopsis           [Set of states satisfying <i>E\[f W g\]</i>.]

 Description        [Computes the set of states satisfying <i>E\[f W g\]</i>.]

 SideEffects        []

 SeeAlso            [ax af ex ef]

 ******************************************************************************/
BddStates ew(BddFsm_ptr fsm, BddStates f, BddStates g) {

	// E[f W g] = NOT A[ NOT g U NOT f AND NOT g]

	bdd_ptr result, tmp_1, tmp_2, tmp_4;

	tmp_1 = bdd_not(dd_manager, f);
	tmp_2 = bdd_not(dd_manager, g);
	tmp_4 = bdd_and(dd_manager, tmp_1, tmp_2);
	bdd_free(dd_manager, tmp_1);
	tmp_1 = au(fsm, tmp_2, tmp_4);
	result = bdd_not(dd_manager, tmp_1);

	bdd_free(dd_manager, tmp_2);
	bdd_free(dd_manager, tmp_1);
	bdd_free(dd_manager, tmp_4);

	return (result);
}
/* ----- sbusard ------------------------------------------------------------ */

/**Function********************************************************************

 Synopsis           [Set of states satisfying <i>EG(g)</i>.]

 Description        [Computes the set of states satisfying <i>EG(g)</i>.]

 SideEffects        []

 SeeAlso            [eu ex ef]

 ******************************************************************************/
BddStatesInputs ex_si(BddFsm_ptr fsm, BddStatesInputs si) {
	BddEnc_ptr enc = Enc_get_bdd_encoding();
	BddStates states;
	BddStatesInputs si_preimage;

	/* Eliminate input variables */
	states = BddFsm_states_inputs_to_states(fsm, si);

	/* Perform weak preimage */
	si_preimage = BddFsm_get_weak_backward_image(fsm, states);

	if (opt_use_reachable_states(options)) {
		bdd_ptr reachable_states_bdd = BddFsm_get_reachable_states(fsm);

		bdd_and_accumulate(dd_manager, &si_preimage, reachable_states_bdd);
		bdd_free(dd_manager, reachable_states_bdd);
	}

	bdd_free(dd_manager, states);

	return si_preimage;
}

/* sbusard 16/02/11 */
/**Function********************************************************************

 Synopsis           [Set of states satisfying <i>EX(g)</i>.]

 Description        [Computes the set of states satisfying <i>EX(g)</i>,
 keeping the input variables.]

 SideEffects        []

 SeeAlso            [eu ex ef]

 ******************************************************************************/
BddStates ex_si_aware(BddFsm_ptr fsm, BddStates g) {
	bdd_ptr result;
	BddStates states;
	BddInputs inputs;

	bdd_ptr tmp = bdd_dup(g);

	{
		/*
		 The explicit restriction to fair states is required (it affects
		 the result from a logical point of view.)
		 */
		bdd_ptr fair_states_bdd = BddFsm_get_fair_states(fsm);

		bdd_and_accumulate(dd_manager, &tmp, fair_states_bdd);
		bdd_free(dd_manager, fair_states_bdd);
	}

	if (opt_use_reachable_states(options)) {
		bdd_ptr reachable_states_bdd = BddFsm_get_reachable_states(fsm);
		bdd_and_accumulate(dd_manager, &tmp, reachable_states_bdd);
		bdd_free(dd_manager, reachable_states_bdd);
	}

	//Separate input variables
	states = BddFsm_states_inputs_to_states(fsm, tmp);
	inputs = BddFsm_states_inputs_to_inputs(fsm, tmp);

	result = BddFsm_get_constrained_backward_image(fsm, states, inputs);

	bdd_free(dd_manager, tmp);
	bdd_free(dd_manager, states);
	bdd_free(dd_manager, inputs);

	if (opt_use_reachable_states(options)) {
		bdd_ptr reachable_states_bdd = BddFsm_get_reachable_states(fsm);
		bdd_and_accumulate(dd_manager, &result, reachable_states_bdd);
		bdd_free(dd_manager, reachable_states_bdd);
	}

	return (result);
}

/**Function********************************************************************

 Synopsis [Computes the set of state-input pairs that satisfy
 E(f U g), with f and g sets of state-input pairs.]

 Description  []

 SeeAlso      []

 SideEffects  []

 ******************************************************************************/
BddStatesInputs eu_si(BddFsm_ptr fsm, bdd_ptr f, bdd_ptr g) {
	int i = 0;
	BddEnc_ptr enc = Enc_get_bdd_encoding();
	bdd_ptr oldY;
	bdd_ptr resY;
	bdd_ptr newY;
	bdd_ptr rg = bdd_dup(g);

	if (opt_use_reachable_states(options)) {
		bdd_ptr reachable_states_bdd = BddFsm_get_reachable_states(fsm);

		bdd_and_accumulate(dd_manager, &rg, reachable_states_bdd);
		bdd_free(dd_manager, reachable_states_bdd);
	}

	oldY = bdd_dup(rg);
	resY = bdd_dup(rg);
	newY = bdd_dup(rg);

	bdd_free(dd_manager, rg);

	while (bdd_isnot_zero(dd_manager, newY)) {
		bdd_ptr tmp_1, tmp_2;

		if (opt_verbose_level_gt(options, 1)) {
			fprintf(nusmv_stderr,
					"    size of Y%d = %g <states>x<inputs>, %d BDD nodes\n",
					i++, BddEnc_count_states_inputs_of_bdd(enc, resY),
					bdd_size(dd_manager, resY));
		}

		bdd_free(dd_manager, oldY);

		oldY = bdd_dup(resY);
		tmp_1 = ex_si(fsm, newY);
		tmp_2 = bdd_and(dd_manager, tmp_1, f);
		bdd_free(dd_manager, tmp_1);

		bdd_or_accumulate(dd_manager, &resY, tmp_2);
		bdd_free(dd_manager, tmp_2);

		tmp_1 = bdd_not(dd_manager, oldY);
		bdd_free(dd_manager, newY);

		newY = bdd_and(dd_manager, resY, tmp_1);
		bdd_free(dd_manager, tmp_1);
	}

	bdd_free(dd_manager, newY);
	bdd_free(dd_manager, oldY);

	return BDD_STATES_INPUTS( resY );
}

/**Function********************************************************************

 Synopsis           [Set of states-inputs satisfying <i>EG(g)</i>.]

 Description        []

 SideEffects        []

 SeeAlso            [eu ex]

 ******************************************************************************/
bdd_ptr eg_si(BddFsm_ptr fsm, bdd_ptr g_si) {
	BddEnc_ptr enc = Enc_get_bdd_encoding();
	bdd_ptr applicable_states_inputs;
	bdd_ptr fair_states_inputs;

	applicable_states_inputs = BddFsm_get_states_inputs_constraints(fsm);
	bdd_and_accumulate(dd_manager, &applicable_states_inputs, g_si);

	if (opt_use_reachable_states(options)) {
		bdd_ptr reachable_states_bdd = BddFsm_get_reachable_states(fsm);

		bdd_and_accumulate(dd_manager, &applicable_states_inputs,
				reachable_states_bdd);
		bdd_free(dd_manager, reachable_states_bdd);
	}

	fair_states_inputs = Mc_get_fair_si_subset(fsm, applicable_states_inputs);

	bdd_free(dd_manager, applicable_states_inputs);

	return fair_states_inputs;
}

/**Function********************************************************************

 Synopsis           [Set of states satisfying <i>E\[f U^{inf..sup} g\]</i>.]

 Description        [Computes the set of states satisfying
 <i>E\[f U^{inf..sup} g\]</i></i>.]

 SideEffects        []

 SeeAlso            [eu]

 ******************************************************************************/
BddStates ebu(BddFsm_ptr fsm, BddStates f, BddStates g, int inf, int sup) {
	BddEnc_ptr enc = Enc_get_bdd_encoding();
	int i;
	bdd_ptr Y, oldY, tmp_1, tmp_2;
	int n = 1;

	if (inf > sup || inf < 0)
		return (bdd_zero(dd_manager));

	Y = bdd_dup(g);

	{
		bdd_ptr fair_states_bdd = BddFsm_get_fair_states(fsm);

		bdd_and_accumulate(dd_manager, &Y, fair_states_bdd);
		bdd_free(dd_manager, fair_states_bdd);
	}

	if (opt_use_reachable_states(options)) {
		bdd_ptr reachable_states_bdd = BddFsm_get_reachable_states(fsm);

		bdd_and_accumulate(dd_manager, &Y, reachable_states_bdd);
		bdd_free(dd_manager, reachable_states_bdd);
	}

	if (opt_verbose_level_gt(options, 1))
		indent_node(nusmv_stderr,
				"ebu: computing fixed point approximations for ",
				get_the_node(), " ...\n");

	/* compute Y = g | (f & ex(Y)) for states within the bound */
	for (i = sup; i > inf; i--) {
		/* There are more states within the bounds */
		if (opt_verbose_level_gt(options, 1)) {
			indent(nusmv_stderr);
			fprintf(nusmv_stderr, "size of Y%d = %g states, %d BDD nodes\n",
					n++, BddEnc_count_states_of_bdd(enc, Y), bdd_size(
							dd_manager, Y));
		}
		oldY = Y;
		tmp_1 = ex(fsm, Y);
		tmp_2 = bdd_and(dd_manager, f, tmp_1);
		bdd_or_accumulate(dd_manager, &Y, tmp_2);
		bdd_free(dd_manager, tmp_1);
		bdd_free(dd_manager, tmp_2);

		if (Y == oldY) {
			/* fixpoint found. collect garbage, and goto next phase */
			break;
		}
	}

	/* compute Y = f & ex(Y) for states before the bound */
	for (i = inf; i > 0; i--) {
		if (opt_verbose_level_gt(options, 1)) {
			indent(nusmv_stderr);
			fprintf(nusmv_stderr, "size of Y%d = %g states, %d BDD nodes\n",
					n++, BddEnc_count_states_of_bdd(enc, Y), bdd_size(
							dd_manager, Y));
		}
		oldY = bdd_dup(Y);
		tmp_1 = ex(fsm, Y);
		bdd_free(dd_manager, Y);
		Y = bdd_and(dd_manager, f, tmp_1);
		bdd_free(dd_manager, tmp_1);
		bdd_free(dd_manager, oldY);
		if (Y == oldY) {
			/* fixpoint found. collect garbage, and finish */
			break;
		}
	}
	return (Y);
}

/**Function********************************************************************

 Synopsis           [Set of states satisfying <i>EF^{inf..sup}(g)</i>.]

 Description        [Computes the set of states satisfying
 <i>EF^{inf..sup}(g)</i>.]

 SideEffects        []

 SeeAlso            [ef]

 ******************************************************************************/
BddStates ebf(BddFsm_ptr fsm, BddStates g, int inf, int sup) {
	bdd_ptr one, result;

	one = bdd_one(dd_manager);
	result = ebu(fsm, one, g, inf, sup);
	bdd_free(dd_manager, one);
	return (result);
}

/**Function********************************************************************

 Synopsis           [Set of states satisfying <i>EG^{inf..sup}(g)</i>.]

 Description        [Computes the set of states satisfying
 <i>EG^{inf..sup}(g)</i>.]

 SideEffects        []

 SeeAlso            [eg]

 ******************************************************************************/
BddStates ebg(BddFsm_ptr fsm, BddStates g, int inf, int sup) {
	BddEnc_ptr enc = Enc_get_bdd_encoding();
	int i;
	bdd_ptr Y, oldY, tmp_1;
	int n = 1;

	if (inf > sup || inf < 0)
		return bdd_one(dd_manager);

	Y = bdd_dup(g);

	/* Limitation to fair states should be imposed. */
	{
		bdd_ptr fair_states_bdd = BddFsm_get_fair_states(fsm);

		bdd_and_accumulate(dd_manager, &Y, fair_states_bdd);
		bdd_free(dd_manager, fair_states_bdd);
	}

	if (opt_use_reachable_states(options)) {
		bdd_ptr reachable_states_bdd = BddFsm_get_reachable_states(fsm);

		bdd_and_accumulate(dd_manager, &Y, reachable_states_bdd);
		bdd_free(dd_manager, reachable_states_bdd);
	}

	if (opt_verbose_level_gt(options, 1)) {
		indent_node(nusmv_stderr,
				"ebg: computing fixed point approximations for ",
				get_the_node(), " ...\n");
	}

	/* compute Y = g & ex(Y) for states within the bound */
	for (i = sup; i > inf; i--) {
		if (opt_verbose_level_gt(options, 1)) {
			indent(nusmv_stderr);
			fprintf(nusmv_stderr, "size of Y%d = %g states, %d BDD nodes\n",
					n++, BddEnc_count_states_of_bdd(enc, Y), bdd_size(
							dd_manager, Y));
		}
		oldY = bdd_dup(Y);
		tmp_1 = ex(fsm, Y);
		bdd_and_accumulate(dd_manager, &Y, tmp_1);
		bdd_free(dd_manager, tmp_1);
		if (Y == oldY) {
			bdd_free(dd_manager, oldY);
			/* fixpoint found. goto next phase */
			break;
		}
		bdd_free(dd_manager, oldY);
	}
	/* compute Y = ex(Y) for states before the bound */
	for (i = inf; i > 0; i--) {
		if (opt_verbose_level_gt(options, 1)) {
			indent(nusmv_stderr);
			fprintf(nusmv_stderr, "size of Y%d = %g states, %d BDD nodes\n",
					n++, BddEnc_count_states_of_bdd(enc, Y), bdd_size(
							dd_manager, Y));
		}
		oldY = Y;
		tmp_1 = ex(fsm, Y);
		bdd_free(dd_manager, Y);
		Y = tmp_1;
		if (Y == oldY) {
			break; /* fixpoint found. */
		}
	}
	return Y;
}

/**Function********************************************************************

 Synopsis           [Set of states satisfying <i>A\[f U^{inf..sup} g\]</i>.]

 Description        [Computes the set of states satisfying
 <i>A\[f U^{inf..sup} g\]</i>.]

 SideEffects        []

 SeeAlso            [au]

 ******************************************************************************/
BddStates abu(BddFsm_ptr fsm, BddStates f, BddStates g, int inf, int sup) {
	BddEnc_ptr enc = Enc_get_bdd_encoding();
	int i;
	bdd_ptr Y, oldY, tmp_1, tmp_2;
	int n = 1;

	if (inf > sup || inf < 0)
		return (bdd_zero(dd_manager));

	Y = bdd_dup(g);

	{
		bdd_ptr fair_states_bdd = BddFsm_get_fair_states(fsm);

		bdd_and_accumulate(dd_manager, &Y, fair_states_bdd);
		bdd_free(dd_manager, fair_states_bdd);
	}

	if (opt_use_reachable_states(options)) {
		bdd_ptr reachable_states_bdd = BddFsm_get_reachable_states(fsm);

		bdd_and_accumulate(dd_manager, &Y, reachable_states_bdd);
		bdd_free(dd_manager, reachable_states_bdd);
	}

	if (opt_verbose_level_gt(options, 1))
		indent_node(nusmv_stderr,
				"abu: computing fixed point approximations for ",
				get_the_node(), " ...\n");
	/* compute Y = g | (f & ax(Y)) for states within the bound */
	for (i = sup; i > inf; i--) {
		if (opt_verbose_level_gt(options, 1)) {
			indent(nusmv_stderr);
			fprintf(nusmv_stderr, "size of Y%d = %g states, %d BDD nodes\n",
					n++, BddEnc_count_states_of_bdd(enc, Y), bdd_size(
							dd_manager, Y));
		}
		oldY = Y;
		tmp_1 = bdd_not(dd_manager, Y);
		tmp_2 = ex(fsm, tmp_1);
		bdd_free(dd_manager, tmp_1);
		tmp_1 = bdd_not(dd_manager, tmp_2);
		bdd_free(dd_manager, tmp_2);
		tmp_2 = bdd_and(dd_manager, f, tmp_1);
		bdd_or_accumulate(dd_manager, &Y, tmp_2);
		bdd_free(dd_manager, tmp_1);
		bdd_free(dd_manager, tmp_2);
		if (Y == oldY) {
			break; /* fixpoint found. goto next phase */
		}
	}
	/* compute Y = f & ax(Y) for states before the bound */
	for (i = inf; i > 0; i--) {
		if (opt_verbose_level_gt(options, 1)) {
			indent(nusmv_stderr);
			fprintf(nusmv_stderr, "size of Y%d = %g states, %d BDD nodes\n",
					n++, BddEnc_count_states_of_bdd(enc, Y), bdd_size(
							dd_manager, Y));
		}
		oldY = bdd_dup(Y);
		tmp_1 = bdd_not(dd_manager, Y);
		tmp_2 = ex(fsm, tmp_1);
		bdd_free(dd_manager, tmp_1);
		tmp_1 = bdd_not(dd_manager, tmp_2);
		bdd_free(dd_manager, tmp_2);
		bdd_free(dd_manager, Y);
		Y = bdd_and(dd_manager, f, tmp_1);
		bdd_free(dd_manager, oldY);
		bdd_free(dd_manager, tmp_1);

		if (Y == oldY) {
			break; /* fixpoint found. finish */
		}
	}
	return (Y);
}

/**Function********************************************************************

 Synopsis           [Computes the minimum length of the shortest path
 from <i>f</i> to <i>g</i>.]

 Description        [This function computes the minimum length of the
 shortest path from <i>f</i> to <i>g</i>.<br>
 Starts from <i>f</i> and proceeds forward until finds a state in <i>g</i>.
 Notice that this function works correctly only if <code>-f</code>
 option is used.]

 SideEffects        []

 SeeAlso            [maxu]

 ******************************************************************************/
int minu(BddFsm_ptr fsm, bdd_ptr arg_f, bdd_ptr arg_g) {
	BddEnc_ptr enc = Enc_get_bdd_encoding();
	int i;
	int n = 1;
	bdd_ptr R, Rp, tmp_1;
	bdd_ptr f = bdd_dup(arg_f);
	bdd_ptr g = bdd_dup(arg_g);
	bdd_ptr invar_bdd = BddFsm_get_state_constraints(fsm);
	bdd_ptr fair_states_bdd = BddFsm_get_fair_states(fsm);
	bdd_ptr reachable_states_bdd = BddFsm_get_reachable_states(fsm);

	R = (bdd_ptr) NULL;

	/* We restrict f and g to the seat of fair states */
	bdd_and_accumulate(dd_manager, &g, fair_states_bdd);
	bdd_and_accumulate(dd_manager, &f, fair_states_bdd);

	/* We restrict to reachable states */
	bdd_and_accumulate(dd_manager, &f, reachable_states_bdd);
	bdd_and_accumulate(dd_manager, &g, reachable_states_bdd);

	bdd_free(dd_manager, reachable_states_bdd);

	if (opt_verbose_level_gt(options, 1))
		indent_node(nusmv_stderr,
				"minu: computing fixed point approximations for ",
				get_the_node(), " ...\n");
	i = 0;

	Rp = bdd_and(dd_manager, f, invar_bdd); /* starts searching from f */

	do {
		if (opt_verbose_level_gt(options, 1)) {
			indent(nusmv_stderr);
			fprintf(nusmv_stderr, "size of Rp%d = %g states, %d BDD nodes\n",
					n++, BddEnc_count_states_of_bdd(enc, Rp), bdd_size(
							dd_manager, Rp));
		}

		tmp_1 = bdd_and(dd_manager, Rp, g);

		if (bdd_isnot_zero(dd_manager, tmp_1)) {
			/* If current frontier intersects g return minimum */
			bdd_free(dd_manager, tmp_1);
			bdd_free(dd_manager, f);
			bdd_free(dd_manager, g);
			bdd_free(dd_manager, Rp);
			bdd_free(dd_manager, invar_bdd);
			bdd_free(dd_manager, fair_states_bdd);
			if (R != (bdd_ptr) NULL)
				bdd_free(dd_manager, R);

			return (i);
		}

		bdd_free(dd_manager, tmp_1);

		if (R != (bdd_ptr) NULL)
			bdd_free(dd_manager, R);

		R = Rp;

		/* go forward */
		tmp_1 = BddFsm_get_forward_image(fsm, R);

		/* We restrict the image to the set of fair states */
		bdd_and_accumulate(dd_manager, &tmp_1, fair_states_bdd);

		Rp = bdd_or(dd_manager, R, tmp_1);

		bdd_free(dd_manager, tmp_1);

		i++;

	} while (Rp != R);
	/* could not find g anywhere. A fixpoint has been found. g will not be
	 ever found, so return infinity. */
	bdd_free(dd_manager, f);
	bdd_free(dd_manager, g);
	bdd_free(dd_manager, Rp);
	bdd_free(dd_manager, R);
	bdd_free(dd_manager, invar_bdd);
	bdd_free(dd_manager, fair_states_bdd);

	return (-1);
}

/**Function********************************************************************

 Synopsis           [This function computes the maximum length of the
 shortest path from <i>f</i> to <i>g</i>.]

 Description        [This function computes the maximum length of the
 shortest path from <i>f</i> to <i>g</i>. It starts from !g and
 proceeds backward until no states in <i>f</i> can be found. In other
 words, it looks for the maximum length of <i>f->AG!g</i>.
 Notice that this function works correctly only if <code>-f</code>
 option is used.]

 SideEffects        []

 SeeAlso            [minu]

 ******************************************************************************/
int maxu(BddFsm_ptr fsm, bdd_ptr f, bdd_ptr g) {
	BddEnc_ptr enc = Enc_get_bdd_encoding();
	int i;
	int n = 1;
	bdd_ptr R, Rp;
	bdd_ptr notg, tmp_1;
	bdd_ptr invar_bdd, fair_states_bdd, reachable_states_bdd;

	invar_bdd = BddFsm_get_state_constraints(fsm);
	fair_states_bdd = BddFsm_get_fair_states(fsm);
	reachable_states_bdd = BddFsm_get_reachable_states(fsm);

	if (opt_verbose_level_gt(options, 1))
		indent_node(nusmv_stderr,
				"maxu: computing fixed point approximations for ",
				get_the_node(), " ...\n");

	tmp_1 = bdd_not(dd_manager, g);
	notg = bdd_and(dd_manager, tmp_1, invar_bdd);

	/* We restrict to fair states */
	bdd_and_accumulate(dd_manager, &notg, fair_states_bdd);

	bdd_free(dd_manager, tmp_1);
	bdd_free(dd_manager, invar_bdd);

	i = 0;
	R = bdd_one(dd_manager);
	Rp = bdd_dup(notg); /* starts from !g */

	/* We restrict to reachable states */
	{
		bdd_ptr reachable_states_bdd = BddFsm_get_reachable_states(fsm);
		bdd_and_accumulate(dd_manager, &Rp, reachable_states_bdd);
		bdd_free(dd_manager, reachable_states_bdd);
	}

	/* We restrict to fair states */
	bdd_and_accumulate(dd_manager, &Rp, fair_states_bdd);

	do {
		if (opt_verbose_level_gt(options, 1)) {
			indent(nusmv_stderr);
			fprintf(nusmv_stderr, "size of Rp%d = %g states, %d BDD nodes\n",
					n++, BddEnc_count_states_of_bdd(enc, Rp), bdd_size(
							dd_manager, Rp));
		}

		tmp_1 = bdd_and(dd_manager, Rp, f);

		if (bdd_is_zero(dd_manager, tmp_1)) {
			/* !g does not intersect f anymore. The maximum length of a path
			 completely in !g is i. This is the maximum. */
			bdd_free(dd_manager, tmp_1);
			bdd_free(dd_manager, R);
			bdd_free(dd_manager, Rp);
			bdd_free(dd_manager, notg);
			bdd_free(dd_manager, fair_states_bdd);
			bdd_free(dd_manager, reachable_states_bdd);

			return (i);
		}

		bdd_free(dd_manager, tmp_1);
		bdd_free(dd_manager, R);

		R = Rp;

		tmp_1 = BddFsm_get_backward_image(fsm, R);

		/* We restrict to reachable states */
		bdd_and_accumulate(dd_manager, &tmp_1, reachable_states_bdd);

		/* We restrict to fir states */
		bdd_and_accumulate(dd_manager, &tmp_1, fair_states_bdd);

		Rp = bdd_and(dd_manager, tmp_1, notg);

		bdd_free(dd_manager, tmp_1);

		i++;

	} while (R != Rp);

	/* a fixpoint has been found in which !g & f holds, so return infinity */
	bdd_free(dd_manager, R);
	bdd_free(dd_manager, Rp);
	bdd_free(dd_manager, notg);
	bdd_free(dd_manager, fair_states_bdd);
	bdd_free(dd_manager, reachable_states_bdd);

	return (-1);
}

/**Function********************************************************************

 Synopsis           [Prints out a CTL specification]

 Description        [Prints out a CTL specification]

 SideEffects        []

 SeeAlso            []

 ******************************************************************************/
void print_spec(FILE *file, node_ptr n) {
	node_ptr context = Nil;

	if (node_get_type(n) == CONTEXT) {
		context = car(n);
		n = cdr(n);
	}
	indent_node(file, "specification ", n, " ");
	if (context) {
		(void) fprintf(file, "(in module ");
		(void) print_node(file, context);
		(void) fprintf(file, ") ");
	}
}

/**Function********************************************************************

 Synopsis           [Prints out a COMPUTE specification]

 Description        [Prints out a COMPUTE specification]

 SideEffects        []

 SeeAlso            []

 ******************************************************************************/
void print_compute(FILE *file, node_ptr n) {
	node_ptr context = Nil;

	if (node_get_type(n) == CONTEXT) {
		context = car(n);
		n = cdr(n);
	}
	indent_node(file, "the result of ", n, " ");
	if (context) {
		fprintf(file, "(in module ");
		print_node(file, context);
		fprintf(file, ") ");
	}
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

 Synopsis [ ]

 Description [Perform one iteration over the list of fairness
 conditions (order is statically determined). Compute states that are
 backward reachable from each of the fairness conditions.

 MAP( ApplicableStatesInputs ) over Fairness constraints

 (Q /\ ex_si ( Z /\ AND_i eu_si(Z, (Z/\ StatesInputFC_i))))

 ]

 SeeAlso      []

 SideEffects  []

 ******************************************************************************/
static bdd_ptr Mc_fair_si_iteration(BddFsm_ptr fsm, BddStatesInputs states,
		BddStatesInputs subspace) {
	bdd_ptr res;
	FairnessListIterator_ptr iter;
	bdd_ptr partial_result;

	res = bdd_one(dd_manager);
	partial_result = bdd_dup(states);

	iter = FairnessList_begin(FAIRNESS_LIST( BddFsm_get_justice(fsm) ));
	while (!FairnessListIterator_is_end(iter)) {
		bdd_ptr fc_si;
		bdd_ptr constrained_fc_si;
		bdd_ptr temp;

		/* Extract next fairness constraint */
		fc_si = JusticeList_get_p(BddFsm_get_justice(fsm), iter);

		/* Constrain it to current set */
		constrained_fc_si = bdd_and(dd_manager, states, fc_si);

		/* Collect states-input that can reach constrained_fc_si without leaving subspace */
		temp = eu_si(fsm, subspace, constrained_fc_si);

		bdd_free(dd_manager, constrained_fc_si);
		bdd_free(dd_manager, fc_si);

		bdd_and_accumulate(dd_manager, &partial_result, temp);
		bdd_free(dd_manager, temp);

		iter = FairnessListIterator_next(iter);
	}

	/* Compute preimage */
	res = ex_si(fsm, partial_result);
	bdd_free(dd_manager, partial_result);

	return res;
}

/**Function********************************************************************

 Synopsis     []

 Description [Returns the set of state-input pairs in si that are
 fair, i.e. beginning of a fair path.]

 SeeAlso      []

 SideEffects  []

 ******************************************************************************/
static BddStatesInputs Mc_get_fair_si_subset(BddFsm_ptr fsm, BddStatesInputs si) {
	int i = 0;
	BddStatesInputs res;
	BddStatesInputs old;
	BddEnc_ptr enc = Enc_get_bdd_encoding();

	BDD_FSM_CHECK_INSTANCE(fsm);

	res = BDD_STATES_INPUTS(bdd_one(dd_manager));
	old = BDD_STATES_INPUTS(bdd_zero(dd_manager));

	/* GFP computation */
	while (res != old) {
		BddStatesInputs new;

		if (opt_verbose_level_gt(options, 1)) {
			fprintf(nusmv_stderr,
					"  size of res%d = %g <states>x<input>, %d BDD nodes\n",
					i++, BddEnc_count_states_inputs_of_bdd(enc, res), bdd_size(
							dd_manager, res));
		}

		bdd_free(dd_manager, old);
		old = bdd_dup(res);

		/* One iteration over fairness conditions */
		new = Mc_fair_si_iteration(fsm, res, si);

		bdd_and_accumulate(dd_manager, &res, (bdd_ptr) new);
		bdd_and_accumulate(dd_manager, &res, (bdd_ptr) si);

		bdd_free(dd_manager, (bdd_ptr) new);
	}
	bdd_free(dd_manager, old);

	return res;
}

