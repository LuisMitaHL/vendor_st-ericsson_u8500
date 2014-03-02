#ifndef __STECAMFRAMEREPLAY_INL__
#define __STECAMFRAMEREPLAY_INL__

inline void CFrameReplay::process(void* pBuffer, int aLength, TDumpReplayType aReplayType) {
#ifdef CAM_DUMP_REPLAY_FRAME_BUFFERS
    replayBuffer(pBuffer, aLength, aReplayType);
#endif
}

inline void CFrameReplay::process(void* pBuffer, TDumpReplayType aReplayType) {
#ifdef CAM_DUMP_REPLAY_FRAME_BUFFERS
    replayBuffer(pBuffer, aReplayType);
#endif
}

inline bool CFrameReplay::start(TDumpReplayType aReplayType, int aWidth, int aHeight, TColorFmt aColorFormat,
                                STECamera* aCameraHal, int aAllocLen, MMHwBuffer* aMMHwBuffer) {
#ifdef CAM_DUMP_REPLAY_FRAME_BUFFERS
    return startReplay(aReplayType, aWidth, aHeight, aColorFormat, aCameraHal, aAllocLen, aMMHwBuffer);
#endif
}

inline void CFrameReplay::stop(TDumpReplayType aReplayType) {
#ifdef CAM_DUMP_REPLAY_FRAME_BUFFERS
    stopReplay(aReplayType);
#endif
}
#endif //__STECAMFRAMEREPLAY_INL__
