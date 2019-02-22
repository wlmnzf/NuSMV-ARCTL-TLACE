divert(-1)

-- M4 header file for CTLK in SMV
-- Ch. Pecheur, RIACS / NASA Ames, Jun 2004
--       
-- Version 2: support disabling of CTLK features
--
-- Version 3: 
-- - Generalize ACTL operators
-- - support bi-directional temporal transitions
-- - compute reachable states
--
-- Version 4:
-- - convert from CPP to M4 (MacOSX CPP causes trouble)
-- - full re-engineering
--
-- Version 5:
-- - swapping terms to improve trace generation (cf note 2)
--
-- version 6:
-- - fix error in ACTL operators
-- - add EU_AB and AU_AB
--
-- version 7:
-- - fix error in AGENT() and subsidiaries
--
-- note 1: as of 2.2.1, NuSMV refuses input variables in CTL formulas 
-- (this makes sense, as branching quantification would otherwise 
-- be ill-defined).  This forces to use post-transition conditions
-- to select between different transition types.
--
-- note 2: as of 2.2.1, NuSMV has trouble generating traces for complex 
-- formulas.  In formulas of the form P & Q, it will wrongly try to
-- generate a trace for P when in fact Q is false.

----------------------------------------------------------------

-- ACTL operators
--
-- The encoding is based on tracking actions in the target states that
-- they reach.  For any action 'a', or more generally action set
-- 'as', we use a propositional formula 'p_a' (or 'p_as') such that
--
-- for all s --a'--> s' .
--   s' sat p_a  iff  a'=a
--   s' sat p_as  iff  a' in as
--
-- NB: this implies that a single state in the LTS becomes several
-- states separated by the action that reach them in the FSM.

-- AX_A(a,p) = AX_a p  (etc.)
-- EU_A(a,p,q) = A(p a_U q)  (etc.)
-- EU_A(a,b,p,q) = A(p a_U_b q)  (etc.)

