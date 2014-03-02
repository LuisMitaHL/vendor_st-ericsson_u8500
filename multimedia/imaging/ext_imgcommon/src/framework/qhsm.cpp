/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/** @file scf_api.cpp
  @brief This file implements api functions/class of the statechart framework for OMX camera code

  To simplify the State Machine author, states are filed as functions and members of derivated class.
  The member pointer functions are here encapsulated in Objects ScfOneState.
  For each call to the framework, the pointer is first associated to his registered State Object.
  All State Objects of type ScfOneState are registered in a class ScfAllState instanciated in ScfStateMachine the main Framework.
  The Framework allow to navigate through states and apply entry/exit/init states with following definition.

  - Init of a state: Entering in all states from Current State to destination State. Send Init signal to destination State
  - Enter in a state: Send signal Enter to the destination state
  - Exiting a state : Send signal Exit to the state
  - Transit from one state to another: Exit all state from current state to common parent with destination state,
  then enter all state up to destination state. Init destination state. destinatino state become current state
  - Send Event to a state. If not entry/exit or init, send the event to current state. If current state return 0 stop, else send event to returned state.
  */

#define DBGT_LAYER  3
#undef  DBGT_PREFIX
#define DBGT_PREFIX "QHSM"
#include "debug_trace.h"
#include "ext_qassert.h"
#include "ext_port.h"
#include "ext_qhsm.h"

#include "ext_error.h"
#ifdef SCF_LOS
#include <los/api/los_api.h>
#endif

int CScfStateMachine::StateMachineNumber=1;

/* ************************************************************** */
/* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| */
/* |||| C SCF STATE MACHINE ||||||||||||||||||||||||||||||||||||| */
/* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| */
/** Constructor of the states container */
/** Constructor of the SCF framework*/
CScfStateMachine::CScfStateMachine ()
{
    char sname[] = "CScfStateMachine::ScfTop"; /*** Name of the Top State */
    StateMachineID = StateMachineNumber;
    StateMachineNumber++;
    SCF_MSG1(0,"SCF_DEBUG ---> CREATE NEW SCF STATE MACHINE CScfStateMachine Number %.1d",StateMachineID);
    // Init Pointer for First Function to be called when starting State Machine
    ScfTopInit = 0;
#ifdef SCF_LOS
    ullTimerLast= LOS_getSystemTime();
#endif

    // Create States Object containers
    cStates = new ScfAllState(this);

    // Initialize Current State as the TopMost State
    cpCurrentState = GetState(&CScfStateMachine::top,sname);
}

/** Destructor of the SCF framework*/
CScfStateMachine::~CScfStateMachine ()
{
    delete cStates;
    cStates=0;
}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn StateInit
 * \brief SCF_INIT execution function.
 * \param cpToState		Object State to which we need to Initialize
 *
 * Init of function should only call a function from the
 * under hierarchy.
 * Managed here as a go through hierarchy to the 'TO' state
 * with 'Entry' signals only.
 * 'Init' signal sent only to the 'TO' state.
 */
void CScfStateMachine::StateInit(ScfOneState *cpToState)
{
    //s_scf_event sevt_init = {SCF_STATE_INIT_SIG,{{0}},{}}};	///< Event for Init
ScfOneState *pcOSTop;							///< only for DBC_ASSERT

//if(cpToState->first_time) {
/// Check current state is a Parent of 'To State'
pcOSTop=CommonFather(cpToState);
DBC_ASSERT(pcOSTop==cpCurrentState);

/// Switch to State through 'Entry' signal sent to all
ScfTransit(cpToState);

}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn ScfTransit
 * \brief TRANSIT_TO execution function.
 * \param cpToState		Object State to which we need to Initialize
 *
 * First find common Parent, then Exit hierarchy up to it and go down to 'To' State
 * current state is maintained as the one where entry has just been sent.
 * 'Init' signal sent only to the 'TO' state.
 */
