/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include<stdio.h>
#include<string.h>
#ifdef __JPEGDEC_SOFTWARE
	#include <jpegdec/arm_nmf/parser_arm.nmf>

#elif __JPEGDEC_DUAL
	#include <jpegdec/arm_nmf/parser_dual.nmf>
#else
	#include <jpegdec/arm_nmf/parser_mpc.nmf>
#endif
#ifdef __PERF_MEASUREMENT
#include "../../../proxy/test_arm/osi_perf.h"
#endif

void JPEGDec_ARMNMF_Parser_CLASS::copy_hufftable(t_uint16 p_huffbits[], t_uint16 p_huffval[], t_uint16 p_bits[],t_uint16 p_val[], signed short is_dc)
{    t_uint16 val,last_val;
    if (is_dc) last_val = DC_HUFFVAL_TABLE_SIZE;
    else last_val = AC_HUFFVAL_TABLE_SIZE;

    for (val = 0;val< HUFF_BITS_TABLE_SIZE;val++)
    {
       p_huffbits[val] = p_bits[val + 1];
    }

    for (val = 0;val<last_val;val++)
    {
       p_huffval[val] = p_val[val];
    }

}
JPEGDec_ARMNMF_Parser_CLASS::JPEGDec_ARMNMF_Parser_CLASS()
{
	isInitialized = OMX_FALSE;
	jpegInfo.restartInterval=0;
	totalNumberOfScans =0;
	sizeInBitsRead=0;
	firstItem = NULL;
	firstBtstBuffer = NULL;
	traverseEncodedData=OMX_FALSE;
	pJpegNmfComp = 0;
	pParamAndConfig = 0;
	processMarker = OMX_TRUE;

	processingState.huffTableNumber = -1;
	processingState.lastScan = -1;
	processingState.lastMarker = 0;
	processingState.lastMarkerDataLength = -1;
	processingState.qpTableNumber = -1;
	processingState.scanNumber = -1;
	processingState.headerOrBtst=0;

	memset(&jpegInfo,0,sizeof(ts_jpgInfo));
	// Initilaize Free links Lists
	for(int i=0;i<MAX_BITSTREAM_BUFFER_LINKS;i++)
	{
	    ENS_ListItem_p item;
	    item=freeLinks.pushBack((void*)&bitstreamBuffer[i]);
	    if(!item) return;
	}
}

JPEGDec_ARMNMF_Parser_CLASS::~JPEGDec_ARMNMF_Parser_CLASS()
{
}


void JPEGDec_ARMNMF_Parser_CLASS::initializeReferences(JPEGDEC_CLASS *pComp,JPEGDec_ARMNMF_ParamAndConfig_CLASS *pPC)
{
	pJpegNmfComp=pComp;
	pParamAndConfig=pPC;
}
#if defined (__JPEGDEC_SOFTWARE)||(__JPEGDEC_DUAL)
void JPEGDec_ARMNMF_Parser_CLASS::jpeg_make_d_derived_tbl_hdr (ts_jpgDCHuffTable *dc_tables,ts_jpgACHuffTable *ac_tables,signed short isDC)
{
  t_uint16* p_bits;
  t_uint16* p_huffval;
  t_sint32* p_maxcode;
  t_sint32* p_valoffset;
  t_sint16 * p_look_bits;
  t_uint16* p_look_sym;
  t_uint16 last_val;
  t_sint16 p, i, l,a, si, numsymbols;
  t_sint16 lookbits, ctr;
  t_sint8 huffsize[257];
  t_uint16 huffcode[257];
  t_uint16 code;



  /* Note that huffsize[] and huffcode[] are filled in code-length order,
   * paralleling the order of the symbols themselves in htbl->huffval[].
   */

  /* Find the input Huffman table */


  if(isDC)
  	{
	p_bits = dc_tables->bits;
	p_huffval = dc_tables->val;
	p_maxcode=dc_tables->maxcode;
	p_valoffset = dc_tables->valoffset;
	p_look_bits = dc_tables->look_nbits;
	p_look_sym = dc_tables->look_sym;
    last_val = 12;

  	}
  else
  	{
	p_bits = ac_tables->bits;
	p_huffval = ac_tables->val;
	p_maxcode=ac_tables->maxcode;
	p_valoffset = ac_tables->valoffset;
	p_look_bits = ac_tables->look_nbits;
	p_look_sym = ac_tables->look_sym;
	last_val = 16;
  	}


  /* Figure C.1: make table of Huffman code length for each symbol */

  p = 0;
  for (l = 0,a=1; l <last_val; l++,a++) {
    i = (t_uint16) p_bits[l];
    if (i < 0 || p + i > 256)	/* protect against table overrun */
    	{
      printf("wrong value for bits");
    	}
    while (i--)
      huffsize[p++] = (t_sint8) a;
  }
  huffsize[p] = 0;
  numsymbols = p;

  /* Figure C.2: generate the codes themselves */
  /* We also validate that the counts represent a legal Huffman code tree. */

  code = 0;
  si = huffsize[0];
  p = 0;
  while (huffsize[p]) {
    while (((t_sint16) huffsize[p]) == si) {
      huffcode[p++] = code;
      code++;
    }
    /* code is now 1 more than the last code used for codelength si; but
     * it must still fit in si bits, since no code is allowed to be all ones.
     */
    if (((t_sint32) code) >= (((t_sint32) 1) << si)){
      printf("wrong value for code\n");
    	}
    code <<= 1;
    si++;
  }

  /* Figure F.15: generate decoding tables for bit-sequential decoding */
  p_maxcode[0] = -1;
  p = 0;
  for (l = 1; l <=last_val; l++)
  {
    if (p_bits[l-1])
    {
      /* valoffset[l] = huffval[] index of 1st symbol of code length l,
       * minus the minimum code of length l
       */
      p_valoffset[l] = (t_sint32) p - (t_sint32) huffcode[p];
      p += p_bits[l-1];
      p_maxcode[l] = huffcode[p-1]; /* maximum code of length l */
    } else
    {
      p_maxcode[l] = -1;	/* -1 if no codes of this length */
    }
  }
  p_maxcode[l] = 0xFFFFFL; /* ensures jpeg_huff_decode terminates */

  /* Compute lookahead tables to speed up decoding.
   * First we set all the table entries to 0, indicating "too long";
   * then we iterate through the Huffman codes that are short enough and
   * fill in all the entries that correspond to bit sequences starting
   * with that code.
   */

  memset(p_look_bits, 0, sizeof(t_sint16)*256);

  p = 0;
  for (a=0,l = 1; l <= HUFF_LOOKAHEAD; a++,l++) {
    for (i = 1; i <= p_bits[a]; i++, p++) {
      /* l = current code's length, p = its index in huffcode[] & huffval[]. */
      /* Generate left-justified code followed by all possible bit sequences */
      lookbits = huffcode[p] << (HUFF_LOOKAHEAD-l);
      for (ctr = 1 << (HUFF_LOOKAHEAD-l); ctr > 0; ctr--) {
	p_look_bits[lookbits] = l;
	p_look_sym[lookbits] = p_huffval[p];
	lookbits++;
      }
    }
  }

  /* Validate symbols as being reasonable.
   * For AC tables, we make no check, but accept all byte values 0..255.
   * For DC tables, we require the symbols to be in range 0..15.
   * (Tighter bounds could be applied depending on the data depth and mode,
   * but this is sufficient to ensure safe decoding.)
   */
  if (isDC) {
    for (i = 0; i < numsymbols; i++)
    {
      t_sint16 sym = p_huffval[i];
      if (sym < 0 || sym > 15)
      	{
	 printf("wrong value for code\n");
      	}
    }
            }
}
#endif
/****************************************************************************/
/**
 * \brief 	compressed huffman table (BITS,HUFFVAL) expansion
 * \author 	jean-marc volle
 * \param 	p_bits[] BITS list
 * \param 	p_val[] HUFFVAL list
 * \param 	p_code[] EHUFCO list
 * \param 	p_size[] EHUFSI list
 *
 *  This function generate a hufman table (EHUFCO, EHUFSI) according to
 *  CCITT_Rec_T81 Annex C.
 **/
/****************************************************************************/
void JPEGDec_ARMNMF_Parser_CLASS::expandHuffmanTable(OMX_U16 pBits[],OMX_U16 pVal[], OMX_U16 pCode[], OMX_U16 pSize[])
{
    OMX_U16 k,i,j;
    OMX_U16 lastK;
    OMX_U16 huffCode[256];
    OMX_U16 huffSize[256];
    OMX_U32 code;
    OMX_U16 size;

    /* we reset the tables */
    memset (huffCode,0x0,256*sizeof(OMX_U16));
    memset (huffSize,0x0,256*sizeof(OMX_U16));


    /* generation of huffman code size (CCITT_Rec_T81 figure C.1) */
    /* ---------------------------------------------------------- */

    k=0;i=1;j=1;

startLoopC1:
    if (j > pBits[i])
    {
        i = i + 1;
        j = 1;
        if (i>16) goto endLoopC1;
        else goto startLoopC1;
    }
    else
    {
        huffSize[k] = i;
        k = k +1;
        j = j + 1;
        goto startLoopC1;
    }

endLoopC1:

    huffSize[k] = 0;
    lastK = k;

    /* generation of huffman code table (CCITT_Rec_T81 figure C.2) */
    /* ----------------------------------------------------------- */

    k = 0;
    code = 0;
    size = huffSize[0];
startLoopC2:
    huffCode[k] = code;
    code = code + 1;
    k = k + 1;
    if (huffSize[k] == size) goto startLoopC2;
    else
    {
        if (huffSize[k]==0) goto endLoopC2;
        else goto startCodeLoop;
    }

startCodeLoop:
    code = code << 1;
    size = size +1;
    if (huffSize[k] == size) goto startLoopC2;
    else goto startCodeLoop;

endLoopC2:



    /* reordering of codes  */
    /* -------------------- */
    k = 0;
    do
    {
        i = pVal[k];
        pCode[i] = (OMX_U16) huffCode[k];
        pSize[i] = huffSize[k];
        k++;

    } while (k < lastK);

}

