/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides the log message
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef __INC_LOGMSG_H
#define __INC_LOGMSG_H

/*--------------------------------------------------------------------------*
 * C++                                                                       *
 *--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __DEBUG  
 
/* Exit Macros */

#define DBGEXIT0(cr)                                                                \
        ((DBGL_PUBLIC_FUNC_OUT & MY_DEBUG_LEVEL_VAR_NAME) != DBGL_OFF)?             \
        logMsg(MY_DEBUG_ID, DBGFUNCNAME, 0, "Exiting",0, 0, 0, 0, 0, 0, cr):        \
        (0)

#define DBGEXIT1(cr,ch,p1)                                                                                \
        ((DBGL_PUBLIC_FUNC_OUT & MY_DEBUG_LEVEL_VAR_NAME) != DBGL_OFF)?                                   \
        logMsg(MY_DEBUG_ID, DBGFUNCNAME, ch, "Exiting",(unsigned long)(p1), 0, 0, 0, 0, 0,cr):            \
        (0)        
        
#define DBGEXIT2(cr,ch,p1,p2)                                                                                               \
        ((DBGL_PUBLIC_FUNC_OUT & MY_DEBUG_LEVEL_VAR_NAME) != DBGL_OFF)?                                                     \
        logMsg(MY_DEBUG_ID, DBGFUNCNAME, ch, "Exiting",(unsigned long)(p1), (unsigned long)(p2), 0, 0, 0, 0,cr):            \
        (0)        
        
#define DBGEXIT3(cr,ch,p1,p2,p3)                                                                                                              \
        ((DBGL_PUBLIC_FUNC_OUT & MY_DEBUG_LEVEL_VAR_NAME) != DBGL_OFF)?                                                                       \
        logMsg(MY_DEBUG_ID, DBGFUNCNAME, ch, "Exiting",(unsigned long)(p1), (unsigned long)(p2), (unsigned long)(p3), 0, 0, 0,cr):            \
        (0)
        
#define DBGEXIT4(cr,ch,p1,p2,p3,p4)                                                                                                                  \
        ((DBGL_PUBLIC_FUNC_OUT & MY_DEBUG_LEVEL_VAR_NAME) != DBGL_OFF)?                                                                              \
        logMsg(MY_DEBUG_ID, DBGFUNCNAME, ch, "Exiting",(unsigned long)(p1), (unsigned long)(p2), (unsigned long)(p3), (unsigned long)(p4), 0, 0,cr): \
        (0)                    


#define DBGEXIT5(cr,ch,p1,p2,p3,p4,p5)                                                 \
        ((DBGL_PUBLIC_FUNC_OUT & MY_DEBUG_LEVEL_VAR_NAME) != DBGL_OFF)?                \
        logMsg(MY_DEBUG_ID, DBGFUNCNAME, ch, "Exiting",(unsigned long)(p1), (unsigned long)(p2), (unsigned long)(p3), (unsigned long)(p4), (unsigned long)(p5), 0,cr):                                                                              \
        (0)

#define DBGEXIT6(cr,ch,p1,p2,p3,p4,p5,p6)                                              \
        ((DBGL_PUBLIC_FUNC_OUT & MY_DEBUG_LEVEL_VAR_NAME) != DBGL_OFF)?                \
        logMsg(MY_DEBUG_ID, DBGFUNCNAME, ch, "Exiting",(unsigned long)(p1), (unsigned long)(p2), (unsigned long)(p3), (unsigned long)(p4), (unsigned long)(p5), (unsigned long)(p6),cr):                                                            \
        (0)            
        
/* Enter macro's */

#define DBGENTER0()                                                                    \
        ((DBGL_PUBLIC_FUNC_IN & MY_DEBUG_LEVEL_VAR_NAME) != DBGL_OFF)?                 \
        logMsg(MY_DEBUG_ID, DBGFUNCNAME, 0, "Entering Function",0, 0, 0, 0, 0, 0, 0):  \
        (0)        

