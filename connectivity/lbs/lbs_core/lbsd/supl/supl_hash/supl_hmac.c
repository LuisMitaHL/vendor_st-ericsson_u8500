/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include "supl_hmac.h"
#include "GN_SUPL_api.h"


/****************************** SHA256 procedure *************************************/
/***********************************************************************************/




#define     SIZE_HMAC_BLOCK_IN_BYTES    64
#define     Key1                        0x36
#define     Key2                        0x5c



/* Define the SHA shift, rotate left and rotate right macro */
#define GN_SUPL_SHA256_SHIFT_RIGHT(bits,word)      ((word) >> (bits))
#define GN_SUPL_SHA256_CIRCULAR_SHIFT_LEFT(bits,word)                         \
  ((((word) << (bits)) & 0xFFFFFFFF) | ((word) >> (32-(bits))))
#define GN_SUPL_SHA256_CIRCULAR_SHIFT_RIGHT(bits,word)                         \
  ((((word) >> (bits)) & 0xFFFFFFFF) | ((word) << (32-(bits))))

/* Define the SHA SIGMA and sigma macros */
#define GN_SUPL_SHA256_SIGMA0(word)   \
  (GN_SUPL_SHA256_CIRCULAR_SHIFT_RIGHT( 2,word) ^ GN_SUPL_SHA256_CIRCULAR_SHIFT_RIGHT(13,word) ^ GN_SUPL_SHA256_CIRCULAR_SHIFT_RIGHT(22,word))
#define GN_SUPL_SHA256_SIGMA1(word)   \
  (GN_SUPL_SHA256_CIRCULAR_SHIFT_RIGHT( 6,word) ^ GN_SUPL_SHA256_CIRCULAR_SHIFT_RIGHT(11,word) ^ GN_SUPL_SHA256_CIRCULAR_SHIFT_RIGHT(25,word))
#define GN_SUPL_SHA256_sigma0(word)   \
  (GN_SUPL_SHA256_CIRCULAR_SHIFT_RIGHT( 7,word) ^ GN_SUPL_SHA256_CIRCULAR_SHIFT_RIGHT(18,word) ^ GN_SUPL_SHA256_SHIFT_RIGHT( 3,word))
#define GN_SUPL_SHA256_sigma1(word)   \
  (GN_SUPL_SHA256_CIRCULAR_SHIFT_RIGHT(17,word) ^ GN_SUPL_SHA256_CIRCULAR_SHIFT_RIGHT(19,word) ^ GN_SUPL_SHA256_SHIFT_RIGHT(10,word))


