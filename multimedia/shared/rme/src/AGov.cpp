/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file AGov.cpp
 * Audio specific RM governors class
 *
 * *****************************************************************************/
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/inotify.h>
#include <sys/stat.h>

#include <los/api/los_api.h>

#include "RMEimpl.h"
#include "PfRoles.h"
#include "SharedInst.h"
#include "AGov.h"

namespace rme {

const char* AGov::mFileNameWaitDisplayOff = "/sys/power/wait_for_fb_sleep";
const char* AGov::mFileNameWaitDisplayOn = "/sys/power/wait_for_fb_wake";
#ifndef __RMEUNITARYDEV__
const char* AGov::mFileNameUcGovVoiceCall = "/sys/devices/system/cpu/usecase/voice-call";
const char* AGov::mFileNameUcGovAlp = "/sys/devices/system/cpu/usecase/low-power-audio";
const char* AGov::mFileNameUcVisual = "/data/local/tmp/rme_act_visual";
const char* AGov::mFileNameUcAlp    = "/data/local/tmp/rme_act_alp";
const char* AGov::mFileNameAdm      = "/data/local/tmp/rme_act_adm";
#else
const char* AGov::mFileNameUcGovVoiceCall = "/dev/null";
const char* AGov::mFileNameUcGovAlp = "/dev/null";
const char* AGov::mFileNameUcVisual = "/tmp/rme_act_visual";
const char* AGov::mFileNameUcAlp    = "/tmp/rme_act_alp";
const char* AGov::mFileNameAdm      = "/tmp/rme_act_adm";
#endif

extern "C" {
  void displaydetectorthread(void * arg) {
#ifdef __RMEUNITARYDEV__
    return;
#endif
    AGov* agovM = static_cast<AGov*>(arg);
    if (agovM) {
      agovM->DisplayDetectorThreadImpl();
    } else {
      RLOG_ERROR("no argument for display detector thread\n");
    }
  }

  void activitydetectorthread(void * arg) {
#ifdef __RMEUNITARYDEV__
    return;
#endif
    AGov* agovM = static_cast<AGov*>(arg);
    if (agovM) {
      agovM->ActivityDetectorThreadImpl();
    } else {
      RLOG_ERROR("no argument for activity detector thread\n");
    }
  }
}

/// starts detector threads
void AGov::StartDetectorThreads() {
  /// thread to detect if screen is turned on or off.
  if (!mDisplayDetectorThreadCreated) {
    mDisplayDetectorThreadCreated = true;
    RLOG_DEBUG("starting display detector thread\n");

    LOS_ThreadCreate(displaydetectorthread, this,
                     1024, LOS_USER_NORMAL_PRIORITY,
                     "RME_DisplayDetector");
  }

  /// thread to detect visual activity
  if (!mActivityDetectorThreadCreated) {
    mActivityDetectorThreadCreated = true;
    RLOG_DEBUG("starting activity detector thread\n");

    // force creation and init of activity file
    CreateUcActivity(mFileNameUcAlp, mIsAlpActive, true);

    LOS_ThreadCreate(activitydetectorthread, this,
                     1024, LOS_USER_NORMAL_PRIORITY,
                     "RME_ActivityDetector");
  }
}

// implements thread that detects screen on/off
void AGov::DisplayDetectorThreadImpl() {
  RLOG_DEBUG("DisplayDetectorThread\n");
  // increment refcount for RME singleton users
  if (RME::getInstance() == NULL)
    return;

  while (RMEimpl::mpMutexAsync && RMEimpl::mInstance->CountComponents()) {
    int ret;
    char dummyBuf;

    // open device for early suspend: read will block while display is on
    int filedoff = open(mFileNameWaitDisplayOff, O_RDONLY, 0);
    if (filedoff < 0) {
      RLOG_ERROR("could not open %s -- aborting DisplayDetectorThread\n", mFileNameWaitDisplayOff);
      break;
    }
    do {
      ret = read(filedoff, &dummyBuf, 1);
    } while (ret < 0 && errno == EINTR);

    // ALP update
    if (RMEimpl::MutexAsyncMsgLock()) {
      mIsDisplayOn = false;  // shall authorize ALP
      RMEimpl::MutexAsyncMsgUnlock();
    }
    ProcessALP("DisplayDetector", false);

    close(filedoff);

    // open device for early resume: read will block while display is off
    int filedon = open(mFileNameWaitDisplayOn, O_RDONLY, 0);
    if (filedon < 0) {
      RLOG_ERROR("could not open %s -- aborting DisplayDetectorThread\n", mFileNameWaitDisplayOn);
      close(filedon);
      break;
    }
    do {
      ret = read(filedon, &dummyBuf, 1);
    } while (ret < 0 && errno == EINTR);

    // ALP update; *no* timeout handling possible here
    if (RMEimpl::MutexAsyncMsgLock()) {
      mIsDisplayOn = true;  // shall forbid ALP
      RMEimpl::MutexAsyncMsgUnlock();
    }
    ProcessALP("DisplayDetector", false);

    close(filedon);
  }

  RME::delInstance();
  mDisplayDetectorThreadCreated = false;
}

// implements thread that detects adm or visual activity
void AGov::ActivityDetectorThreadImpl() {
  RLOG_DEBUG("ActivityDetectorThread\n");

  // creation of activity files in case files were destroyed:
  // don't overwrite if already exit
  CreateUcActivity(mFileNameUcVisual, mIsVisualActive, false);
  CreateUcActivity(mFileNameUcAlp, mIsAlpActive, false);
  CreateUcActivity(mFileNameAdm, false, false);

  int fd = inotify_init();
  if (fd < 0) {
    RLOG_ERROR("inotify_init\n");
    return;
  }

  // watch visual activity file for modification or suppression
  int wd_va = inotify_add_watch(fd, mFileNameUcVisual, IN_MODIFY | IN_ATTRIB |  IN_MOVE_SELF | IN_DELETE_SELF);
  if (wd_va < 0) {
    RLOG_WARNING("inotify_add_watch failed (%d) on %s\n", errno, mFileNameUcVisual);
  }

  // watch alp activity file for suppression
  int wd_alp = inotify_add_watch(fd, mFileNameUcAlp, IN_ATTRIB |  IN_MOVE_SELF | IN_DELETE_SELF);
  if (wd_alp < 0) {
    RLOG_WARNING("inotify_add_watch failed (%d) on %s\n", errno, mFileNameUcAlp);
  }

  // watch ADM activity file for modification or suppression
  int wd_adm = inotify_add_watch(fd, mFileNameAdm, IN_MODIFY | IN_ATTRIB |  IN_MOVE_SELF | IN_DELETE_SELF);
  if (wd_adm < 0) {
    RLOG_WARNING("inotify_add_watch failed (%d) on %s\n", errno, mFileNameAdm);
  }

  if ((wd_alp < 0)&&(wd_va < 0)&&(wd_adm < 0)) {
    RLOG_ERROR("aborting ActivityDetectorThread\n");
    close(fd);
    return;
  }

  // increment refcount for RME singleton users
  if (RME::getInstance() == NULL)
    return;

  struct timeval timeout;
  struct timeval * ptimeout = NULL;
  while (RMEimpl::mpMutexAsync && RMEimpl::mInstance->CountComponents()) {
    fd_set ddt_set;
    FD_ZERO(&ddt_set);
    FD_SET(fd, &ddt_set);

    const char* eventprocessalp = NULL;
    int retval = select(fd+1, &ddt_set, NULL, NULL, ptimeout);
    if (retval < 0) {
      if (errno != EINTR) {
        RLOG_ERROR("select failed (%d) for inotify dev -- aborting ActivityDetectorThread\n", errno);
        break;
      } // else keep going
    } else if (retval == 0) {
      // timeout occurred.. ALP update
      eventprocessalp = "TimeoutDetector";
    } else {
      // can read from inotify fd without blocking => read one event at a time
      struct inotify_event ievent;
      unsigned int length;

      length = read(fd, &ievent, sizeof(struct inotify_event));
      if (length < sizeof(struct inotify_event)) {
        RLOG_ERROR("read failed (%d) for inotify dev\n", errno);
      } else {
        if ((ievent.mask & IN_MODIFY)) {
          if (ievent.wd == wd_va) {
            // visual activity occurred.. ALP update
            eventprocessalp = "VisualDetector";
          } else if (ievent.wd == wd_adm) {
            // adm activity occurred.. ALP update
            eventprocessalp = "AdmDetector";
          }
        } else if ((ievent.mask & IN_ATTRIB) || (ievent.mask & IN_MOVE_SELF) || (ievent.mask & IN_DELETE_SELF)) {
          if (ievent.wd == wd_va) {
            RLOG_DEBUG("visual activity file modified or moved or deleted! fixing it\n");
            inotify_rm_watch(fd, wd_va);
            unlink(mFileNameUcVisual);
            CreateUcActivity(mFileNameUcVisual, mIsVisualActive, true);
            wd_va = inotify_add_watch(fd, mFileNameUcVisual, IN_MODIFY | IN_ATTRIB |  IN_MOVE_SELF | IN_DELETE_SELF);
            if (wd_va < 0) {
              RLOG_WARNING("inotify_add_watch failed (%d) on %s\n", errno, mFileNameUcVisual);
            }
          } else if (ievent.wd == wd_alp) {
            RLOG_DEBUG("alp activity file modified or moved or deleted! fixing it\n");
            inotify_rm_watch(fd, wd_alp);
            unlink(mFileNameUcAlp);
            CreateUcActivity(mFileNameUcAlp, mIsAlpActive, true);
            wd_alp = inotify_add_watch(fd, mFileNameUcAlp, IN_ATTRIB |  IN_MOVE_SELF | IN_DELETE_SELF);
            if (wd_alp < 0) {
              RLOG_WARNING("inotify_add_watch failed (%d) on %s\n", errno, mFileNameUcAlp);
            }
          } else if (ievent.wd == wd_adm) {
            RLOG_DEBUG("adm activity file modified or moved or deleted! fixing it\n");
            inotify_rm_watch(fd, wd_adm);
            unlink(mFileNameAdm);
            CreateUcActivity(mFileNameAdm, false, true);
            wd_adm = inotify_add_watch(fd, mFileNameAdm, IN_MODIFY | IN_ATTRIB |  IN_MOVE_SELF | IN_DELETE_SELF);
            if (wd_adm < 0) {
              RLOG_WARNING("inotify_add_watch failed (%d) on %s\n", errno, mFileNameAdm);
            }
          }
        }
      }
    }

    ptimeout = NULL;
    if (eventprocessalp) {
      int timeout_msec = ProcessALP(eventprocessalp, true);
      if (timeout_msec) {
        long tv_sec = 0;
        if (timeout_msec > 1000) {
          tv_sec = timeout_msec / 1000;
          timeout_msec -= tv_sec*1000;
        }
        timeout.tv_sec = tv_sec;
        timeout.tv_usec = 1000*timeout_msec;
        ptimeout = &timeout;
      }
    }
  }

  if (wd_va >= 0) {
    inotify_rm_watch(fd, wd_va);
  }
  if (wd_alp >= 0) {
    inotify_rm_watch(fd, wd_alp);
  }
  if (wd_adm >= 0) {
    inotify_rm_watch(fd, wd_adm);
  }
  close(fd);

  RME::delInstance();
  mActivityDetectorThreadCreated = false;
}

/// processes audio low power management; returns ALP timeout if any
unsigned int AGov::ProcessALP(const char* callerid, bool allow_timeout) {
  if (callerid == NULL)
    callerid = "n.a.";
  int timeout_msec = 0;
  // ALP update
  if (RMEimpl::MutexAsyncMsgLock()) {
    // in all case, make sure ALP file status is up to date
    CreateUcActivity(mFileNameUcAlp, mIsAlpActive, true);
    // process audio low power management
    if (mpResAlp) {
      // network cleanup needed in case synchronous unregister has been called
      // and main processing was not yet called
      ORPCompNetwork* networkM = SharedInst::GetORPCompNetwInst();
      // cleanup: remove any pending unregistered component
      networkM->RemoveUnregComponents();
      // cleanup: update halflines when needed
      // and especially remove unused halflines
      networkM->UpdateHalflines();
      // resort network (especialy for components in loaded state: clear grant date)
      networkM->SortComponents();
      timeout_msec = mpResAlp->ProcessAudioLowPower(callerid, allow_timeout);
    } else {
      RLOG_WARNING("no ALP process for %s\n", callerid);
    }
    RMEimpl::MutexAsyncMsgUnlock();
  }
  return timeout_msec;
}

/// checks if in 'visual' process
bool AGov::IsVisualProcess(void) const {
  bool ret = false;
  int file = open("/proc/self/comm", O_RDONLY);
  if (file < 0) {
    RLOG_WARNING("could not open /proc/self/comm (read)\n");
  } else {
    char val[12] = {'\0'};
    if (read(file, &val, 11) >= 1) {
      if (strncmp("admsrv", val, 6) == 0) {
        RLOG_DEBUG("process admsrv\n");
      } else if (strncmp("mediaserver", val, 11) == 0) {
        RLOG_DEBUG("process mediaserver\n");
        ret = true;
      }
    } else {
        RLOG_WARNING("/proc/self/comm read issue\n");
    }
    close(file);
  }
  return ret;
}

/// checks if there is SxA based ALP activity
bool AGov::IsActivityTrackerAlpOn(void) const {
  int value = 0;
  ReadUcActivity(mFileNameUcAlp, &value);
  return static_cast<bool>(value);
}

/// checks if there is SxA based visual activity
bool AGov::IsActivityTrackerVisualOn(void) const {
  int value = 0;
  ReadUcActivity(mFileNameUcVisual, &value);
  return static_cast<bool>(value);
}

/// checks if there is ADM based activity reported by tracker file
bool AGov::IsActivityTrackerAdmOn(void) const {
  int value = 0;
  ReadUcActivity(mFileNameAdm, &value);
  return static_cast<bool>(value);
}

/// checks if ALP use case governor is available or not
bool AGov::IsUcGovAlpAvailable(void) const {
  int file = open(mFileNameUcGovAlp, O_RDONLY);
  if (file < 0) {
    RLOG_INFO("could not open %s (read)\n", mFileNameUcGovAlp);
    return false;
  } else {
    close(file);
    return true;
  }
}

/// reacts to ALP enter requested (pending enter)
void AGov::UccReactAlpEnterRequested(void) {
  if (mIsAlpActive == false) {
    mIsAlpActive = true;
    RLOG_INFO("ALP activity tracker switched ON\n");
    WriteUcActivity(mFileNameUcAlp, mIsAlpActive, true);
  }
}

/// reacts to ALP enter applied signal
void AGov::UccReactAlpEnterApplied(void) {
  // burst mode enter OK, set low power audio CPU configuration
  WriteUcActivity(mFileNameUcGovAlp, true, false);
}

/// reacts to ALP exit requested signal
void AGov::UccReactAlpExitRequested(void) {
  // TODO(pht) could move WriteUcActivity(mFileNameUcGovAlp, false, false) here
}

/// reacts to ALP exit applied signal
void AGov::UccReactAlpExitApplied(void) {
  // burst mode exit OK, unset low power audio CPU configuration
  WriteUcActivity(mFileNameUcGovAlp, false, false);

  if (mIsAlpActive == true) {
    mIsAlpActive = false;
    RLOG_INFO("ALP activity tracker switched OFF\n");
    WriteUcActivity(mFileNameUcAlp, mIsAlpActive, true);
  }
}

/// reacts to component tunnel
void AGov::UccReactTunnel(const char* rolecompin, const char* rolecompout) {
  // if connecting the CsCall component we're setting up a voice call
  // load VC CPU configuration
  if (PfRoles::IsCsCallRole(rolecompin) || PfRoles::IsCsCallRole(rolecompout)) {
    if (!mIsUcgVoiceCall) {
      mIsUcgVoiceCall = true;
      WriteUcActivity(mFileNameUcGovVoiceCall, mIsUcgVoiceCall, false);
    }
  }
}

/// reacts to component unregister
void AGov::UccReactUnregister(const char* rolecomp) {
  // if unregistering speech processing or CsCall component when in a voice call, unload VC CPU configuration
  if (mIsUcgVoiceCall) {
    if (PfRoles::IsSpeechProcessingRole(rolecomp) || PfRoles::IsCsCallRole(rolecomp)) {
      mIsUcgVoiceCall = false;
      WriteUcActivity(mFileNameUcGovVoiceCall, mIsUcgVoiceCall, false);
    }
  }
}

/// reacts on visual activity detected *from network graph*
void AGov::UccReactVisualActivityOn(void) {
  if (mIsVisualActive == false) {
    mIsVisualActive = true;
    RLOG_INFO("visual activity tracker switched ON\n");
    WriteUcActivity(mFileNameUcVisual, mIsVisualActive, false);
  }
}

/// reacts on no visual activity detected *from network graph*
void AGov::UccReactVisualActivityOff(void) {
  if (mIsVisualActive == true) {
    mIsVisualActive = false;
    RLOG_INFO("visual activity tracker switched OFF\n");
    WriteUcActivity(mFileNameUcVisual, mIsVisualActive, false);
  }
}

/// trigs activity detector wakeup
/// hypothesis: ADM synchro patch not present!
void AGov::UccTrigsActivityDetector(void) {
  WriteUcActivity(mFileNameAdm, 0, false);
}

/// deletes activity files
void AGov::DeleteAllUcActivity() const {
  // unlink(mFileNameUcVisual);
  unlink(mFileNameAdm);
  unlink(mFileNameUcAlp);
}

/// creates activity file
void AGov::CreateUcActivity(const char* ucname, int initvalue, bool forceinit) const {
  if (!ucname) {
    return;
  }

  int file = open(ucname, O_RDONLY);
  if (file < 0) {
    file = open(ucname, O_RDWR | O_CREAT, 0666);
    if (file < 0) {
      RLOG_WARNING("could not create %s\n", ucname);
      return;
    } else {
      forceinit = true;
    }
  }

  if (file >= 0) {
    // explicitly changing the file mode to 0666
    // required in case global umask filtered out g+o rights: needed for root - non root communication
    fchmod(file, 0666);
    close(file);
    if (forceinit == true) {
      // write init value only in case of pure creation case or if requested
      WriteUcActivity(ucname, initvalue, false);
    }
  }
}

/// writes to activity file
void AGov::WriteUcActivity(const char* ucname, int value, bool allowcreate) const {
  if (!ucname) {
    return;
  }

  int file;
  if (allowcreate) {
    file = open(ucname, O_WRONLY | O_CREAT, 0666);
  } else {
    file = open(ucname, O_WRONLY);
  }
  if (file < 0) {
    RLOG_WARNING("could not open %s (write)\n", ucname);
  } else {
    char val[3] = {'\0'};
    sprintf(val, "%d", value);
    flock(file, LOCK_EX);
    if (write(file, &val, 2) < 1) {
      RLOG_WARNING("could not write to %s\n", ucname);
    }
    flock(file, LOCK_UN);
    close(file);
  }
}

/// reads from activity file
void AGov::ReadUcActivity(const char* ucname, int *pvalue) const {
  if ((!ucname)||(!pvalue)) {
    return;
  }

  *pvalue = 0;

  int file = open(ucname, O_RDONLY);
  if (file < 0) {
    RLOG_WARNING("could not open %s (read)\n", ucname);
  } else {
    char val[3] = {'\0'};
    flock(file, LOCK_EX);
    if (read(file, &val, 2) >= 1) {
      *pvalue = atoi(val);
    }
    flock(file, LOCK_UN);
    close(file);
  }
}

}  // namespace

