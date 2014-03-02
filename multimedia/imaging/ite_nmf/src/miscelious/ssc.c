/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* --- includes ----------------------------------------------------------- */
 #include "ssc.h"                /* don't forget the header file !!!         */
#include <string.h>             /* because of string manipulations          */
#include <inc/type.h>
#include "ite_main.h"

/* --- local defines ------------------------------------------------------ */
#define MAX_HANDLES      4

// #define USE_90KHZ_TIMER  undefine this one if you want to use ARM loops instead of timer

#define SCKRelease  *(DU32*) (SSCHndl->SCKPIOBase+0x18)  =  (SSCHndl->SCKPIOMask);      // INPUT
#define SCKForceLow *(DU32*) (SSCHndl->SCKPIOBase+0x14)  =  (SSCHndl->SCKPIOMask);      // OUTPUT
#define SCKInit     *(DU32*) (SSCHndl->SCKPIOBase+0x20) &= ~(SSCHndl->SCKPIOMask); \
                    *(DU32*) (SSCHndl->SCKPIOBase+0x24) &= ~(SSCHndl->SCKPIOMask); \
                    *(DU32*) (SSCHndl->SCKPIOBase+0x18)  =  (SSCHndl->SCKPIOMask); \
                    *(DU32*) (SSCHndl->SCKPIOBase+0x08)  =  (SSCHndl->SCKPIOMask);      // CLEAR 0
#define SCKRead  (((*(DU32*) (SSCHndl->SDAPIOBase+0x00))>>   SSCHndl->SCKPIOBit)&0x1)

#define SDARelease  *(DU32*) (SSCHndl->SDAPIOBase+0x18)  =  (SSCHndl->SDAPIOMask);      // INPUT
#define SDAForceLow *(DU32*) (SSCHndl->SDAPIOBase+0x14)  =  (SSCHndl->SDAPIOMask);      // OUTPUT
#define SDAInit     *(DU32*) (SSCHndl->SDAPIOBase+0x20) &= ~(SSCHndl->SDAPIOMask);  \
                    *(DU32*) (SSCHndl->SDAPIOBase+0x24) &= ~(SSCHndl->SDAPIOMask);  \
                    *(DU32*) (SSCHndl->SDAPIOBase+0x18)  =  (SSCHndl->SDAPIOMask);  \
                    *(DU32*) (SSCHndl->SDAPIOBase+0x08)  =  (SSCHndl->SDAPIOMask);      // CLEAR 0
#define SDARead  (((*(DU32*) (SSCHndl->SDAPIOBase+0x00))>>   SSCHndl->SDAPIOBit)&0x1)





/* --- local enumerations ------------------------------------------------- */
typedef struct
{
    /* --- input / output buffers --- */
    t_uint32                 SCKPIOBase      ;
    t_uint32                 SCKPIOBit       ;
    t_uint32                 SCKPIOMask      ;

    t_uint32                 SDAPIOBase      ;
    t_uint32                 SDAPIOBit       ;
    t_uint32                 SDAPIOMask      ;
    t_uint32                 TimerWait       ;

    /* --- interrupts and CPU system --- */
    t_uint32                 DeviceFrequency ;
    t_uint8                  Retries         ;
    /* --- speed of the generator --- */
    t_uint32                 BaudRateDefault ;
    t_uint32                 BaudRate        ;
}   SSC_Handle_t;

SSC_Handle_t  LocalSSC [MAX_HANDLES];
t_uint32           MaskSSC   = 0;
/* --- local variables ---------------------------------------------------- */


/* --- prototypes of local functions and static --------------------------- */
static  void Delay          ( SSC_Handle_t *SSCHndl  );
static  void I2CStretch     ( SSC_Handle_t *SSCHndl  );
static  void I2CStart       ( SSC_Handle_t *SSCHndl  );
static  void I2CStop        ( SSC_Handle_t *SSCHndl  );
static  t_bool I2CAcknowledge ( SSC_Handle_t *SSCHndl,t_bool Master, t_uint8 Val   );
static  void I2CSendByte    ( SSC_Handle_t *SSCHndl, t_uint32 Data  , t_uint8 NBits );
static  t_uint32  I2CReceiveByte ( SSC_Handle_t *SSCHndl, t_uint16 NBits );