define(`AX_A',`(AX (($1) & ($2)))')
define(`AU_AB',`((AX A[($1) & ($3) U ($2) & ($4)]) & ($3))')
define(`AU_A',`(((AX A[($1) & ($2) U ($1) & ($3)]) & ($2)) | ($3))')
define(`AG_A',`((AX !E[($1) U ($1) & !($2)]) & ($2))')
define(`AF_A',`((AX A[($1) U ($1) & ($2)]) | ($2))')

define(`EX_A',`(EX (($1) & ($2)))')
define(`EU_AB',`((EX E[($1) & ($3) U ($2) & ($4)]) & ($3))')
define(`EU_A',`(((EX E[($1) & ($2) U ($1) & ($3)]) & ($2)) | ($3))')
define(`EG_A',`((EX !A[($1) U ($1) & !($2)]) & ($2))')
define(`EF_A',`((EX E[($1) U ($1) & ($2)]) | ($2))')

----------------------------------------------------------------

-- HML operators

define(`Box',`(AX (($1) -> ($2)))')
define(`Dia',`(EX (($1) & ($2)))')

----------------------------------------------------------------

-- Action-restricted CTL operators
--
-- These differ from ACTL in that they match the corresponding
-- CTL operators, but restricted to transitions matching the action
-- formula.
--
-- Note: these operators do not check for totality of the (restricted) 
-- execution trees.  For example, AAX(A,F) is trivially true in states 
-- where there is no A action.  Similarly, AAF(A,F) can succeed by 
-- reaching states with no A successor.

define(`AAX',`(AX (($1) -> ($2)))')
define(`AAU',`(((AX A[($2) U ($3) | !($1)]) & ($2)) | ($3))')
define(`AAG',`((AX !E[($1) U ($1) & !($2)]) & ($2))')
define(`AAF',`((AX AF (($2) | !($1))) | ($2))')

define(`AEX',`(EX (($1) & ($2)))')
define(`AEU',`(((EX E[($1) & ($2) U ($1) & ($3)]) & ($2)) | ($3))')
define(`AEG',`((EX EG (($1) & ($2))) & ($2))')
define(`AEF',`((EX E[($1) U ($1) & ($2)]) | ($2))')

----------------------------------------------------------------

--  Bi-directional transitions
--
--  Transitions can be made bi-directional through the use of macros
--  NEXT() and PREV().  NEXT() is used in place of next() and PREV() is
--  used correspondingly for "current state" variables.  This requires
--  the use of "TRANS" (not "ASSIGN").
--
--  This works only in a context where a boolean variable PAST is
--  defined.  PAST is true after backwards transitions.

define(`NEXT', `(case next(PAST) : ($@) ; 1 : next($@) ; esac)')
define(`PREV', `(case next(PAST) : next($@) ; 1 : ($@) ; esac)')

-- Declare all temporal, reversible transitions as TTRANS(<trans>)

define(`TTRANS',`TRANS next(RUN) -> ($@)')

-- Declare all initial conditions as TINIT(<cond>)

define(`TINIT',`dnl
INIT $@
INVAR INITIAL -> ($@)')

-- Add "VAR_ENV" in declarations of main module, and pass "ENV"
-- as a parameter to all the module hierarchy:
--
-- MODULE main
--   VAR_ENV
--   ...
--   VAR sub :submodule(arg,ENV)
--
-- MODULE submodule(var,ENV)
--   ...

define(`VAR_ENV',`dnl
VAR 
  RUN : boolean; -- label for temporal transitions
  PAST : boolean; -- label for reverse temporal
  INITIAL : boolean; -- initial state flag
INIT INITIAL')

define(`RUNFUTURE',`(RUN & !PAST)')
define(`RUNPAST',`(RUN & PAST)')

define(`ENV',`RUN,PAST,INITIAL')

-- A state is reachable if it can past-reach an initial state

define(`REACHABLE',`EF_A(RUNPAST,INITIAL)')

----------------------------------------------------------------

-- Accessibility relations
--
-- An agent is defined as a set of variables using:
--
--   AGENT(<name>,<var>,<var>,...)
--
-- This creates a boolean variable <name> suitable for ACTL operators as
-- defined here; i.e. that variable is true after the accessibility
-- transitions from the agent.

define(`AGENT',`dnl
VAR $1 : boolean;')

define(`AGENT__TRANSITION',`TRANS next($1) -> ($2)=next($2)')

----------------------------------------------------------------

-- Temporal CTL operators
--
-- The action-restricted operators with RUNFUTURE as the action, i.e.
--   T<op>(F) = A<op>(RUNFUTURE,F)

define(`TAX',`AAX(RUNFUTURE,$1)')
define(`TAF',`AAF(RUNFUTURE,$1)')
define(`TAG',`AAG(RUNFUTURE,$1)')
define(`TAU',`AAU(RUNFUTURE,$1,$2)')
define(`TEX',`AEX(RUNFUTURE,$1)')
define(`TEF',`AEF(RUNFUTURE,$1)')
define(`TEG',`AEG(RUNFUTURE,$1)')
define(`TEU',`AEU(RUNFUTURE,$1,$2)')

----------------------------------------------------------------

-- Knowledge operators
--
-- Each agent is represented by the post-condition A of the action
-- corresponding to its accessibility relation.  There is a single
-- operator KK(A,P) for individual, group and distributed knowledge,
-- where A is, respectively, (the condition for the accessibility of)
-- a single agent, a disjunction of agents or a conjunction of agents.
-- Common knowledge is denoted K(A,P), where A is the disjunction of
-- actors.
--
-- NB: action conditions for each agent are not mutually exclusive.
-- This is necessary to support the distributed knowledge operator,
-- which asks for an accessibility step common to all agents in the
-- group.  Otherwise, since our model is enriched so that, by
-- construction, target states are different when accessed through
-- different actions, this intersection would always be empty and
-- distrubted knowledge could not be easily implemented.

define(`KK',`AAX(($1) & REACHABLE, $2)')
define(`KC',`AAG(($1) & REACHABLE, $2)')

----------------------------------------------------------------

divert
