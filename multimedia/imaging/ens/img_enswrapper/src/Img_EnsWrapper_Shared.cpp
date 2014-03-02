/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
#include "Img_EnsWrapper_Shared.h"
#if 0
// def _MSC_VER
//Missing implementation
//#include "mmhwbuffer.h"

IMG_ENSWRAPPER_API OMX_ERRORTYPE MMHwBuffer::GetIndexExtension(OMX_STRING /*aParameterName*/, OMX_INDEXTYPE* /*apIndexType*/) 
{
	return(OMX_ErrorNone);
}

OMX_ERRORTYPE MMHwBuffer::GetPortIndexExtension(enum OMX_INDEXTYPE,void *,unsigned long &)
{
	return(OMX_ErrorNone);
}

OMX_ERRORTYPE MMHwBuffer::Open(void *,enum OMX_INDEXTYPE,void *,class MMHwBuffer * &)
{
	return(OMX_ErrorNone);
}

IMG_ENSWRAPPER_API OMX_ERRORTYPE  MMHwBuffer::Destroy(class MMHwBuffer * &)
{
	return(OMX_ErrorNone);
}

IMG_ENSWRAPPER_API OMX_ERRORTYPE MMHwBuffer::Close(class MMHwBuffer * &)
{
	return(OMX_ErrorNone);
}

//LOs implementation
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

typedef size_t t_los_size;
typedef FILE t_los_file;
typedef int  t_los_origin;

extern "C"
{

void LOS_Log(const char *format, ...)
{
	va_list arg;
	va_start(arg, format);
	vprintf(format, arg);
	va_end(arg);
}

t_los_file* LOS_fopen ( const char * filename, const char * mode )
{
	return(fopen(filename, mode));
}


t_los_size LOS_fread ( void * ptr, t_los_size size, t_los_size count, t_los_file * stream )
{
	return(fread( ptr, size, count, stream));
}

int LOS_fseek( t_los_file * stream, long int offset, t_los_origin origin)
{
	return(fseek(stream, offset, origin));
}

long int    LOS_ftell ( t_los_file * stream )
{
	return(ftell(stream));
}

t_los_size LOS_fwrite( const void * ptr, t_los_size size, t_los_size count, t_los_file * stream )
{
	return(fwrite(ptr, size, count, stream ));
}

int LOS_fclose( t_los_file * stream )
{// 0 or EOF
	return (fclose(stream));
}

typedef void * t_los_memory_handle;
typedef void * t_los_logical_address;
typedef int    t_los_mapping_mode;


t_los_memory_handle LOS_Alloc(unsigned int sizeInBytes, unsigned int /*alignmentInBytes*/, t_los_mapping_mode /*mode*/)
{
	return(malloc(sizeInBytes));
}

void LOS_Free(t_los_memory_handle handle)
{
	free(handle);
}

t_los_logical_address LOS_GetLogicalAddress(t_los_memory_handle handle)
{
	return(handle);
}

}

#endif /* _IMG_ENSWRAPPER_SHARED_H_ */