/****************************************************************************/
/**
 * \brief 	advance to next marker
 * \author 	jean-marc volle
 * \param 	pBitstream pointer on the bitstream struct
 * \return      btst error code
 * \note        We OR bitstream state as much much as possible to avoid too
 *              many return cases. Ignore restart marker.
 *
 *  remove any 1 bits until byte alignement and remove any 0xff bytes before the
 *  next marker.
 *
 **/
/****************************************************************************/
t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::btparAdvanceToNextMarker()
{
    t_djpegErrorCode state=DJPEG_NO_ERROR;
    OMX_U32 bits;

    /* advance to next byte */
    if (!btstIsByteAligned()) state = btstAlignByte();
    if (state) return state;

    /*advance until next marker */
    state = btstShowbits(16,&bits);
    if (state) return state;

    while  ( !( ((bits & 0xff00) == 0xff00) && (((bits & 0xff) != 0x00)) && ((bits & 0xff)!= 0xff) && ((bits & 0xfff8) != 0xffD0)))
    {
        state = btstFlushbits(8);
        if (state) return state;
        state = btstShowbits(16,&bits);
        if (state) return state;
    }
    return state;

} /* end of advance_to_next_marker */

/****************************************************************************/
/**
 * \brief 	Restart interval (DRI) definition reading funtion
 * \author 	jean-marc volle
 * \param 	pBitstream: pointer on bitstream struct
 * \param 	p_restart: pointer on resulting restart value
 * \return 	 bitstream reading error code
 *
 * This function reads a DRI marker segment and extracts the restart interval
 * value (Ri). See CCITT_Rec_T81 B2.4.4
 **/
/****************************************************************************/

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::readRestartIntervalDefinition(OMX_U16 *pRestart)
{
    t_djpegErrorCode state;
    OMX_U32 bits;

    /* we skip Lr */
    state = btstFlushbits(16);
    if (state) return state;

    /* we read Ri */
    state = btstGetbits(16,&bits);
    *pRestart = (OMX_U16)bits;
    return state;
}

/****************************************************************************/
/**
 * \brief 	huffman table (DHT) reading function.
 * \author 	jean-marc volle
 * \param 	pBitstream :pointer on bitstream struct
 * \param       p_bits[]: BITS list to update
 * \param       p_val[]: HUFFVAL list to update
 * \param       p_tc:  tc read value (table classe tc =0/1)
 * \param       p_th: th read value: (table id)
 * \return 	bitstream reading error code
 *
 * This function reads a compressed table definition after a DHT marker
 * according to CCITT_Rec_T81 B2.4.2 and stores it at index Th in p_comp_tables
 * list
 **/
/****************************************************************************/

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::readDhtTable(OMX_U16 pBits[], OMX_U16 pVal[], OMX_U32 *pTc, OMX_U32 *pTh)
{
    t_djpegErrorCode state;
    OMX_U32 bits;
    OMX_U16 cnt;
    OMX_U32 sum; /* number of codes */

    /* we read Tc */
    state = btstGetbits(4,pTc);
    if (state) return state;

    /* we read Th */
    state = btstGetbits(4,pTh);
    if (state) return state;

    /* we read the Li */
    sum = 0;
    for (cnt=1; cnt<=16;cnt++){
        state = btstGetbits(8,&bits);
        if (state) return state;

        pBits[cnt] = bits;
        sum += bits;
    }

       #ifdef __PERF_MEASUREMENT
       write_ticks_to_buffer("\nEnter readDhtTable loop");
       #endif


    /* we can now read the Vij */
 /*  for (cnt=0;cnt<sum;cnt++){
        state = btstGetbits(8,&bits);
        if (state) return state;
        pVal[cnt] = bits;
    }*/


        for (cnt=0;cnt<sum/4;cnt++){
            state = btstGetbits(32,&bits);
             if (state) return state;

             pVal[cnt*4+3] = (0xFF&bits);
             pVal[cnt*4+2] = 0xFF&(bits>>=8);
             pVal[cnt*4+1] = 0xFF&(bits>>=8);
             pVal[cnt*4] = 0xFF&(bits>>=8);
       }

       if(sum%4) {
           state = btstGetbits((sum%4)*8,&bits);
           if (state) return state;
           for(int i =sum%4 ;i>0;i--) {
               pVal[cnt*4+i-1] = (0xFF&bits);
               bits>>=8;
           }

       }

       #ifdef __PERF_MEASUREMENT
       write_ticks_to_buffer("\nEnter readDhtTable loop");
       #endif


    return state;
} /*  end of read_huffman_table */

/****************************************************************************/
/**
 * \brief 	read huffman tables after a DHT marker
 * \author 	jean-marc volle
 * \param 	pBitstream: pointer on bitstream buffer
 * \param 	dc_tables[]: list of dc table to update
 * \param 	ac_tables[]: list of ac table to update
 * \return 	bitstream error code
 * \note        There can be several tables after a DHT marker
 *
 *
 **/
