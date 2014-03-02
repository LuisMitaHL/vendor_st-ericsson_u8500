/*******************************************************************************
*
*    File name: LCDriverThread.h
*     Language: Visual C++
*  Description: Active Thread Object class declarations
*
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
*******************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                 File LCDriverThread.h

#ifndef _LCDRIVERTHREAD_H_
#define _LCDRIVERTHREAD_H_

#include "CaptiveThreadObject.h"
#include "Timer.h"
#include "Logger.h"
#include "t_communication_service.h"

class CLCDriverMethods;

class CLCDriverThread: public CCaptiveThreadObject
{
public:
    CLCDriverThread(CLCDriverMethods *lcdriverMethods);

    void ResumeThread() {
        Thread.ResumeThread();
    }

    ErrorCode_e SetLcmFamily(Family_t family, Do_CEH_Call_t CEHCallback);

    void TimerOn();
    void TimerOff();
private:
    void InitializeCaptiveThreadObject() {}
    void MainExecutionLoop();
    void SignalDeath();
private:
    CLCDriverMethods *lcdriverMethods_;

    bool timerOn_;
    bool shutdown_;

    CCriticalSectionObject  LCMPollCS;
};

#endif // _LCDRIVERTHREAD_H_

//                                                                                          End of file LCDriverThread.h
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
