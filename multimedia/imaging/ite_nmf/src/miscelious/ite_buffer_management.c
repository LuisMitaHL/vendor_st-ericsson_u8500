/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#include "ite_buffer_management.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MMTE_USB_TRUE
#    include <time.h>
#endif /*MMTE_USB_TRUE*/

//For 8Mpx Sensor
#define MAX_WIDTH_SENSOR 3280
#define MAX_HEIGHT_SENSOR 2464

extern t_uint32 ITE_readPE(t_uint16);

//To store bmp data before writing to file
//t_uint8 bmpData[MAX_WIDTH_SENSOR*MAX_HEIGHT_SENSOR*3];

/********************************************************/
/*  t_uint8 ITE_StoreBufferInFile(char*     filename,   */
/*  ts_siapicturebuffer* p_Buffer, e_ITE_FileType type) */
/*  store buffer in file                        */
/********************************************************/
t_uint8 ITE_StoreBufferInFile(char* filename,ts_siapicturebuffer* p_Buffer, e_ITE_FileType type)
{
   t_los_file *out_file;
   t_uint32 gam_header[11];
#ifndef MMTE_USB_TRUE
  time_t rawtime;
#else
   long board_time = LOS_getSystemTime();
#endif
   char out_filename[100]="";
   char date[30]="";

   if (strlen(filename)>60)
   {
    LOS_Log("file name too long !!!!!!! \n",  out_filename,NULL,NULL,NULL,NULL,NULL);
    return FALSE;
   }

#ifndef MMTE_USB_TRUE /* with USB, no semihosting, and time() isn't implemented */
  //get local date
  time ( &rawtime );
  strncpy(date,ctime (&rawtime), 29);//29 to allow null termination on "date" destination buffer
  //replace LF and tab by underscore
  date[3]=date[7]=date[10]=date[13]=date[16]=date[19]=date[24]=95;
#else
  sprintf(date, "_usb_nodate_%d", board_time);
#endif /* MMTE_USB_TRUE */

  // copy filename
  strcpy(out_filename,filename);
  //add local date
  strcat(out_filename,date);

   // add extension according to the type
   switch(type)
   {
    case ITE_FILE_GAM:  strcat(out_filename,".gam");
    break;
    case ITE_FILE_RAW:  strcat(out_filename,".raw");
    break;
    default:
    break;
   }

   out_file = LOS_fopen(out_filename,"wb");
   if(out_file==NULL)
   {
        LOS_Log("Open file error %s!!!!!!!!!!!!\n",  out_filename,NULL,NULL,NULL,NULL,NULL);
        return FALSE;
   }
   // add header according to the type
   switch(type)
   {
    case ITE_FILE_GAM:

    //write gamma header
    gam_header[0]=0x444f0006;
    gam_header[1]=0x00000080;
    gam_header[2]=p_Buffer->framesizeX;
    gam_header[3]=p_Buffer->framesizeY;
    gam_header[4]=p_Buffer->framesizeX*p_Buffer->framesizeY;
    gam_header[5]=0x00000000;
    LOS_fwrite((t_uint8 *)(gam_header), 6*4, 1, out_file);
    break;

    default:
    break;
  }

   LOS_fwrite((t_uint8 *)(p_Buffer->logAddress), (p_Buffer->framesizeX*p_Buffer->framesizeY*2), 1, out_file);
   LOS_fclose(out_file);
   LOS_Log("output file %s written\n",  out_filename,NULL,NULL,NULL,NULL,NULL);

   return TRUE;
}

/********************************************************/
/*  t_uint8 ITE_LoadBufferFromFile(char* filename,      */
/*  ts_siapicturebuffer* p_Buffer)                      */
/*  load buffer from file                       */
/********************************************************/
t_uint8 ITE_LoadBufferFromFile(char* filename,ts_siapicturebuffer* p_Buffer)
{
   t_los_file *out_file;
   size_t file_size;
   size_t read;

   out_file = LOS_fopen(filename,"r");
   if(out_file==NULL)
   {
        LOS_Log("Open file error !!!!!!!!!!!!\n",  NULL,NULL,NULL,NULL,NULL,NULL);
        return FALSE;
   }
   else
   {
       LOS_fseek(out_file, 0, LOS_SEEK_END);
       file_size = (size_t)LOS_ftell(out_file);
       LOS_fseek(out_file, 0, LOS_SEEK_SET);
       LOS_Log("file size is %i bytes\n",  (void*)file_size, NULL, NULL, NULL, NULL, NULL);

       read = LOS_fread((void *)(p_Buffer->logAddress), 1, file_size, out_file);
       if (read != file_size)
       {
            LOS_Log("Read %i bytes instead of %i\n", (void*)read, (void*)file_size, NULL, NULL, NULL, NULL);
            LOS_fclose(out_file);
            return FALSE;
       }
       else
       {
           LOS_fclose(out_file);
           LOS_Log("output file %s written\n",  filename,NULL,NULL,NULL,NULL,NULL);
           return TRUE;
       }
   }

}

/********************************************************/
/*  t_uint8 ITE_CompareBuffers(t_uint8*  p_Buffer0,     */
/*         t_uint8* p_Buffer, t_uint32 buffer_size)     */
/*  compare 2 buffers, return FALSE if not equal    */
/********************************************************/
t_uint8 ITE_CompareBuffers(t_uint32  adr_Buffer0,t_uint32 adr_Buffer1, t_uint32 buffer_size)
{
 t_uint32 i=0;
 t_uint8* p_Buffer0;
 t_uint8* p_Buffer1;
 p_Buffer0=(t_uint8*)adr_Buffer0;
 p_Buffer1=(t_uint8*)adr_Buffer1;

   while(i!=buffer_size)
   {
    if( *(p_Buffer0+i)!=*(p_Buffer1+i) ) return FALSE;

    i++;
   };

   return TRUE;
}

/********************************************************/
/*  t_uint8 ITE_DuplicateBuffer(tps_siapicturebuffer p_sourcebuffer,         */
/*         tps_siapicturebuffer p_destbuffer)                        */
/*  copy source buffer content to destination buffer                */
/* ITE_createSiaPictureBuffer is supposed to be called previously       */
/* Buffers are supposed to be the same size                     */
/********************************************************/
void ITE_DuplicateBuffer(tps_siapicturebuffer p_sourcebuffer, tps_siapicturebuffer p_destbuffer)
{
   memcpy((void *) p_destbuffer->logAddress, (void *) p_sourcebuffer->logAddress,  p_sourcebuffer->buffersizeByte);

}

/********************************************************/
/*  void ITE_FillBufferWith(t_uint8* p_Buffer,          */
/* t_uint32 buffer_size, t_int8 value)                  */
/*  init full buffer with value                     */
/********************************************************/
void ITE_FillBufferWith(t_uint32 adr_Buffer, t_uint32 buffer_size, t_uint8 value)
{
 t_uint32 i;
 t_uint8* p_Buffer;
 p_Buffer=(t_uint8*)adr_Buffer;
 for(i=0;i<buffer_size;i++) *(p_Buffer+i)=value;
}


void ITE_CleanBuffer(ts_siapicturebuffer* p_Buffer)
{
    ITE_FillBufferWith(p_Buffer->logAddress,p_Buffer->buffersizeByte,0);
}

/********************************************************/
/*  t_uint8 ITE_CheckGrabInBuffer(t_uint32 adr_Buffer,  */
/* t_uint32 grab_size, t_uint8 default_value)           */
/* check if buffer filled by the grab.                  */
/* return TRUE if more than 18 samples changed      */
/********************************************************/
t_uint8 ITE_CheckGrabInBuffer(t_uint32 adr_Buffer, t_uint32 grab_size, t_uint8 default_value)
{
 t_uint32 i=0;
 t_uint8 flag=0;
 t_uint8* p_Buffer;
 t_uint32 temp;
 p_Buffer = (t_uint8*)adr_Buffer;

 // check samples in the buffer
   for(i=0;i<=20;i++)
   {
    // if last byte != default_value
    temp=( (grab_size-1)/20*i);
    if( *( p_Buffer+temp )!= default_value ) flag++;
   }

   // if last byte != default_value
   if(*(p_Buffer+grab_size-1)!= default_value) flag++;

   // return result TRUE if flag > 18
   if(flag>18) return TRUE;
   else       return FALSE;
}