/* --- functions in this driver ------------------------------------------- */
/* ========================================================================
   Name:        STSSC_GetRevision
   Description: Returns the revision of the driver

   ======================================================================== */
ST_Revision_t  STSSC_GetRevision   ( void )
{
    static char     Revision[] = "Soft i2C 0.0.0";
    return ((ST_Revision_t) Revision);
}

/* =======================================================================
   Name:        STSSC_Init
   Description: Initializes a i2c cell, returns the handle to work on it

   ======================================================================== */
ST_ErrorCode_t STSSC_Init ( STSSC_InitParams_t *InitParams_p,STSSC_Handle_t *Handle )
{
    t_uint32             Loop=0;
    SSC_Handle_t   *SSCHndl;

    if ( InitParams_p == NULL )
    {
        return ( ST_ERROR_BAD_PARAMETER );
    }
    while ( Loop < MAX_HANDLES )
    {
        if ( (MaskSSC & (1<<Loop))==0 )
        {
            break;
        }
        Loop ++;
    }
    if ( Loop == MAX_HANDLES )
    {
        return ( ST_ERROR_NO_MEMORY );
    }
    *Handle                     = Loop;
    MaskSSC                     |= (1<<Loop);
    SSCHndl                     = (SSC_Handle_t*) &LocalSSC[Loop];

    SSCHndl->BaudRateDefault    = InitParams_p->BaudRate        ;
    SSCHndl->BaudRate           = InitParams_p->BaudRate        ;
#if defined(USE_90KHZ_TIMER)
    SSCHndl->TimerWait          = ((90000    >>1)/SSCHndl->BaudRate);   // make assumption of 90kHz timer
#else
    SSCHndl->TimerWait          = ((100000000>>1)/SSCHndl->BaudRate);   // make assumption of ARM running at 400MHz
#endif

    SSCHndl->SCKPIOBase         = InitParams_p->SCKPIOBase      ;
    SSCHndl->SCKPIOBit          = InitParams_p->SCKPIOBit       ;
    SSCHndl->SCKPIOMask         = 1<<(SSCHndl->SCKPIOBit)       ;

    SSCHndl->SDAPIOBase         = InitParams_p->SDAPIOBase      ;
    SSCHndl->SDAPIOBit          = InitParams_p->SDAPIOBit       ;
    SSCHndl->SDAPIOMask         = 1<<(SSCHndl->SDAPIOBit)       ;

    SCKInit
    SDAInit

    return ( ST_NO_ERROR );
}

/* ========================================================================
   Name:        STSSC_Term
   Description: Terminates a SSC handle

   ======================================================================== */
ST_ErrorCode_t STSSC_Term ( STSSC_Handle_t         Handle )
{
    MaskSSC &= ~(1<<Handle);
    return ( ST_NO_ERROR );
}

/* ========================================================================
   Name:        STSSC_Read
   Description: Read a chain of bytes from a slave device.

   ======================================================================== */
ST_ErrorCode_t STSSC_Read           ( STSSC_Handle_t        Handle,
                                      t_uint8                    SlaveAddress,
                                      t_uint8                    *Buffer,
                                      t_uint32                   MaxLength,
                                      t_uint32                   *ActualLength,
                                      t_uint32                   TimeOut )
{
    SSC_Handle_t    *SSCHndl;
    t_uint32             Loop;
     UNUSED(TimeOut);

    if ( ((1<<Handle)&MaskSSC)==0  )
    {
        return ( ST_ERROR_BAD_PARAMETER );
    }
    SSCHndl         = (SSC_Handle_t*)  &LocalSSC[Handle];
    *ActualLength   = 0;
    I2CStart        (SSCHndl);
    I2CSendByte     (SSCHndl,SlaveAddress|1,8);
    if ( I2CAcknowledge  (SSCHndl,FALSE,0) )
    {
        I2CStop(SSCHndl);
        return (ST_ERROR_TIMEOUT );
    }
    for ( Loop = 0 ; Loop < MaxLength ; Loop ++ )
    {
        *Buffer = I2CReceiveByte     (SSCHndl,8);
        if ( I2CAcknowledge  (SSCHndl,TRUE,(Loop+1==MaxLength?1:0)) )
        {
            I2CStop(SSCHndl);
            return (ST_ERROR_TIMEOUT );
        }
        (*ActualLength) ++;
        Buffer ++;
    }
    I2CStop(SSCHndl);
    return ( ST_NO_ERROR );
}

