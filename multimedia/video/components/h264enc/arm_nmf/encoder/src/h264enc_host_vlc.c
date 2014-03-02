/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
//#include <string.h>
//#include <assert.h>

#include "types.h"

#include "h264enc_host_vlc.h"




/*!
 ************************************************************************
 * \brief
 *    writes UVLC code to the appropriate buffer
 ************************************************************************
 */
static void host_writeUVLC2buffer(CodElement *codel, Bitstream *currStream)
{
  t_sint32 i;
  t_uint32 mask = 1 << (codel->nbit - 1);

  /* Add the new bits to the bitstream. */
  /* Write out a byte if it is full */
  for (i = 0; i < codel->nbit; i++) {
    currStream->byte_buf <<= 1;
    if (codel->data & mask)
      currStream->byte_buf |= 1;
    currStream->bits_to_go--;
    mask >>= 1;
    if (currStream->bits_to_go == 0) {
      currStream->bits_to_go = 8;
      currStream->streamBuffer[currStream->byte_pos++] = (t_uint8) currStream->byte_buf;
      currStream->byte_buf = 0;
    }
  }
}


/*!
 ************************************************************************
 * \brief
 *    generates UVLC code and passes the codeword to the buffer
 ************************************************************************
 */
static t_sint8 host_writeSyntaxElement_UVLC(t_uint16 value, Bitstream *bitstream, t_sint32 sign)
{
#if TRACE
  SyntaxElement syntaxel, *se = &syntaxel;
#endif
  CodElement codelem, *codel = &codelem;

  if (sign == 0) host_ue_linfo(value, codel);
  else           host_se_linfo(value, codel);

  host_writeUVLC2buffer(codel, bitstream);

#if TRACE
  se->value1 = value;
  se->nbit = codel->nbit;
  snprintf(se->tracestring, TRACESTRING_SIZE, "");
  se->data = codel->data;
  trace2out(se);
#endif

  return(codel->nbit);
}


/*!
 ************************************************************************
 * \brief
 *    mapping for ue(v) syntax elements
 * \param ue
 *    value to be mapped
 * \param dummy
 *    dummy parameter
 * \param info
 *    returns mapped value
 * \param len
 *    returns mapped value length
 ************************************************************************
 */
void host_ue_linfo(t_uint16 ue, CodElement *sym)
{
  t_sint8  suffix_len;
  t_sint16 nn;
  t_sint16 tmp_info; /* info part of UVLC code */

  nn = (ue + 1) >> 1;
  for (suffix_len = 0; (nn != 0) && (suffix_len < 16); suffix_len++) {
    nn >>= 1;
  }

  tmp_info = ue + 1 - (1 << suffix_len);
  sym->nbit  = 2 * suffix_len + 1;
  sym->data = (1 << suffix_len) | (tmp_info & ((1 << suffix_len) - 1));
}

/*!
 *************************************************************************************
 * \brief
 *    ue_v, writes an ue(v) syntax element, returns the length in bits
 *
 * \param tracestring
 *    the string for the trace file
 * \param value
 *    the value to be coded
 *  \param part
 *    the Data Partition the value should be coded into
 *
 * \return
 *    Number of bits used by the coded syntax element
 *
 * \ note
 *    This function writes always the bit buffer for the progressive scan flag, and
 *    should not be used (or should be modified appropriately) for the interlace crap
 *    When used in the context of the Parameter Sets, this is obviously not a
 *    problem.
 *
 *************************************************************************************
 */
t_sint8 ue_v(t_sint32 value, Bitstream *bitstream)
{
  t_sint8 len;
  ASSERT (bitstream->streamBuffer != NULL);
  assert(value >= 0);
  len = host_writeSyntaxElement_UVLC(value, bitstream, 0);
  return(len);
}


/*!
 ************************************************************************
 * \brief
 *    mapping for se(v) syntax elements
 * \param se
 *    value to be mapped
 * \param dummy
 *    dummy parameter
 * \param len
 *    returns mapped value length
 * \param info
 *    returns mapped value
 ************************************************************************
 */
void host_se_linfo(t_sint16 se, CodElement *sym)
{
  t_sint8 suffix_len, n, sign;
  t_sint16 nn;
  t_sint16 tmp_info; /* info part of UVLC code */
  sign = 0;

  if (se <= 0) sign = 1;
  n = ABS(se) << 1;

  nn = n/2;
  for (suffix_len = 0; (nn != 0) && (suffix_len < 16); suffix_len++) {
    nn /= 2;
  }
  tmp_info = n - (1 << suffix_len) + sign;
  /* Makes code word and passes it back
     A code word has the following format: 0 0 0 ... 1 Xn ...X2 X1 X0.
     Info   : Xn..X2 X1 X0
     Length : Total number of bits in the codeword */
  sym->nbit = suffix_len * 2 + 1;
  sym->data = (1 << suffix_len) | (tmp_info & ((1 << suffix_len) - 1));
}


