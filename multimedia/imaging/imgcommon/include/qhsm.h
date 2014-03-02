/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/** @file scf_api.h
  *  @brief This file is the header file of the statechart framework
  *  used in Camera code to implement event driven behavior.
  *  This file defines all macros, functions, bases classes required to design the camera main state machine
  *
  * To Enable DEBUG, use flag DEBUG_SCF=<0 to 5>, 0 none, 1=Start SCF+event transition,...,5=Max with debug message for SCF only
    */

//rename_file as scf_api.h


#ifndef _SCF_API_H_
#define _SCF_API_H_
#include "qevent.h" // rename_me as scf_events.h
#define PR_IMP // to be removed one day as there should not be any #ifndef PR_IMP anywhere in the code.

#ifdef DEBUG_CHECKALIVE 
#define SCF_STRING
#define DEBUG_SCF_STRING
#endif

#ifdef DEBUG_SCF_STRING
#include <stdio.h>
#include <string.h>
#endif

#ifndef __SYMBIAN32__
#include <assert.h> /* For framework usage sanity checks*/
#endif
/**
* \defgroup structures, pointers and enums definition
*/
/*@{*/
#ifndef DBC_ASSERT
#define DBC_ASSERT assert
#endif
//#define DEBUG_SCF 1
#ifndef DEBUG_SCF
#define DEBUG_SCF 0
#define SCF_MSG0(level,arg0) 
#define SCF_MSG1(level,arg0,arg1) 
#define SCF_MSG2(level,arg0,arg1,arg2) 
#define SCF_MSG3(level,arg0,arg1,arg2,arg3) 
#define SCF_MSG4(level,arg0,arg1,arg2,arg3,arg4) 
#define SCF_MSG5(level,arg0,arg1,arg2,arg3,arg4,arg5) 
#else
#define SCF_MSG0(level,arg0)				(if(DEBUG_SCF>level) {MSG0(arg1);})
#define SCF_MSG1(level,arg0,arg1)			(if(DEBUG_SCF>level) {MSG1(arg1,arg2);})
#define SCF_MSG2(level,arg0,arg1,arg2)		(if(DEBUG_SCF>level) {MSG2(arg1,arg2,arg3);})
#define SCF_MSG3(level,arg0,arg1,arg2,arg3) (if(DEBUG_SCF>level) {MSG3(arg1,arg2,arg3,arg4);})
#define SCF_MSG4(level,arg0,arg1,arg2,arg3,arg4) (if(DEBUG_SCF>level) {MSG3(arg1,arg2,arg3,arg4);})
#define SCF_MSG5(level,arg0,arg1,arg2,arg3,arg4,arg5) (if(DEBUG_SCF>level) {MSG3(arg1,arg2,arg3,arg4);})
#endif

#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0

#define SCF_ARRAY_TABLE 4096

/** list of signals that are handled by the framework.
* SCF dispatches the signal and relevant context information to the current active state by merging them into an event.
*/

/** return type for a state function. This type is defined by the framework
and allows a function state to return a pointer on a parent state.
*/
// TO BE DELETED typedef void* t_scf_state_ret_type;

class CScfStateMachine;
class ScfAllState;
struct ScfOneState;
typedef ScfOneState *( CScfStateMachine::*ScfStatePtr )(s_scf_event const *);






struct ScfOneState
{	
	ScfOneState( ScfStatePtr ptr_of_func, ScfAllState *pS, char const *s, CScfStateMachine *b ) : Level(0),first_time(1),ptr( ptr_of_func ),base(b),cpStates(pS),prevT(0),nextT(0),prevB(0),nextB(0),
		pOSFather(0),pOSChildren(0),pOSNextChild(0),pOSCChild(0) {
            StateName[0]='\0';
			if(ptr!=0) {
                SCF_MSG1(3,"NEW STATE CREATED %s\n",s);
#ifdef SCF_STRING
                DBC_ASSERT(strlen(s)<255);
                strcpy(StateName,s);
#endif
                LinkHierarchy();
			}
	};
	ScfOneState()
		: Level(0),first_time(1),ptr(0),base(0),cpStates(0),prevT(0),nextT(0),prevB(0),nextB(0),
		pOSFather(0),pOSChildren(0),pOSNextChild(0),pOSCChild(0)
	{StateName[0]='\0';};
	void Init(ScfAllState *pS, CScfStateMachine *b)
	{
		cpStates = pS;
		base = b;
	}
	