//*****************************************************************************
/// \brief
///      This function will process the next 512 bits of the message
///      stored in the Message_Block array.
/// \parameters
///      *context  
/// \returns   
///      void  
//*****************************************************************************
static void GN_SUPL_SHA256MessageProcessBlock(s_GN_SUPL_SHA256Context *context)
{
   U4   temp, temp4,i;            /* Loop counter */
   U4   t1, t2;                   /* Temporary word value */
   U4   Word[64]={0};             /* Word sequence */
   U4   H0,H1,H2,H3,H4,H5,H6,H7;  /* Word buffers */

   static const U4 CubeRoot[64] = {
      0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b,
      0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01,
      0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7,
      0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
      0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152,
      0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
      0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
      0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
      0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819,
      0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08,
      0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f,
      0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
      0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
  };

  /*
   * Initialize the first 16 words in the array Word
   */
  for (temp = temp4 = 0; temp <= 15; temp++, temp4 += 4)
  {
  for (i = 0; i < 4; i++)
  Word[temp] |= (((U4)context->Message_Block[temp4 + i]) << (8*(3-i)));  // method to assign first 32 bits into 16 sections
  }

  for (temp = 16; temp < 64; temp++)
  {
      Word[temp] = GN_SUPL_SHA256_sigma1(Word[temp-2] ) + Word[temp-7] +
                   GN_SUPL_SHA256_sigma0(Word[temp-15]) + Word[temp-16];
  }

  H0 = context->Intermediate_HashValue[0];
  H1 = context->Intermediate_HashValue[1];
  H2 = context->Intermediate_HashValue[2];
  H3 = context->Intermediate_HashValue[3];
  H4 = context->Intermediate_HashValue[4];
  H5 = context->Intermediate_HashValue[5];
  H6 = context->Intermediate_HashValue[6];
  H7 = context->Intermediate_HashValue[7];

  for (temp = 0; temp < 64; temp++)
  {
    t1 = H7 + GN_SUPL_SHA256_SIGMA1(H4) + ((H4 & H5) | ((~H4) & H6)) + CubeRoot[temp] + Word[temp]; 
    t1 &= 0xFFFFFFFF;
    t2 = GN_SUPL_SHA256_SIGMA0(H0) + ((H0 & H1) | (H0 & H2) | (H1 & H2));           
    t2 &= 0xFFFFFFFF;

    H7 = H6;
    H6 = H5;
    H5 = H4;
    H4 = H3 + t1;
    H3 = H2;
    H2 = H1;
    H1 = H0;
    H0 = t1 + t2;
  }

  context->Intermediate_HashValue[0] += H0 & 0xFFFFFFFF;
  context->Intermediate_HashValue[1] += H1 & 0xFFFFFFFF;
  context->Intermediate_HashValue[2] += H2 & 0xFFFFFFFF;
  context->Intermediate_HashValue[3] += H3 & 0xFFFFFFFF;
  context->Intermediate_HashValue[4] += H4 & 0xFFFFFFFF;
  context->Intermediate_HashValue[5] += H5 & 0xFFFFFFFF;
  context->Intermediate_HashValue[6] += H6 & 0xFFFFFFFF;
  context->Intermediate_HashValue[7] += H7 & 0xFFFFFFFF;
}

//*****************************************************************************
/// \brief
///      According to the standard, the message must be padded to an even
///      512 bits.The first padding bit must be a '1'.The last 64 bits r-
///      epresent the length of the original message. All bits in between
///      should be 0. This function will pad the message according to th-
///      ose rules by filling the Message_Block array accordingly. It will
///      also call the ProcessMessageBlock function provided appropriately.
///      When it returns, it can be assumed that the message digest has b-
///      een computed.
/// \parameters
///      *context  
/// \returns   
///      void  
//*****************************************************************************
static void GN_SUPL_SHA256MessagePadding(s_GN_SUPL_SHA256Context *context)
{
    int i=0;

    context->Message_Block[context->Message_Block_Index++] = 0x80;

    for(i=context->Message_Block_Index ; i<64 ; i++ )
        {
          context->Message_Block[i] = 0;
        }

    /*
     *  Check to see if the current message block is too small to hold
     *  the initial padding bits and length.  If so, we will pad the
     *  block, process it, and then continue padding into a second
     *  block.
     */

    if ( !(context->Message_Block_Index <= 55))
     {
        GN_SUPL_SHA256MessageProcessBlock(context);
        context->Message_Block_Index = 0;
     }

    for(i= context->Message_Block_Index ; i< 56 ; i++ )
    {
          context->Message_Block[context->Message_Block_Index++] = 0;
    }
    /*
     *  Store the message length as the last 8 octets
     */
    for(i= 3 ; i >= 0 ; i-- )
    context->Message_Block[context->Message_Block_Index++] = context->Length_Msb >> (8*i)  & 0xFF ;
   
    for(i= 3 ; i >= 0 ; i-- )
    context->Message_Block[context->Message_Block_Index++] = context->Length_Lsb  >> (8*i)  & 0xFF ;
   
    GN_SUPL_SHA256MessageProcessBlock(context);
    context->Message_Block_Index = 0;
}