/****************************************************************************/

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::readHuffmanTable(ts_jpgDCHuffTable dcTables[],
                                                         ts_jpgACHuffTable acTables[])//ts_jpg_dc_huff_table
{
    #ifdef __JPEGDEC_SOFTWARE
    t_djpegErrorCode state;
    OMX_U32 tc,th,lh;
    OMX_U16 bits[256];
    OMX_U16 val[256];
    OMX_U16 *p_bits; //t_uint16 *p_bits;
    OMX_U16 *p_val; //t_uint16 *p_val;


    /* indexes of the first and last bit read */
    OMX_U32 startBit;
    OMX_U32 endBit;
    OMX_U32 readLength;

    //startBit = btstGetBitIndex(pBitstream);
	startBit = sizeInBitsRead;
    /* we read lh to store the length of the header */
    state = btstGetbits(16,&lh);
    if (state) return state;
    do{
       #ifdef __PERF_MEASUREMENT
       write_ticks_to_buffer("\nEnter readDhtTable");
       #endif

        /* we read the dht table */
        state = readDhtTable(bits,val,&tc,&th);
        if (state) return state;
           #ifdef __PERF_MEASUREMENT
           write_ticks_to_buffer("\nExit readDhtTable");
           #endif

        if (th>NB_OF_HUFF_TABLES-1) return DJPEG_BAD_TABLE_INDEX; /* we can only store 2 tables */
           #ifdef __PERF_MEASUREMENT
           write_ticks_to_buffer("\nEnter Copy table");
           #endif

        /* we select the good component table */
        if (tc == 0){
            /*dc table */
            p_bits = dcTables[th].bits;
            p_val = dcTables[th].val;

            memset (p_bits,0x0,HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
            memset (p_val,0x0,DC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
        }
        else{
            p_bits = acTables[th].bits;
            p_val = acTables[th].val;

            memset (p_bits,0x0,HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
            memset (p_val,0x0,AC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
        }
           #ifdef __PERF_MEASUREMENT
           write_ticks_to_buffer("\nExit Copy table");
           #endif


        /* we expand the table */
        copy_hufftable(p_bits,p_val,bits,val,!tc);
        jpeg_make_d_derived_tbl_hdr(&(dcTables[th]),&(acTables[th]),!tc);

           #ifdef __PERF_MEASUREMENT
           write_ticks_to_buffer("\nExit expandHuffmanTable");
           #endif


        //endBit = btstGetBitIndex(pBitstream);
		endBit = sizeInBitsRead;
        readLength = ( endBit - startBit)/8;
    } while (readLength < lh);

    return state;
 #elif __JPEGDEC_DUAL
 t_djpegErrorCode state;
    OMX_U32 tc,th,lh;
    OMX_U16 bits[256];
    OMX_U16 val[256];
    OMX_U16 *p_bits; //t_uint16 *p_bits;
    OMX_U16 *p_val; //t_uint16 *p_val;

    OMX_U16 *pSize;
    OMX_U16 *pCode;

    /* indexes of the first and last bit read */
    OMX_U32 startBit;
    OMX_U32 endBit;
    OMX_U32 readLength;

    //startBit = btstGetBitIndex(pBitstream);
	startBit = sizeInBitsRead;
    /* we read lh to store the length of the header */
    state = btstGetbits(16,&lh);
    if (state) return state;
    do{
       #ifdef __PERF_MEASUREMENT
       write_ticks_to_buffer("\nEnter readDhtTable");
       #endif

        /* we read the dht table */
        state = readDhtTable(bits,val,&tc,&th);
        if (state) return state;
           #ifdef __PERF_MEASUREMENT
           write_ticks_to_buffer("\nExit readDhtTable");
           #endif

        if (th>NB_OF_HUFF_TABLES-1) return DJPEG_BAD_TABLE_INDEX; /* we can only store 2 tables */
           #ifdef __PERF_MEASUREMENT
           write_ticks_to_buffer("\nEnter Copy table");
           #endif

        /* we select the good component table */
        if (tc == 0){
            /*dc table */
	    pSize = dcTables[th].size;
	    pCode = dcTables[th].code;
        p_bits = dcTables[th].bits;
        p_val = dcTables[th].val;

            memset (p_bits,0x0,HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
            memset (p_val,0x0,DC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
            memset (pCode,0x0,DC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
            memset (pSize,0x0,DC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
        }
        else{
            p_bits = acTables[th].bits;
            p_val = acTables[th].val;
            pSize = acTables[th].size;
            pCode = acTables[th].code;

            memset (p_bits,0x0,HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
            memset (p_val,0x0,AC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
            memset (pCode,0x0,AC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
            memset (pSize,0x0,AC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
        }
           #ifdef __PERF_MEASUREMENT
           write_ticks_to_buffer("\nExit Copy table");
           #endif


        /* we expand the table */
        copy_hufftable(p_bits,p_val,bits,val,!tc);
        jpeg_make_d_derived_tbl_hdr(&(dcTables[th]),&(acTables[th]),!tc);
	/* we expand the table */
        expandHuffmanTable(bits,val,pCode,pSize);

           #ifdef __PERF_MEASUREMENT
           write_ticks_to_buffer("\nExit expandHuffmanTable");
           #endif


        //endBit = btstGetBitIndex(pBitstream);
		endBit = sizeInBitsRead;
        readLength = ( endBit - startBit)/8;
    } while (readLength < lh);

    return state;
#else
    t_djpegErrorCode state;
    OMX_U32 tc,th,lh;
    OMX_U16 bits[256];
    OMX_U16 val[256];

    OMX_U16 *pSize;
    OMX_U16 *pCode;

    /* indexes of the first and last bit read */
    OMX_U32 startBit;
    OMX_U32 endBit;
    OMX_U32 readLength;

    //startBit = btstGetBitIndex(pBitstream);
	startBit = sizeInBitsRead;
    /* we read lh to store the length of the header */
    state = btstGetbits(16,&lh);
    if (state) return state;

    do{
       #ifdef __PERF_MEASUREMENT
       write_ticks_to_buffer("\nEnter readDhtTable");
       #endif

        /* we read the dht table */
        state = readDhtTable(bits,val,&tc,&th);
        if (state) return state;
           #ifdef __PERF_MEASUREMENT
           write_ticks_to_buffer("\nExit readDhtTable");
           #endif

        if (th>NB_OF_HUFF_TABLES-1) return DJPEG_BAD_TABLE_INDEX; /* we can only store 2 tables */
           #ifdef __PERF_MEASUREMENT
           write_ticks_to_buffer("\nEnter Copy table");
           #endif

        /* we select the good component table */
        if (tc == 0){
            /*dc table */
            pSize = dcTables[th].size;
            pCode = dcTables[th].code;

            memset (pCode,0x0,DC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
            memset (pSize,0x0,DC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
        }
        else{
            pSize = acTables[th].size;
            pCode = acTables[th].code;

            memset (pCode,0x0,AC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
            memset (pSize,0x0,AC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
        }
           #ifdef __PERF_MEASUREMENT
           write_ticks_to_buffer("\nExit Copy table");
           #endif


        /* we expand the table */
        expandHuffmanTable(bits,val,pCode,pSize);

           #ifdef __PERF_MEASUREMENT
           write_ticks_to_buffer("\nExit expandHuffmanTable");
           #endif


        //endBit = btstGetBitIndex(pBitstream);
		endBit = sizeInBitsRead;
        readLength = ( endBit - startBit)/8;
    } while (readLength < lh);

    return state;
#endif
}

/****************************************************************************/
/**
 * \brief 	 quantization table reading function
 * \author 	jean-marc volle
 * \param 	pBitstream: pointer on bitstream struct
 * \param 	p_quant_tables list of quantization tables
 * \param 	p_tq: id of the updated quantization table
 * \note        Only 8 bits quantization table supported
 * \return 	BTPAR|BTST error code
 *
 *  This function reads a jpeg quantization table. See CCITT_Rec_T81 B.2.4.1
 **/
/****************************************************************************/
t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::readQuantizationTable(ts_quantTable pQuantTables[],OMX_U16 *pTq)
{
    t_djpegErrorCode state;
    OMX_U16 cnt;
    OMX_U32 bits;

    /* indexes of the first and last bit read */
    OMX_U32 startBit;
    OMX_U32 endBit;
    OMX_U32 readLength;
    OMX_U32 lq;

    //startBit = btstGetBitIndex(pBitstream);
	startBit= sizeInBitsRead;

    /* we read lq to store the length of the header */
    state = btstGetbits(16,&lq);
    if (state) return state;

    /* we loop on all quant tables */
    do
    {
        /* we check Pq */
        state = btstGetbits(4,&bits);
        if (state) return state;

        if (bits != 0){
            return DJPEG_INVALID_PQ;
        }

        /* we read  tq */
        state = btstGetbits(4,&bits);
        if (state) return state;

        bits = bits & 0x3;
        *pTq = (OMX_U16)bits;

        /* We read Qp values */
        for (cnt = 0; cnt < QUANT_TABLE_SIZE;cnt++){
            state = btstGetbits(8,&bits);
            if (state) return state;

            pQuantTables[*pTq].q[ZZ_TO_NATURAL(cnt)] = bits;
        }

        //endBit = btstGetBitIndex(pBitstream);
		endBit=sizeInBitsRead;
        readLength = ( endBit - startBit)/8;

    } while (readLength < lq);

    return state;
}

/****************************************************************************/
/**
 * \brief 	skip (jump) other APP marker segments
 * \param 	pBitstream : pointer on bitstream struct
 * \note        APP marker has just been consummed
 * \return 	 error code
 *
 *
 **/
/****************************************************************************/

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::skipMarkerSegment()
{
    OMX_U32 bits;
    t_djpegErrorCode state=DJPEG_NO_ERROR;
    OMX_U16 cnt;

    /* we've just read a marker, next 2 bytes are marker segment length */
    state = btstGetbits(16,&bits);
    if (state) return state;


    /* segment length includes marker segment length parameter */
    for (cnt=0;cnt<bits-2 ;cnt++){
        state = btstFlushbits(8);
        if (state) return state;
    }
    return state;
}

/****************************************************************************/
/**
 * \brief 	scan header reading function
 * \author 	jean-marc volle
 * \param 	pBitstream: pointer on bitstream struct
 * \param 	pScan pointer on jpeg scan descriptor
 * \return 	 btst error code
 *
 * This function reads a Frame header segment according to CCITT_Rec_T81 B.2.3
 * as well as JFIF standard : 1 or 3 components
 **/
/****************************************************************************/

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::readScanHeader (tps_jpgScan pScan)
{
    OMX_U32 bits;
    t_djpegErrorCode state = DJPEG_NO_ERROR;
    OMX_S16 cnt;

    /* we've just read a marker, next 2 bytes are marker segment length */
    state = btstFlushbits(16);
    if (state) return state;

    /* first byte is Ns */
    state = btstGetbits(8,&bits);
    if (state) return state;

    pScan->numberOfComponents = (OMX_U16) bits;

    /* reading of scan component infos */
    for (cnt = 0; cnt<pScan->numberOfComponents;cnt++){
        /*  next byte is Csj */
        state = btstGetbits(8,&bits);
        if (state) return state;

        pScan->scanComponent[cnt].componentId = (OMX_U16) bits;

        /* next 4 bits are Tdj */
        state = btstGetbits(4,&bits);
        if (state) return state;

        pScan->scanComponent[cnt].dcTableId = (OMX_U16) bits;

        /* next 4 bits are Taj */
        state = btstGetbits(4,&bits);
        if (state) return state;

        pScan->scanComponent[cnt].acTableId = (OMX_U16) bits;
    } /* end of reading of scan component informations */

    /*  next byte is Ss */
    state = btstGetbits(8,&bits);
    if (state) return state;

    pScan->startSpectralSelection = (OMX_U16) bits;

    /*  next byte is Sw */
    state = btstGetbits(8,&bits);
    if (state) return state;

    pScan->endSpectralSelection = (OMX_U16) bits;

    /* next 4 bits are Ah */
    state = btstGetbits(4,&bits);
    if (state) return state;

    pScan->approxHigh = (OMX_U16) bits;

    /* next 4 bits are Al */
    state = btstGetbits(4,&bits);
    if (state) return state;

    pScan->approxLow = (OMX_U16) bits;

    return state;
} /* end of read_scan_header */

/****************************************************************************/
/**
 * \brief 	frame header reading function
 * \author 	jean-marc volle
 * \param 	pBitstream: pointer on bitstream struct
 * \param 	p_frame pointer on jpeg frame descriptor
 * \return 	 btst error code
 *
 * This function reads a Frame header segment according to CCITT_Rec_T81 B.2.2
 * as well as JFIF standard : 1 or 3 components
 **/
/****************************************************************************/

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::readFrameHeader (tps_jpgFrame pFrame)
{
    OMX_U32 bits;
    t_djpegErrorCode state = DJPEG_NO_ERROR;
    OMX_S16 cnt;

    /* we've just read a marker, next 2 bytes are marker segment length */
    state = btstFlushbits(16);
    if (state) return state;

    /* next byte is P */
    state = btstGetbits(8,&bits);
    if (state) return state;

    pFrame->samplePrecision = (OMX_U16) bits;

    /* next 2 bytes are Y */
    state = btstGetbits(16,&bits);
    if (state) return state;

    pFrame->numberOfLines = (OMX_U16) bits;

    /* next 2 bytes are X */
    state = btstGetbits(16,&bits);
    if (state) return state;

    pFrame->numberSamplePerLine = (OMX_U16) bits;

    /*  next byte is Nf */
    state = btstGetbits(8,&bits);
    if (state) return state;

    pFrame->numberOfComponents = (OMX_U16) bits;

	if (!(pFrame->numberOfComponents==1 || pFrame->numberOfComponents==3)){
		//printf("Warning: Only 1 or 3 components supported!!!");
		return DJPEG_UNSUPPORTED_MODE;
	}

    /* now we read components specification parameters */
    for (cnt=0; cnt<pFrame->numberOfComponents; cnt++){
        /* first byte is Ci */
        state = btstGetbits(8,&bits);
        if (state) return state;

        if (bits != (OMX_U32) cnt+1){
            //printf("  warning: component should  have Ci =i (not jfif file)\n");
        }
        pFrame->frameComponent[cnt].id = (OMX_U16)bits;

        /* next 4 bits are Hi */
        state = btstGetbits(4,&bits);
        if (state) return state;

        pFrame->frameComponent[cnt].h = (OMX_U16) bits;

        /* next 4 bits are Vi */
        state = btstGetbits(4,&bits);
        if (state) return state;

        pFrame->frameComponent[cnt].v = (OMX_U16) bits;

        /* next byte is Tq */
        state = btstGetbits(8,&bits);
        if (state) return state;

        pFrame->frameComponent[cnt].tq = (OMX_U16) bits;

    } /* end of component specification reading */

    return state;
}/*  end of read_frame_header */

/****************************************************************************/
/**
 * \brief 	process the current marker in the bitstream
 * \author 	jean-marc volle
 * \param 	pBitstream pointer on current bitstream struct
 * \param 	p_jpeg_info pointer of jpeg bitstream info to update
 * \param 	p_process_state pointer on processing state to update
 * \note        bitstream must be aligned on the marker to consume prior to
 *              calling this function.
 * \return 	bitstream error code
 *
 *
 **/
/****************************************************************************/

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::btparProcessMarker(tps_jpgInfo pJpegInfo,
                                                           tps_bitstreamProcessingState pProcessState)
{
    t_djpegErrorCode state=DJPEG_NO_ERROR;
    //OMX_U16 marker;
    OMX_U16 tq;   /* quant table identifier */

    /*we read the marker */
    //state = btstGetbits(16,&marker);
    //if (state) return state;

    //Now we can start the processing */
    //pProcessState->lastMarker = marker;

    switch (pProcessState->lastMarker)
    {
    case SOF0:
    	#ifdef __PERF_MEASUREMENT
    	write_ticks_to_buffer("\nEnter SOF0");
    	#endif
        pJpegInfo->frame.type = BASELINE;
        state = readFrameHeader(&(pJpegInfo->frame));
        if (state) return state;
		pParamAndConfig->isFrameHeaderUpdated =1;
    	#ifdef __PERF_MEASUREMENT
    	write_ticks_to_buffer("\nExit SOF0");
    	#endif

        break;
	case SOF1:
		// tarun
	    //printf("Warning:Extended(sequential) Not supported!!!");
		return DJPEG_UNSUPPORTED_MODE;
		#if 0
		pJpegInfo->frame.type = EXTENDED_SEQUENTIAL;
        state = readFrameHeader(&(pJpegInfo->frame));
        if (state) return state;
        break;
		#endif
	case SOF2:
        //tarun
        //printf("Warning: Extended Mode(Progressive) Not supported!!!");
		return DJPEG_UNSUPPORTED_MODE;
		#if 0
        pJpegInfo->frame.type = EXTENDED_PROGRESSIVE;
        state = readFrameHeader(&(pJpegInfo->frame));
        if (state) return state;
        break;
		#endif
    case SOF3:
    case SOF5:
    case SOF6:
    case SOF7:
    case SOF9:
    case SOF10:
    case SOF11:
    case SOF13:
    case SOF14:
    case SOF15:
        return DJPEG_UNSUPPORTED_MODE;
        //break;
    case SOI:
        break;
    case JPG:
        state = skipMarkerSegment();
        if (state) return state;
        break;
    case DHT:
    	#ifdef __PERF_MEASUREMENT
    	write_ticks_to_buffer("\nEnter DHT");
    	#endif
        pProcessState->huffTableNumber ++;
        state = readHuffmanTable(pJpegInfo->dcHuffTable,pJpegInfo->acHuffTable);
        if (state) return state;
		pParamAndConfig->isHuffmanTableUpdated =1;
    	#ifdef __PERF_MEASUREMENT
    	write_ticks_to_buffer("\nExit DHT");
    	#endif
        break;
    case DAC:
        return DJPEG_UNSUPPORTED_MODE;
        //break;
	case EOI:
		totalNumberOfScans = pProcessState->scanNumber + 1;
        return DJPEG_END_OF_IMAGE;
        //break;
    case SOS:
    	#ifdef __PERF_MEASUREMENT
    	write_ticks_to_buffer("\nEnter SOS");
    	#endif
        state = readScanHeader(&(pJpegInfo->scan));
        if (state) return state;
        pProcessState->scanNumber++;
		pParamAndConfig->isScanHeaderUpdated =1;
    	#ifdef __PERF_MEASUREMENT
    	write_ticks_to_buffer("\nExit SOS");
    	#endif
        break;
    case DQT:
    	#ifdef __PERF_MEASUREMENT
    	write_ticks_to_buffer("\nEnter DQT");
    	#endif
        pProcessState->qpTableNumber ++;
        state = readQuantizationTable(pJpegInfo->quantTable,&tq);
        if (state) return state;
		pParamAndConfig->isQuantizationTableUpdated =1;
    	#ifdef __PERF_MEASUREMENT
    	write_ticks_to_buffer("\nExit DQT");
    	#endif
        break;
    case DNL:
        state = skipMarkerSegment();
        if (state) return state;
        return DJPEG_UNSUPPORTED_MODE;
        //break;
    case DRI:
    	#ifdef __PERF_MEASUREMENT
    	write_ticks_to_buffer("\nEnter DRI");
    	#endif
        state =readRestartIntervalDefinition(&(pJpegInfo->restartInterval));
        if (state) return state;
		pParamAndConfig->isRestartIntervalUpdated=1;
    	#ifdef __PERF_MEASUREMENT
    	write_ticks_to_buffer("\nExit DRI");
    	#endif
        break;
    case DHP:
        return DJPEG_UNSUPPORTED_MODE;
        //break;
    case EXP:
        return DJPEG_UNSUPPORTED_MODE;
        //break;
    case APP0:
    case APP1:
    case APP2:
    case APP3:
    case APP4:
    case APP5:
    case APP6:
    case APP7:
    case APP8:
    case APP9:
    case APP10:
    case APP11:
    case APP12:
    case APP13:
    case APP14:
    case APP15:
    	#ifdef __PERF_MEASUREMENT
    	write_ticks_to_buffer("\nEnter APPx");
    	#endif
        state = skipMarkerSegment();
        if (state) return state;
    	#ifdef __PERF_MEASUREMENT
    	write_ticks_to_buffer("\nExit APPx");
    	#endif
        break;
    case JPG0:
    case JPG1:
    case JPG2:
    case JPG3:
    case JPG4:
    case JPG5:
    case JPG6:
    case JPG7:
    case JPG8:
    case JPG9:
    case JPG10:
    case JPG11:
    case JPG12:
    case JPG13:
        state = skipMarkerSegment();
        if (state) return state;
        break;
    case COM:
        state =skipMarkerSegment();
        if (state) return state;
        break;
    } /*  end of switch marker */

    return state;

} /* end of processMarker */

/****************************************************************************/
/**
 * \brief 	this function initialized bitstream parser processing state
 * \author 	jean-marc volle
 * \param 	pProcessingState: pointer on parser processing state info
 *
 *
 **/
/****************************************************************************/

void JPEGDec_ARMNMF_Parser_CLASS::btparInit (tps_bitstreamProcessingState pProcessingState)
{
    pProcessingState->lastMarker = 0;
    pProcessingState->qpTableNumber = -1;
    pProcessingState->scanNumber = -1;
	pProcessingState->lastMarkerDataLength = -1;
	pProcessingState->headerOrBtst=0;
	sizeInBitsRead=0;
	totalNumberOfScans=0;
	processMarker=OMX_TRUE;
	traverseEncodedData=OMX_FALSE;
}

/****************************************************************************/
/**
 * \brief 	check if current bit position is byte aligned
 * \author 	jean-marc volle
 * \param       pBitstream pointer on current bitstrean_buffer
 * \return 	TRUE if current bit in bistream is aligned on byte boundary,
 *              FALSE  otherwise
 *
 **/
/****************************************************************************/
OMX_BOOL  JPEGDec_ARMNMF_Parser_CLASS::btstIsByteAligned()
{
	if(!firstItem)
		return OMX_FALSE;

    if ((firstBtstBuffer->indexCurrentBit % 8) == 0) return OMX_TRUE;
    else return OMX_FALSE;
}

/****************************************************************************/
/**
 * \brief 	align bit pointer on next byte boundary if not current aligned
 * \author 	jean-marc volle
 * \param 	pBitstream : pointer on bitstream struct
 * \return 	 bitstream error code
 *
 *
 **/
/****************************************************************************/

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::btstAlignByte()
{
    OMX_U16 offset;

	if(!firstItem)
		return DJPEG_END_OF_DATA;

    offset = firstBtstBuffer->indexCurrentBit % 8;
    if (offset == 0) return DJPEG_NO_ERROR;

    offset = 8 - offset;
    return btstFlushbits(offset);

} /* end of btst_align_byte */


t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::btstShowbits(OMX_U16 n, OMX_U32 *pBits)
{
	//It is assumed that maximum of two buffers are required to support 16 bits of data required.
	OMX_U32 indexLastBit;  /*index of the last bit to be read */
	OMX_U32 tmpBits;
	t_djpegErrorCode state;
	ENS_ListItem_p pFirstBuffer;
	tps_bitstreamBuffer pFirstBufferData;

	if(firstItem){
		pFirstBuffer= firstItem;
		pFirstBufferData = firstBtstBuffer;
	}
	else
		return DJPEG_END_OF_DATA;

	*pBits=0;
	indexLastBit = pFirstBufferData->indexCurrentBit + n;
	if (indexLastBit > pFirstBufferData->sizeInBits) {
		ENS_ListItem_p pSecondBuffer;

		pSecondBuffer = pFirstBuffer->getNext();
		if(pSecondBuffer){
			tps_bitstreamBuffer pSecondBufferData = (tps_bitstreamBuffer)pSecondBuffer->getData();
			OMX_U16 nbBitsFirstBuffer = pFirstBufferData->sizeInBits-pFirstBufferData->indexCurrentBit;
			OMX_U16 nbBitsSecondBuffer = n-nbBitsFirstBuffer;

			//Read upper part of the data
			state=BTSTSHOWBITS(pFirstBufferData,nbBitsFirstBuffer,&tmpBits);
			if(state) return state;
			*pBits=tmpBits|(*pBits<<nbBitsFirstBuffer);
			//Read lower part of the data
			tmpBits=0;
			state=BTSTSHOWBITS(pSecondBufferData,nbBitsSecondBuffer,&tmpBits);
			if(state) return state;
            *pBits=tmpBits|(*pBits<<nbBitsSecondBuffer);
		}
		else{
			return DJPEG_END_OF_DATA;
		}
	}
	else{
		state=BTSTSHOWBITS(pFirstBufferData,n,pBits);
		if(state) return state;
	}

	return DJPEG_NO_ERROR;
}

/****************************************************************************/
/**
 * \brief 	show next n bits in bitstream
 * \author 	jean-marc volle
 * \param 	pBitstream pointer on bitstream struct
 * \param 	n: number of bits to show
 * \param 	p_bits pointer on bits to retrieve
 * \return 	btst error code
 *
 * This function shows the next bits (up to 32) in the bitstream. The current
 * bit index is not changed.
 **/
/****************************************************************************/

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::BTSTSHOWBITS(tps_bitstreamBuffer pBitstream, OMX_U16 n, OMX_U32 *pBits)
{

    OMX_U32 nbOfBytesToRead;
    OMX_U32 res;
    OMX_U8 *ptByteToRead;
    OMX_U32 i;
    OMX_U32 offsetFirstByte;
    OMX_U32 offsetLastByte;
    OMX_U16 currentByte; /*because char >> 8 gives 0xff!*/
    OMX_U16 previousByte; /* used fro byte stuffing removal */
    OMX_U32 indexLastBit;  /*index of the last bit to be read */

    pBitstream->cntByteRemoved = 0;
    //assert(n <= 16);

    /* can we access the bits asked ?*/
    indexLastBit = pBitstream->indexCurrentBit + n;

    if ( indexLastBit > pBitstream->sizeInBits)
		return DJPEG_END_OF_DATA;

    nbOfBytesToRead = ((pBitstream->indexCurrentBit + n)>>3) - (pBitstream->indexCurrentBit >> 3)+1 ;
    offsetFirstByte = pBitstream->indexCurrentBit % 8;
    offsetLastByte = (pBitstream->indexCurrentBit+n) % 8;

    ptByteToRead = pBitstream->ptCurrentByte;
    previousByte = pBitstream->lastReadByte;

    res = 0x00000000UL;

    /*  reading of first complete bytes */
    if (pBitstream->removeStuffedBytes!=OMX_TRUE){
        /* simple reading */
        for (i=0;i<nbOfBytesToRead;i++){
            currentByte = *ptByteToRead ;
            currentByte = currentByte & 0x00ff ;

            if (i==0){
                currentByte = currentByte & (0x00ff>>offsetFirstByte);
            }

            if (i != nbOfBytesToRead -1) res = (res << 8) |(OMX_U32) currentByte;
            else{
                currentByte = currentByte >> (8-offsetLastByte);
                res = (res << offsetLastByte) | currentByte;
            }
            ptByteToRead++;
        } /* end of for */
    } /* end of simple reading */
    else{
        /* we must remove any 0x00 byte in 0xff00 sequences */
        for (i=0;i<nbOfBytesToRead;i++)
        {
            currentByte = *ptByteToRead ;
            currentByte = currentByte & 0x00ff ;
            if ( (currentByte == 0x00) && (previousByte == 0xff)){
                /* we store current byte as previous  */
                previousByte = currentByte;
                /* we do not keep current byte value but
                 * will have to read one more byte
                 */
                i--;
                indexLastBit += 8;

                if (indexLastBit >= pBitstream->sizeInBits) return DJPEG_END_OF_DATA;
                pBitstream->cntByteRemoved ++;
            }
            else{
                /* we store current byte as previous before modifying it */
                previousByte = currentByte;

                /* we keep the byte value */
                if (i==0){
                    currentByte = currentByte & (0x00ff>>offsetFirstByte);
                }

                if (i != nbOfBytesToRead -1) res = (res << 8) |(OMX_U32) currentByte;
                else{
                    currentByte = currentByte >> (8-offsetLastByte);
                    res = (res << offsetLastByte) | currentByte;
                }

            } /*  end of byte reading */

            ptByteToRead++;
        } /* end of for */
    } /* end of  byte stuffing removal case */

    //   if (pBitstream->is_byte_removed) DBG_PRINTF((" res(%d) = 0x%x\n",n,res ));
    *pBits = (OMX_U32) res;
    return DJPEG_NO_ERROR;
}

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::btstFlushbits(OMX_U16 n)
{
	t_djpegErrorCode state;
	OMX_U16 actualBitsToBeRemoved=0;
	ENS_ListItem_p pFirstBuffer= firstItem;
	tps_bitstreamBuffer pFirstBufferData = firstBtstBuffer;

    if (pFirstBufferData->cntByteRemoved >0 )
		actualBitsToBeRemoved = n + 8*pFirstBufferData->cntByteRemoved ;

	if (pFirstBufferData->indexCurrentBit + actualBitsToBeRemoved > pFirstBufferData->sizeInBits) {
		ENS_ListItem_p pSecondBuffer;

		pSecondBuffer = pFirstBuffer->getNext();
		if(pSecondBuffer){
			tps_bitstreamBuffer pSecondBufferData = (tps_bitstreamBuffer)pSecondBuffer->getData();
			OMX_U16 nbBitsFirstBuffer = pFirstBufferData->sizeInBits-pFirstBufferData->indexCurrentBit
										-(8*pFirstBufferData->cntByteRemoved);
			OMX_U16 nbBitsSecondBuffer = n-nbBitsFirstBuffer;

			state=BTSTFLUSHBITS(pFirstBufferData,nbBitsFirstBuffer);
			if(state) return state;
			state=deleteBtstBufferLinkFront();
			if(state) return state;

			state=BTSTFLUSHBITS(pSecondBufferData,nbBitsSecondBuffer);
			if(state) return state;
			if(pSecondBufferData->indexCurrentBit==pSecondBufferData->sizeInBits){
				state=deleteBtstBufferLinkFront();
				if(state) return state;
			}
		}
		else{
			return DJPEG_END_OF_DATA;
		}
	}
	else{
		state = BTSTFLUSHBITS(pFirstBufferData,n);
		if(state) return state;
		if(pFirstBufferData->indexCurrentBit==pFirstBufferData->sizeInBits){
			state=deleteBtstBufferLinkFront();
			if(state) return state;
		}
	}
	sizeInBitsRead+=n;
	return DJPEG_NO_ERROR;
}

/****************************************************************************/
/**
 * \brief 	advance the Bitstream current bit index and the current byte
 * \author 	jean-marc volle
 * \param 	pBitstream pointer on bitstream struct
 * \param 	n: increment to add to the bit index
 * \return      btst error code
 *
 * This function increments the bit index in the bitstream. It also update the
 * lastReadByte for byte stuffing removal.
 **/
/****************************************************************************/

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::BTSTFLUSHBITS(tps_bitstreamBuffer pBitstream, OMX_U16 n)
{

/*     the caller does not know if a byte has been removed but we */
/*     must advance one more byte if this is the case                                  */

    if (pBitstream->cntByteRemoved >0 ){
        n = n + 8 * pBitstream->cntByteRemoved ;
        pBitstream->cntByteRemoved = 0;
    }

    if (pBitstream->indexCurrentBit + n > pBitstream->sizeInBits) return DJPEG_END_OF_DATA;

    pBitstream->indexCurrentBit += n;
    pBitstream->ptCurrentByte = pBitstream->data + (pBitstream->indexCurrentBit >>3);

    /* store previous byte value for byte stuffing removal*/
    if (btstIsByteAligned()) pBitstream->lastReadByte = *(pBitstream->ptCurrentByte -1);

    return DJPEG_NO_ERROR;
}

/****************************************************************************/
/**
 * \brief 	get next n bits in the bitstream
 * \author 	jean-marc volle
 * \param 	pBitstream pointer on bitstream struct
 * \param 	n : number of bits to get
 * \param 	p_bits: pointer on bits to retrieve
 * \return      btst error code
 *
 * This function returns the next n bits in the bitstream and increases the
 * the bit index. It is equivalent to showbits (n) then flushbits(n)
 **/
/****************************************************************************/

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::btstGetbits(OMX_U16 n, OMX_U32 *pBits)
{
    t_djpegErrorCode state;
    state  = btstShowbits(n,pBits);
    state  = btstFlushbits(n);
    return state;
}

/****************************************************************************/
/**
 * \brief 	return the index of the current bit in the bitstream handling structure
 * \author 	jean-marc volle
 * \return 	 index of current bit
 *
 *
 **/
/****************************************************************************/
OMX_U32 JPEGDec_ARMNMF_Parser_CLASS::btstGetBitIndex()
{
	if(!firstItem)
		return DJPEG_END_OF_DATA;

    return (firstBtstBuffer->indexCurrentBit);
}

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::jpegParser()
{
    t_djpegErrorCode btstState;

    /*init of the bistream struct */
    if(isInitialized == OMX_FALSE){
	    /* we init the bitstream parser processing state */
	    btparInit(&processingState);
	    isInitialized = OMX_TRUE;
        btstState = updateBtstBufferList();
		if(btstState) return btstState;
	}
    /* Now we read the marker segment one after the other until we have reached the end of the bitstream */
    btstState = DJPEG_NO_ERROR;
   #ifdef __PERF_MEASUREMENT
   write_ticks_to_buffer("\nActual start");
   #endif
	while(!pParamAndConfig->completeImageProcessed){
    if(processMarker){

		while (!(btstState == DJPEG_END_OF_IMAGE || btstState == DJPEG_END_OF_SCAN)){
			/* let's advance to the next marker */
			if(!processingState.lastMarker){
				OMX_U32 bits = 0;
				btstState = btparAdvanceToNextMarker();
				if (btstState == DJPEG_END_OF_DATA)
                {
					btstState = updateBtstBufferList();
					if(btstState){
						processingState.lastMarker=0;
						return btstState;
					}
					else
						btstState = btparAdvanceToNextMarker();
				}
				btstState = btstShowbits(16,&bits);
                processingState.lastMarker = bits;

				//FIXME Tarun: consider case of a marker with first byte at end of last release buffer
				processingState.headerOrBtst=0x01; 	//header present
				firstBtstBuffer->btstBufferContentInfo|=processingState.headerOrBtst;
				btstState = btstFlushbits(16);
			}

			if(processingState.lastMarkerDataLength==-1){
				OMX_U16 bits = 0;
				btstState=findLengthMarkerData(&bits);
				if (btstState == DJPEG_END_OF_DATA)
				{
					btstState = updateBtstBufferList();
					if(btstState){
						processingState.lastMarkerDataLength=-1;
						return btstState;
					}
					else
						btstState = findLengthMarkerData(&bits);
				}
				processingState.lastMarkerDataLength=bits;
			}

			btstState=isProcessingmarkerPossible(processingState.lastMarkerDataLength);
			if(btstState == DJPEG_END_OF_DATA){
				do{
					btstState = updateBtstBufferList();
					if(btstState)
						return btstState;
					else
						btstState = isProcessingmarkerPossible(processingState.lastMarkerDataLength);
				}while(btstState!=DJPEG_NO_ERROR);
			}

			/* and now we can process it */
			btstState = btparProcessMarker(&jpegInfo,&processingState);
			if (btstState == DJPEG_END_OF_DATA)
				return btstState;

			if(btstState == DJPEG_UNSUPPORTED_MODE)
				return btstState;

			/* according to the last marker read in the bitstream we can decide to do different things: */
			switch (processingState.lastMarker)
			{
				case SOS:
					switch (jpegInfo.frame.type)
					{
						case BASELINE:
                        #ifdef __PERF_MEASUREMENT
                        write_ticks_to_buffer("\nEnter SOS copy");
                        #endif
						 if(pParamAndConfig->isFrameHeaderUpdated)
							 updateFrameHeader(pParamAndConfig);
						 if(pParamAndConfig->isScanHeaderUpdated)
							 updateScanHeader(pParamAndConfig);
						 if(pParamAndConfig->isHuffmanTableUpdated)
							 updateHuffmanTables(pParamAndConfig);
						 if(pParamAndConfig->isQuantizationTableUpdated)
							 updateQuantizationTables(pParamAndConfig);
						 if(pParamAndConfig->isRestartIntervalUpdated)
							 setRestartInterval(pParamAndConfig);

						 setYUVId(pParamAndConfig);
						 pParamAndConfig->updateData = 0;
						 processMarker=OMX_FALSE;
						 btstState = DJPEG_END_OF_SCAN;
                        #ifdef __PERF_MEASUREMENT
                        write_ticks_to_buffer("\nExit SOS copy");
                        #endif

						 break;

					    case EXTENDED_SEQUENTIAL:
                            return DJPEG_UNSUPPORTED_MODE;
					    case EXTENDED_PROGRESSIVE:
                            return DJPEG_UNSUPPORTED_MODE;
                        default:
                            //Added to remove warnings
                            return DJPEG_UNSUPPORTED_MODE;
					}
					break;

				case EOI:
				  isInitialized =OMX_FALSE;
				  pParamAndConfig->completeImageProcessed = OMX_TRUE;
				  //FIXME Tarun: Check is this freeing of the buffer required.
                  #ifdef __PERF_MEASUREMENT
                  write_ticks_to_buffer("\nEnter EOI");
                  #endif

                  btstState=deleteBtstBufferLinkFront();
				  if(btstState) return btstState;

				  btstState = DJPEG_END_OF_IMAGE;
                  #ifdef __PERF_MEASUREMENT
                  write_ticks_to_buffer("\nEnter EOI");
                  #endif

				  break;
			}; /* end of switch last_marker */

			processingState.lastMarker=0;
			processingState.lastMarkerDataLength=-1;
		} /* end of while loop */
	}
	while(!processMarker)
	{
		if(traverseEncodedData){
            btstState = findEOIMarker();
			if(btstState) return btstState;
			/*
			if(btstState==DJPEG_END_OF_DATA)
			{
				btstState = deleteBtstBufferLinkFront();
				return btstState;
			}*/
		}
		else
		{
			OMX_U32 bits;
			//process stream now
			btstState=btstShowbits(8,&bits);
            if(btstState==DJPEG_END_OF_DATA){
				processingState.headerOrBtst=0x10;
				btstState = updateBtstBufferList();
				if(btstState) return btstState;
			}
			else{
				processingState.headerOrBtst=0x10; 	//Bitstream present
				pParamAndConfig->scan.bitstreamOffset = firstBtstBuffer->indexCurrentBit;
				pParamAndConfig->scan.encodedDataSize = 0;
                #ifdef __PERF_MEASUREMENT
                write_ticks_to_buffer("\nEnter send Header");
                #endif

				btstState=sendHeader(firstBtstBuffer->omxBitstreamBuffer);
                #ifdef __PERF_MEASUREMENT
                write_ticks_to_buffer("\nExit send Header");
                #endif

				if(btstState) return btstState;
				traverseEncodedData=OMX_TRUE;
			}
		}
	}
    }

   	return DJPEG_NO_ERROR;
}


t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::findEncodedScanData()
{
    t_djpegErrorCode state=DJPEG_NO_ERROR;
    OMX_U32 bits = 0;
	while(traverseEncodedData)
	{
		/*advance until next marker */
		state = btstShowbits(16,&bits);
		if (state==DJPEG_END_OF_DATA){
			state = updateBtstBufferList();
			if(state) return state;
			state = btstShowbits(16,&bits);
		}

		if(!( ((bits & 0xff00) == 0xff00) && (((bits & 0xff) != 0x00)) && ((bits & 0xff)!= 0xff) && ((bits & 0xfff8) != 0xffD0)))
		{
			state = btstFlushbits(8);
			if (state) return state;
		}
		else{
           traverseEncodedData=OMX_FALSE;
		   processMarker=OMX_TRUE;
		}
	}

	return DJPEG_NO_ERROR;
}

OMX_U32 JPEGDec_ARMNMF_Parser_CLASS::LastMarker()
{
	return processingState.lastMarker;
}

OMX_U32 JPEGDec_ARMNMF_Parser_CLASS::GetScanNumber()
{
	return processingState.scanNumber;
}

OMX_BOOL JPEGDec_ARMNMF_Parser_CLASS::CheckParsingFinished()
{
	 if (processingState.lastMarker==EOI)
	 {
		 return OMX_TRUE;
	 }
	 else
		 return OMX_FALSE;
}

OMX_S32 JPEGDec_ARMNMF_Parser_CLASS::GetTotalNumberOfScans()
{
	return totalNumberOfScans;
}

void JPEGDec_ARMNMF_Parser_CLASS::updateFrameHeader(JPEGDec_ARMNMF_ParamAndConfig_CLASS *jpegdecParamAndConfig)
{
    OMX_U8 cnt;
    jpegdecParamAndConfig->frameHeight = jpegInfo.frame.numberOfLines;
	jpegdecParamAndConfig->frameWidth = jpegInfo.frame.numberSamplePerLine;
	jpegdecParamAndConfig->nbComponents = jpegInfo.frame.numberOfComponents;

	if(jpegInfo.frame.numberOfComponents==1)
	{
		jpegdecParamAndConfig->samplingFactors[0].hSamplingFactor= jpegInfo.frame.frameComponent[0].h;
	    jpegdecParamAndConfig-> samplingFactors[0].vSamplingFactor= jpegInfo.frame.frameComponent[0].v;
		jpegdecParamAndConfig->samplingFactors[1].hSamplingFactor= 0;
		jpegdecParamAndConfig->samplingFactors[1].vSamplingFactor= 0;
		jpegdecParamAndConfig->samplingFactors[2].hSamplingFactor= 0;
		jpegdecParamAndConfig->samplingFactors[2].vSamplingFactor= 0;
	}
	else
	{
		jpegdecParamAndConfig->samplingFactors[0].hSamplingFactor= jpegInfo.frame.frameComponent[0].h;
		jpegdecParamAndConfig->samplingFactors[0].vSamplingFactor= jpegInfo.frame.frameComponent[0].v;
		jpegdecParamAndConfig->samplingFactors[1].hSamplingFactor= jpegInfo.frame.frameComponent[1].h;
		jpegdecParamAndConfig->samplingFactors[1].vSamplingFactor= jpegInfo.frame.frameComponent[1].v;
		jpegdecParamAndConfig->samplingFactors[2].hSamplingFactor= jpegInfo.frame.frameComponent[2].h;
		jpegdecParamAndConfig->samplingFactors[2].vSamplingFactor= jpegInfo.frame.frameComponent[2].v;
	}

	if(jpegInfo.frame.type ==BASELINE)
	{
		jpegdecParamAndConfig->mode = SEQUENTIAL_JPEG;
	}
	else
	{
		//printf("\nError: Only Baseline is suppoted");  //todo tarun: add a return mechanism
	}

	for(cnt=0;cnt<jpegInfo.frame.numberOfComponents;cnt++)
	{
		jpegdecParamAndConfig->components[cnt].componentId = jpegInfo.frame.frameComponent[cnt].id;
		jpegdecParamAndConfig->components[cnt].quantizationTableDestinationSelector = jpegInfo.frame.frameComponent[cnt].tq;
	}
}

void JPEGDec_ARMNMF_Parser_CLASS::updateScanHeader(JPEGDec_ARMNMF_ParamAndConfig_CLASS *jpegdecParamAndConfig)
{
	OMX_U8 cnt, componentId, componentCnt;

	jpegdecParamAndConfig->nbScanComponents = jpegInfo.scan.numberOfComponents;
	jpegdecParamAndConfig->startSpectralSelection = 0;
	jpegdecParamAndConfig->endSpectralSelection = 0;
	jpegdecParamAndConfig->successiveApproxPosition =0;

	for (componentCnt=0;componentCnt<MAX_FRAME_COMPONENT;componentCnt++)
	{
		jpegdecParamAndConfig->components[componentCnt].componentSelector = 0;
    }

	for(cnt=0;cnt<jpegInfo.scan.numberOfComponents;cnt++)
	{
        componentId = jpegInfo.scan.scanComponent[cnt].componentId;
		for (componentCnt=0;componentCnt<MAX_FRAME_COMPONENT;componentCnt++)
		{
			if(componentId == jpegdecParamAndConfig->components[componentCnt].componentId)
			{
				jpegdecParamAndConfig->components[componentCnt].componentSelector = 1;
				jpegdecParamAndConfig->components[componentCnt].DCHuffmanTableDestinationSelector = jpegInfo.scan.scanComponent[cnt].dcTableId;
				jpegdecParamAndConfig->components[componentCnt].ACHuffmanTableDestinationSelector = jpegInfo.scan.scanComponent[cnt].acTableId;
			}
		}
	}

	//if(jpegInfo.frame.type !=BASELINE) // Some Error if enters.
	{
		jpegdecParamAndConfig->startSpectralSelection = jpegInfo.scan.startSpectralSelection;
		jpegdecParamAndConfig->endSpectralSelection = jpegInfo.scan.endSpectralSelection;
		jpegdecParamAndConfig->successiveApproxPosition = (jpegInfo.scan.approxLow << 4) | jpegInfo.scan.approxHigh;
	}
}

void JPEGDec_ARMNMF_Parser_CLASS::updateHuffmanTables(JPEGDec_ARMNMF_ParamAndConfig_CLASS *jpegdecParamAndConfig)
{
	OMX_U8 cnt;
#ifdef __JPEGDEC_SOFTWARE
	for(cnt =0;cnt<NB_OF_HUFF_TABLES;cnt++){

        ///DC HUFFMAN TABLES
		memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].DCHuffmanBits,
			   jpegInfo.dcHuffTable[cnt].bits,
			   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
		memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].DCHuffmanVal,
			   jpegInfo.dcHuffTable[cnt].val,
			   DC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
        //////////////////////////// new members being copied
        memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].MaxCode,
			   jpegInfo.dcHuffTable[cnt].maxcode,
			   18*sizeof(OMX_S32));
        memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].ValOffset,
			   jpegInfo.dcHuffTable[cnt].valoffset,
			   17*sizeof(OMX_S32));
        memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].Look_Nbits,
			   jpegInfo.dcHuffTable[cnt].look_nbits,
			   256*sizeof(OMX_S16));
        memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].Look_Sym,
			   jpegInfo.dcHuffTable[cnt].look_sym,
			   256*sizeof(OMX_U16));

        ///AC HUFFMAN TABLES
		memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].ACHuffmanBits,
			   jpegInfo.acHuffTable[cnt].bits,
			   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
		memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].ACHuffmanVal,
			   jpegInfo.acHuffTable[cnt].val,
			   AC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
        //////////////////////////// new members being copied
        memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].MaxCode,
			   jpegInfo.acHuffTable[cnt].maxcode,
			   18*sizeof(OMX_S32));
        memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].ValOffset,
			   jpegInfo.acHuffTable[cnt].valoffset,
			   17*sizeof(OMX_S32));
        memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].Look_Nbits,
			   jpegInfo.acHuffTable[cnt].look_nbits,
			   256*sizeof(OMX_S16));
        memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].Look_Sym,
			   jpegInfo.acHuffTable[cnt].look_sym,
			   256*sizeof(OMX_U16));
	}