void CScfStateMachine::ScfTransit(ScfOneState * cpToState) {
    s_scf_event sevt_init = {sig:SCF_STATE_INIT_SIG, type:{other:0}, {}}; ///< Event for Init
    ScfOneState * cpCommonFather ; ///< Pointer on Common Father

    SCF_MSG3(1,"SCF_DEBUG(%d) TRANSIT FROM %s TO %s", StateMachineID,cpCurrentState->Name(), cpToState->Name());
    /// Find Common Father between current State and To State
    cpCommonFather = CommonFather(cpToState);
    /// Exit current hierarchy from Current State up to Common Parent
    ExitHierarchy(cpCommonFather);

    /// Enter Hierarchy to 'TO state'
    EnterHierarchy(cpToState);

    /// Send Event init to 'TO' state only if it has the possibility to call a Child.
    if(SendEvent(&sevt_init,cpToState,0)) {SCF_MSG0(0,"No Init available");}
}
/* -------------------------------------------------------------- */


/* ************************************************************** */
/** \fn ProcessEvent
 * \brief To execute an Event external to the State Machine
 * \param aEvt	Event received to be processed
 *
 * Just Call SendEvent with the Current State, nothing specific has been added.
 */
void CScfStateMachine::ProcessEvent(s_scf_event const *aEvt)
{
    SendEvent(aEvt,cpCurrentState);
}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn SendEvent
 * \brief Send Event to the indicated State. Specific overload without the 'do_loop=1'
 * \param aEvt	Event received to be processed
 * \param cpState Object State that is the destination
 * \
 *
 * Just Call SendEvent full proto, nothing specific has been added.
 */
void CScfStateMachine::SendEvent(s_scf_event const *aEvt, ScfOneState *cpState) {
    SendEvent(aEvt,cpState,1);
}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn SendEvent
 * \brief Send Event to the indicated State. Full interface
 * \param aEvt	Event received to be processed
 * \param cpState Object State that is the destination
 * \param do_loop Specific parameter to know if Event has to be reported to Parent (Entry/Exit/Init signals)
 *
 * Loop on all parents until one State accept to process current event. If do-loop=0, only process current state and return object.
 * Return value should be 0 in all cases except Init State not handled.
 */
ScfOneState * CScfStateMachine::SendEvent(s_scf_event const *aEvt, ScfOneState *cpState,bool do_loop)
{
    bool go = 1; ///< Default value for go. Set at do_loop after first iteration.
    ScfOneState * cpPrevious = cpState;

#ifdef SCF_LOS
    SCF_MSG5(0,"SCF_DEBUG(%d) Process Event %.3d (%.25s) to State %.35s %lld 10^2us",StateMachineID,aEvt->sig,CError::stringSMSig(aEvt->sig),cpState->Name(),(LOS_getSystemTime()-ullTimerLast)/100);
#else
    SCF_MSG4(0,"SCF_DEBUG(%d) Process Event %.3d (%.25s) to State %.35s ",StateMachineID,aEvt->sig,CError::stringSMSig(aEvt->sig),cpState->Name());
#endif

    /// Loop until State return 0 (event processed)
    while(cpState && go) {

        //SCF_MSG2(0,"Process Event %d to State %s",aEvt->sig,pscf_State->Name());
        if(DEBUG_SCF>4) {cpState->print_received_event(aEvt,cpState==cpCurrentState&&aEvt->sig>SCF_LAST_SIG?-1:cpPrevious->Level);}
        cpPrevious = cpState;

        /// Call State function with Event receive Parent State or 0 if processed
        cpState=(*this.*((ScfStatePtr)*cpState))(aEvt);


        /// Validate loop or only current state to execute
        go = do_loop;

    }
    return cpState;
}
/* -------------------------------------------------------------- */


/* ************************************************************** */
/** \fn EnterHierarchy
 * \brief Enter in the full hierarchy. Only prototype when current State is the From
 * \param cpTo Object State that is the destination
 *
 * Call full proto with Current State as From
 */