//*****************************************************************************
/// \brief
///      This function accepts an array of octets as the next portion of
///      the message.
/// \parameters
///      *context, *message, length  
/// \returns   
///      void  
//*****************************************************************************
void GN_SUPL_SHA256Input(s_GN_SUPL_SHA256Context *context, const U1 *Message_Block, U4 length)
{
    for( ;length > 0 ;length--)
    {

        context->Message_Block[context->Message_Block_Index++] = *Message_Block++;
        if (context->Message_Block_Index == 64)
        {
          GN_SUPL_SHA256MessageProcessBlock(context);
          context->Message_Block_Index = 0;
        }
        context->Length_Lsb+= 8;

        context->Length_Lsb &= 0xFFFFFFFF;

        if (context->Length_Lsb == 0)
            ++context->Length_Msb;

        context->Length_Msb &= 0xFFFFFFFF;
    }

}

//*****************************************************************************
/// \brief
///      This function will return the 256-bit message digest into the
///      Message_Digest array provided by the caller.
///      NOTE: The first octet of hash is stored in the 0th element, the
///      last octet of hash in the 31th element.
/// \parameters
///      *context, *digest  
/// \returns   
///      void  
//*****************************************************************************
void GN_SUPL_SHA256Output(s_GN_SUPL_SHA256Context *context,U1 *digest)
{
    int i,j=0;

    GN_SUPL_SHA256MessagePadding(context);

    for(i=0 ; i < SUPL_SHA256_SIZE ; i+=4 )
     {
    digest[i]  = context->Intermediate_HashValue[j] >> 24 ;
    digest[i+1]= context->Intermediate_HashValue[j] >> 16 ;
    digest[i+2]= context->Intermediate_HashValue[j] >> 8  ;
    digest[i+3]= context->Intermediate_HashValue[j++]     ;
     }
   for(i=0 ; i<64 ; i++ )
    {
    context->Message_Block[i]=0x00;
    }
   context->Length_Lsb =context->Length_Msb = 0;    /* and clear length */
}

//*****************************************************************************
/// \brief
///      This is the sha-256 hashing function.It wraps around the SHA256 hash implementation to provide one-shot hash 
///      functionality.
/// \parameters
///      *pData, dataLen, *pHash  
/// \returns   
///      int  
//*****************************************************************************
int GN_SUPL_GetSha256Hash(U1*  pData, U4  dataLen, U1*  pHash )
{


    s_GN_SUPL_SHA256Context    ctx;

    ctx.Length_Lsb= ctx.Length_Msb = 0;

    ctx.Intermediate_HashValue[0]   = 0x6A09E667;
    ctx.Intermediate_HashValue[1]   = 0xBB67AE85;
    ctx.Intermediate_HashValue[2]   = 0x3C6EF372;
    ctx.Intermediate_HashValue[3]   = 0xA54FF53A;
    ctx.Intermediate_HashValue[4]   = 0x510E527F;
    ctx.Intermediate_HashValue[5]   = 0x9B05688C;
    ctx.Intermediate_HashValue[6]   = 0x1F83D9AB;
    ctx.Intermediate_HashValue[7]   = 0x5BE0CD19;

    ctx.Message_Block_Index    = 0;

    GN_SUPL_SHA256Input ( &ctx , pData , dataLen );

    GN_SUPL_SHA256Output( &ctx , pHash );

    return TRUE;
}