	~ScfOneState() {if(nextB) {delete nextB;nextB=0;}};
	ScfOneState *FindChainT(ScfStatePtr pF);
	void RegisterChainT(ScfOneState *pcOS);
	void RegisterChainB(ScfOneState *pcOS);

	ScfOneState *UP() {DBC_ASSERT(pOSFather);pOSFather->pOSCChild=this;return pOSFather;};	
	ScfOneState *DOWN() {return pOSCChild;};	
	bool IsParent() {return !(pOSChildren==0);}
	operator ScfStatePtr() { return ptr; };
	void print_hierarchy();
	void print_received_event(s_scf_event const *evt, int level_from);
	int Level;
	char *Name() {return StateName;};
	bool first_time;

private:
	ScfStatePtr ptr;
	CScfStateMachine* base;
	char StateName[256];
	ScfAllState *cpStates;
	ScfOneState *prevT,*nextT;
	ScfOneState *prevB,*nextB;
	ScfOneState *pOSFather,*pOSChildren,*pOSNextChild,*pOSCChild;

	void LinkHierarchy();
	void print_hierarchy(ScfOneState *p,int level);
};

class ScfAllState{
public:
	ScfAllState(CScfStateMachine *pSM);
	~ScfAllState();
	ScfOneState *RegisterState(ScfStatePtr pf, int idx, char const *s);
	ScfOneState *LookState(ScfStatePtr pF);
	ScfOneState *LookState(ScfStatePtr pF, char const *s);

private:

	CScfStateMachine* base;
	ScfOneState pspt[SCF_ARRAY_TABLE];
	int sptr_nb;
	ScfOneState *pspt_start;
};

/*@} end of structures and enums definition*/

/**
* \defgroup Macros definition
*/
/*@{*/

#ifdef SCF_STRING
#define SCF_STATE ScfOneState */**< return type of state functions */
#define SCF_INIT(state)  StateInit(GetState((ScfStatePtr)state,#state)) /**< MACRO used to exectute init of a given state */
#define SCF_STATE_PTR(state) GetState((ScfStatePtr)state,#state)  /**< MACRO used to return a state method pointer to the framework*/
#define SCF_TRANSIT_TO(state) ScfTransit(GetState((ScfStatePtr)(state),#state)) /**< MACRO used to do a state transition from one state to an other*/
#define SCF_START_SM(statemachine) statemachine->ScfCallTopInit(); /**< MAcro used to start the state machine must call at least ScfInit of SM derived from framework*/
#define Q_DEFINE_CALL_MEMBER_FN(class_)  /** added during API implemention checks. Don't know what to do with it. Hopefully not needed with PR implementation */
#define SCF_REGISTER_TOPSTATE_INIT(state) (ScfRegisterInit((ScfStatePtr)state)) /** added during API implementation checks: this macro is used to tell the framework what is the topmost state init transition. */
#define STATE_IS_ACTIVE(state) (CheckActive(GetState((ScfStatePtr)state,#state))) /** added during API replacement. return 1 if (state) is the current active state */