/*********************************************************/
/* t_uint8 ITE_CheckBufsamevalue(t_uint32 adr_Buffer,    */
/* t_uint32 grab_size)                       */
/* check if buffer filled by the grab.                   */
/* return TRUE if more than 18 samples not equal 0 &&    */
/*                     not equal 0xff && */
/*                     not same      */
/*********************************************************/
t_uint8 ITE_CheckBufsamevalue(t_uint32 adr_Buffer, t_uint32 grab_size)
{
 t_uint32 i=0;
 t_uint8 flag=0;
 t_uint8* p_Buffer;
 t_uint32 temp;
 t_uint8 first = 0;
 t_uint8 result0 = FALSE;
 t_uint8 result1 = FALSE;
 t_uint8 result2 = FALSE;
 p_Buffer = (t_uint8*)adr_Buffer;

 result0 = ITE_CheckGrabInBuffer(adr_Buffer,grab_size, 0);
 result1 = ITE_CheckGrabInBuffer(adr_Buffer,grab_size, 0xff);


 // check samples in the buffer
   for(i=0;i<=20;i++)
   {
    // if last byte != default_value
    temp=( (grab_size-1)/20*i);
    if (i==0) first = temp;
    if( *( p_Buffer+temp )!= first ) flag++;
   }

   // if last byte != default_value
   if(*(p_Buffer+grab_size-1)!= first) flag++;

   // return result TRUE if flag > 18
   if(flag>18) result2 = TRUE;
   else        result2 = FALSE;

   if ((result0==TRUE) && (result1==TRUE) && (result2==TRUE))
       return TRUE;
   else return FALSE;
}