void CScfStateMachine::EnterHierarchy(ScfOneState *  cpTo) {
    EnterHierarchy(cpCurrentState,cpTo);
}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn EnterHierarchy
 * \brief Enter in the full hierarchy.
 * \param cpFrom Object State that is the start
 * \param cpTo Object State that is the destination
 *
 * FROM must be a parent of TO. As only 1 parent but multiple child,
 * loop from bottom to top to create the path. Then loop from top to bottom
 */
void CScfStateMachine::EnterHierarchy(ScfOneState *  cpFrom, ScfOneState *  cpTo) {
    s_scf_event sevt_entry = {sig:SCF_STATE_ENTRY_SIG,type:{other:0},{}};	///< Define Entry event
    s_scf_event sevt_exit = {sig:SCF_STATE_EXIT_SIG,type:{other:0},{}};	///< Define Entry event
    ScfOneState * cpCurrent = cpFrom;					///< Dummy State to Ensure Starting point is part of 'TO' Hierarchy

    /// Go through invert path from To to From... (Bottom to Top)
    /// This is not to find the common Father but to go through the hierarchy.
    /// Path is saved during this action (for DOWN usage.)
    cpCurrent= CommonFather(cpFrom,cpTo);
    DBC_ASSERT(cpCurrent==cpFrom); /// Bonus Check "From is a Parent of To"

    /// If To == From, need extra exit to re-enter state
    if(cpFrom==cpTo) {
        /// Send Event Entry and ensure return is 0 as Entry must be managed...
        if(this->SendEvent(&sevt_exit,cpFrom,0)) DBC_ASSERT(0);
        cpFrom = cpFrom->UP();
    }

    /// Loop on all hierarchy from top to bottom
    while(cpFrom!=cpTo) {
        /// Go DOWN according to path registered previously (CommonFather)
        cpCurrentState=cpFrom=cpFrom->DOWN();
        DBC_ASSERT(cpFrom);

        /// Send Event Entry and ensure return is 0 as Entry must be managed...
        if(this->SendEvent(&sevt_entry,cpFrom,0)) DBC_ASSERT(0);

    }
}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn ExitHierarchy
 * \brief Exit out of the full hierarchy. Only prototype when current State is the From
 * \param cpTo Object State that is the destination
 *
 * Call full proto with Current State as From
 */
void CScfStateMachine::ExitHierarchy(ScfOneState * pscf_To) {
    ExitHierarchy(cpCurrentState,pscf_To);
}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn EnterHierarchy
 * \brief Exit out of the full hierarchy.
 * \param cpFrom Object State that is the start
 * \param cpTo Object State that is the destination
 *
 * TO must be a parent of FROM.
 * Simpler than EntryHierarchy as natural loop from bottom to top
 * with 1 parent each time.
 * Error is triggered if TopState is tried to be exited... This means
 * TO is not parent of FROM and has not been found while looping.
 */
void CScfStateMachine::ExitHierarchy(ScfOneState * pscf_From, ScfOneState * pscf_To) {
    s_scf_event sevt_exit = {sig:SCF_STATE_EXIT_SIG,type:{other:0},{}}; ///< Define Exit event

    /// Bottom to Top loop
    while(pscf_From!=pscf_To) {
        /// Send Exit event and ensure return is 0 as Exit must be managed...
        if(this->SendEvent(&sevt_exit,pscf_From,0)) DBC_ASSERT(0);

        /// Get Parent
        cpCurrentState=pscf_From=pscf_From->UP();
    }
}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn CommonFather
 * \brief Go Up until common parent is found. Only prototype when current State is the 2nd state
 * \param cpTo Object State that is the destination
 *
 * Call full proto with Current State as Second State
 */