#define SCF_PSTATE ScfOneState *  /* *? */ /*added during API replacement for managing QTRAN QTRAN_DYN might be useless in PR implementation */
#define SCF_TRANSIT_TO_PSTATE(state) ScfTransit(state) /**< added during API replacement MACRO used to do a state dynamic (using state pointer) transition from one state to an other */
#else
// NO STRING
#define SCF_STATE ScfOneState * /**< return type of state functions */
#define SCF_INIT(state)  StateInit(GetState((ScfStatePtr)state)) /**< MACRO used to exectute init of a given state */
#define SCF_STATE_PTR(state) GetState((ScfStatePtr)state)  /**< MACRO used to return a state method pointer to the framework*/
#define SCF_TRANSIT_TO(state) ScfTransit(GetState((ScfStatePtr)(state))) /**< MACRO used to do a state transition from one state to an other*/
#define SCF_START_SM(statemachine) statemachine->ScfCallTopInit(); /**< MAcro used to start the state machine must call at least ScfInit of SM derived from framework*/
#define Q_DEFINE_CALL_MEMBER_FN(class_)  /** added during API implemention checks. Don't know what to do with it. Hopefully not needed with PR implementation */
#define SCF_REGISTER_TOPSTATE_INIT(state) (ScfRegisterInit((ScfStatePtr)state)) /** added during API implementation checks: this macro is used to tell the framework what is the topmost state init transition. */
#define STATE_IS_ACTIVE(state) (CheckActive(GetState((ScfStatePtr)state))) /** added during API replacement. return 1 if (state) is the current active state */

#define SCF_PSTATE ScfOneState *  /* *? */ /*added during API replacement for managing QTRAN QTRAN_DYN might be useless in PR implementation */
#define SCF_TRANSIT_TO_PSTATE(state) ScfTransit(state) /**< added during API replacement MACRO used to do a state dynamic (using state pointer) transition from one state to an other */

#endif

/*@} end of macro definitions*/


/**
* \defgroup base class
*/
/*@{*/

/** Definition of the SCF base class. This class is the placeholder for all states of the statemachine to implement.
States are members of this class. This class implements all framework related functionalities
eg dispatching of events, initialisation, states transitions,....
*/

class CScfStateMachine
{
public:
	CScfStateMachine();
	~CScfStateMachine();

	SCF_STATE ScfTop(s_scf_event const *aEvt) {DBC_ASSERT (aEvt->sig<SCF_LAST_SIG);/*(ScfStatePtrN_::SetName(__FUNCTION__));*/return 0;}; /**< top most state of the framework. Should never be functionnaly reached. If reached an event has been left unprocessed */
	SCF_STATE top(s_scf_event const *aEvt) {return ScfTop(aEvt);} ///< Only for compatibility with old def.

	void ProcessEvent(s_scf_event const *aEvt);

	/* To adress Macro */
	void ScfRegisterInit(ScfStatePtr pscfInit) {DBC_ASSERT(pscfInit!=0);ScfTopInit = pscfInit;};
    void ScfCallTopInit() {
        SCF_MSG0(0,"Calling Top Init function\n");
        if (0 != ScfTopInit) {
            (*this.*ScfTopInit)(0);
        }
        else {
            DBC_ASSERT(0);
        }
    };
	ScfOneState *GetState(ScfStatePtr pF) {return cStates->LookState(pF);};
	ScfOneState *GetState(ScfStatePtr pF,char const *s) {SCF_MSG1(4,"GET STATE %s\n",s);
										   return cStates->LookState(pF,s);};
	bool CheckActive(ScfOneState *pscf_State);
	void StateInit(ScfOneState *pscf_State);
void ScfTransit(ScfOneState * pscf_State);
int smid(void) {return StateMachineID;};

    /* for debug only ! */
    ScfOneState * getCurrentState() { return cpCurrentState; }
    

private:
	ScfOneState *cpCurrentState;
	unsigned long long ullTimerLast;
	ScfStatePtr ScfTopInit;
	ScfAllState *cStates;

    void ExitHierarchy(ScfOneState *pscf_From, ScfOneState *pscf_To);
    void ExitHierarchy(ScfOneState *pscf_From);
    void EnterHierarchy(ScfOneState *  pscf_To);
    void EnterHierarchy(ScfOneState *  pscf_From, ScfOneState *  pscf_To);

	ScfOneState *CommonFather(ScfOneState *pscf_State);
	ScfOneState *CommonFather(ScfOneState *pscf_State, ScfOneState *pscf_Current);
void SendEvent(s_scf_event const *aEvt, ScfOneState *pscf_State);
ScfOneState * SendEvent(s_scf_event const *aEvt, ScfOneState *pscf_State, bool do_loop);
 int StateMachineID;
static int StateMachineNumber;

};
/*@} end base class*/


#endif                                                      // _SCF_API_H_
