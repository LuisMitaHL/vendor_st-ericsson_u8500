/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   FileBuffer.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "FileBuffer.hpp"

// constructor for input buffer.=> Allocate a buffer and load the size inside
FileBuffer::FileBuffer(const char *filename, unsigned int input_block_size, bool loop_mode):
  size_written(0),
  size_read(0),
  size_to_read(input_block_size),
  loop(loop_mode),
  eos_reached(false)
{
  unsigned int	 readsize;
  TESTAPP_FILE	*file;
  
  file = TESTAPP_FOPEN(filename, "rb");
  
  if (file == NULL)
  {
    TESTAPP_PRINTF("Unable to open input file %s\n", filename);
    ARMNMF_DBC_ASSERT(0);
  }
  
  TESTAPP_FSEEK(file, 0, TESTAPP_SEEK_END);
  size = TESTAPP_FTELL(file);
  TESTAPP_FSEEK(file, 0, TESTAPP_SEEK_SET);
  
  buffer_start = (char *)TESTAPP_MALLOC(size);
  if(buffer_start == NULL)
  {
    TESTAPP_PRINTF("Unable to allocate input buffer for file %s\n", filename);
    ARMNMF_DBC_ASSERT(0);
  }
  buffer_end = buffer_start + size;
  current_pos = buffer_start;
  
  readsize = TESTAPP_FREAD(buffer_start, 1, size, file);
  if(readsize != size)
  {
    TESTAPP_PRINTF("** Unable to read input file %s\n", filename);
    ARMNMF_DBC_ASSERT(0);
  }
  
  TESTAPP_FCLOSE(file);
}


// Constructor for output buffer => Allocate buffer
FileBuffer::FileBuffer(unsigned int needed_size):
  size(needed_size),
  size_written(0),
  size_read(0),
  size_to_read(0),
  loop(true),
  eos_reached(false)
{
  buffer_start = (char *)TESTAPP_MALLOC(size);
  if(buffer_start == NULL)
  {
    TESTAPP_PRINTF("Unable to allocate output buffer\n");
    ARMNMF_DBC_ASSERT(0);
  }
  buffer_end = buffer_start + size;
  current_pos = buffer_start;
  
}


// Destructor
FileBuffer::~FileBuffer(void)
{
  TESTAPP_FREE(buffer_start);
}


// write
void FileBuffer::write(OMX_BUFFERHEADERTYPE *omx_buffer)
{
  unsigned int	 i	 = omx_buffer->nFilledLen;
  char		*src_ptr = (char *)omx_buffer->pBuffer;
  
  src_ptr += omx_buffer->nOffset;
  
  while(((unsigned int)current_pos + i) > ((unsigned int)buffer_end))
  {
    while((unsigned int)current_pos < ((unsigned int)buffer_end))
    {
      i--;
      *current_pos++ = *src_ptr++;
    }
    current_pos = buffer_start;
  }
  
  for(;i>0;i--)
  {
    *current_pos++ = *src_ptr++;
  }
  
  size_written += omx_buffer->nFilledLen;
}



// read
void FileBuffer::read(OMX_BUFFERHEADERTYPE *omx_buffer)
{
  char *dst_ptr = (char *)omx_buffer->pBuffer;

  omx_buffer->nFlags  = 0;
  omx_buffer->nOffset = 0;  
  
  if(loop)
  {
    int i = size_to_read;

    while(((unsigned int)current_pos + i) > ((unsigned int)buffer_end))
    {
      while((unsigned int)current_pos < ((unsigned int)buffer_end))
      {
	i--;
	*dst_ptr++ = *current_pos++;
      }

      current_pos = buffer_start;
    }

    for(;i>0;i--)
    {
      *dst_ptr++ = *current_pos++;
    }
  }
  else
  {
    int nb_copy = size_to_read;
    int i = 0;
    
    if(((unsigned int)current_pos + size_to_read) >= ((unsigned int)buffer_end))
    {
      nb_copy = ((unsigned int)buffer_end) - (unsigned int)current_pos;
      omx_buffer->nFlags = OMX_BUFFERFLAG_EOS;
      eos_reached = true;
    }

    for(i=0;i<nb_copy;i++)
    {
      *dst_ptr++ = *current_pos++;
    }
    
    for(;i<size_to_read;i++)
    {
      *dst_ptr++ = 0;
    }
  }

  size_read += size_to_read;
  omx_buffer->nFilledLen = size_to_read;
}


//save buffer to file
void FileBuffer::save(const char *filename)
{
  unsigned int size_to_save = size;
  unsigned int size_saved;
  TESTAPP_FILE	*file;
  file = TESTAPP_FOPEN(filename, "wb");
  
  if (file == NULL)
  {
    TESTAPP_PRINTF("Unable to open output file %s\n", filename);
    ARMNMF_DBC_ASSERT(0);
  }

  
  if(size_written <= size)
  {
    size_to_save = size_written;
    size_saved	 = TESTAPP_FWRITE(buffer_start, 1, size_written, file);
  }
  else
  {
    size_saved =  TESTAPP_FWRITE(current_pos, 1, ((unsigned int)buffer_end - (unsigned int)current_pos), file);
    size_saved += TESTAPP_FWRITE(buffer_start, 1, ((unsigned int)current_pos - (unsigned int)buffer_start), file);
  }
  
  if(size_saved != size_to_save)
  {
    TESTAPP_PRINTF("** Unable to write output file %s\n",filename);
    ARMNMF_DBC_ASSERT(0);
  }
  
  TESTAPP_FCLOSE(file);
}
