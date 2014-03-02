/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste3a_internal.h
 * \brief   STE 3A internal macros
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STE3A_INTERNAL_H_
#define STE3A_INTERNAL_H_

#ifdef STE3A_DEBUG

#define STE3A_TRACE(context, cb, level, text)	if ((cb) != NULL)(* (cb))((context), (level), (text));

#else

#define STE3A_TRACE(context, cb, level, text)

#endif

#ifdef _MSC_VER

#define snprintf _snprintf

#endif

#define DELTADUMP(algo, lside, rside, pretext, midtext, psttext, level, field, format) if (lside.field != rside.field) { snprintf(layout, layout_length, "\t\t%s%s%s%s%s%s", #field, pretext, format, midtext, format, psttext); snprintf(message, message_length, layout, lside.field, rside.field); STE3A_TRACE(algo->com_cbs.dbg.context, algo->com_cbs.dbg.ptr_dbg_trace, level, message); }
#define DELTADUMPC(algo, lside, rside, pretext, midtext, psttext, level, field, format, cast) if (lside.field != rside.field) { snprintf(layout, layout_length, "\t\t%s%s%s%s%s%s", #field, pretext, format, midtext, format, psttext); snprintf(message, message_length, layout, (cast) lside.field, (cast) rside.field); STE3A_TRACE(algo->com_cbs.dbg.context, algo->com_cbs.dbg.ptr_dbg_trace, level, message); }
#define DELTADUMPS(algo, lside, rside, pretext, midtext, psttext, level, field, format, start, count) { for (int i = start; i < (int) (start + count); i++) { if (lside.field [i] != rside.field [i]) { snprintf(layout, layout_length, "\t\t%s[%d]%s%s%s%s%s", #field, i, pretext, format, midtext, format, psttext); snprintf(message, message_length, layout, lside.field [i], rside.field [i]); STE3A_TRACE(algo->com_cbs.dbg.context, algo->com_cbs.dbg.ptr_dbg_trace, level, message); } } }

#endif /* STE3A_INTERNAL_H_ */