#define DBGENTER1(ch,p1)                                                                                  \
        ((DBGL_PUBLIC_FUNC_IN & MY_DEBUG_LEVEL_VAR_NAME) != DBGL_OFF)?                                    \
        logMsg(MY_DEBUG_ID, DBGFUNCNAME, ch, "Entering Function",(unsigned long)(p1), 0, 0, 0, 0, 0,0):   \
        (0)
        
#define DBGENTER2(ch,p1,p2)                                                                                                 \
        ((DBGL_PUBLIC_FUNC_IN & MY_DEBUG_LEVEL_VAR_NAME) != DBGL_OFF)?                                                      \
        logMsg(MY_DEBUG_ID, DBGFUNCNAME, ch, "Entering Function",(unsigned long)(p1), (unsigned long)(p2), 0, 0, 0, 0, 0):  \
        (0)
        
#define DBGENTER3(ch,p1,p2,p3)                                                                                                               \
        ((DBGL_PUBLIC_FUNC_IN & MY_DEBUG_LEVEL_VAR_NAME) != DBGL_OFF)?                                                                       \
        logMsg(MY_DEBUG_ID, DBGFUNCNAME, ch, "Entering Function",(unsigned long)(p1), (unsigned long)(p2), (unsigned long)(p3), 0, 0, 0, 0): \
        (0)
        
#define DBGENTER4(ch,p1,p2,p3,p4)                                                      \
        ((DBGL_PUBLIC_FUNC_IN & MY_DEBUG_LEVEL_VAR_NAME) != DBGL_OFF)?                 \
        logMsg(MY_DEBUG_ID, DBGFUNCNAME, ch, "Entering Function",(unsigned long)(p1), (unsigned long)(p2), (unsigned long)(p3), (unsigned long)(p4), 0, 0, 0):\
        (0)    
        
#define DBGENTER5(ch,p1,p2,p3,p4,p5)                                                    \
        ((DBGL_PUBLIC_FUNC_IN & MY_DEBUG_LEVEL_VAR_NAME) != DBGL_OFF)?                  \
        logMsg(MY_DEBUG_ID, DBGFUNCNAME, ch, "Entering Function",(unsigned long)(p1), (unsigned long)(p2), (unsigned long)(p3), (unsigned long)(p4), (unsigned long)(p5), 0, 0):\
        (0)

#define DBGENTER6(ch,p1,p2,p3,p4,p5,p6)                                                 \
        ((DBGL_PUBLIC_FUNC_IN & MY_DEBUG_LEVEL_VAR_NAME) != DBGL_OFF)?                  \
        logMsg(MY_DEBUG_ID, DBGFUNCNAME, ch, "Entering Function",(unsigned long)(p1), (unsigned long)(p2), (unsigned long)(p3), (unsigned long)(p4), (unsigned long)(p5), (unsigned long)(p6), 0):\
        (0)                                            


#define DBGEXIT  DBGEXIT0
#define DBGENTER DBGENTER0

#define DBGPRINT(dbg_level,ch)                                            \
        ((dbg_level & MY_DEBUG_LEVEL_VAR_NAME) != DBGL_OFF)?              \
        logMsg(MY_DEBUG_ID, DBGFUNCNAME, ch, "",0, 0, 0, 0, 0, 0, 0):     \
        (0)
        
#define DBGPRINTHEX(dbg_level,ch, uint32)                                                    \
        ((dbg_level & MY_DEBUG_LEVEL_VAR_NAME) != DBGL_OFF)?                                 \
        logMsg(MY_DEBUG_ID, DBGFUNCNAME, ch, "",(unsigned long)uint32, 0, 0, 0, 0, 0, 0):    \
        (0)    

#define DBGPRINTDEC(dbg_level,ch, uint32)                                                    \
        ((dbg_level & MY_DEBUG_LEVEL_VAR_NAME) != DBGL_OFF)?                                 \
        logMsg(MY_DEBUG_ID, DBGFUNCNAME, ch, "",(unsigned long)uint32, 0, 0, 0, 0, 0, 0):    \
        (0)                

#endif /* __DEBUG */



/*--------------------------------------------------------------------------*
 * C++                                                                       *
 *--------------------------------------------------------------------------*/
#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */


#endif /* __INC_LOGMSG_H */

/* End of file - logmsg.h */