ScfOneState *CScfStateMachine::CommonFather(ScfOneState *cpState) {
    return CommonFather(cpState,cpCurrentState);
}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn CommonFather
 * \brief Go Up until common parent is found.
 * \param cpState_REF Object State that is the reference
 * \param cpState_TWO Object State that is the one to find the Parent with
 *
 * Max Parent must be the TOP.
 * First Reach same level in hierarchy for the two State as same Parent can't be
 * a lower generation than the oldest one in the tree.
 * Then both are going up at the the same time until a common state is found.
 */
ScfOneState *CScfStateMachine::CommonFather(ScfOneState *cpState_REF, ScfOneState *cpState_TWO) {
    int level_two=0;		///< Level of 2nd State
    int level_ref=0;		///< Level of Reference

    /// Get Levels from States (defined at creation)
    level_ref=cpState_REF->Level;
    level_two=cpState_TWO->Level;

    /// Loop until REF is at same level than TWO
    while(level_ref>level_two) {
        cpState_REF=cpState_REF->UP();
        level_ref--;
    }

    /// Loop until TWO is at same level than REF
    while(level_two>level_ref) {
        cpState_TWO=cpState_TWO->UP();
        level_two--;
    }

    /// Loop for both until Parent is common (May be one of the state)
    while(cpState_TWO!=cpState_REF) {
        cpState_REF=cpState_REF->UP();
        cpState_TWO=cpState_TWO->UP();
    }
    SCF_MSG2(2,"SCF_DEBUG(%d) Common Father : %s",StateMachineID,cpState_REF->Name());

    /// As the 2 states are identical REF is returned.
    return cpState_REF;
}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn CheckActive
 * \brief Just check if proposed State is the current active one in the framework.
 * \param cpState Object State that is the reference
 */
bool CScfStateMachine::CheckActive(ScfOneState *cpState) {
    if(cpState == cpCurrentState) {
        return TRUE;
    } else {
        return FALSE;
    }
}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| */
/* |||| SCF ALL STATE|||||||||||||||||||||||||||||||||||||||||||| */
/* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| */
/** Constructor of the states container */
ScfAllState::ScfAllState(CScfStateMachine *pSM) : base(pSM), sptr_nb(0) {
    int i=0;
    char vname []="Dummy";

    for(i=0;i<SCF_ARRAY_TABLE;i++) {
        pspt[i]=new ScfOneState(0,this,vname,base);
    }
    pspt_start=new ScfOneState(0,this,vname,base);
};
/* -------------------------------------------------------------- */

ScfAllState::~ScfAllState() {
    /* +ER351066 */
    delete pspt_start;
    for(int i=0;i<SCF_ARRAY_TABLE;i++) {
        delete pspt[i];
    }
    /* -ER351066 */
};

/* ************************************************************** */
/** \fn RegisterState
 * \brief When no more hope to find a state as already registered as an Object State, call Register State.
 * \param pF Pointer to the State function
 * \param idx ID of the group of State
 * \param s name of the function if available
 *
 * Create a new state of type ScfOneState with given parameters.
 * Register it in the different chained list requested.
 * Return the created state.
 */
ScfOneState *ScfAllState::RegisterState(ScfStatePtr pF, int idx, char const *s) {
    ScfOneState *pcOneState;
    DBC_ASSERT(idx>=0 && idx<SCF_ARRAY_TABLE);
    pcOneState = new ScfOneState(pF,this,s,base);
    pspt[idx]->RegisterChainT(pcOneState);
    pspt_start->RegisterChainB(pcOneState);
    return pcOneState;
}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn LookState
 * \brief Search Object State from member function pointer of the State. Prototype with no name.
 * Default name "None" is given.
 * \param pF Pointer to the State function
 *
 * If needed for debug a specific ID could be given instead of None.
 */
ScfOneState *ScfAllState::LookState(ScfStatePtr pF) {
    char vname []="None";
    return LookState(pF,vname);
}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn LookState
 * \brief Search Object State from member function pointer of the State.
 * \param s is used in case the function state is not found. This will be the name given to the created object.
 * \param pF Pointer to the State function
 *
 * Look in chained list of state if current state is already registered as an object.
 * If not found then a new state is created.
 * The found or created state is returned.
 */