t_uint16 ITE_GetRGBvalue(ts_siapicturebuffer* p_Buffer,t_uint32 Xcoord,t_uint32 Ycoord,ts_PelRGB888 *Pel) {

t_uint32 offset_byte;
t_uint32 AddrMBLuma;
t_uint32 AddrMBChroma;
t_uint32 AddrLuma;
t_uint32 AddrCr;
t_uint32 AddrCb;
t_uint32 RedAddr,BlueAddr,GreenBAddr,GreenRAddr,LogAddrPicture;
double RedAddr_fl,BlueAddr_fl,GreenBAddr_fl,GreenRAddr_fl;
t_uint16 RedValue,BlueValue,GreenBValue,GreenRValue;
//t_uint16 PelOrder;
t_uint32 OffsetLuma;
t_uint32 OffsetChroma;
t_uint8 Lumavalue;
t_uint16 Chromavalue;
t_uint8 Cbvalue;
t_uint8 Crvalue;
t_sint8 signCbvalue;
t_sint8 signCrvalue;
t_uint16 Rvalue;
t_uint16 Gvalue;
t_uint16 Bvalue;
int CrCb = 0;

if (Xcoord > p_Buffer->framesizeX) {
    LOS_Log("WARNING Xcoord bigger than picture size X\n", NULL,NULL,NULL,NULL,NULL,NULL);
    // mle Pel->RedValue=Pel->GreenValue=Pel->BlueValue=0;
    // mle return (1);
    }
if (Ycoord > p_Buffer->framesizeY) {
    LOS_Log("WARNING Ycoord bigger than picture size Y\n", NULL,NULL,NULL,NULL,NULL,NULL);
    // mle Pel->RedValue=Pel->GreenValue=Pel->BlueValue=0;
    // mle return (1);
    }
switch (p_Buffer->Grb_colorformat) {
    case (GRBFMT_R5G6B5) :
            offset_byte = (Ycoord * (p_Buffer->framesizeX) + Xcoord) * 2;
        Pel->RedValue =   0xff & (( (*((t_uint16 *)((p_Buffer->logAddress) + offset_byte))) & 0xf800) >> 8);
        Pel->GreenValue = 0xff & (( (*((t_uint16 *)((p_Buffer->logAddress) + offset_byte))) & 0x07e0) >> 3);
        Pel->BlueValue =  0xf8 & (( (*((t_uint16 *)((p_Buffer->logAddress) + offset_byte))) & 0x001f) << 3);
        break;
    case (GRBFMT_A4R4G4B4):
            offset_byte = (Ycoord * (p_Buffer->framesizeX) + Xcoord) * 2;
        Pel->RedValue =    0xff & (( (*((t_uint16 *)((p_Buffer->logAddress) + offset_byte))) & 0x0f00) >> 4);
        Pel->GreenValue =  0xff & (( (*((t_uint16 *)((p_Buffer->logAddress) + offset_byte))) & 0x00f0));
        Pel->BlueValue =   0xf0 & (( (*((t_uint16 *)((p_Buffer->logAddress) + offset_byte))) & 0x000f) <<4);
        break;
    case (GRBFMT_A1R5G5B5):
            offset_byte = (Ycoord * (p_Buffer->framesizeX) + Xcoord) * 2;
        Pel->RedValue =   0xff & (( (*((t_uint16 *)((p_Buffer->logAddress) + offset_byte))) & 0x7c00) >> 7);
        Pel->GreenValue = 0xff & (( (*((t_uint16 *)((p_Buffer->logAddress) + offset_byte))) & 0x03e0) >> 2);
        Pel->BlueValue =  0xf8 & (( (*((t_uint16 *)((p_Buffer->logAddress) + offset_byte))) & 0x001f) << 3);
        break;
    case (GRBFMT_A8R8G8B8):
            offset_byte = (Ycoord * (p_Buffer->framesizeX) + Xcoord) * 4;
        Pel->RedValue =   0xff & (( (*((t_uint32 *)((p_Buffer->logAddress) + offset_byte))) & 0x00ff0000) >> 16);
        Pel->GreenValue = 0xff & (( (*((t_uint32 *)((p_Buffer->logAddress) + offset_byte))) & 0x0000ff00) >> 8);
        Pel->BlueValue =  0xff & (( (*((t_uint32 *)((p_Buffer->logAddress) + offset_byte))) & 0x000000ff));
        break;
    case (GRBFMT_R8G8B8):
            offset_byte = (Ycoord * (p_Buffer->framesizeX) + Xcoord) * 3;
        Pel->RedValue = 0xff & (( (*((t_uint32 *)((p_Buffer->logAddress) + offset_byte))) & 0x00ff0000) >> 16);
        Pel->GreenValue = 0xff & (( (*((t_uint32 *)((p_Buffer->logAddress) + offset_byte))) & 0x0000ff00) >> 8);
        Pel->BlueValue = 0xff & (( (*((t_uint32 *)((p_Buffer->logAddress) + offset_byte))) & 0x000000ff) >> 0);
        break;
    case (GRBFMT_RGB30):
            offset_byte = (Ycoord * (p_Buffer->framesizeX) + Xcoord) * 4;
        Pel->RedValue =   0xff & (( (*((t_uint32 *)((p_Buffer->logAddress) + offset_byte))) & 0x3ff00000) >> 22);
        Pel->GreenValue = 0xff & (( (*((t_uint32 *)((p_Buffer->logAddress) + offset_byte))) & 0x000ffc00) >> 12);
        Pel->BlueValue =  0xff & (( (*((t_uint32 *)((p_Buffer->logAddress) + offset_byte))) & 0x000003ff) >> 2);
        break;
    case (GRBFMT_YUV422_RASTER_INTERLEAVED):
        CrCb = 0;
        AddrLuma = (p_Buffer->logAddress);
            offset_byte = (Ycoord * (p_Buffer->framesizeX) + Xcoord) * 2;
        Lumavalue =  ((*((t_uint16 *)(AddrLuma + offset_byte))) & 0xff00) >> 8;
        if ((Xcoord % 2) == 0) {
            Cbvalue = (*((t_uint16 *)(AddrLuma + offset_byte))) & 0x00ff;
        if(  (Xcoord ==  (p_Buffer->framesizeX-1)) && (Ycoord == (p_Buffer->framesizeY-1) ) ){
            Crvalue = (*((t_uint16 *)(AddrLuma + offset_byte - 2))) & 0x00ff;
            }
        else
            {
            Crvalue = (*((t_uint16 *)(AddrLuma + offset_byte + 2))) & 0x00ff;
            }
            }
        else {   // Warning: how can we compute last pel in the line?
            Crvalue = (*((t_uint16 *)(AddrLuma + offset_byte))) & 0x00ff;
        if(  (Xcoord ==  (p_Buffer->framesizeX-1)) && (Ycoord == (p_Buffer->framesizeY-1) ) ){
            Cbvalue = (*((t_uint16 *)(AddrLuma + offset_byte - 2))) & 0x00ff;
            }
        else
            {
            Cbvalue = (*((t_uint16 *)(AddrLuma + offset_byte + 2))) & 0x00ff;
            }
            }
        signCrvalue = (t_sint8)(Crvalue - 0x80);
        signCbvalue = (t_sint8)(Cbvalue - 0x80);
        if (CrCb == 1) {
            signCbvalue = (t_sint8)(Crvalue - 0x80);
                signCrvalue = (t_sint8)(Cbvalue - 0x80);
                }
        Rvalue = (float)Lumavalue + (1.402 * (float)signCrvalue);
        Gvalue = (float)Lumavalue - (0.34414 * (float)signCbvalue) - (0.71414* (float)signCrvalue);
        Bvalue = (float)Lumavalue + (1.772 * (float)signCbvalue);
        if (Rvalue > 255) Pel->RedValue=255; else Pel->RedValue = Rvalue;
        if (Gvalue > 255) Pel->GreenValue=255; else Pel->GreenValue = Gvalue;
        if (Bvalue > 255) Pel->BlueValue=255; else Pel->BlueValue = Bvalue;
        break;
    case (GRBFMT_YUV420_RASTER_PLANAR_I420):   // Blue Chroma first
        AddrLuma = (p_Buffer->logAddress);
        AddrCb = (p_Buffer->logAddress) + ((p_Buffer->framesizeX)*(p_Buffer->framesizeY));
        AddrCr = (p_Buffer->logAddress) + ((p_Buffer->framesizeX)*(((p_Buffer->framesizeY)*5)/4));
        OffsetLuma = (Ycoord * (p_Buffer->framesizeX) + Xcoord);
        OffsetChroma = (((Ycoord/2) * ((p_Buffer->framesizeX)/2)) + (Xcoord/2));
        Lumavalue = *((t_uint8 *)(AddrLuma + OffsetLuma));
        Cbvalue = *((t_uint8 *)(AddrCb + OffsetChroma));
        Crvalue = *((t_uint8 *)(AddrCr + OffsetChroma));
        signCrvalue = (t_sint8)(Crvalue - 0x80);
        signCbvalue = (t_sint8)(Cbvalue - 0x80);
        Rvalue = (float)Lumavalue + (1.402 * (float)signCrvalue);
        Gvalue = (float)Lumavalue - (0.34414 * (float)signCbvalue) - (0.71414* (float)signCrvalue);
        Bvalue = (float)Lumavalue + (1.772 * (float)signCbvalue);
        if (Rvalue > 255) Pel->RedValue=255; else Pel->RedValue = Rvalue;
        if (Gvalue > 255) Pel->GreenValue=255; else Pel->GreenValue = Gvalue;
        if (Bvalue > 255) Pel->BlueValue=255; else Pel->BlueValue = Bvalue;
        break;
    case (GRBFMT_YUV420_RASTER_PLANAR_YV12):   // Red Chroma first
        AddrLuma = (p_Buffer->logAddress);
        AddrCr = (p_Buffer->logAddress) + ((p_Buffer->framesizeX)*(p_Buffer->framesizeY));
        AddrCb = (p_Buffer->logAddress) + ((p_Buffer->framesizeX)*(((p_Buffer->framesizeY)*5)/4));
        OffsetLuma = (Ycoord * (p_Buffer->framesizeX) + Xcoord);
        OffsetChroma = (((Ycoord/2) * ((p_Buffer->framesizeX)/2)) + (Xcoord/2));
        Lumavalue = *((t_uint8 *)(AddrLuma + OffsetLuma));
        Cbvalue = *((t_uint8 *)(AddrCb + OffsetChroma));
        Crvalue = *((t_uint8 *)(AddrCr + OffsetChroma));
        signCrvalue = (t_sint8)(Crvalue - 0x80);
        signCbvalue = (t_sint8)(Cbvalue - 0x80);
        Rvalue = (float)Lumavalue + (1.402 * (float)signCrvalue);
        Gvalue = (float)Lumavalue - (0.34414 * (float)signCbvalue) - (0.71414* (float)signCrvalue);
        Bvalue = (float)Lumavalue + (1.772 * (float)signCbvalue);
        if (Rvalue > 255) Pel->RedValue=255; else Pel->RedValue = Rvalue;
        if (Gvalue > 255) Pel->GreenValue=255; else Pel->GreenValue = Gvalue;
        if (Bvalue > 255) Pel->BlueValue=255; else Pel->BlueValue = Bvalue;
        break;
    case (GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED):
    case (GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED):
        CrCb = 0;
        AddrMBLuma =   (p_Buffer->logAddress) + ((Ycoord/16)*((p_Buffer->framesizeX)/16) + (Xcoord/16))*(32*8);
        AddrMBChroma = (p_Buffer->logAddress) + ((p_Buffer->framesizeX)*(p_Buffer->framesizeY))
                                       + ((Ycoord/16)*((p_Buffer->framesizeX)/16) + (Xcoord/16))*(16*8);
        OffsetLuma = (((((Xcoord % 16) / 8) * 16) + (Ycoord % 16)) * 8) + ((Xcoord % 16)%8);
        OffsetChroma = (((((Xcoord % 16) / 8) * 8) + (Ycoord % 8)) * 8) + ((Xcoord % 16)%8);
        Lumavalue = *((t_uint8 *)(AddrMBLuma + OffsetLuma));
        Chromavalue = *((t_uint16 *)((AddrMBChroma + OffsetChroma) & 0xfffffffe));
        Crvalue = (Chromavalue & 0xff00)>>8;
        Cbvalue = (Chromavalue & 0xff);
        signCrvalue = (t_sint8)(Crvalue - 0x80);
        signCbvalue = (t_sint8)(Cbvalue - 0x80);
        if (CrCb == 1) {
            signCbvalue = (t_sint8)(Crvalue - 0x80);
                signCrvalue = (t_sint8)(Cbvalue - 0x80);
                }
        Rvalue = (float)Lumavalue + (1.402 * (float)signCrvalue);
        Gvalue = (float)Lumavalue - (0.34414 * (float)signCbvalue) - (0.71414* (float)signCrvalue);
        Bvalue = (float)Lumavalue + (1.772 * (float)signCbvalue);
        if (Rvalue > 255) Pel->RedValue=255; else Pel->RedValue = Rvalue;
        if (Gvalue > 255) Pel->GreenValue=255; else Pel->GreenValue = Gvalue;
        if (Bvalue > 255) Pel->BlueValue=255; else Pel->BlueValue = Bvalue;
/*      LOS_Log("xcoor: %d\t",Xcoord);
        LOS_Log("ycoord: %d\t",Ycoord);
        LOS_Log("AddrBuffer: %x\t",p_Buffer->logAddress);
        LOS_Log("AddrMBLuma: %x\t",AddrMBLuma);
        LOS_Log("AddrMBChroma: %x\t",AddrMBChroma);
        LOS_Log("LumaOffset: %d\t",OffsetLuma);
        LOS_Log("ChromaOffset: %d\n",OffsetChroma);
        LOS_Log("Lumavalue: %x\t",Lumavalue);
        LOS_Log("Chromavalue: %x\t",Chromavalue);
        LOS_Log("Cbvalue: %x\t",Cbvalue);
        LOS_Log("Crvalue: %x\n",Crvalue);
        LOS_Log("Pel Red: %x Green: %x Blue: %x\n",Pel->RedValue,Pel->GreenValue,Pel->BlueValue); */
        break;
    case (GRBFMT_YUV422_MB_SEMIPLANAR):
        CrCb = 0;
        AddrMBLuma =   (p_Buffer->logAddress) + ((Ycoord/16)*((p_Buffer->framesizeX)/16) + (Xcoord/16))*(32*8);
        AddrMBChroma = (p_Buffer->logAddress) + ((p_Buffer->framesizeX)*(p_Buffer->framesizeY))
                                       + ((Ycoord/16)*((p_Buffer->framesizeX)/16) + (Xcoord/16))*(32*8);
        OffsetLuma = (((((Xcoord % 16) / 8) * 16) + (Ycoord % 16)) * 8) + ((Xcoord % 16)%8);
        OffsetChroma = (((((Xcoord % 16) / 8) * 16) + (Ycoord % 16)) * 8) + ((Xcoord % 16)%8);
        Lumavalue = *((t_uint8 *)(AddrMBLuma + OffsetLuma));
        Chromavalue = *((t_uint16 *)((AddrMBChroma + OffsetChroma) & 0xfffffffe));
        Crvalue = (Chromavalue & 0xff00)>>8;
        Cbvalue = (Chromavalue & 0xff);
        signCrvalue = (t_sint8)(Crvalue - 0x80);
        signCbvalue = (t_sint8)(Cbvalue - 0x80);
        /*
        if (CrCb == 1) {
            signCbva     ITE_GetRGBvalue(&(GrabBufferLR[0]),ITE_ConvToInt32(Xpos),ITE_ConvToInt32(Ypos),&color);
     LOS_Log("Pel Red: %x Green: %x Blue: %x\n",color->RedValue,color->GreenValue,color->BlueValue);
lue = (t_sint8)(Crvalue - 0x80);
                signCrvalue = (t_sint8)(Cbvalue - 0x80);
                }
        */
        Rvalue = (float)Lumavalue + (1.402 * (float)signCrvalue);
        Gvalue = (float)Lumavalue - (0.34414 * (float)signCbvalue) - (0.71414* (float)signCrvalue);
        Bvalue = (float)Lumavalue + (1.772 * (float)signCbvalue);
        if (Rvalue > 255) Pel->RedValue=255; else Pel->RedValue = Rvalue;
        if (Gvalue > 255) Pel->GreenValue=255; else Pel->GreenValue = Gvalue;
        if (Bvalue > 255) Pel->BlueValue=255; else Pel->BlueValue = Bvalue;
        break;
    case (GRBFMT_RAW8): //  + (nb_of_embedded_line*(p_Buffer->framesizeX))

    case (GRBFMT_FAST_RAW8): //  + (nb_of_embedded_line*(p_Buffer->framesizeX))
        //PelOrder = 0;  

        LogAddrPicture = p_Buffer->logAddress + (p_Buffer->nonactivelines * p_Buffer->framesizeX);
        GreenRAddr = (LogAddrPicture) + ((Ycoord&0xfffe)*(p_Buffer->framesizeX)) + (Xcoord & 0xFFFE);
        GreenBAddr = (LogAddrPicture) + ((Ycoord|0x1)*(p_Buffer->framesizeX)) + (Xcoord|0x1);
        RedAddr = (LogAddrPicture) + ((Ycoord&0xfffe)*(p_Buffer->framesizeX)) + (Xcoord|0x1);
        BlueAddr = (LogAddrPicture) + ((Ycoord|0x1)*(p_Buffer->framesizeX)) + (Xcoord&0xfffe);
    /*
        Pel->RedValue = (*((volatile unsigned char *) (RedAddr))) ;    //color.RedValue;
        Pel->GreenValue = ((*((volatile unsigned char *)(GreenRAddr)))+(*((volatile unsigned char *) (GreenBAddr))))/2; //color.GreenValue;
        Pel->BlueValue = (*((volatile unsigned char *) (BlueAddr))) ;    //color.BlueValue;
    */
        switch (p_Buffer->pelorder) {
            case (0):    // GR
                Pel->RedValue = (*((volatile unsigned char *) (RedAddr))) ;
                Pel->GreenValue = ((*((volatile unsigned char *)(GreenRAddr)))+(*((volatile unsigned char *) (GreenBAddr))))/2;
                Pel->BlueValue = (*((volatile unsigned char *) (BlueAddr))) ;
                break;
            case (1):    // RG
                Pel->RedValue = (*((volatile unsigned char *) (GreenRAddr))) ;
                Pel->GreenValue = ((*((volatile unsigned char *)(RedAddr)))+(*((volatile unsigned char *) (BlueAddr))))/2;
                Pel->BlueValue = (*((volatile unsigned char *) (GreenBAddr))) ;
                break;
            case (2):    // BG
                Pel->RedValue = (*((volatile unsigned char *) (GreenBAddr))) ;
                Pel->GreenValue = ((*((volatile unsigned char *)(RedAddr)))+(*((volatile unsigned char *) (BlueAddr))))/2;
                Pel->BlueValue = (*((volatile unsigned char *) (GreenRAddr))) ;
                break;
            case (3):    //  GB
                Pel->RedValue = (*((volatile unsigned char *) (BlueAddr))) ;
                Pel->GreenValue = ((*((volatile unsigned char *)(GreenRAddr)))+(*((volatile unsigned char *) (GreenBAddr))))/2;
                Pel->BlueValue = (*((volatile unsigned char *) (RedAddr))) ;
                break;
            }

        break;
    case (GRBFMT_RAW12): //  + (nb_of_embedded_line*(p_Buffer->framesizeX))

    case (GRBFMT_FAST_RAW12): //  + (nb_of_embedded_line*(p_Buffer->framesizeX))
        //PelOrder = 3;

        LogAddrPicture = p_Buffer->logAddress + (p_Buffer->nonactivelines * p_Buffer->framesizeX * 1.5);
        GreenRAddr_fl = (1.5)*(((Ycoord&0xfffe)*(p_Buffer->framesizeX)) + (Xcoord & 0xFFFE));
        GreenBAddr_fl = (1.5)*(((Ycoord|0x1)*(p_Buffer->framesizeX)) + (Xcoord|0x1));
        RedAddr_fl = (1.5)*(((Ycoord&0xfffe)*(p_Buffer->framesizeX)) + (Xcoord|0x1));
        BlueAddr_fl = (1.5)*(((Ycoord|0x1)*(p_Buffer->framesizeX)) + (Xcoord&0xfffe));
        GreenRAddr = GreenRAddr_fl;
        GreenBAddr = GreenBAddr_fl;
        RedAddr = RedAddr_fl;
        BlueAddr = BlueAddr_fl;
        if (GreenRAddr == GreenRAddr_fl)
            GreenRValue =  (*((volatile char *) ((LogAddrPicture) + GreenRAddr)))
                     + (((*((volatile char *) ((LogAddrPicture) + GreenRAddr + 2)))&0x0f) << 8);
        else
            GreenRValue = ((*((volatile char *) ((LogAddrPicture) + GreenRAddr))) & 0xff)
                     + (((*((volatile char *) ((LogAddrPicture) + GreenRAddr +1))) & 0xf0) << 4);
        if (GreenBAddr == GreenBAddr_fl)
            GreenBValue =  (*((volatile char *) ((LogAddrPicture) + GreenBAddr)))
                     + (((*((volatile char *) ((LogAddrPicture) + GreenBAddr + 2)))&0x0f) << 8);
        else
            GreenBValue = ((*((volatile char *) ((LogAddrPicture) + GreenBAddr))) & 0xff)
                     + (((*((volatile char *) ((LogAddrPicture) + GreenBAddr +1))) & 0xf0) << 4);
        if (RedAddr == RedAddr_fl)
            RedValue = (*((volatile char *) ((LogAddrPicture) + RedAddr)))
                  + (((*((volatile char *) ((LogAddrPicture) + RedAddr + 2)))&0x0f) << 8);
        else
            RedValue = ((*((volatile char *) ((LogAddrPicture) + RedAddr))) & 0xff)
                     + (((*((volatile char *) ((LogAddrPicture) + RedAddr +1))) & 0xf0) << 4);
        if (BlueAddr == BlueAddr_fl)
            BlueValue = (*((volatile char *) ((LogAddrPicture) + BlueAddr)))
                  + (((*((volatile char *) ((LogAddrPicture) + BlueAddr + 2)))&0x0f) << 8);
        else
            BlueValue = ((*((volatile char *) ((LogAddrPicture) + BlueAddr))) & 0xff)
                     + (((*((volatile char *) ((LogAddrPicture) + BlueAddr +1))) & 0xf0) << 4);
        switch (p_Buffer->pelorder) {
            case (0):    // GR
                Pel->RedValue = (RedValue >> 4) ;
                Pel->GreenValue = (((GreenRValue + GreenBValue)/2)>>4);
                Pel->BlueValue = (BlueValue >> 4) ;
                break;
            case (1):    // RG
                Pel->RedValue = (GreenRValue >> 4) ;
                Pel->GreenValue = (((RedValue + BlueValue)/2)>>4);
                Pel->BlueValue = (GreenBValue >> 4) ;
                break;
            case (2):    // BG
                Pel->RedValue = (GreenBValue >> 4) ;
                Pel->GreenValue = (((RedValue + BlueValue)/2)>>4);
                Pel->BlueValue = (GreenRValue >> 4) ;
                break;
            case (3):    //  GB
                Pel->RedValue = (BlueValue >> 4) ;
                Pel->GreenValue = (((GreenRValue + GreenBValue)/2)>>4);
                Pel->BlueValue = (RedValue >> 4) ;
                break;
            }
        /*
        LOS_Log("xcoor: %d\t",Xcoord);
        LOS_Log("ycoord: %d\n",Ycoord);
        LOS_Log("GreenRAddr and GreenRaddr_float: %d\t%f\n",GreenRAddr,GreenRAddr_fl);
        LOS_Log("RedAddr and Redaddr_float: %d\t%f\n",RedAddr,RedAddr_fl);
        LOS_Log("BlueAddr and Blueaddr_float: %d\t%f\n",BlueAddr,BlueAddr_fl);
        LOS_Log("GreenBAddr and GreenBaddr_float: %d\t%f\n",GreenBAddr,GreenBAddr_fl);
        if (GreenRAddr == GreenRAddr_fl)
        {LOS_Log("GreenRAddr = %x\t%x\n",((LogAddrPicture) + GreenRAddr)
                     ,((LogAddrPicture) + GreenRAddr + 2));}
        else
        {LOS_Log("GreenRAddr = %x\t%x\n",((LogAddrPicture) + GreenRAddr)
                     ,((LogAddrPicture) + GreenRAddr + 1));}
        LOS_Log("GreenRvalue = %x\n",GreenRValue);
        if (GreenBAddr == GreenBAddr_fl)
        {LOS_Log("GreenBAddr = %x\t%x\n",((LogAddrPicture) + GreenBAddr)
                     ,((LogAddrPicture) + GreenBAddr + 2));}
        else
        {LOS_Log("GreenBAddr = %x\t%x\n",((LogAddrPicture) + GreenBAddr)
                     ,((LogAddrPicture) + GreenBAddr + 1));}
        LOS_Log("GreenBvalue = %x\n",GreenBValue);
        if (RedAddr == RedAddr_fl)
        {LOS_Log("RedAddr = %x\t%x\n",((LogAddrPicture) + RedAddr)
                     ,((LogAddrPicture) + RedAddr + 2));}
        else
        {LOS_Log("RedAddr = %x\t%x\n",((LogAddrPicture) + RedAddr)
                     ,((LogAddrPicture) + RedAddr + 1));}
        LOS_Log("Redvalue = %x\n",RedValue);
        if (BlueAddr == BlueAddr_fl)
        {LOS_Log("BlueAddr = %x\t%x\n",((LogAddrPicture) + BlueAddr)
                     ,((LogAddrPicture) + RedAddr + 2));}
        else
        {LOS_Log("BlueAddr = %x\t%x\n",((LogAddrPicture) + BlueAddr)
                     ,((LogAddrPicture) + RedAddr + 1));}
        LOS_Log("BlueValue = %x\n",BlueValue);
        LOS_Log("Pel Red: %x Green: %x Blue: %x\n",Pel->RedValue,Pel->GreenValue,Pel->BlueValue);
        */
        break;
    default:
             LOS_Log_Err("Unhandled Value Grab Format\n");

    }
return(0);

}

