#include "halgpshci.h"

#undef  MODULE_NUMBER
#define MODULE_NUMBER   MODULE_HALGPS

#undef  PROCESS_NUMBER
#define PROCESS_NUMBER  PROCESS_HALGPS

#undef  FILE_NUMBER
#define FILE_NUMBER     10

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0

uint8_t HALGPS10_00HciInit()
{
    int            err;           // Error code
    struct stat    status;          // Returns the status of the baseband HCIport.

    MC_HALGPS_TRACE_INF( "BEGIN: %s" , __func__);
    MC_HALGPS_TRACE_INF( "%s: + port = %s" , __func__ , gn_Port_GNB);
    // first we will open the CG2900 GNSS character device driver
    gn_iPort_GNB = -1;

    // Check for a valid PC COM Port Name
    if ( gn_Port_GNB[0] == '\0' )
    {
        MC_HALGPS_TRACE_INF( "HALGPS10_00HciInit:  ERROR: Port %s  Invalid Port Name", gn_Port_GNB );
        MC_HALGPS_TRACE_INF( "END: HALGPS10_00HciInit");
        return(FALSE );
    }

    // First check the port exists.
    // This avoids thread cancellation if the port doesn't exist.
    gn_iPort_GNB = -1;
    if ( ( err = stat( gn_Port_GNB, &status ) ) == -1 )
    {
        MC_HALGPS_TRACE_INF( "HALGPS10_00HciInit: stat(%s,*) = %d,  errno %d", gn_Port_GNB, err, errno );
        MC_HALGPS_TRACE_INF( "END: HALGPS10_00HciInit");

        return(FALSE);
    }

    // Open the HCI port.
    gn_iPort_GNB = open( gn_Port_GNB, (O_RDWR) );

    MC_HALGPS_TRACE_INF ("HALGPS10_00HciInit: gn_iPortGNB %d", gn_iPort_GNB);
#ifdef LBS_LINUX_UBUNTU_FTR
    MC_HALGPS_TRACE_INF("Open gnss error no is %d",errno);
#endif

    if( gn_iPort_GNB <= 0)
    {
        MC_HALGPS_TRACE_INF( "END: HALGPS10_00HciInit");
        return FALSE;
    }

    return TRUE;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1

uint32_t HALGPS10_01HciReadRaw( uint8_t *pp_RcvBuffer , uint32_t vp_Length )
{
    struct pollfd dev_pollfd;
    uint32_t tempBytesRead = 0;

    if(gn_iPort_GNB < 0)
    {
       return 0;
    }

    /*    MC_HALGPS_TRACE_INF("HALGPS9_01HciRcv BEGIN : Request %d bytes from protocol driver", vp_length+3);*/

    dev_pollfd.fd      = gn_iPort_GNB;
    dev_pollfd.events  = POLLIN;
    dev_pollfd.revents = 0;

/*    MC_HALGPS_TRACE_INF("HALGPS9_01HciRcv: Calling poll");*/
    poll(&dev_pollfd, 1, 1);
/*    MC_HALGPS_TRACE_INF("HALGPS9_01HciRcv: Exit poll");*/
    if((dev_pollfd.revents & POLLIN) == 0)
    {
        MC_HALGPS_HCI_TRACE_STRING(("HALGPS10_01HciReadRaw : No Data Available"));
    }
    else

    {
        tempBytesRead = read(gn_iPort_GNB,pp_RcvBuffer,vp_Length);
        MC_HALGPS_HCI_TRACE_STRING_PARAM(("HALGPS10_01HciReadRaw : Read <%d> bytes" , tempBytesRead ));
    }

    return tempBytesRead;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/******************************************************************************/
/**                           HALGPS10_02HciWriteRaw                           **/
/******************************************************************************/
uint32_t HALGPS10_02HciWriteRaw(uint8_t * pp_txBuffer, uint32_t  vp_length)
{
    int32_t byteswritten = 0;

    MC_HALGPS_HCI_TRACE_STRING(("BEGIN: HALGPS10_02HciWriteRaw"));

    if( pp_txBuffer == NULL || vp_length == 0 )
    {
        return 0;
    }

    if(gn_iPort_GNB < 0)
    {
       return 0;
    }

    byteswritten = (int)write(gn_iPort_GNB, pp_txBuffer,vp_length );

    if( byteswritten < 0 )
    {
        MC_HALGPS_TRACE_INF(("HALGPS10_02HciWriteRaw : ERR %d , %s" , errno , strerror(errno)));
        byteswritten = 0;
    }

    return byteswritten;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
uint8_t HALGPS10_03HciDeinit()
{
    close(gn_iPort_GNB);

    gn_iPort_GNB = -1;

    return TRUE;
}

/* ++ CG2900 Deep Slee Mode */
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
/*******************************************************************************/
/* HALGPS10_04EnterDSM : Enter DSM Mode                    */
/*******************************************************************************/
void HALGPS10_04EnterDSM(void)
{
    MC_HALGPS_TRACE_INF( "BEGIN: HALGPS10_04EnterDSM");
    if( gn_iPort_GNB >= 0 )
    {
        ioctl(gn_iPort_GNB, CG2900_CHAR_DEV_IOCTL_ENTER_DSM, NULL);
        MC_HALGPS_TRACE_INF( "Exit: HALGPS10_04EnterDSM : Success");
    }
    else
    {
        MC_HALGPS_TRACE_INF( "Exit: HALGPS10_04EnterDSM : ERR gn_iPort_GNB < 0");
    }
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
/*******************************************************************************/
/* HALGPS10_05ExitDSM : Exit DSM Mode                    */
/*******************************************************************************/
void HALGPS10_05ExitDSM(void)
{
    MC_HALGPS_TRACE_INF( "BEGIN: HALGPS10_05ExitDSM");
    if( gn_iPort_GNB >= 0 )
    {
        ioctl(gn_iPort_GNB, CG2900_CHAR_DEV_IOCTL_EXIT_DSM, NULL);
        MC_HALGPS_TRACE_INF( "Exit: HALGPS10_05ExitDSM : Success");
    }
    else
    {
        MC_HALGPS_TRACE_INF( "Exit: HALGPS10_05ExitDSM : ERR gn_iPort_GNB < 0");
    }

}
/* -- CG2900 Deep Slee Mode */