#elif __JPEGDEC_DUAL
for(cnt =0;cnt<NB_OF_HUFF_TABLES;cnt++)
    {
		/*memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].DCHuffmanBits,
			   jpegInfo.dcHuffTable[cnt].bits,
			   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
		memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].DCHuffmanVal,
			   jpegInfo.dcHuffTable[cnt].val,
			   DC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
		memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].ACHuffmanBits,
			   jpegInfo.acHuffTable[cnt].bits,
			   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
		memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].ACHuffmanVal,
			   jpegInfo.acHuffTable[cnt].val,
			   AC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));*/
        ///DC HUFFMAN TABLES
		memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].DCHuffmanBits,
			   jpegInfo.dcHuffTable[cnt].bits,
			   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
		memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].DCHuffmanVal,
			   jpegInfo.dcHuffTable[cnt].val,
			   DC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
        //////////////////////////// new members being copied
        memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].MaxCode,
			   jpegInfo.dcHuffTable[cnt].maxcode,
			   18*sizeof(OMX_S32));
        memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].ValOffset,
			   jpegInfo.dcHuffTable[cnt].valoffset,
			   17*sizeof(OMX_S32));
        memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].Look_Nbits,
			   jpegInfo.dcHuffTable[cnt].look_nbits,
			   256*sizeof(OMX_S16));
        memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].Look_Sym,
			   jpegInfo.dcHuffTable[cnt].look_sym,
			   256*sizeof(OMX_U16));

        ///AC HUFFMAN TABLES
		memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].ACHuffmanBits,
			   jpegInfo.acHuffTable[cnt].bits,
			   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
		memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].ACHuffmanVal,
			   jpegInfo.acHuffTable[cnt].val,
			   AC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
        //////////////////////////// new members being copied
        memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].MaxCode,
			   jpegInfo.acHuffTable[cnt].maxcode,
			   18*sizeof(OMX_S32));
        memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].ValOffset,
			   jpegInfo.acHuffTable[cnt].valoffset,
			   17*sizeof(OMX_S32));
        memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].Look_Nbits,
			   jpegInfo.acHuffTable[cnt].look_nbits,
			   256*sizeof(OMX_S16));
        memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].Look_Sym,
			   jpegInfo.acHuffTable[cnt].look_sym,
			   256*sizeof(OMX_U16));
	}

        for(cnt =0;cnt<NB_OF_HUFF_TABLES;cnt++)
    {
		memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].DCHuffmanCode,
			   jpegInfo.dcHuffTable[cnt].code,
			   DC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
		memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].DCHuffmanSize,
			   jpegInfo.dcHuffTable[cnt].size,
			   DC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
		memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].ACHuffmanCode,
			   jpegInfo.acHuffTable[cnt].code,
			   AC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
		memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].ACHuffmanSize,
			   jpegInfo.acHuffTable[cnt].size,
			   AC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
	}