/* ========================================================================
   Name:        STSSC_Write
   Description: Write some characters into the i2c FIFO. Note that if the
                character chain is less than what contains the FIFO, then
                no interrupt is called.
   ======================================================================== */
ST_ErrorCode_t STSSC_Write          ( STSSC_Handle_t        Handle,
                                      t_uint8                    SlaveAddress,
                                      t_uint8                    *Buffer,
                                      t_uint32                   Length,
                                      t_uint32                   *NumWritten,
                                      t_uint32                   TimeOut,
                                      t_bool                  NoStop  )
{
    SSC_Handle_t    *SSCHndl;
    t_uint32             Loop;
    UNUSED(TimeOut);
    UNUSED(NoStop);

    if ( ((1<<Handle)&MaskSSC)==0  )
    {
        return ( ST_ERROR_BAD_PARAMETER );
    }
    SSCHndl         = (SSC_Handle_t*) &LocalSSC[Handle];
    *NumWritten     = 0;
    I2CStart        (SSCHndl);
    I2CSendByte     (SSCHndl,SlaveAddress&0xFE,8);
    if ( I2CAcknowledge  (SSCHndl,FALSE,0) )
    {
        I2CStop(SSCHndl);
        return (ST_ERROR_TIMEOUT );
    }
    for ( Loop = 0 ; Loop < Length ; Loop ++ )
    {
        I2CSendByte     (SSCHndl, *Buffer,8 );
        if ( I2CAcknowledge  (SSCHndl,FALSE,0) )
        {
            I2CStop(SSCHndl);
            return (ST_ERROR_TIMEOUT );
        }
        Buffer ++;
        (*NumWritten)++;
    }
    I2CStop(SSCHndl);
    return ( ST_NO_ERROR );
}

/* ========================================================================
   Name:        STSSC_SetBaudRate
   Description: Modify the baud rate generator, disable or restore to default
                baudrate.
   ======================================================================== */
ST_ErrorCode_t STSSC_SetBaudRate    ( STSSC_Handle_t Handle, t_uint32 NewBaudRate  )
{
    SSC_Handle_t   *SSCHndl;

    if ( ((1<<Handle)&MaskSSC)==0  )
    {
        return ( ST_ERROR_BAD_PARAMETER );
    }
    SSCHndl                = (SSC_Handle_t*) &LocalSSC[Handle];
    SSCHndl->BaudRate      = NewBaudRate                      ;
#if defined(USE_90KHZ_TIMER)
    SSCHndl->TimerWait     = ( (90000>>1)   /SSCHndl->BaudRate);    // make assumption of 90 kHZ timer
#else
    SSCHndl->TimerWait     = ((100000000>>1)/SSCHndl->BaudRate);    // make assumption of ARM running at 400MHz
#endif
    return ( ST_NO_ERROR );
}

/* --- Internal routines in this driver ( prefix = SSC_Xxxx ) ------------ */
/*************** INTERNAL ROUTINES ******************************************/
/* ========================================================================
   Name:        Delay
   Description: make 1 clock phase delay ( minimum) cycle for given I2C
                At high frequency ( >100kbits), the limit is the precision
                                    of the timer ).
   ======================================================================== */
void Delay                      ( SSC_Handle_t * SSCHndl )
{
#if defined(USE_90KHZ_TIMER)
    hostdelay ( SSCHndl->TimerWait );
#else
    volatile    t_uint32  count = SSCHndl->TimerWait;

    while (count)
    {
        count--;
    }
#endif
}