//*****************************************************************************
/// \brief
///      Defines the hashing algorithm for SHA256.
/// \parameters
///      * pKey, keyLen, * pData,dataLen,*pHash  
/// \returns   
///      int  
//*****************************************************************************
int GN_SUPL_GetHmacSha256(
   U1*  pKey,           //
   U4  keyLen,          //
   U1*  pData,          //
   U4  dataLen,         //
   U1*  pHash          //

   )
{
    U1    *pInnerHash;
    U1    *pKeyUsed;
    U1    *MessageKey1Hash,*DataKey2Hash;
    U4     i=0;
    BL    retVal = TRUE;
    U4    keyLenUsed;
    U4    SizeOfHash;

    SizeOfHash = SIZE_SHA256_HASH_IN_BYTES;

 /* Allocate memory for Key1 || message   and   Key2 || hash(ipad || message )  */

    MessageKey1Hash = (U1*) calloc(1, SIZE_HMAC_BLOCK_IN_BYTES+ dataLen  );  
    DataKey2Hash    = (U1*) calloc(1, SIZE_HMAC_BLOCK_IN_BYTES+ SizeOfHash );

    /* The inner hash has to be appended to the DataKey2Hash and passed into the outer hash. Instead of
       appending, we point the inner hash directly to the outer hash buffer */
       
    pInnerHash = DataKey2Hash + SIZE_HMAC_BLOCK_IN_BYTES; /* SIZE_HMAC_BLOCK_IN_BYTES is length of Key2 */


/* ========== Key to be used for HMAC ========== */
    if( keyLen < SIZE_HMAC_BLOCK_IN_BYTES )
    {
        keyLenUsed   = keyLen;
        pKeyUsed     = pKey;
    }
    else                                                     // if (length(key) > blocksize) then
    {
        pKeyUsed   = (U1*) calloc(1,SizeOfHash);
        GN_SUPL_GetSha256Hash( pKey , keyLen ,pKeyUsed );    // key = hash(key) ,keys longer than blocksize are shortened to Block_Size
        keyLenUsed   = SizeOfHash;
    }
/* ========== End Key to be used for HMAC ========== */

/* ========== Generate Key1 and Key2 ========== */
    /*
        for i from 0 to length(key) - 1 step 1
            MessageKey1Hash[i] = Key1[i] XOR key[i]
            DataKey2Hash[i]    = Key2[i] XOR key[i]
        end for
      */


    while( i < keyLenUsed )
    {

      MessageKey1Hash[i] = pKeyUsed[i] ^ Key1;
      DataKey2Hash[i]    = pKeyUsed[i] ^ Key2;
      i++ ;

    }
    if( keyLenUsed < SIZE_HMAC_BLOCK_IN_BYTES )
    {
        memset( MessageKey1Hash + keyLenUsed , Key1 , SIZE_HMAC_BLOCK_IN_BYTES - keyLenUsed );
        memset( DataKey2Hash    + keyLenUsed , Key2 , SIZE_HMAC_BLOCK_IN_BYTES - keyLenUsed );
    }
/* ========== End Generate Key1 and Key2  ( MessageKey1Hash & DataKey2Hash ) ========== */

    /*  MessageKey1Hash || message */
    memcpy( MessageKey1Hash + SIZE_HMAC_BLOCK_IN_BYTES , pData , dataLen );                      
    /* hash( MessageKey1Hash || message ) */
    GN_SUPL_GetSha256Hash( MessageKey1Hash , SIZE_HMAC_BLOCK_IN_BYTES + dataLen , pInnerHash );  
    /* DataKey2Hash now is DataKey2Hash || ( hash( MessageKey1Hash || message ) ) */
    /* hash( DataKey2Hash || ( hash( MessageKey1Hash || message ) ) )*/
    GN_SUPL_GetSha256Hash( DataKey2Hash , SizeOfHash + SIZE_HMAC_BLOCK_IN_BYTES , pHash );   


    GN_Free( MessageKey1Hash );

    GN_Free( DataKey2Hash );

    return retVal;
}

/****************************** SHA1 procedure *************************************/
/***********************************************************************************/

void GN_SUPL_SHA1Reset(SHA1Context *);
U1 GN_SUPL_SHA1Result(SHA1Context *);
void GN_SUPL_SHA1Input( SHA1Context *,const U1 *, U4);
static void GN_SUPL_SHA1PadMessage(SHA1Context *);
void GN_SUPL_SHA1ProcessMessageBlock(SHA1Context *);
static void GN_SUPL_movebits(U4 * , U1 * );
static void GN_SUPL_sha1(U1 *fqdn, U1 *msg, U4 *MessageDigest, U4 length);

#define SHA1CircularShift(bits,word) \
                ((((word) << (bits)) & 0xFFFFFFFF) | \
                ((word) >> (32-(bits))))


//*****************************************************************************
/// \brief
///      This function pads the input into a multiple of 64 BYTES.
/// \parameters
///      *context
/// \returns   
///      void  
//*****************************************************************************