#else
        for(cnt =0;cnt<NB_OF_HUFF_TABLES;cnt++)
	{
		memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].DCHuffmanCode,
			   jpegInfo.dcHuffTable[cnt].code,
			   DC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
		memcpy(jpegdecParamAndConfig->DCHuffmanTable[cnt].DCHuffmanSize,
			   jpegInfo.dcHuffTable[cnt].size,
			   DC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
		memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].ACHuffmanCode,
			   jpegInfo.acHuffTable[cnt].code,
			   AC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
		memcpy(jpegdecParamAndConfig->ACHuffmanTable[cnt].ACHuffmanSize,
			   jpegInfo.acHuffTable[cnt].size,
			   AC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
	}
#endif
}

void JPEGDec_ARMNMF_Parser_CLASS::updateQuantizationTables(JPEGDec_ARMNMF_ParamAndConfig_CLASS *jpegdecParamAndConfig)
{
	OMX_U8 cnt;
	for(cnt =0;cnt<NB_OF_QUANT_TABLES;cnt++)
	{
		memcpy(jpegdecParamAndConfig->quantizationTables[cnt].QuantizationTable,
			   jpegInfo.quantTable[cnt].q,
			   QUANT_TABLE_SIZE*sizeof(OMX_U16));
	}
}

