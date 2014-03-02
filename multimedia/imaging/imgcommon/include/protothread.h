/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


class COM_SM;

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(ProtoThread);
#endif
class ProtoThread
//***************************************************************************************
{
public:
	ProtoThread();

	enum enumStatus
	{ // Current Protothread status
		eStatus_Idle,
		eStatus_Waiting,
		eStatus_Yield,
		eStatus_OnCall,
		eStatus_End,
	};

	virtual int Start();
	virtual int Reset(bool force=false);
	virtual int ptBegin();


	enumStatus GetStatus() const { return((enumStatus)mStatus); };
	int  SetStatus(enumStatus val)  { mStatus=val; return(0); };

	int  GetPTState() const { return(mProtoThreadState); };
	int  SetPTState(int val)  { mProtoThreadState=val; return(0); };

	bool IsThreated() const { return(mThreated); }
	void SetThreated(bool val) {mThreated=val;}

	bool GetOnCall() const { return(mOnCall); }

protected:
	int  mProtoThreadState; // For protothread memorize current state
	int  mStatus;           // For protothread return value
	bool mThreated;         // For protothread True when signal has been handled
	bool mOnCall;           // For protothread True when a call has been invoked
} ;



#ifdef _WIN32
	#define HSM_SET_NEW_POINT(s) HSM_SET_NEW_POINT2(s, __COUNTER__+1)
	#define HSM_SET_NEW_POINT2(s, GUID) s.SetPTState(GUID); case GUID:
#else
	#define HSM_SET_NEW_POINT(s) s.SetPTState(__LINE__); case __LINE__:
#endif

#define HSM_BEGIN(pt, pEvent) pt.ptBegin();                   \
	if (pEvent->sig== SCF_STATE_EXIT_SIG) { pt.Reset(); return(0);}   \
	else if (pEvent->sig== SCF_PARENT_SIG) { }         \
	else if (pEvent->sig== SCF_STATE_INIT_SIG) { return(0);}         \
	else{                 \
		{                                                     \
			int PT_YIELD_FLAG = 1;                            \
			switch(pt.GetPTState()) { case 0:

#define HSM_WAIT_UNTIL(pt, condition)                    \
	do {                                                 \
		HSM_SET_NEW_POINT(pt);                           \
		if (pt.IsThreated() !=false) return(0);          \
		if(!(condition)) {                               \
			pt.SetStatus(ProtoThread::eStatus_Waiting);  \
			goto End; }                                  \
	} while(0);                                          \
	pt.SetThreated(true);

#define HSM_END(pt)                                  \
		} /*End of protothread switch*/              \
		PT_YIELD_FLAG = 0;                           \
		if (pt.GetOnCall()==false)                   \
		{	pt.SetPTState(0);                        \
			pt.SetStatus(ProtoThread::eStatus_End);  \
		}                                            \
		return 0;                                    \
		} /* end of */              \
	}/* */                      \
End:

#define HSM_CALL(pt, nextstate)        \
	whatNext=(QSTATE)getState();       \
	pt.mOnCall=true;                   \
	SCF_TRANSIT_TO(&COM_SM::nextstate); \
	HSM_NEXT(pt)

#define HSM_NEXT(pt)                               \
do {                                               \
	PT_YIELD_FLAG = 0;                             \
	HSM_SET_NEW_POINT(pt);                         \
	if(PT_YIELD_FLAG == 0) {                       \
		pt.SetStatus(ProtoThread::eStatus_OnCall); \
		return 0;}                                 \
} while(0)


//***************************************************************************************
//***************************************************************************************
//***************************************************************************************
//***************************************************************************************

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(HSMProtoThread);
#endif
class HSMProtoThread: public ProtoThread
//***************************************************************************************
{
public:
	HSMProtoThread(COM_SM &hsm);
	friend class COM_SM;

protected:
	COM_SM &m_HSM;
} ;




