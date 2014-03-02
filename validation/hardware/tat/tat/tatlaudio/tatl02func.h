/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides routines to manage ab dth functions
* \author  ST-Ericsson
*/
/*****************************************************************************/

#if !defined(TATL02FUNC_H)
#define TATL02FUNC_H

#define PLAYBACK_UPLOAD_FILE    (TAT_PATH_VAR"/tat/tatlaudio/playback.wav")
#define PLAYBACK_FINAL_FILE     (TAT_PATH_VAR"/tat/tatlaudio/playbackS32.wav")
#define RECORD_FILE     (TAT_PATH_VAR"/tat/tatlaudio/record.wav")

int DthAudio_01SimpleGet(struct dth_element *elem, void *value);
int DthAudio_02SimpleSet(struct dth_element *elem, void *value);
int DthAudio_03SimpleExec(struct dth_element *elem);

#define DTHAUDIO_NONE           0xFFFFFFFF


Dthaudio_path  a_dthaudio_Srclistpath[NB_MAX]
#if defined TATL02_C
= {
    "/DTH/AUDIO/Tests/Record/Action/Source"
    , "/DTH/AUDIO/Tests/AudioLoop/APDigitalAudioLoop/Source"
    , "/DTH/AUDIO/Tests/AudioLoop/APDigitalAudioLoop/Source"
    , "/DTH/AUDIO/Tests/AudioLoop/ModemDigitalAudioLoop/Source"
    , ""
}
#endif
;

Dthaudio_path  a_dthaudio_Sinklistpath[NB_MAX]
#if defined TATL02_C
= {
    "/DTH/AUDIO/Tests/Playback/Action/Sink"
    , "/DTH/AUDIO/Tests/AudioLoop/ABDigitalAudioLoop/Sink"
    , "/DTH/AUDIO/Tests/AudioLoop/APDigitalAudioLoop/Sink"
    , "/DTH/AUDIO/Tests/AudioLoop/ModemDigitalAudioLoop/Sink"
    , ""
}
#endif
;


#endif /* !defined(TATL02FUNC_H) */