static void GN_SUPL_SHA1PadMessage(SHA1Context *context)
{
    int i=0;

    context->Message_Block[context->Message_Block_Index++] = 0x80;
    
    for(i=context->Message_Block_Index ; i<64 ; i++ )
        {
          context->Message_Block[i] = 0;
        }

    /*
     *  Check to see if the current message block is too small to hold
     *  the initial padding bits and length.  If so, we will pad the
     *  block, process it, and then continue padding into a second
     *  block.
     */

    if ( !(context->Message_Block_Index <= 55))
    {
        GN_SUPL_SHA1ProcessMessageBlock(context);
        context->Message_Block_Index = 0;
    }

    for(i= context->Message_Block_Index ; i< 56 ; i++ )
    {
          context->Message_Block[context->Message_Block_Index++] = 0;
    }
    /*
     *  Store the message length as the last 8 octets
     */
    for(i= 3 ; i >= 0 ; i-- )
    context->Message_Block[context->Message_Block_Index++] = context->Length_High >> (8*i)  & 0xFF ;
   
    for(i= 3 ; i >= 0 ; i-- )
    context->Message_Block[context->Message_Block_Index++] = context->Length_Low  >> (8*i)  & 0xFF ;
   
    GN_SUPL_SHA1ProcessMessageBlock(context);
    context->Message_Block_Index = 0;
}

//*****************************************************************************
/// \brief
///      This function process the message block using SHA-1 algorithm.
/// \parameters
///      *context
/// \returns   
///      void  
//*****************************************************************************
void GN_SUPL_SHA1ProcessMessageBlock(SHA1Context *context){
    const unsigned SHA1Constants[] =            /* Constants defined in SHA-1   */
    {
        0x5A827999,
        0x6ED9EBA1,
        0x8F1BBCDC,
        0xCA62C1D6
    };
    U2    loop_counter;       /* Loop counter                 */
    U4    temp_word_value;    /* Temporary word value         */
    U4    word_seq[80];       /* Word sequence                */
    U4    word_buff1, word_buff2, word_buff3, word_buff4, word_buff5;  /* Word buffers    */

    /*
     *  Initialize the first 16 words in the array word_seq
     */
    for(loop_counter = 0; loop_counter < 16; loop_counter++){
        word_seq[loop_counter] = ((unsigned) context->Message_Block[loop_counter * 4]) << 24;
        word_seq[loop_counter] |= ((unsigned) context->Message_Block[loop_counter * 4 + 1]) << 16;
        word_seq[loop_counter] |= ((unsigned) context->Message_Block[loop_counter * 4 + 2]) << 8;
        word_seq[loop_counter] |= ((unsigned) context->Message_Block[loop_counter * 4 + 3]);
    }

    for(loop_counter = 16; loop_counter < 80; loop_counter++)
       word_seq[loop_counter] = SHA1CircularShift(1,word_seq[loop_counter-3] 
                                ^ word_seq[loop_counter-8] ^ word_seq[loop_counter-14] ^ word_seq[loop_counter-16]);

    word_buff1 = context->Message_Digest[0];
    word_buff2 = context->Message_Digest[1];
    word_buff3 = context->Message_Digest[2];
    word_buff4 = context->Message_Digest[3];
    word_buff5 = context->Message_Digest[4];

    for(loop_counter = 0; loop_counter < 20; loop_counter++){
        temp_word_value =  SHA1CircularShift(5,word_buff1) +
                ((word_buff2 & word_buff3) | ((~word_buff2) & word_buff4)) + word_buff5 + word_seq[loop_counter] + SHA1Constants[0];
        temp_word_value &= 0xFFFFFFFF;
        word_buff5 = word_buff4;
        word_buff4 = word_buff3;
        word_buff3 = SHA1CircularShift(30,word_buff2);
        word_buff2 = word_buff1;
        word_buff1 = temp_word_value;
    }

    for(loop_counter = 20; loop_counter < 40; loop_counter++){
        temp_word_value = SHA1CircularShift(5,word_buff1) + (word_buff2 ^ word_buff3 ^ word_buff4) + word_buff5 + word_seq[loop_counter] + SHA1Constants[1];
        temp_word_value &= 0xFFFFFFFF;
        word_buff5 = word_buff4;
        word_buff4 = word_buff3;
        word_buff3 = SHA1CircularShift(30,word_buff2);
        word_buff2 = word_buff1;
        word_buff1 = temp_word_value;
    }

    for(loop_counter = 40; loop_counter < 60; loop_counter++){
        temp_word_value = SHA1CircularShift(5,word_buff1) +
               ((word_buff2 & word_buff3) | (word_buff2 & word_buff4) | (word_buff3 & word_buff4)) + word_buff5 + word_seq[loop_counter] + SHA1Constants[2];
        temp_word_value &= 0xFFFFFFFF;
        word_buff5 = word_buff4;
        word_buff4 = word_buff3;
        word_buff3 = SHA1CircularShift(30,word_buff2);
        word_buff2 = word_buff1;
        word_buff1 = temp_word_value;
    }

    for(loop_counter = 60; loop_counter < 80; loop_counter++){
        temp_word_value = SHA1CircularShift(5,word_buff1) + (word_buff2 ^ word_buff3 ^ word_buff4) + word_buff5 + word_seq[loop_counter] + SHA1Constants[3];
        temp_word_value &= 0xFFFFFFFF;
        word_buff5 = word_buff4;
        word_buff4 = word_buff3;
        word_buff3 = SHA1CircularShift(30,word_buff2);
        word_buff2 = word_buff1;
        word_buff1 = temp_word_value;
    }

    context->Message_Digest[0] =(context->Message_Digest[0] + word_buff1) & 0xFFFFFFFF;
    context->Message_Digest[1] =(context->Message_Digest[1] + word_buff2) & 0xFFFFFFFF;
    context->Message_Digest[2] =(context->Message_Digest[2] + word_buff3) & 0xFFFFFFFF;
    context->Message_Digest[3] =(context->Message_Digest[3] + word_buff4) & 0xFFFFFFFF;
    context->Message_Digest[4] =(context->Message_Digest[4] + word_buff5) & 0xFFFFFFFF;

    context->Message_Block_Index = 0;

}


