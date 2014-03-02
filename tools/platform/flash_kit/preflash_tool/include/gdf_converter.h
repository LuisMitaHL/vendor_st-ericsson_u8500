/*******************************************************************************
 * $Copyright ST-Ericsson 2010 $
 ******************************************************************************/
#ifndef _GDF_CONVERTER_H_
#define _GDF_CONVERTER_H_

#include "t_io_subsystem.h"

typedef struct {
    uint16 Index;
    uint16 Block;
    uint16 DataSize;
    uint8 Data[1];
} GDData_t;

typedef struct GDDataNode_ {
    GDData_t *GDData_p;
    struct GDDataNode_ *Next_p;
} GDDataNode_t;

ErrorCode_e GetGDFList(const IO_Handle_t IOHandle, GDDataNode_t **const Root_pp);

void DestroyGDFList(GDDataNode_t **const Root_pp);

#endif