ScfOneState *ScfAllState::LookState(ScfStatePtr pF, char const *s) {
    int idx = reinterpret_cast<int &>(pF) &0xFFF;
    ScfOneState *pcOneState;

    DBC_ASSERT(idx>=0 && idx<SCF_ARRAY_TABLE);

    pcOneState=pspt[idx]->FindChainT(pF);
    if(!pcOneState) {
        pcOneState=RegisterState(pF,idx,s);
    }
    DBC_ASSERT(pcOneState!=0);
    return pcOneState;
}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| */
/* |||| SCF ONE STATE|||||||||||||||||||||||||||||||||||||||||||| */
/* |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| */
/* ************************************************************** */
/** \fn FindChainT
 * \brief Search in chained list from member function pointer of the State.
 * \param pF Pointer to the State function
 *
 * Stop when end of list is found (0) or state pointer is found.
 */
ScfOneState *ScfOneState::FindChainT(ScfStatePtr pF) {
    if(ptr==pF) {return this;}
    if(nextT == 0) {return 0;}
    return nextT->FindChainT(pF);

}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn RegisterChainT
 * \brief Insert the current Object at the end of chained list.
 * \param pcOS Pointer to the State Object
 *
 * Pass the pointer to Next in the chain until last of the chain.
 * Update Next and Previous.
 */
void ScfOneState::RegisterChainT(ScfOneState *pcOS) {
    if(nextT) {
        nextT->RegisterChainT(pcOS);
    } else {
        nextT=pcOS;
        pcOS->prevT = this;
    }

}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn RegisterChainB
 * \brief Insert the current Object at the end of chained list.
 * \param pcOS Pointer to the State Object
 *
 * Pass the pointer to Next in the chain until last of the chain.
 * Update Next and Previous.
 */
void ScfOneState::RegisterChainB(ScfOneState *pcOS) {
    if(nextB) {
        nextB->RegisterChainB(pcOS);
    } else {
        nextB=pcOS;
        pcOS->prevB = this;
    }

}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn LinkHierarchy
 * \brief Get All Parent of the current State. Update Parents with Childrens.
 *
 * Use Special Parent Signal to request the parent object.
 * Update Parent Children chained list.
 */
void ScfOneState::LinkHierarchy(void) {
    s_scf_event sevt_Parent= {sig:SCF_PARENT_SIG,type:{other:0},{}}; ///< Signal to request Parent
    ScfOneState *pcOneState;                    ///< State to Navigate in chained list

    /// Get Father object through function state Parent signal (Function SendSignal not used).
    pOSFather = (*base.*ptr)(&sevt_Parent);

    /// If a father (special case for Top State)
    if(pOSFather) {
        /// Check if Chained list already initiated by a Brother
        if(pOSFather->pOSChildren) {
            /// Loop until End of chained list and Register as Next
            pcOneState = pOSFather->pOSChildren;
            while(pcOneState->pOSNextChild) {
                pcOneState=pcOneState->pOSNextChild;
            }
            pcOneState->pOSNextChild = this;

        } else {
            /// Else Initiate chained list as first Children (many gifts expected!)
            pOSFather->pOSChildren = this;
        }
        Level=pOSFather->Level+1;
    } else {
        /// Only case when No father if current state is TOP
#ifndef PR_IMP
        DBC_ASSERT(ptr==&CScfStateMachine::ScfTop);
#else
        DBC_ASSERT(ptr==&CScfStateMachine::top);
#endif
    }
    if(DEBUG_SCF>1) {print_hierarchy();}
}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn print_hierarchy
 * \brief Recursively go down and print all State.
 * \param p Pointer on current State
 * \param level Could be taken from the registered Level in State object (it is printed for debug).
 *
 * For Debug Purpose
 */