/************************************************************************/
/* test if first line and first column are red!             */
/* return                               */
/************************************************************************/
t_uint16 TestDZbuffer(ts_siapicturebuffer* p_Buffer) {
ts_PelRGB888 color0;
ts_PelRGB888 color1;
ts_PelRGB888 color2;
ts_PelRGB888 color3;
t_uint16 test=0;

ITE_GetRGBvalue(p_Buffer,1,1,&color0);
//LOS_Log("pixel 1,1:  %d,%d,%d\n",color0.RedValue,color0.GreenValue,color0.BlueValue,NULL,NULL,NULL);

ITE_GetRGBvalue(p_Buffer,p_Buffer->framesizeX-1,1,&color1);
//LOS_Log("pixel %d,1:  %d,%d,%d\n",p_Buffer->framesizeX-1,color1.RedValue,color1.GreenValue,color1.BlueValue,NULL,NULL);

ITE_GetRGBvalue(p_Buffer,1,p_Buffer->framesizeY-1,&color2);
//LOS_Log("pixel 1,%d:  %d,%d,%d\n",p_Buffer->framesizeY-1,color2.RedValue,color2.GreenValue,color2.BlueValue,NULL,NULL);

ITE_GetRGBvalue(p_Buffer,p_Buffer->framesizeX-1,p_Buffer->framesizeY-1,&color3);
//LOS_Log("pixel %d,%d:  %d,%d,%d\n",p_Buffer->framesizeX-1,p_Buffer->framesizeY-1,
//      color3.RedValue,color3.GreenValue,color3.BlueValue,NULL);
if ((IsRed(color0)==TRUE) && (IsRed(color1)==TRUE) && (IsRed(color2)==TRUE) && (IsRed(color3)==FALSE)) {
     test = 0;
     LOS_Log("Test DZ red Cross passed\n",NULL,NULL,NULL,NULL,NULL,NULL);
     }
else {test = 1;
     LOS_Log("Test DZ red Cross failed\n",NULL,NULL,NULL,NULL,NULL,NULL);
     }
return(test);
}

