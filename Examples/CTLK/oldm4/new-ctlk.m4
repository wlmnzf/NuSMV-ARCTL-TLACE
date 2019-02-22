divert(-1)


# Translate interpreted systems into Kripke structures and
# CTLK specifications into ARCTL specification.


# Agent-related macros
define(`AGENT',`dnl
IVAR
	$1 : boolean;
AGENT_VARS($@)')
define(`AGENT_VARS',`dnl
	ifelse($#,1,,$#,2,`AGENT_VAR($1,$2)',
		`AGENT_VAR($1,$2)AGENT_VARS($1,shift(shift($@)))')')
define(`AGENT_VAR',`
TRANS
	($1) -> ($2) = next($2)')

define(`TINIT',`dnl
INIT
	$@
INVAR
	INITIAL -> ($@)')
define(`TTRANS',`dnl
TRANS
	RUN -> ($@)')
define(`NEXT', `(case PAST : ($@) ; 1 : next($@) ; esac)')
define(`PREV', `(case PAST : next($@) ; 1 : ($@) ; esac)')


# Environment-related macros
define(`ENV',`RUN,PAST,INITIAL')
define(`VAR_ENV',`dnl
IVAR
	RUN : boolean;
	PAST : boolean;
VAR
	INITIAL : boolean;
INIT
	INITIAL')


# CTLK-related macros
define(`RUNFUTURE',`RUN & !PAST')
define(`RUNPAST',`RUN & PAST')

# Temporal operators
define(`TAG',`AAG(RUNFUTURE)($1)')
define(`TEG',`EAG(RUNFUTURE)($1)')
define(`TAF',`AAF(RUNFUTURE)($1)')
define(`TEF',`EAF(RUNFUTURE)($1)')
define(`TAX',`AAX(RUNFUTURE)($1)')
define(`TEX',`EAX(RUNFUTURE)($1)')
define(`TAU',`AA(RUNFUTURE)[($1) U ($2)]')
define(`TEU',`EA(RUNFUTURE)[($1) U ($2)]')

define(`REACHABLE',`EAF(RUNPAST)(INITIAL)')

# Epistemic operators
define(`KK',`AAX($1)(REACHABLE -> ($2))')
define(`KC',`AAG($1)(REACHABLE -> ($2))')


divert(0)