void JPEGDec_ARMNMF_Parser_CLASS::setYUVId(JPEGDec_ARMNMF_ParamAndConfig_CLASS *jpegdecParamAndConfig)
{
	// considering case of JFIF right now. For JFIF Ci = i. Also Y=1, Cb=2, Cr=3
	// otherwise it has been assumed first component will be for Y,
	// second will be Cb
	// third will be Cr

    jpegdecParamAndConfig->IdY = jpegInfo.frame.frameComponent[0].id;
	jpegdecParamAndConfig->IdCb = jpegInfo.frame.frameComponent[1].id;
	jpegdecParamAndConfig->IdCr = jpegInfo.frame.frameComponent[2].id;
}

void JPEGDec_ARMNMF_Parser_CLASS::setRestartInterval(JPEGDec_ARMNMF_ParamAndConfig_CLASS *jpegdecParamAndConfig)
{
	jpegdecParamAndConfig->restartInterval = jpegInfo.restartInterval;
}


t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::parserReset()
{
	t_djpegErrorCode error = DJPEG_NO_ERROR;

	isInitialized = OMX_FALSE;
    jpegInfo.restartInterval=0; 			//May or may not present next bitstream
	btparInit(&processingState);

	error = clearBtstBufferList();
	if(error) return error;

	firstItem = 0;
	firstBtstBuffer =0;
	return DJPEG_NO_ERROR;
}

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::clearBtstBufferList()
{
	t_djpegErrorCode error = DJPEG_NO_ERROR;

	if(!bitstreamBufferList.isEmpty())
	{
		int btstBuffersPresent = bitstreamBufferList.getSize();
		while(btstBuffersPresent)
		{
			ENS_ListItem_p pListItem;
			void *pBtstBuffer;

			pBtstBuffer = bitstreamBufferList.popFront();
			if(!pBtstBuffer) return DJPEG_MALLOC_ERROR;

			pJpegNmfComp->releaseBtstBuffer(0,((ts_bitstreamBuffer*)pBtstBuffer)->omxBitstreamBuffer);

			pListItem=freeLinks.pushBack(pBtstBuffer);
			if(!pListItem) return DJPEG_MALLOC_ERROR;

			pListItem=bitstreamBufferList.getFirst();
			if(pListItem){
				firstItem = pListItem;
				firstBtstBuffer = (ts_bitstreamBuffer*)firstItem->getData();
			}
			else{
				firstItem = 0;
				firstBtstBuffer = 0;
			}
			return DJPEG_NO_ERROR;
		}
	}

	return error;
}