//*****************************************************************************
/// \brief
///      This function process the final result of the hash.
/// \parameters
///      *context
/// \returns   
///      U8 ,0 - correctly computed,1 - corrupted  
//*****************************************************************************
U1 GN_SUPL_SHA1Result(SHA1Context *context){

    if (context->Corrupted)
        return 0;

    if (!context->Computed)
    {
        GN_SUPL_SHA1PadMessage(context);
        context->Computed = 1;
    }

    return 1;
}

//*****************************************************************************
/// \brief
///      This function recieves the input for hashing.
/// \parameters
///      *context, *message_array, length
/// \returns   
///      void  
//*****************************************************************************
void GN_SUPL_SHA1Input(SHA1Context *context, const U1 *message_array, U4 length){

    if (!length)
        return;

    if (context->Computed || context->Corrupted)
    {
        context->Corrupted = 1;
        return;
    }

    while(length-- && !context->Corrupted){
        context->Message_Block[context->Message_Block_Index++] =(*message_array & 0xFF);

        context->Length_Low += 8;
        /* Force it to 32 bits */
        context->Length_Low &= 0xFFFFFFFF;
        if (context->Length_Low == 0)
        {
            context->Length_High++;
            /* Force it to 32 bits */
            context->Length_High &= 0xFFFFFFFF;
            if (context->Length_High == 0)
            {
                /* Message is too long */
                context->Corrupted = 1;
            }
        }

        if (context->Message_Block_Index == 64){
            GN_SUPL_SHA1ProcessMessageBlock(context);
            context->Message_Block_Index = 0;

        }
        message_array++;
    }

}