t_uint16 TestColorBarbufferGeneric(ts_siapicturebuffer* p_Buffer, t_uint16 border_testing_offset,  t_uint32 full_fov, t_uint32 fov_x)
{
    ts_PelRGB888 color0,color1,color2,color3,color4,color5,color6,color7;
    t_uint16 u32_test=0;
    t_uint32 u32_half_colum_size = 0, u32_colum_size = 0;
    t_uint32 height_position = 0;
    t_uint32 offeset_bottom_and_top = border_testing_offset;
    t_uint32 x_start_offset = 0;
    t_uint32 u32_columns_to_skip = 0;
    t_uint32 bms_color_bar_column_size = 0;
    float f_bms_column_count = 0;
    t_uint32 u32_error_count = 0;

    x_start_offset = (full_fov - fov_x)/2;
    bms_color_bar_column_size = full_fov/8;

    f_bms_column_count = (float)fov_x/bms_color_bar_column_size;
    u32_columns_to_skip = 8 - (t_uint32)(f_bms_column_count + 0.5);

    u32_colum_size = (float)p_Buffer->framesizeX /8;
    u32_half_colum_size = u32_colum_size /2;
    x_start_offset = (u32_half_colum_size + (f_bms_column_count - (t_uint32)(f_bms_column_count))*u32_half_colum_size)/2;

     //Read PE and Update Pixel order of buffer to be tested. <Sandeep Sharma>
     p_Buffer->pelorder = ITE_readPE(SystemConfig_Status_e_PixelOrder_Byte0);

    // u32_test middle of image
    height_position = (p_Buffer->framesizeY - p_Buffer->nonactivelines)/2;

    switch(u32_columns_to_skip)
        {
        case 0:
                 ITE_GetRGBvalue(p_Buffer,x_start_offset + 0 * u32_colum_size,height_position,&color0);
                 if(FALSE == IsWhite(color0))
                    u32_error_count++;
         case 1:
                ITE_GetRGBvalue(p_Buffer,x_start_offset + 1 * u32_colum_size,height_position,&color1);
                 if(FALSE == IsYellow(color1))
                    u32_error_count++;
         case 2:
               ITE_GetRGBvalue(p_Buffer,x_start_offset + 2 * u32_colum_size,height_position,&color2);
                 if(FALSE == IsCyan(color2))
                    u32_error_count++;
         case 3:
                ITE_GetRGBvalue(p_Buffer,x_start_offset + 3 * u32_colum_size,height_position,&color3);
                 if(FALSE == IsGreen(color3))
                     u32_error_count++;
          case 4:
                switch(u32_columns_to_skip)
                {
                  case 0:
                    ITE_GetRGBvalue(p_Buffer,  x_start_offset + 7 * u32_colum_size,height_position,&color7);
                    if(FALSE == IsBlack(color7))
                       u32_error_count++;
                  case 1:
                    ITE_GetRGBvalue(p_Buffer, x_start_offset + 6 * u32_colum_size,height_position,&color6);
                    if(FALSE == IsBlue(color6))
                       u32_error_count++;
                  case 2:
                    ITE_GetRGBvalue(p_Buffer, x_start_offset + 5 * u32_colum_size,height_position,&color5);
                    if(FALSE == IsRed(color5))
                       u32_error_count++;
                  case 3:
                    ITE_GetRGBvalue(p_Buffer, x_start_offset + 4 * u32_colum_size,height_position,&color4);
                    if(FALSE == IsMagenta(color4))
                       u32_error_count++;
                }
        }

        // u32_test bottom of image
    if((p_Buffer->framesizeY - p_Buffer->nonactivelines) >= offeset_bottom_and_top)
        height_position = offeset_bottom_and_top;
    else
        height_position = 1;

    switch(u32_columns_to_skip)
        {
        case 0:
                 ITE_GetRGBvalue(p_Buffer,x_start_offset + 0 * u32_colum_size,height_position,&color0);
                 if(FALSE == IsWhite(color0))
                    u32_error_count++;
         case 1:
                ITE_GetRGBvalue(p_Buffer,x_start_offset + 1 * u32_colum_size,height_position,&color1);
                 if(FALSE == IsYellow(color1))
                    u32_error_count++;
         case 2:
               ITE_GetRGBvalue(p_Buffer,x_start_offset + 2 * u32_colum_size,height_position,&color2);
                 if(FALSE == IsCyan(color2))
                    u32_error_count++;
         case 3:
                ITE_GetRGBvalue(p_Buffer,x_start_offset + 3 * u32_colum_size,height_position,&color3);
                 if(FALSE == IsGreen(color3))
                     u32_error_count++;
          case 4:
                switch(u32_columns_to_skip)
                {
                  case 0:
                    ITE_GetRGBvalue(p_Buffer, x_start_offset + 7 * u32_colum_size,height_position,&color7);
                    if(FALSE == IsBlack(color7))
                       u32_error_count++;
                  case 1:
                    ITE_GetRGBvalue(p_Buffer, x_start_offset + 6 * u32_colum_size,height_position,&color6);
                    if(FALSE == IsBlue(color6))
                       u32_error_count++;
                  case 2:
                    ITE_GetRGBvalue(p_Buffer, x_start_offset + 5 * u32_colum_size,height_position,&color5);
                    if(FALSE == IsRed(color5))
                       u32_error_count++;
                  case 3:
                    ITE_GetRGBvalue(p_Buffer, x_start_offset + 4 * u32_colum_size,height_position,&color4);
                    if(FALSE == IsMagenta(color4))
                       u32_error_count++;
                }
        }

        // u32_test top of image
    if((p_Buffer->framesizeY - p_Buffer->nonactivelines) >= offeset_bottom_and_top)
        height_position = p_Buffer->framesizeY - p_Buffer->nonactivelines - offeset_bottom_and_top;
    else
        height_position = 0;

    switch(u32_columns_to_skip)
        {
        case 0:
                 ITE_GetRGBvalue(p_Buffer,x_start_offset + 0 * u32_colum_size,height_position,&color0);
                 if(FALSE == IsWhite(color0))
                    u32_error_count++;
         case 1:
                ITE_GetRGBvalue(p_Buffer,x_start_offset + 1 * u32_colum_size,height_position,&color1);
                 if(FALSE == IsYellow(color1))
                    u32_error_count++;
         case 2:
               ITE_GetRGBvalue(p_Buffer,x_start_offset + 2 * u32_colum_size,height_position,&color2);
                 if(FALSE == IsCyan(color2))
                    u32_error_count++;
         case 3:
                ITE_GetRGBvalue(p_Buffer,x_start_offset + 3 * u32_colum_size,height_position,&color3);
                 if(FALSE == IsGreen(color3))
                     u32_error_count++;
          case 4:
                switch(u32_columns_to_skip)
                {
                  case 0:
                    ITE_GetRGBvalue(p_Buffer, x_start_offset + 7 * u32_colum_size,height_position,&color7);
                    if(FALSE == IsBlack(color7))
                       u32_error_count++;
                  case 1:
                    ITE_GetRGBvalue(p_Buffer, x_start_offset + 6 * u32_colum_size,height_position,&color6);
                    if(FALSE == IsBlue(color6))
                       u32_error_count++;
                  case 2:
                    ITE_GetRGBvalue(p_Buffer, x_start_offset + 5 * u32_colum_size,height_position,&color5);
                    if(FALSE == IsRed(color5))
                       u32_error_count++;
                  case 3:
                    ITE_GetRGBvalue(p_Buffer, x_start_offset + 4 * u32_colum_size,height_position,&color4);
                    if(FALSE == IsMagenta(color4))
                       u32_error_count++;
                }
        }

   if (0==u32_error_count)
    {
         u32_test = 0;//pass
    }
    else
    {
        u32_test = 1; //fail
        LOS_Log("u32_test colorbar result = 0x%x, border_testing_offset = %d\n", u32_error_count, border_testing_offset);
    }

    return(u32_test);
}