t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::deleteBtstBufferLinkFront()
{
	ENS_ListItem_p pListItem;
	void *pBtstBuffer;
   #ifdef __PERF_MEASUREMENT
   write_ticks_to_buffer("\ndeleteBtstBufferLinkFront");
   #endif

	pBtstBuffer = bitstreamBufferList.popFront();
	if(!pBtstBuffer) return DJPEG_MALLOC_ERROR;
   #ifdef __PERF_MEASUREMENT
   write_ticks_to_buffer("\npopFrontt");
   #endif

	pJpegNmfComp->releaseBtstBuffer(((ts_bitstreamBuffer*)pBtstBuffer)->btstBufferContentInfo,((ts_bitstreamBuffer*)pBtstBuffer)->omxBitstreamBuffer);

   #ifdef __PERF_MEASUREMENT
   write_ticks_to_buffer("\n After releaseBtstBuffer");
   #endif

	pListItem=freeLinks.pushBack(pBtstBuffer);
   #ifdef __PERF_MEASUREMENT
   write_ticks_to_buffer("\npushBack");
   #endif

	if(!pListItem) return DJPEG_MALLOC_ERROR;

	pListItem=bitstreamBufferList.getFirst();
	if(pListItem){
		firstItem = pListItem;
		firstBtstBuffer = (ts_bitstreamBuffer*)firstItem->getData();
    }
	else{
		firstItem = 0;
		firstBtstBuffer = 0;
	}

	return DJPEG_NO_ERROR;
}