/*!
 *************************************************************************************
 * \brief
 *    se_v, writes an se(v) syntax element, returns the length in bits
 *
 * \param tracestring
 *    the string for the trace file
 * \param value
 *    the value to be coded
 *  \param part
 *    the Data Partition the value should be coded into
 *
 * \return
 *    Number of bits used by the coded syntax element
 *
 * \ note
 *    This function writes always the bit buffer for the progressive scan flag, and
 *    should not be used (or should be modified appropriately) for the interlace crap
 *    When used in the context of the Parameter Sets, this is obviously not a
 *    problem.
 *
 *************************************************************************************
 */
t_sint8 se_v(t_sint32 value, Bitstream *bitstream)
{
  t_sint8 len;
  ASSERT (bitstream->streamBuffer != NULL);
  len = host_writeSyntaxElement_UVLC(value, bitstream, 1);
  return(len);
}


/*!
 ************************************************************************
 * \brief
 *    passes the fixed codeword to the buffer
 ************************************************************************
 */
static t_sint8 host_writeSyntaxElement_fixed(CodElement *codel, Bitstream *bitstream)
{
#if TRACE
  SyntaxElement symbol, *se = &symbol;
#endif
  host_writeUVLC2buffer(codel, bitstream);

#if TRACE
  se->value1 = codel->data;
  se->nbit = codel->nbit;
  se->data = codel->data;
  trace2out(se);
#endif

  return(codel->nbit);
}


/*!
 *************************************************************************************
 * \brief
 *    u_1, writes a flag (u(1) syntax element, returns the length in bits,
 *    always 1
 *
 * \param tracestring
 *    the string for the trace file
 * \param value
 *    the value to be coded
 *  \param part
 *    the Data Partition the value should be coded into
 *
 * \return
 *    Number of bits used by the coded syntax element (always 1)
 *
 * \ note
 *    This function writes always the bit buffer for the progressive scan flag, and
 *    should not be used (or should be modified appropriately) for the interlace crap
 *    When used in the context of the Parameter Sets, this is obviously not a
 *    problem.
 *
 *************************************************************************************
 */
t_sint8 u_1(t_sint32 value, Bitstream *bitstream)
{
  CodElement codelem, *codel = &codelem;

  assert(value >= 0);
  codel->data = value;
  codel->nbit = 1;
  ASSERT (bitstream->streamBuffer != NULL);
  return host_writeSyntaxElement_fixed(codel, bitstream);
}

/*!
 *************************************************************************************
 * \brief
 *    u_v, writes a n bit fixed length syntax element, returns the length in bits,
 *
 * \param n
 *    length in bits
 * \param tracestring
 *    the string for the trace file
 * \param value
 *    the value to be coded
 *  \param part
 *    the Data Partition the value should be coded into
 *
 * \return
 *    Number of bits used by the coded syntax element
 *
 * \ note
 *    This function writes always the bit buffer for the progressive scan flag, and
 *    should not be used (or should be modified appropriately) for the interlace crap
 *    When used in the context of the Parameter Sets, this is obviously not a
 *    problem.
 *
 *************************************************************************************
 */
t_sint8 u_v(t_sint32 n, t_sint32 value, Bitstream *bitstream)
{
  CodElement codelem, *codel = &codelem;

  assert(value >= 0);
  codel->data = value;
  codel->nbit = n;
  ASSERT (bitstream->streamBuffer != NULL);
  return host_writeSyntaxElement_fixed(codel, bitstream);
}


/*!
 ************************************************************************
 * \brief
 *    generates UVLC code and passes the codeword to the buffer
 * \author
 *  Tian Dong
 ************************************************************************
 */
t_sint8 host_writeSyntaxElement2Buf_UVLC(CodElement *codel, Bitstream* this_streamBuffer)
{
#if TRACE
  SyntaxElement symbol, *se = &symbol;
  se->value1 = codel->data;
#endif
  /* FP: WARNING!!! always host_ue_linfo is assumed here !!! */
  host_ue_linfo(codel->data, codel);
  host_writeUVLC2buffer(codel, this_streamBuffer);

#if TRACE
  se->nbit = codel->nbit;
  se->data = codel->data;
  trace2out(se);
#endif

  return(codel->nbit);
}


/*!
 ************************************************************************
 * \brief
 *    generates UVLC code and passes the codeword to the buffer
 * \author
 *  Tian Dong
 ************************************************************************
 */
t_sint8 host_writeSyntaxElement2Buf_Fixed(CodElement *codel, Bitstream* this_streamBuffer)
{
#if TRACE
  SyntaxElement symbol, *se = &symbol;
#endif
  host_writeUVLC2buffer(codel, this_streamBuffer);

#if TRACE
  se->value1 = codel->data;
  se->nbit = codel->nbit;
  se->data = codel->data;
  trace2out(se);
#endif

  return(codel->nbit);
}