t_uint16 TestColorBarbuffer(ts_siapicturebuffer* p_Buffer, t_uint16 border_testing_offset)
{
    ts_PelRGB888 color0,color1,color2,color3,color4,color5,color6,color7;
    ts_PelRGB888 color10,color11,color12,color13,color14,color15,color16,color17;
    ts_PelRGB888 color20,color21,color22,color23,color24,color25,color26,color27;
    t_uint16 test=0;
    t_uint32 error = 0;
    t_uint32 half_colum_size = 0, colum_size = 0;
    t_uint32 height_position = 0;
    t_uint32 offeset_bottom_and_top = border_testing_offset;

    half_colum_size = p_Buffer->framesizeX / 16;
    colum_size = p_Buffer->framesizeX / 8;

     //Read PE and Update Pixel order of buffer to be tested. <Sandeep Sharma>
        p_Buffer->pelorder = ITE_readPE(SystemConfig_Status_e_PixelOrder_Byte0);

    // test middle of image
    height_position = (p_Buffer->framesizeY - p_Buffer->nonactivelines)/2;

    ITE_GetRGBvalue(p_Buffer,half_colum_size + 0 * colum_size,height_position,&color0);
    error = IsWhite(color0);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 1 * colum_size,height_position,&color1);
    error = error + (IsYellow(color1)<<1);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 2 * colum_size,height_position,&color2);
    error = error + (IsCyan(color2)<<2);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 3 * colum_size,height_position,&color3);
    error = error + (IsGreen(color3)<<3);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 4 * colum_size,height_position,&color4);
    error = error + (IsMagenta(color4)<<4);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 5 * colum_size,height_position,&color5);
    error = error + (IsRed(color5)<<5);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 6 * colum_size,height_position,&color6);
    error = error + (IsBlue(color6)<<6);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 7 * colum_size,height_position,&color7);
    error = error + (IsBlack(color7)<<7);


    // test bottom of image
    if((p_Buffer->framesizeY - p_Buffer->nonactivelines) >= offeset_bottom_and_top)
        height_position = offeset_bottom_and_top;
    else
        height_position = 1;

    ITE_GetRGBvalue(p_Buffer,half_colum_size + 0 * colum_size,height_position,&color10);
    error = error + (IsWhite(color10)<<8);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 1 * colum_size,height_position,&color11);
    error = error + (IsYellow(color11)<<9);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 2 * colum_size,height_position,&color12);
    error = error + (IsCyan(color12)<<10);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 3 * colum_size,height_position,&color13);
    error = error + (IsGreen(color13)<<11);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 4 * colum_size,height_position,&color14);
    error = error + (IsMagenta(color14)<<12);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 5 * colum_size,height_position,&color15);
    error = error + (IsRed(color15)<<13);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 6 * colum_size,height_position,&color16);
    error = error + (IsBlue(color16)<<14);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 7 * colum_size,height_position,&color17);
    error = error + (IsBlack(color17)<<15);


    // test top of image
    if((p_Buffer->framesizeY - p_Buffer->nonactivelines) >= offeset_bottom_and_top)
        height_position = p_Buffer->framesizeY - p_Buffer->nonactivelines - offeset_bottom_and_top;
    else
        height_position = 0;

    ITE_GetRGBvalue(p_Buffer,half_colum_size + 0 * colum_size,height_position,&color20);
    error = error + (IsWhite(color20)<<16);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 1 * colum_size,height_position,&color21);
    error = error + (IsYellow(color21)<<17);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 2 * colum_size,height_position,&color22);
    error = error + (IsCyan(color22)<<18);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 3 * colum_size,height_position,&color23);
    error = error + (IsGreen(color23)<<19);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 4 * colum_size,height_position,&color24);
    error = error + (IsMagenta(color24)<<20);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 5 * colum_size,height_position,&color25);
    error = error + (IsRed(color25)<<21);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 6 * colum_size,height_position,&color26);
    error = error + (IsBlue(color26)<<22);
    ITE_GetRGBvalue(p_Buffer,half_colum_size + 7 * colum_size,height_position,&color27);
    error = error + (IsBlack(color27)<<23);


    if ((IsWhite(color0)   ==TRUE) && (IsWhite(color10)  ==TRUE) && (IsWhite(color20)  ==TRUE) &&
        (IsYellow(color1)  ==TRUE) && (IsYellow(color11) ==TRUE) && (IsYellow(color21) ==TRUE) &&
        (IsCyan(color2)    ==TRUE) && (IsCyan(color12)   ==TRUE) && (IsCyan(color22)   ==TRUE) &&
        (IsGreen(color3)   ==TRUE) && (IsGreen(color13)  ==TRUE) && (IsGreen(color23)  ==TRUE) &&
        (IsMagenta(color4) ==TRUE) && (IsMagenta(color14)==TRUE) && (IsMagenta(color24)==TRUE) &&
        (IsRed(color5)     ==TRUE) && (IsRed(color15)    ==TRUE) && (IsRed(color25)    ==TRUE) &&
        (IsBlue(color6)    ==TRUE) && (IsBlue(color16)   ==TRUE) && (IsBlue(color26)   ==TRUE) &&
        (IsBlack(color7)   ==TRUE) && (IsBlack(color17)  ==TRUE) && (IsBlack(color27)  ==TRUE) )
    {
         test = 0;//pass
    }
    else
    {
        test = 1; //fail
        LOS_Log("test colorbar result = 0x%x, border_testing_offset = %d\n",error, border_testing_offset);
    }

    return(test);
}


