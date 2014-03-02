/******************************************************************************
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
******************************************************************************/

#ifndef _EVENT_H_
#define _EVENT_H_

enum {
    EVENT_GR_RECEIVED  = 0x00000001,
    EVENT_CMD_RECEIVED = 0x00000002,
    EVENT_SPEEDFLASH   = 0x00000004,
    EVENT_ERROR        = 0x00000008
};

struct Event {
    Event(uint32 ev, uint32 err, uint8 gr = 0, uint8 cmd = 0):
        event(ev), error(err), group(gr), command(cmd) {}
    uint32 event;
    uint32 error;
    uint8  group;
    uint8  command;
};

#endif // _EVENT_H_
