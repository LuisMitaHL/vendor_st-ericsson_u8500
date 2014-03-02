/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file AGov.h
 * Audio specific RM governors class interface
 *
 * *****************************************************************************/
#ifndef _AGOV_H_
#define _AGOV_H_

#include "uosal.h"
#include "ResAlp.h"

namespace rme {

/** Audio specific RM governors class */
class AGov {
 public:
   /// starts detector threads
   void StartDetectorThreads(void);

  /// implements thread that detects changes in display state,
  /// checks the component network and enables/disables burst mode accordingly
  /// => needed since decoders are not necessarily wrapped by NMF/ENS
  /// *and* audio and visual processes are separated and no RME server implemented (one per process)
  /// =>visual components not usable to indicate display activity
  void DisplayDetectorThreadImpl(void);
  /// implements thread that detects changes in adm or visual activity
  void ActivityDetectorThreadImpl(void);

  /// processes audio low power management; returns ALP timeout if any
  unsigned int ProcessALP(const char* callerid, bool allow_timeout);

  /// checks if in 'visual' process
  bool IsVisualProcess(void) const;

  /// checks if display is On
  bool IsDisplayOn(void) const { return mIsDisplayOn; }

  /// checks if there is SxA based ALP activity reported by tracker file
  bool IsActivityTrackerAlpOn(void) const;
  /// checks if there is SxA based visual activity reported by tracker file
  bool IsActivityTrackerVisualOn(void) const;
 /// checks if there is ADM based activity reported by tracker file
  bool IsActivityTrackerAdmOn(void) const;

  /// checks if ALP use case governor is available or not
  bool IsUcGovAlpAvailable(void) const;

  /// sets ResAlp pointer
  void SetResAlp(ResAlp *presalp) { mpResAlp = presalp; }

  /// reacts to ALP enter requested (pending enter)
  void UccReactAlpEnterRequested(void);
  /// reacts to ALP enter applied signal
  void UccReactAlpEnterApplied(void);
  /// reacts to ALP exit requested signal
  void UccReactAlpExitRequested(void);
  /// reacts to ALP exit applied signal
  void UccReactAlpExitApplied(void);

  /// reacts to component tunnel
  void UccReactTunnel(const char* rolecompin, const char* rolecompout);
  /// reacts to component unregister
  void UccReactUnregister(const char* rolecomp);

  /// reacts on visual activity detected *from network graph*
  void UccReactVisualActivityOn(void);
  /// reacts on no visual activity detected *from network graph*
  void UccReactVisualActivityOff(void);

  /// trigs activity detector wakeup
  void UccTrigsActivityDetector(void);

  /// ctor
  AGov() :
    mDisplayDetectorThreadCreated(false),
    mActivityDetectorThreadCreated(false),
    mIsDisplayOn(true),
    mIsVisualActive(false),
    mIsAlpActive(false),
    mIsUcgVoiceCall(false),
    mpResAlp(NULL) {
#ifdef __RMEUNITARYDEV__
      mIsDisplayOn = false;
#endif
      if (IsVisualProcess() == false) {
        DeleteAllUcActivity();
        CreateUcActivity(mFileNameAdm, false, true);
        CreateUcActivity(mFileNameUcAlp, mIsAlpActive, true);
      } else {
        CreateUcActivity(mFileNameUcVisual, mIsVisualActive, true);
      }
    }

  /// dtor
  ~AGov() {
    if (mIsVisualActive) {
      UccReactVisualActivityOff();
    }
    if (mIsAlpActive) {
      UccReactAlpExitRequested();
      UccReactAlpExitApplied();
    }
  }

 private:
  /// display detector thread flag
  bool mDisplayDetectorThreadCreated;
  /// activity detector thread flag
  bool mActivityDetectorThreadCreated;
  /// current display state
  bool mIsDisplayOn;
  /// current visual activity tracker state
  bool mIsVisualActive;
  /// current ALP activity tracker state
  bool mIsAlpActive;
  /// current use case gov mode is voicecall
  bool mIsUcgVoiceCall;

  /// ResAlp class instance pointer
  ResAlp *mpResAlp;

  static const char* mFileNameWaitDisplayOff;
  static const char* mFileNameWaitDisplayOn;
  static const char* mFileNameUcGovVoiceCall;
  static const char* mFileNameUcGovAlp;
  // hack files for multiple rme instances
  static const char* mFileNameUcVisual;
  static const char* mFileNameUcAlp;
  // hack file for ADM synchro
  static const char* mFileNameAdm;

  /// deletes activity files
  void DeleteAllUcActivity() const;
  /// creates activity file
  void CreateUcActivity(const char* ucname, int initvalue, bool forceinit) const;
  /// writes to activity file
  void WriteUcActivity(const char* ucname, int value, bool allowcreate) const;
  /// reads from activity file
  void ReadUcActivity(const char* ucname, int *pvalue) const;

  // disallow copy and assign default constructors
  DISALLOW_COPY_AND_ASSIGN(AGov);
};

}  // namespace

#endif