/* ========================================================================
   Name:        I2CStretch
   Description: Apply some clock strechting. This is in case the driver
                is facing slow I2C devices (or high capacitance buses )
   ======================================================================== */
void I2CStretch     ( SSC_Handle_t *SSCHndl  )
{
    t_uint32 TimeOut=0;

    while ( TimeOut < 1000000 )
    {
        if ( SCKRead == 1 )
        {
            break;
        }
        TimeOut++;
    }

}

/* ========================================================================
   Name:        I2CStart
   Description: Performs a start transaction

   ======================================================================== */
void I2CStart                  ( SSC_Handle_t *SSCHndl)
{
    TraceI2C(("\r\nStart -"));
    SCKRelease              /* should be already high ...                   */
    I2CStretch  (SSCHndl);  /* and check the clock is high                  */
    SDARelease              /* should be already high                       */
    Delay       (SSCHndl);  /* wait for a phase to stabilize                */
    SDAForceLow;            /* only authorized actions -> pull low SDA      */
    Delay       (SSCHndl);  /* wait a phase with SDA Low                    */
    SCKForceLow;            /* now pull low SCK                             */
    Delay       (SSCHndl);  /* we are in low /low phase established         */
}

/* ========================================================================
   Name:        I2CStop
   Description: Performs a stop transaction

   ======================================================================== */
void I2CStop                  ( SSC_Handle_t *SSCHndl)
{
    TraceI2C((" - Stop "));
    Delay(SSCHndl);         /* prepare SDA low to high                      */
    SCKForceLow             /* SCK should be already low                    */
    Delay(SSCHndl);         /* wait one more phase with SDA low             */
    SDAForceLow             /* got the acknoledge, so set it low, but drive */
    Delay(SSCHndl);         /* wait one more phase with SDA low             */
    SCKRelease              /* now rise SCK                                 */
    I2CStretch  (SSCHndl);  /* and check the clock is high                  */
    Delay(SSCHndl);         /* wait for a phase                             */
    SDARelease              /* rise SDA, sign of end of transfer            */
    Delay(SSCHndl);         /* wait for half a phase for security           */
}

/* ========================================================================
   Name:        I2CAcknowledge
   Description: Performs an acknoledge ( without checking yet ...)

   ======================================================================== */
t_bool I2CAcknowledge ( SSC_Handle_t *SSCHndl,t_bool Master, t_uint8 Val )
{
    t_bool Answer= FALSE;

    TraceI2C(("wait ack   -"));
    if ( Master == FALSE )
    {
        SCKForceLow;            /* and retake control of SCK is driven low  */
        Delay(SSCHndl);         /* wait for a phase                         */
        SDARelease;             /* release SDA if not done                  */
        Delay(SSCHndl);         /* wait for a phase                         */
        SCKRelease;             /* then allows SCK to be high again         */
        I2CStretch(SSCHndl);    /* and strech it                            */
        Delay(SSCHndl);         /* wait for a phase                         */
        if ( SDARead != 0 )
        {
            Answer = TRUE;      /* if SDA is now high, then acknoledge is 1 */
        }
        Delay(SSCHndl);         /* wait for a phase                         */
        SCKForceLow;            /* end of acknoledge clock cycle            */
        Delay(SSCHndl);         /* wait again for a phase cycle             */
    }
    else
    {
        SCKForceLow;            /* take control of clock again              */
        Delay(SSCHndl);         /* wait for a phase                         */
        if ( Val )              /* set SDA high or low, according to Val    */
        {   /* Val = 1,acknoledge = FALSE */
            SDARelease;
            Delay(SSCHndl);     /* wait for a phase                         */
            SCKRelease          /* drive clock high now                     */
            Delay(SSCHndl);     /* wait for a phase                         */
        }
        else
        {   /* Val = 0,acknoledge = TRUE */
            SDAForceLow;
            Delay(SSCHndl);     /* wait for a phase                         */
            SCKRelease          /* drive clock high now                     */
            I2CStretch(SSCHndl);/* wait for clock to go high                */
            Delay(SSCHndl);     /* wait for a phase                         */
        }
        SCKForceLow;            /* end of acknoledge clock cycle            */
        Delay(SSCHndl);         /* wait for a phase                         */
        SDARelease              /* release SDA                              */
    }
    return ( Answer );
}

