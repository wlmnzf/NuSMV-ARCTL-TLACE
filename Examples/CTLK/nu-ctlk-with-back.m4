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
-- note 1: as of 2.2.1, NuSMV refuses input variables in CTL formulas 
-- (this makes sense, as branching quantification would otherwise 
-- be ill-defined).  This forces to use post-transition conditions
-- to select between different transition types.
--
-- note 2: as of 2.2.1, NuSMV has trouble generating traces for complex 
-- formulas.  In formulas of the form P & Q, it will wrongly try to
-- generate a trace for P when in fact Q is false.
--
-- Version 6: (Franco Raimondi, 06/08/2004). Use of IVAR and formulas
-- with IVAR's (EAX, EAF, etc.), REACHABLE and START [check NuSMV 
-- extensions]
--
-- Version 7: (Simon Busard, 01/12/2011).
-- (Re)introduce the BACK action to express reachability.
--

-- Declare all temporal transitions as TTRANS(<trans>)
-- Use PREV and NEXT to refer to variables and their next value

define(`NEXT', `(case PAST : ($@) ; 1 : next($@) ; esac)')
define(`PREV', `(case PAST : next($@) ; 1 : ($@) ; esac)')

define(`TINIT',`
INIT ($@)
INVAR INITIAL -> ($@)')

define(`TTRANS',`TRANS RUN -> ($@)')

define(`VAR_ENV',`dnl
IVAR 
  RUN : boolean; -- label for temporal transitions
  PAST : boolean; -- label for reverse temporal transitions
VAR
  INITIAL : boolean;
INIT INITIAL')

define(`ENV',`RUN, PAST, INITIAL')

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
IVAR $1 : boolean;
AGENT__TRANSITIONS($@)')

define(`AGENT__TRANSITIONS',`dnl
ifelse($#, 1, `-- NO VARIABLES --',
        $#, 2, `AGENT__TRANSITION($1,$2)',
        `AGENT__TRANSITION($1,$2)
AGENT__TRANSITIONS($1,shift(shift($@)))')')

define(`AGENT__TRANSITION',`TRANS $1 -> ($2)=next($2)')

----------------------------------------------------------------

define(`RUNFUTURE',`(RUN & !PAST)')
define(`RUNPAST',`(RUN & PAST)')
define(`REACHABLE',`EAF(RUNPAST)(INITIAL)')

-- Temporal CTL operators

define(`TAX',`AAX(RUNFUTURE)($1)')
define(`TAF',`AAF(RUNFUTURE)($1)')
define(`TAG',`AAG(RUNFUTURE)($1)')
define(`TAU',`AA(RUNFUTURE)[$1 U $2]')
define(`TEX',`EAX(RUNFUTURE)($1)')
define(`TEF',`EAF(RUNFUTURE)($1)')
define(`TEG',`EAG(RUNFUTURE)($1)')
define(`TEU',`EA(RUNFUTURE)[$1 U $2]')

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

define(`KK',`AAX($1)(REACHABLE -> $2)')

-- KP(ag, prop) is "ag knows whether prop is true or false"
define(`KP',`(KK($1,$2) | KK($1,!($2)))')
define(`KC',`AAG($1)(REACHABLE -> $2)')

----------------------------------------------------------------

divert