t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::findLengthMarkerData(OMX_U16 *pBits)
{
    t_djpegErrorCode state=DJPEG_NO_ERROR;
    OMX_U32 bits;

    switch (processingState.lastMarker)
    {
	case SOI:
	case EOI:
		*pBits = 0;
        break;
    case SOF0:
	case SOF1:
	case SOF2:
    case SOF3:
    case SOF5:
    case SOF6:
    case SOF7:
    case SOF9:
    case SOF10:
    case SOF11:
    case SOF13:
    case SOF14:
	case SOF15:
    case JPG:
	case DHT:
    case DAC:
    case SOS:
	case DQT:
    case DNL:
	case DRI:
    case APP0:
    case APP1:
    case APP2:
    case APP3:
    case APP4:
    case APP5:
    case APP6:
    case APP7:
    case APP8:
    case APP9:
    case APP10:
    case APP11:
    case APP12:
    case APP13:
    case APP14:
	case APP15:
    case DHP:
    case EXP:
    case JPG0:
    case JPG1:
    case JPG2:
    case JPG3:
    case JPG4:
    case JPG5:
    case JPG6:
    case JPG7:
    case JPG8:
    case JPG9:
    case JPG10:
    case JPG11:
    case JPG12:
	case JPG13:
	case COM:
		/*advance until next marker */
		state = btstShowbits(16,&bits);
		if (state) return state;

		*pBits = bits;
        break;
    } /*  end of switch marker */

	return DJPEG_NO_ERROR;
}

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::isProcessingmarkerPossible(OMX_U16 dataLength)
{
	ENS_ListItem_p item;
	ts_bitstreamBuffer *btstBuffer;
	OMX_U32 btstSizeInBits;
	OMX_U32 requiredDataSizeInBits = 8*dataLength;

	if(!requiredDataSizeInBits)
		return DJPEG_NO_ERROR;

	if(!firstItem)
		return DJPEG_END_OF_DATA;

	item = firstItem;
	while(requiredDataSizeInBits){
		btstBuffer = (ts_bitstreamBuffer*)(item->getData());
		btstSizeInBits= btstBuffer->sizeInBits-btstBuffer->indexCurrentBit;
		if(requiredDataSizeInBits>btstSizeInBits){
			requiredDataSizeInBits-=btstSizeInBits;
			item=item->getNext();
			if(!item) return DJPEG_END_OF_DATA;
		}
		else
			return DJPEG_NO_ERROR;
	}
	return DJPEG_NO_ERROR;
}

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::updateBtstBufferList()
{
	//Try to Update List with a buffer
	OMX_BUFFERHEADERTYPE *omxBuffer;
	t_djpegErrorCode btstState;
	omxBuffer=pJpegNmfComp->provideBitstreamBuffer();
	if(!omxBuffer) return DJPEG_END_OF_DATA;

	btstState=insertBtstBufferLinkBack(omxBuffer);
	if(btstState) return DJPEG_ERROR;

	return DJPEG_NO_ERROR;
}

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::insertBtstBufferLinkBack(OMX_BUFFERHEADERTYPE *omxBuffer)
{
	void *pBtstBuffer;
	ENS_ListItem_p pListItem;

	pBtstBuffer = freeLinks.popFront();
	if(!pBtstBuffer) return DJPEG_MALLOC_ERROR;

	initializeBtstLink(omxBuffer,(ts_bitstreamBuffer*)pBtstBuffer);

	pListItem=bitstreamBufferList.pushBack(pBtstBuffer);
	if(!pListItem) return DJPEG_MALLOC_ERROR;

	if(!firstItem){
		firstItem = bitstreamBufferList.getFirst();
		firstBtstBuffer = (ts_bitstreamBuffer*)firstItem->getData();
	}

	return DJPEG_NO_ERROR;
}

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::initializeBtstLink(OMX_BUFFERHEADERTYPE *omxBuffer
														   ,ts_bitstreamBuffer *pBtstBuffer)
{
	if(omxBuffer==NULL)
		 return DJPEG_ERROR;

	 /* we init the remaining of the bitstream struct  */
	 pBtstBuffer->omxBitstreamBuffer = omxBuffer;
	 pBtstBuffer->data = omxBuffer->pBuffer + omxBuffer->nOffset;
	 pBtstBuffer->ptCurrentByte = pBtstBuffer->data;
	 pBtstBuffer->indexCurrentBit = 0;
	 pBtstBuffer->sizeInBits = 8*(omxBuffer->nFilledLen);
	 pBtstBuffer->removeStuffedBytes=OMX_FALSE;
	 pBtstBuffer->cntByteRemoved = 0;
	 pBtstBuffer->lastReadByte=0;
     pBtstBuffer->btstBufferContentInfo=0;

	 //copy headerOrBtst contenet specifier flag

	 pBtstBuffer->btstBufferContentInfo|=processingState.headerOrBtst;

	 return DJPEG_NO_ERROR;
}

ts_bitstreamBuffer* JPEGDec_ARMNMF_Parser_CLASS::getCurrentBtstBufferLink()
{
	ENS_ListItem_p item = bitstreamBufferList.getFirst();
	return (ts_bitstreamBuffer*)item->getData();
}

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::sendHeader(OMX_BUFFERHEADERTYPE *pBuffer)
{
	return pJpegNmfComp->fillHeader(pBuffer);
}

t_djpegErrorCode JPEGDec_ARMNMF_Parser_CLASS::findEOIMarker()
{
    t_djpegErrorCode state=DJPEG_NO_ERROR;
    OMX_U32 bits = 0;
    OMX_U32 tempBitPosition = 0;
    OMX_BOOL isDone = OMX_FALSE;

	// the EOI marker can be last two bytes, split between buffer bounndary or first 2 bytes.
	// so we need cntr value 3
	while(traverseEncodedData)
	{
		//Move pointer to last two bytes to remove traversing
		firstBtstBuffer->btstBufferContentInfo|=processingState.headerOrBtst;
		if (!isDone)
		{
			tempBitPosition = firstBtstBuffer->indexCurrentBit;
		firstBtstBuffer->indexCurrentBit = firstBtstBuffer->sizeInBits-16;
		}
		firstBtstBuffer->ptCurrentByte = firstBtstBuffer->data + (firstBtstBuffer->indexCurrentBit>>3);

		state = btstShowbits(16,&bits);
        if(bits==EOI){
			//set EOF
			firstBtstBuffer->omxBitstreamBuffer->nFlags|=OMX_BUFFERFLAG_ENDOFFRAME;
			traverseEncodedData=OMX_FALSE;
			processMarker=OMX_TRUE;
        }
		else
		{
			if (!isDone)
			{
				firstBtstBuffer->indexCurrentBit = tempBitPosition;
				isDone = OMX_TRUE;
			}
			state = btstShowbits(32,&bits);
			if (state==DJPEG_END_OF_DATA)
			{

				state = updateBtstBufferList();
				if(state) return state;
				state = btstShowbits(32,&bits);
			}

			if(((bits&0x00FFFF00)>>8)==EOI){
				//set EOF
				firstBtstBuffer->omxBitstreamBuffer->nFlags|=OMX_BUFFERFLAG_ENDOFFRAME;
				traverseEncodedData=OMX_FALSE;
				processMarker=OMX_TRUE;
				state = btstFlushbits(8);
				if (state) return state;
            }
			else if((bits&0x0000FFFF)==EOI){
				//set EOF
				firstBtstBuffer->omxBitstreamBuffer->nFlags|=OMX_BUFFERFLAG_ENDOFFRAME;
				traverseEncodedData=OMX_FALSE;
				processMarker=OMX_TRUE;
				state = btstFlushbits(16);
				if (state) return state;
			}
			else
			{
				state = btstFlushbits(16);
				if (state) return state;
			}
		}
	}
	return DJPEG_NO_ERROR;
}
/****************************************************************************/
/**
 * \brief 	initialised un-order huffman table from host interface parameters
 * \author 	jean-marc volle
 * \param 	p_size array of huffman code size
 * \param 	p_code: array of huffman codes
 * \param 	p_unordered_table[] array of huffman elements
 * \param 	is_dc: flag indicting a dc table
 *
 *
 **/
/****************************************************************************/