//*****************************************************************************
/// \brief
///      This function Reset the initial parameters of the hash.
/// \parameters
///      *context
/// \returns   
///      void  
//*****************************************************************************
void GN_SUPL_SHA1Reset(SHA1Context *context){
    context->Length_Low             = 0;
    context->Length_High            = 0;
    context->Message_Block_Index    = 0;

    context->Message_Digest[0]      = 0x67452301;
    context->Message_Digest[1]      = 0xEFCDAB89;
    context->Message_Digest[2]      = 0x98BADCFE;
    context->Message_Digest[3]      = 0x10325476;
    context->Message_Digest[4]      = 0xC3D2E1F0;

    context->Computed   = 0;
    context->Corrupted  = 0;
}


//*****************************************************************************
/// \brief
///      This function copy the contents of U32 to U8.
/// \parameters
///      * source, * destination
/// \returns   
///      void  
//*****************************************************************************
static void GN_SUPL_movebits(U4 * source, U1 * destination ){

  U1 i, j, count;
  U4 temp;

  count=0;
  for(i=0;i<5;i++){
    for(j=4;j>0;j--){
      destination[count]=0x00;
      temp = source[i];
      temp>>=(8*(j-1));
      destination[count++]|=(U1)temp;
    }
  }
}


//*****************************************************************************
/// \brief
///      This is the sha-1 hashing function.
/// \parameters
///      *fqdn, *msg, *MessageDigest, length
/// \returns   
///      void  
//*****************************************************************************
static void GN_SUPL_sha1(U1 *fqdn, U1 *msg, U4 *MessageDigest, U4 length){

  SHA1Context sha;
  U1 i;

  GN_SUPL_SHA1Reset(&sha);
  GN_SUPL_SHA1Input(&sha, fqdn,64);
  GN_SUPL_SHA1Input(&sha, msg,length);

  if (!GN_SUPL_SHA1Result(&sha))
         printf("error\n");
  else{
    for(i=0;i<5;i++)
      MessageDigest[i] = sha.Message_Digest[i];
  }

}


//*****************************************************************************
/// \brief
///      Defines the hashing algorithm.
/// \details
///    The HMAC algorithm is defined as follows :
///    function hmac (key, message)
///    opad = [0x5c * blocksize] // Where blocksize is that of the underlying hash function
///   ipad = [0x36 * blocksize]
///
///    if (length(key) > blocksize) then
///        key = hash(key) // keys longer than blocksize are shortened
///    end if
///
///    for i from 0 to length(key) - 1 step 1
///        ipad[i] = ipad[i] XOR key[i]
///        opad[i] = opad[i] XOR key[i]
///    end for
///
///    return hash(opad || hash(ipad || message)) // Where || is concatenation
///    end function
/// \parameters
///      *pKey, keyLen, *msg, length, *output
/// \returns   
///      int  
//*****************************************************************************
int GN_SUPL_GetHmacSha1Block64(
   U1* pKey,            //
   void *msg,           //
   U4 length,         //
   U1 *output )          //
{ /* hash will be store in ouput */
    U1 *temp = (U1 *)malloc(length);//length is greater than U16 so malloc is used
    U4 MessageDigest[5];
    /* www.spirent-lcs.com: fqdn for the ULTS */
    U1 K[64];
    //U8 K[64] = "www.spirent-lcs.com";
    U1 temp1[64];
    U1 c[20];
    U1 i;

    for(i=0;i<64;i++)
      K[i] = 0x00;

    strcpy((CH*)K,(CH*)pKey) ;

    memcpy(temp, msg, length);
    for(i=0;i<64;i++)
        temp1[i]= K[i] ^ 0x36;

    GN_SUPL_sha1(temp1,temp,MessageDigest, length);
    GN_SUPL_movebits(MessageDigest, c);
    for(i=0;i<64;i++)
        temp1[i] = K[i] ^ 0x5c;
    GN_SUPL_sha1(temp1,c,MessageDigest, 20);
    GN_SUPL_movebits(MessageDigest, c);
    for(i=0;i<8;i++)
      output[i] = c[i];
    return TRUE;
}

//****************************************************************************
//****************************************************************************
