/*******************************************************************************
*
*    File name: LCDriverThread.cpp
*     Language: Visual C++
*  Description: Main loop for LCDriver
*
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
*******************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                               File LCDriverThread.cpp

#include "LCDriverThread.h"
#include "LCDriverMethods.h"
#include "lcdriver_error_codes.h"
#include "Event.h"
#ifdef _WIN32
#include "WinApiWrappers.h"
#endif

CLCDriverThread::CLCDriverThread(CLCDriverMethods *lcdriverMethods):
    lcdriverMethods_(lcdriverMethods),
    timerOn_(false),
    shutdown_(false)
{
}

void CLCDriverThread::MainExecutionLoop()
{
    while (!shutdown_) {
        // Limit the scope of the lock to avoid holding lock while sleeping. This
        // solves the problem with starvation of other threads trying to lock LCMPollCS.
        {
            CLockCS lock(LCMPollCS);
            lcdriverMethods_->m_pTimer->DoTimerHandler(timerOn_);

            ErrorCode_e ReturnValue = lcdriverMethods_->m_pLcmInterface->CommunicationPoll();

            if (E_SUCCESS != ReturnValue) {
#ifdef _THREADDEBUG
                lcdriverMethods_->m_pLogger->log("LCD MainThread: LCM polling ERROR = %d", ReturnValue);
#endif

                if (R15_FAMILY == lcdriverMethods_->m_CurrentProtocolFamily) {
                    lcdriverMethods_->SignalError(lcdriverMethods_->MapLcmError(ReturnValue));
                    IsDying = LCDRIVER_THREAD_STOPPED_AFTER_LCM_ERROR;
                    break;
                }
            }
        }

        OS::Sleep(1);
    }
}

void CLCDriverThread::SignalDeath()
{
    CLockCS lock(LCMPollCS);
    shutdown_ = true;
}

void CLCDriverThread::TimerOn()
{
    CLockCS lock(LCMPollCS);
    timerOn_ = true;
}

void CLCDriverThread::TimerOff()
{
    CLockCS lock(LCMPollCS);
    timerOn_ = false;
}

ErrorCode_e CLCDriverThread::SetLcmFamily(Family_t family, Do_CEH_Call_t CEHCallback)
{
    CLockCS lock(LCMPollCS);
    return lcdriverMethods_->m_pLcmInterface->CommunicationSetFamily(family, CEHCallback);
}

//                                                                                        End of file LCDriverThread.cpp
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