/* ========================================================================
   Name:        I2CSendByte
   Description: Sends a data, number of bits is between 8 and 32 bits.
                entering with clock low, exiting with clock high
   ======================================================================== */
void I2CSendByte           ( SSC_Handle_t *SSCHndl, t_uint32 Data, t_uint8 NBits )
{
    t_uint32 Loop;

    TraceI2C(("Send 0x%x -", Data));
    for (Loop = 0; Loop < NBits; Loop++)
    {
        SCKForceLow                     /* make the low clock pulse         */
        Delay(SSCHndl);                 /* wait for a phase                 */
        if ((Data & (1<<(NBits-Loop-1)))!=0)
        {
            SDARelease;                 /* make a pulse with SDA high       */
        }
        else
        {
            SDAForceLow;                /* make a pulse with SDA low        */
        }
        Delay(SSCHndl);                 /* data stable for half a clock phse*/
        SCKRelease;                     /* then rise S Clock (=phy latch)   */
        I2CStretch(SSCHndl);            /* wait for clock to be high        */
        Delay(SSCHndl);                 /* and wait after strechting        */
    }
}

/* ========================================================================
   Name:        I2CReceiveByte
   Description: Receive a data, generally 8 bits, but could go up to 32 bits
                entering with clock low, exiting with clock high
   ======================================================================== */
t_uint32 I2CReceiveByte      ( SSC_Handle_t *SSCHndl, t_uint16 NBits )
{
    t_uint32 Loop,Data=0;

    SDARelease                              /* release data line            */
    for (Loop = 0; Loop < NBits; Loop ++)   /* same for each bit of data    */
    {
        SCKForceLow;                        /* make clock cycle low         */
        Delay(SSCHndl);                     /* and wait for a phase         */
        SCKRelease                          /* make clock high              */
        I2CStretch(SSCHndl);                /* and stretch the clock        */
        Delay(SSCHndl);                     /* wait for half a phase        */
        Data =  (Data<<1) | SDARead;        /* read bit, and push into FIFO */
    }
    TraceI2C(("Receive 0x%x -", Data));
    return (Data);
}


/* ========================================================================
   Name:        Read_16_8
   Description: Simplified routine to read an 8 bits register, using 16 bits
                sub addressing
   ======================================================================== */
t_uint8              Read_16_8   (t_uint16 i2cnum,t_uint16 i2c_ad,t_uint16 i2c_sla )
{
    t_uint8             tab_i2c_data[3];
    t_uint32                 Written,Read;
    ST_ErrorCode_t      ApiError;
    t_uint16                 Res=0xDEAD;
    tab_i2c_data[0]     = (i2c_sla>>8);
    tab_i2c_data[1]     = (i2c_sla&0xFF);
    ApiError = STSSC_Write (i2cnum, i2c_ad, tab_i2c_data,2,&Written,1000,FALSE);
    if ( ApiError == ST_NO_ERROR )
    {
        STSSC_Read  (i2cnum, i2c_ad, tab_i2c_data,1,&Read   ,1000);
        Res                 = tab_i2c_data[0];
    }
    return(Res);
}


/*********************************************************
 * Read_8_8: read a byte in a register, using 8-bit addressing
 * 
 ********************************************************/
t_uint8 Read_8_8 (t_uint16 i2cnum, t_uint16 i2c_addr, t_uint8 i2c_reg) {
    t_uint32            Written,Read;
    ST_ErrorCode_t      ApiError;
    t_uint8             value;
    ApiError = STSSC_Write (i2cnum, i2c_addr, &i2c_reg,1, &Written,1000,FALSE);
    if ( ApiError == ST_NO_ERROR )
    {
        STSSC_Read  (i2cnum, i2c_addr, &value, 1, &Read, 1000);
    }
    return(value);
}