t_uint16 TestColoredbuffer(ts_siapicturebuffer* p_Buffer,e_basic_color color) {

ts_PelRGB888 color0,color1,color2,color3,color4;
t_uint16 test=0;

ITE_GetRGBvalue(p_Buffer,((p_Buffer->framesizeX)/16)
            ,(p_Buffer->framesizeY - p_Buffer->nonactivelines)/2,&color0);
ITE_GetRGBvalue(p_Buffer,((p_Buffer->framesizeX)/16)  + ((p_Buffer->framesizeX)/8)
            ,(p_Buffer->framesizeY - p_Buffer->nonactivelines)/2,&color1);
ITE_GetRGBvalue(p_Buffer,((p_Buffer->framesizeX)/16)  + (2*((p_Buffer->framesizeX)/8))
            ,(p_Buffer->framesizeY - p_Buffer->nonactivelines)/2,&color2);
ITE_GetRGBvalue(p_Buffer,((p_Buffer->framesizeX)/16)  + (3*((p_Buffer->framesizeX)/8))
            ,(p_Buffer->framesizeY - p_Buffer->nonactivelines)/2,&color3);
ITE_GetRGBvalue(p_Buffer,((p_Buffer->framesizeX)/16)  + (4*((p_Buffer->framesizeX)/8))
            ,(p_Buffer->framesizeY - p_Buffer->nonactivelines)/2,&color4);

switch (color) {
    case(YELLOW_COLOR_E) :
    if ((IsYellow(color0)==TRUE) && (IsYellow(color1)==TRUE) && (IsYellow(color2)==TRUE) && (IsYellow(color3)==TRUE) && (IsYellow(color4)==TRUE)) {
            test = 0;
            LOS_Log("Test Colored buffer yellow passed\n",NULL,NULL,NULL,NULL,NULL,NULL);}
    else {test = 1;
            LOS_Log("Test Colored buffer yellow failed\n",NULL,NULL,NULL,NULL,NULL,NULL);
            }
    break;
    case(CYAN_COLOR_E) :
    if ((IsCyan(color0)==TRUE) && (IsCyan(color1)==TRUE) && (IsCyan(color2)==TRUE) && (IsCyan(color3)==TRUE) && (IsCyan(color4)==TRUE)) {
            test = 0;
            LOS_Log("Test Colored buffer cyan passed\n",NULL,NULL,NULL,NULL,NULL,NULL);}
    else {test = 1;
            LOS_Log("Test Colored buffer cyan failed\n",NULL,NULL,NULL,NULL,NULL,NULL);
            }
        break;
    case(MAGENTA_COLOR_E) :
    if ((IsMagenta(color0)==TRUE) && (IsMagenta(color1)==TRUE) && (IsMagenta(color2)==TRUE) && (IsMagenta(color3)==TRUE) && (IsMagenta(color4)==TRUE)) {
            test = 0;
            LOS_Log("Test Colored buffer magenta passed\n",NULL,NULL,NULL,NULL,NULL,NULL);}
    else {test = 1;
            LOS_Log("Test Colored buffer magenta failed\n",NULL,NULL,NULL,NULL,NULL,NULL);
            }
        break;
    case(BLACK_COLOR_E) :
    if ((IsBlack(color0)==TRUE) && (IsBlack(color1)==TRUE) && (IsBlack(color2)==TRUE) && (IsBlack(color3)==TRUE) && (IsBlack(color4)==TRUE)) {
            test = 0;
            LOS_Log("Test Colored buffer Black passed\n",NULL,NULL,NULL,NULL,NULL,NULL);}
    else {test = 1;
            LOS_Log("Test Colored buffer Black failed\n",NULL,NULL,NULL,NULL,NULL,NULL);
            }
        break;
    case(WHITE_COLOR_E) :
    if ((IsWhite(color0)==TRUE) && (IsWhite(color1)==TRUE) && (IsWhite(color2)==TRUE) && (IsWhite(color3)==TRUE) && (IsWhite(color4)==TRUE)) {
            test = 0;
            LOS_Log("Test Colored buffer White passed\n",NULL,NULL,NULL,NULL,NULL,NULL);}
    else {test = 1;
            LOS_Log("Test Colored buffer White failed\n",NULL,NULL,NULL,NULL,NULL,NULL);
            }
        break;
    case(RED_COLOR_E) :
    if ((IsRed(color0)==TRUE) && (IsRed(color1)==TRUE) && (IsRed(color2)==TRUE) && (IsRed(color3)==TRUE) && (IsRed(color4)==TRUE)) {
            test = 0;
            LOS_Log("Test Colored buffer red passed\n",NULL,NULL,NULL,NULL,NULL,NULL);}
    else {test = 1;
            LOS_Log("Test Colored buffer red failed\n",NULL,NULL,NULL,NULL,NULL,NULL);
            }
        break;
    case(BLUE_COLOR_E) :
    if ((IsBlue(color0)==TRUE) && (IsBlue(color1)==TRUE) && (IsBlue(color2)==TRUE) && (IsBlue(color3)==TRUE) && (IsBlue(color4)==TRUE)) {
            test = 0;
            LOS_Log("Test Colored buffer blue passed\n",NULL,NULL,NULL,NULL,NULL,NULL);}
    else {test = 1;
            LOS_Log("Test Colored buffer blue failed\n",NULL,NULL,NULL,NULL,NULL,NULL);
            }
        break;
    case(GREEN_COLOR_E) :
    if ((IsGreen(color0)==TRUE) && (IsGreen(color1)==TRUE) && (IsGreen(color2)==TRUE) && (IsGreen(color3)==TRUE) && (IsGreen(color4)==TRUE)) {
            test = 0;
            LOS_Log("Test Colored buffer green passed\n",NULL,NULL,NULL,NULL,NULL,NULL);}
    else {test = 1;
            LOS_Log("Test Colored buffer green failed\n",NULL,NULL,NULL,NULL,NULL,NULL);
            }
        break;
        }

return(test);
}