void ScfOneState::print_hierarchy(ScfOneState *p,int level) {
    int i;

    for(i=0;i<level;i++) {SCF_MSG0(1,".");}
    SCF_MSG2(1, ".%d| %s " , p->Level,p->StateName);
    p=p->pOSChildren;
    while(p) {
        print_hierarchy(p,level+1);
        p=p->pOSNextChild;

    }
}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn print_hierarchy
 * \brief Go on Top and then go down and print all State.
 *
 * For Debug Purpose
 */
void ScfOneState::print_hierarchy() {
    ScfOneState *p;
    SCF_MSG1(1,"\n----------------HIERARCHY (%d) -------------------",base->smid());
    p=this;
    while(p->pOSFather) {
        p=p->pOSFather;
    }
    p->print_hierarchy(p,0);
    SCF_MSG0(1,"--------------END-----------------------");
}
/* -------------------------------------------------------------- */

/* ************************************************************** */
/** \fn print_hierarchy
 * \brief Go on Top and then go down and print all State.
 * \param evt Event to print
 * \param level_from From which level emitted as shy tentative to have right or left arrow for amazing debugging user experience.
 *
 * For Debug Purpose
 */
void ScfOneState::print_received_event(s_scf_event const *evt, int level_from) {
    int i;
    int l=Level*6+6;
#ifdef STR_STRING
    char s[4];
    switch (evt->sig) {
    case SCF_STATE_INIT_SIG:sprintf(s,"INI");break;
    case SCF_STATE_ENTRY_SIG:sprintf(s,"ENT");break;
    case SCF_STATE_EXIT_SIG:sprintf(s,"EXI");level_from=Level+1;break;
    default:sprintf(s,"%.3d",evt->sig);break;
    }
#endif
    SCF_MSG0(4,"\n");
    //SCF_MSG2(0,"lf %d lt %d\n",level_from, Level);
    for(i=0;i<l-1;i++) {SCF_MSG0(4," ");}SCF_MSG0(4,"|\n");
    if(level_from<0) {
        for(i=0;i<l-7;i++) {SCF_MSG0(4," ");}SCF_MSG1(4,"%s|> ",s);SCF_MSG0(4,"|\n");
        for(i=0;i<l-4;i++) {SCF_MSG0(4," ");}SCF_MSG0(4,"|");for(i++;i<l-1;i++){SCF_MSG0(4," ");}SCF_MSG0(4,"|\n");
    } else if(Level<level_from) {
        for(i=0;i<l-1;i++) {SCF_MSG0(4," ");}SCF_MSG0(4,"|\n");
        for(i=0;i<l-1;i++) {SCF_MSG0(4," ");}SCF_MSG0(4,"|");SCF_MSG1(4," <-%s\n",s);
    } else if(Level==level_from) {
        for(i=0;i<l-1;i++) {SCF_MSG0(4," ");}SCF_MSG0(4,"|\n");
        for(i=0;i<l-7;i++) {SCF_MSG0(4," ");}SCF_MSG1(4,"%s-> ",s);SCF_MSG0(4,"|\n");
    } else if(Level>level_from) {
        for(i=0;i<l-4;i++) {SCF_MSG0(4," ");}SCF_MSG0(4,"|");for(i++;i<l-1;i++){SCF_MSG0(4," ");}SCF_MSG0(4,"|\n");
        for(i=0;i<l-7;i++) {SCF_MSG0(4," ");}SCF_MSG1(4,"%s|>",s);SCF_MSG0(4," |\n");
    }
    for(i=0;i<l-1;i++) {SCF_MSG0(4," ");}SCF_MSG0(4,"|\n");
    for(i=0;i<l-5;i++) {SCF_MSG0(4," ");}SCF_MSG1(4,"%s\n",StateName);
    for(i=0;i<l-1;i++) {SCF_MSG0(4," ");}SCF_MSG0(4,"|\n");
}

