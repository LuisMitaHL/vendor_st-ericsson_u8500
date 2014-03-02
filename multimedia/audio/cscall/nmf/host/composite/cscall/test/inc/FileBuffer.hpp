/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   FileBuffer.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __file_buffer_hpp_
#define __file_buffer_hpp_

#include <OMX_Core.h>
#include <assert.h>
#include "testapp.h"

class FileBuffer
{
public:
  FileBuffer(const char *filename, unsigned int input_block_size, bool loop);
  FileBuffer(unsigned int needed_size);
  ~FileBuffer(void);
  
  void write(OMX_BUFFERHEADERTYPE* buffer);
  void read(OMX_BUFFERHEADERTYPE* buffer);
  void save(const char *filename);
  inline bool is_eos(void){return eos_reached;}

private:
  unsigned int	 size;
  unsigned int	 size_written;
  unsigned int	 size_read;
  unsigned int	 size_to_read;
  bool		 loop;
  bool           eos_reached;
  char		*buffer_start;
  char          *buffer_end;
  char		*current_pos;
};

#endif // __file_buffer_hpp_