t_uint8 IsWhite(ts_PelRGB888 Pixel) {
if ((Pixel.RedValue > 190) && (Pixel.GreenValue > 190) && (Pixel.BlueValue > 190))
    return(TRUE);
else    {// LOS_Log("Pel is not White\n");
    return(FALSE);}
}
t_uint8 IsBlack(ts_PelRGB888 Pixel) {
if ((Pixel.RedValue < 50) && (Pixel.GreenValue < 65) && (Pixel.BlueValue < 65))
    return(TRUE);
else    {// LOS_Log("Pel is not Black\n");
    return(FALSE);}
}
t_uint8 IsRed(ts_PelRGB888 Pixel) {
if ((Pixel.RedValue > 190) && (Pixel.GreenValue < 65) && (Pixel.BlueValue < 65))
    return(TRUE);
else    {// LOS_Log("Pel is not Red\n");
    return(FALSE);}
}
t_uint8 IsGreen(ts_PelRGB888 Pixel) {
if ((Pixel.RedValue < 50) && (Pixel.GreenValue > 190) && (Pixel.BlueValue < 45)) // set to 45 for LLD, was 30
    return(TRUE);
else    {// LOS_Log("Pel is not Green\n");
    return(FALSE);}
}
t_uint8 IsBlue(ts_PelRGB888 Pixel) {
if ((Pixel.RedValue < 50) && (Pixel.GreenValue < 65) && (Pixel.BlueValue > 190))
    return(TRUE);
else    {// LOS_Log("Pel is not Blue\n");
    return(FALSE);}
}
t_uint8 IsYellow(ts_PelRGB888 Pixel) {
if ((Pixel.RedValue > 190) && (Pixel.GreenValue > 190) && (Pixel.BlueValue < 50))
    return(TRUE);
else    {// LOS_Log("Pel is not Yellow\n");
    return(FALSE);}
}
t_uint8 IsCyan(ts_PelRGB888 Pixel) {
if ((Pixel.RedValue < 50) && (Pixel.GreenValue > 190) && (Pixel.BlueValue > 190))
    return(TRUE);
else    {// LOS_Log("Pel is not Cyan\n");
    return(FALSE);}
}
t_uint8 IsMagenta(ts_PelRGB888 Pixel) {
if ((Pixel.RedValue > 190) && (Pixel.GreenValue < 65) && (Pixel.BlueValue > 190))
    return(TRUE);
else    {// LOS_Log("Pel is not Magenta\n");
    return(FALSE);}
}


t_uint8 ITE_StoreinBMPFile(char* filename,ts_siapicturebuffer* p_Buffer)
{
   t_los_file *out_file;
   t_uint16 header[1];
   t_uint32 file_header[3];
   t_uint32 pict_header[10];
   t_uint8  pel[3];
   ts_PelRGB888 color;
   t_uint32 i,j;
   char out_filename[100]="";
   char extension[10]="";
   static t_uint32 frameNb=0;
   t_uint32 sizey;

   sizey = p_Buffer->framesizeY - p_Buffer->nonactivelines;


   if (strlen(filename)>160)
   {
    LOS_Log("file name too long !!!!!!! \n",  out_filename,NULL,NULL,NULL,NULL,NULL);
    return FALSE;
   }

   // copy filename
   strcpy(out_filename,filename);
   snprintf(extension, sizeof(extension),"%d.bmp",(int)frameNb);
   frameNb++;
   strcat(out_filename,extension);

   out_file = LOS_fopen(out_filename,"wb");
   if(out_file==NULL)
   {
        LOS_Log("Open file error !!!!!!!!!!!!\n",  NULL,NULL,NULL,NULL,NULL,NULL);
        return FALSE;
   }
   //write bmp file header
   header[0]=0x4d42;        // Bitmap Windows
   LOS_fwrite((t_uint8 *)(header), 2, 1, out_file);

   file_header[0]= (p_Buffer->framesizeX*sizey*3) + 40 + 14;   // total size of file
   file_header[1]= 0;       // reserved
   file_header[2]= 40 + 14; // picture offset 40+14 ou 14????
   LOS_fwrite((t_uint8 *)(file_header), 3*4, 1, out_file);

   pict_header[0]= 40;      // Picture header size
   pict_header[1]= p_Buffer->framesizeX;
   pict_header[2]= sizey;
   pict_header[3]= 0x00180001;  // color coding 24bit/ plan
   pict_header[4]= 0;       // compression methode 0: no compression
   pict_header[5]= p_Buffer->framesizeX*sizey*3;   // Picture total size
   pict_header[6]= 0x0ec4;  // horiz resolution
   pict_header[7]= 0x0ec4;  // vertical resolution
   pict_header[8]= 0;       // palette nb of color
   pict_header[9]= 0;       // palette nb of important color
   LOS_fwrite((t_uint8 *)(pict_header), 10*4, 1, out_file);

   for (j=0;j<sizey;j++) {
     for (i=0;i<p_Buffer->framesizeX;i++) {
         ITE_GetRGBvalue(p_Buffer,i,j,&color);
     pel[2] = color.RedValue;
     pel[1] = color.GreenValue;
     pel[0] = color.BlueValue;
         LOS_fwrite((t_uint8 *)(pel), 3, 1, out_file);
         }
     }

   LOS_fclose(out_file);
   LOS_Log("output file %s written\n",  out_filename,NULL,NULL,NULL,NULL,NULL);

   return TRUE;
}

/*

t_uint8 ITE_StoreinBMPFile(char* filename,ts_siapicturebuffer* p_Buffer)
{
   t_los_file *out_file;
   t_uint16 header[1];
   t_uint32 file_header[3];
   t_uint32 pict_header[10];
   //t_uint8  pel[3];
   ts_PelRGB888 color;
   t_uint32 i,j;
   char out_filename[100]="";
   char extension[10]="";
   t_uint8 *bmpDataPtr;
   static t_uint32 frameNb=0;

   if (strlen(filename)>60)
   {
    LOS_Log("file name too long !!!!!!! \n",  out_filename,NULL,NULL,NULL,NULL,NULL);
    return FALSE;
   }

   // copy filename
   strcpy(out_filename,filename);
   snprintf(extension, sizeof(extension),"%d.bmp",frameNb);
   frameNb++;
   strcat(out_filename,extension);

   out_file = LOS_fopen(out_filename,"wb");
   if(out_file==NULL)
   {
        LOS_Log("Open file error !!!!!!!!!!!!\n",  NULL,NULL,NULL,NULL,NULL,NULL);
        return FALSE;
   }
   //write bmp file header
   header[0]=0x4d42;        // Bitmap Windows
   LOS_fwrite((t_uint8 *)(header), 2, 1, out_file);

   file_header[0]= (p_Buffer->framesizeX*p_Buffer->framesizeY*3) + 40 + 14;   // total size of file
   file_header[1]= 0;       // reserved
   file_header[2]= 40 + 14; // picture offset 40+14 ou 14????
   LOS_fwrite((t_uint8 *)(file_header), 3*4, 1, out_file);

   pict_header[0]= 40;      // Picture header size
   pict_header[1]= p_Buffer->framesizeX;
   pict_header[2]= p_Buffer->framesizeY;
   pict_header[3]= 0x00180001;  // color coding 24bit/ plan
   pict_header[4]= 0;       // compression methode 0: no compression
   pict_header[5]= p_Buffer->framesizeX*p_Buffer->framesizeY*3;   // Picture total size
   pict_header[6]= 0x0ec4;  // horiz resolution
   pict_header[7]= 0x0ec4;  // vertical resolution
   pict_header[8]= 0;       // palette nb of color
   pict_header[9]= 0;       // palette nb of important color
   LOS_fwrite((t_uint8 *)(pict_header), 10*4, 1, out_file);

   bmpDataPtr = bmpData;

   for (j=0;j<p_Buffer->framesizeY;j++) {
     for (i=0;i<p_Buffer->framesizeX;i++) {
         ITE_GetRGBvalue(p_Buffer,i,j,&color);
         *bmpDataPtr = color.BlueValue;
         bmpDataPtr++;
         *bmpDataPtr = color.GreenValue;
         bmpDataPtr++;
         *bmpDataPtr = color.RedValue;
         bmpDataPtr++;

         }
     }

    LOS_Log("Write %s BMP file in 1 shot \n",  out_filename,NULL,NULL,NULL,NULL,NULL);

    //Write data inf ile in one shot
    LOS_fwrite((t_uint8 *)(bmpData), (p_Buffer->framesizeY * p_Buffer->framesizeX * 3), 1, out_file);

   LOS_fclose(out_file);
   LOS_Log("output file %s written\n",  out_filename,NULL,NULL,NULL,NULL,NULL);

   return TRUE;
}
*/