PUBLIC t_uint16 i2c_read_AD5398(t_uint8 controller_number) {

    t_uint8             tab_i2c_data[3];
    t_uint32                 Read;
    t_uint16                 Res=0xDEAD;
    UNUSED(tab_i2c_data);

STSSC_Read  (controller_number, 0x19, tab_i2c_data,1,&Read   ,1000);
Res                 = ((tab_i2c_data[0]&0xff) << 8)  & (tab_i2c_data[1]&0xff);;
return(Res);

}

PUBLIC void i2c_write_AD5398(t_uint8 controller_number, t_uint16 value) {
    t_uint8             tab_i2c_data[3];
    t_uint32                 Written;
    UNUSED(controller_number);
tab_i2c_data[0]= (value & 0xff00) >> 8;
tab_i2c_data[1]= (value & 0xff);

STSSC_Write (1, 0x18, tab_i2c_data,2,&Written,1000,FALSE);
}


PUBLIC void i2c_read_index16
(t_uint8 controller_number, t_uint32 device_addr, t_uint32 index1, t_uint32 index2, t_uint32 number_of_data, t_uint8 *pread_buff)
{
 UNUSED(number_of_data);
pread_buff[0] =  Read_16_8(controller_number,device_addr,((index1&0xff)<<8)|(index2&0xff) );
}

/* ========================================================================
   Name:        Write_16_8
   Description: Simplified routine to write a 8 bits register
   ======================================================================== */
void            Write_16_8  (t_uint16 i2cnum,t_uint16 i2c_ad,t_uint16 i2c_sla,t_uint8 data)
{
    t_uint8             tab_i2c_data[3];
    t_uint32                 Written;
    tab_i2c_data[0]     = (i2c_sla>>8);
    tab_i2c_data[1]     = (i2c_sla&0xFF);
    tab_i2c_data[2]     = data          ;
    STSSC_Write(i2cnum,i2c_ad,tab_i2c_data,3,&Written,1000,FALSE);
}
/* ========================================================================
   Name:        Write_16_16
   Description: Simplified routine to write a 16 bits register, with 16 bits
                addressing
   ======================================================================== */
void            Write_16_16  (t_uint16 i2cnum,t_uint16 i2c_ad,t_uint16 i2c_sla,t_uint16 data)
{
    t_uint8             tab_i2c_data[4];
    t_uint32                 Written;
    tab_i2c_data[0]     = (i2c_sla>>8);
    tab_i2c_data[1]     = (i2c_sla&0xFF);
    tab_i2c_data[2]     = (data>>8)     ;
    tab_i2c_data[3]     = (data&0xFF)   ;
    STSSC_Write(i2cnum,i2c_ad,tab_i2c_data,4,&Written,1000,FALSE);
}

/* ========================================================================
   Name:        Write_8_8
   Description: Simplified routine to write a 8 bits register
   ======================================================================== */
void            Write_8_8  (t_uint16 i2cnum,t_uint16 i2c_ad,t_uint16 i2c_sla,t_uint8 data)
{
    t_uint8             tab_i2c_data[3];
    t_uint32                 Written;
    tab_i2c_data[0]     = i2c_sla;
    tab_i2c_data[1]     = data          ;
    if ( STSSC_Write (i2cnum, i2c_ad, tab_i2c_data,2,&Written,1000,FALSE) != ST_NO_ERROR)
    {
        // printf("e!");
    }
}


PUBLIC void i2c_write_p(t_uint8 controller_number, t_uint32 device_addr, t_uint32 number_of_data, t_uint8 *pwrite_buff)
{t_uint16 tmp;
if (number_of_data == 2) // index 8 data 8
   {
    Write_8_8  (controller_number,device_addr,pwrite_buff[0],pwrite_buff[1]);
   }
else // index 16 data 8
   {
    tmp = (((t_uint16)pwrite_buff[0]&0xff)<<8) | ((t_uint16)pwrite_buff[1]&0xff);
    Write_16_8  (controller_number,device_addr,tmp ,pwrite_buff[2]);
   }
}
/* ------------------------------- End of file ---------------------------- */


