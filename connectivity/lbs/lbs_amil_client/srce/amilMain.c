/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) ST-Ericsson SA 2009. All rights reserved.
 *  This code is ST-Ericsson proprietary and confidential.
 *  Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson.
 *
 *  Author: Janani Murthy
 *  Email : janani.murthy@stericsson.com
 *****************************************************************************/

#include "amil.h"
#include "amilInternal.h"



static t_AmilClientsBlock ClientDB[K_AMIL_MAX_DBCLIENTS] =
{
#if defined AGPS_FTR || defined GPS_FREQ_AID_FTR
 {lcsClient1_01Init, lcsClient1_12DeInit, K_AMIL_CLIENT_DEINITIALIZED}
#endif
#ifdef MODEM_SUPPORT_CELLINFO_FTR
#ifdef AGPS_UP_FTR
 ,
 {smsClient1_2Init, smsClient1_11DeInit, K_AMIL_CLIENT_DEINITIALIZED},
 {simClient1_1Init,simClient1_6DeInit, K_AMIL_CLIENT_DEINITIALIZED}
#endif /* AGPS_UP_FTR*/
 ,
 {netClient1_1Init,netClient1_12Deinit, K_AMIL_CLIENT_DEINITIALIZED}
#endif
};

t_OsaThreadHandle g_AmilMainThread;
static t_AmilClientsSelectorBlock *pg_FileDescriptorDB= NULL;
t_AmilModemState vg_Modem_status;




#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
void Amil1_01Init( void)
{
    INF("Amil1_01Init : Creating AMIL thread");

    if( !OSA_ThreadCreate( g_AmilMainThread , Amil1_02Main , NULL ))
    {
        ERR( "Amil1_01Init : Failed to create AMIL thread\n" );
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
//Main loop which waits on all the client Fds and handles the events from all the clients
void Amil1_02Main(void)
{
    int32_t vl_SockCreateErr = -1;
    int32_t vl_NetlinkFD = -1;
    uint8_t vl_Count=0;
    int8_t vl_handle;


    /* Iterating for a Maximum of K_AMIL_MAX_SOCKET_CREATE_ATTEMPTS times, since the Creation of netlink socket is essential.*/
    while(         -1 == vl_SockCreateErr
          && vl_Count <= K_AMIL_MAX_SOCKET_CREATE_ATTEMPTS )
    {
        OSA_Sleep(K_NETLINK_SOCKET_CREATE_SLEEP_MS);
        vl_SockCreateErr = netlnk_socket_create(&vl_NetlinkFD);
        INF("Amil1_02Main : Iterating Netlnk socket creation. Iteration %d  SockCreateErr %d \n", vl_Count, vl_SockCreateErr);
        vl_Count++;
    }

    //Netlink Socket creation failed continuosly hence exit the function
    if(    -1 == vl_SockCreateErr
        &&  K_AMIL_MAX_SOCKET_CREATE_ATTEMPTS == vl_Count )
    {
        INF("Amil1_02Main : netlnk socket failed for %d attempts \n", vl_Count);
        goto ThreadExit;
    }
    else
    {
        INF("Amil1_02Main : SUCCESS : Netlink socket created with fd %d\n", vl_NetlinkFD);

        FunctionInit:

        if(Amil1_12CheckModemReady(vl_NetlinkFD) == FALSE)
          goto ThreadExit;

        Amil1_04InitializeAllClients();

        /*  main select loop : Waiting on All the client Fds for receiving events and calling  the registered callback
                   for each fd that is  marked in the vl_MaxFd after a call to select. */
        for(;;)
        {
            int32_t vl_MaxFd = -1;
            int32_t vl_SelectReturn = 0;
            t_AmilClientsSelectorBlock* pl_CurrentClient = NULL;
            t_AmilClientsSelectorBlock* pl_NextClient;
            struct timeval* pl_TimeOut;
            fd_set  db_Rfds;

            int32_t vl_clientsMAXfd = Amil1_10GetMaxFDOfClients(&db_Rfds);

            vl_MaxFd = MC_NETLINK_MAX(vl_clientsMAXfd,vl_NetlinkFD);

            pl_TimeOut = Amil1_11GetTimeOut();

            FD_ZERO(&db_Rfds);
            pl_CurrentClient = pg_FileDescriptorDB;
            while( pl_CurrentClient != NULL )
            {
               FD_SET(pl_CurrentClient->fd, &db_Rfds);
               pl_CurrentClient = pl_CurrentClient->next;

            }

            FD_SET(vl_NetlinkFD,&db_Rfds);


            vl_SelectReturn = select(vl_MaxFd+1,&db_Rfds,NULL,NULL,pl_TimeOut);

            if( pl_TimeOut != NULL )
                OSA_Free(pl_TimeOut);

             /* Reqiured in future for Debugging
                DBG("Amil1_02Main : Select on Client FDs return value: vl_SelectReturn = %d \n",vl_SelectReturn);*/

            if ( vl_SelectReturn== -1 )
            {
                ERR("Amil1_02Main : select returned Error\n");
                goto ThreadExit;
            }
            else if( vl_SelectReturn )
            {
                //NetLink Fd for receiving Data corresponding to Modem Status
                if(FD_ISSET(vl_NetlinkFD,&db_Rfds))
                {
                    INF("Amil1_02Main : Data is available now.\n");
                    Amil1_03RecvNetlinkData(vl_NetlinkFD);

                    if( vg_Modem_status == K_NETLINK_MODEM_NOT_READY )
                    {
                        goto FunctionInit;
                    }
                }

                //polling through the remaining Client Fds in DBFileDescriptor
                else
                {
                    pl_CurrentClient = pg_FileDescriptorDB;
                    do
                    {
                        /*
                         * Get current client handle, and pointer to the next
                         * client in the chain in case current client is
                         * removed in the callback, or gets de-initilized.
                         */
                        vl_handle = pl_CurrentClient->handle;
                        pl_NextClient = pl_CurrentClient->next;

                        if ( FD_ISSET (pl_CurrentClient->fd,&db_Rfds))
                        {
                            if ( pl_CurrentClient->pp_callback(&(pl_CurrentClient->fd), vl_SelectReturn, vl_handle) )
                            {
                                /* Client has lost connection. De-initialize the client
                                 * to clean up and allow re-initialization to happen. */
                                Amil1_13DeInitializeClient(vl_handle);
                            }
                        }
                        pl_CurrentClient = pl_NextClient;
                    }while( pl_CurrentClient != NULL );
                }
            }
            /*Initialize the client if not initialized already or have got closed for some reason*/
            Amil1_04InitializeAllClients();
        }
    }

    ThreadExit:
        ERR("Exiting netlnk_socket_create\n");
        Amil1_05DeInitializeAllClients();
        pthread_exit(NULL);
}



/*
receives a message from SHRM.
@param netlnkfd  : netlink socket descriptor
Returns : THe modem state or Error value of netlnk_socket_recv() - Recv on socket
*/
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
int32_t Amil1_03RecvNetlinkData(int32_t vp_netlinkFD)
{
    int32_t *pl_RecvMsg;
    int32_t  vl_RetVal = -1;

    pl_RecvMsg = OSA_Malloc(K_AMIL_BUFFER_SIZE);

    if (NULL == pl_RecvMsg)
    {
        ERR("Amil1_03RecvNetlinkData : Invalid input param\n");
        goto FunctionExit;
    }

    vl_RetVal = netlnk_socket_recv(vp_netlinkFD,pl_RecvMsg);

    if(vl_RetVal == -1)
    {
        ERR("Amil1_03RecvNetlinkData : netlnk_socket_recv returned Error\n");
    }
    else
    {
        INF("Amil1_03RecvNetlinkData : Data is available now on netlnk_socket_recv");

        vl_RetVal = *pl_RecvMsg;

        DBG("Recvd following modem status from netlink %d\n", vl_RetVal);

        switch( *pl_RecvMsg )
        {
            case MODEM_RESET_IND :
            {
                INF("MODEM_RESET_IND is received initiating Deintialisation of clients \n");
                Amil1_05DeInitializeAllClients();
                vg_Modem_status = K_NETLINK_MODEM_NOT_READY;
                INF("State set to K_NETLINK_MODEM_NOT_READY\n");
            }
            break;

            case MODEM_STATUS_ONLINE :
            {

                INF("MODEM_STATUS_ONLINE is received initiating Intialisation of clients \n");
                Amil1_04InitializeAllClients();
                vg_Modem_status = K_NETLINK_MODEM_READY;
                INF("State set to K_NETLINK_MODEM_READY\n");
            }
            break;

            case MODEM_STATUS_OFFLINE :
            {

                /* TODO : Case not handled */
                /*
                            INF("MODEM_RESET_IND is received initiating Deintialisation of clients \n");
                            Amil1_05DeInitializeAllClients();
                            */
            }
            break;
        }
    }

    OSA_Free(pl_RecvMsg);
    return vl_RetVal;

    FunctionExit:
    ERR("Error in Amil1_03RecvNetlinkData\n");
    OSA_Free(pl_RecvMsg);
    return vl_RetVal;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4

//Calls each Clients Initialization Function

void Amil1_04InitializeAllClients()
{
    int8_t vl_Index;

    for(vl_Index =0; vl_Index<=K_AMIL_MAX_DBCLIENTS; vl_Index++)
    {
        /* Check whether the Client is in deintialised state */
        if(ClientDB[vl_Index].ClientState==K_AMIL_CLIENT_DEINITIALIZED)
        {
            if( ClientDB[vl_Index].init != NULL )
            {
                bool vl_Error = ClientDB[vl_Index].init(vl_Index);

                /* Changing the state in ClientDatabase to initialised */
                if(    !vl_Error
                    && ClientDB[vl_Index].ClientState ==K_AMIL_CLIENT_DEINITIALIZED)
                    ClientDB[vl_Index].ClientState = K_AMIL_CLIENT_INITIALIZED;
            }
        }
    }
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5

//Calls each Clients Initialization Function
void Amil1_05DeInitializeAllClients()
{
    int8_t vl_Index;
    for(vl_Index=0;vl_Index<K_AMIL_MAX_DBCLIENTS;vl_Index++)
    {
        //Check whether the Client is in intialised state
        if(ClientDB[vl_Index].ClientState==K_AMIL_CLIENT_INITIALIZED)
        {
            Amil1_13DeInitializeClient(vl_Index);
        }
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 6

/*Each client calls this function individually,once Deintialisation of all clients is trigerred
*Input param : Client Specific FD,callaback func and Handle which is the client's index in ClientDatabase clients*/
void Amil1_06RegisterForListen(int32_t vp_fd, t_AmilClientsCallback pp_DataRecvCB, int8_t vp_handle)
{
    Amil1_08AddFileDescriptor(vp_fd, pp_DataRecvCB, vp_handle);
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 7

/*Each client calls this function individually,once Deintialisation of all clients is trigerred
  *parameter: fd :- File descriptor and handle,viz.. index of the client in ClientDatabase.*/
void Amil1_07DeRegister(int32_t vp_fd, int8_t vp_handle)
{
    Amil1_09RemoveFileDescriptor(vp_fd);

    INF("%s : Removed client fd %d \n", __func__, vp_fd);
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 8
//Builds a list for  main select loop
//parameter: fd :- File descriptor,callback function and handle : index of the client in ClientDatabase.
void Amil1_08AddFileDescriptor( int32_t vp_fd ,t_AmilClientsCallback pp_callback, int8_t vp_handle)
{
    t_AmilClientsSelectorBlock  *pl_newNode;
    pl_newNode = (t_AmilClientsSelectorBlock  *)malloc(sizeof(t_AmilClientsSelectorBlock));
    pl_newNode->fd          = vp_fd;
    pl_newNode->pp_callback = pp_callback;
    pl_newNode->handle      = vp_handle;
    pl_newNode->next        = NULL;

    INF("%s : Added client %p with fd = %d \n", __func__, pl_newNode, pl_newNode->fd);

    if(pg_FileDescriptorDB == NULL)
    {
        // List is empty, update head
        pg_FileDescriptorDB = pl_newNode;
    }
    else
    {
        // Stick it in first in the list, fastest
        pl_newNode->next = pg_FileDescriptorDB;
        pg_FileDescriptorDB = pl_newNode;
    }
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 9
/*Deletes the Fd from DBFileDescriptor list
  *Input param : Specific Client's FD */
void Amil1_09RemoveFileDescriptor( int32_t vp_fd)
{
    bool vl_result = FALSE;
    t_AmilClientsSelectorBlock  *pl_CurrentClient, *pl_PrevClient;

    INF("%s : Deleting the client with fd = %d\n", __func__, vp_fd);

    for (pl_CurrentClient = pg_FileDescriptorDB, pl_PrevClient = NULL;
            pl_CurrentClient != NULL;
            pl_CurrentClient = pl_CurrentClient->next)
    {

        // Look for matching entry
        if (pl_CurrentClient->fd == vp_fd)
        {
            // Fix up the chain to point past this entry
            if (pl_PrevClient == NULL) {
                // Entry at head of list
                pg_FileDescriptorDB = pl_CurrentClient->next;
            }
            else
            {
                // Entry not at head of list
                pl_PrevClient->next = pl_CurrentClient->next;
            }

            // Chain is fixed. Delete this entry
            INF("%s : Deleted client at %p with fd = %d\n", __func__, pl_CurrentClient, vp_fd);

            OSA_Free(pl_CurrentClient);

            // We're done, exit
            break;
        }
        else
        {
            pl_PrevClient = pl_CurrentClient;
        }
    }

    if (pg_FileDescriptorDB == NULL) {
        INF("All fds removed from database\n");
    }

}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 10

/*This function polls through the fds in DBFileDescriptor and  returns the maximum Fd value
*Input Para : Read Fds in Main select loop
*Returns : Maximum Fd value */
int32_t Amil1_10GetMaxFDOfClients(fd_set *set)
{

    t_AmilClientsSelectorBlock *pl_Client = pg_FileDescriptorDB;
    int32_t vl_MaxFD = -1;
    FD_ZERO(set);

    if(pg_FileDescriptorDB == NULL)
    {
        INF("Amil1_10GetMaxFDOfClients : DBClients is a Null pointer \n");
        return vl_MaxFD;
    }

    do
    {
        if (pl_Client->fd >= 0)
        {
            vl_MaxFD = MC_NETLINK_MAX(vl_MaxFD, pl_Client->fd);
        }

        pl_Client = pl_Client->next;
    } while (pl_Client != NULL);

    /* Reqiured in future for Debugging
       INF("Amil1_10GetMaxFDOfClients : Max FD of clients is : %d",vl_MaxFD);*/

    return vl_MaxFD;
}




#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 11
/*This function polls through the fds in DBFileDescriptor and
* calculates the timeout value for how long select on client socket needs to be blocked based on the state of the Fds in DBFileDescriptor
* If all the clients are initialized, select is intended to be blocked infinitely.
* Input parameters : void
*   Returns : Parameter of type timeval containing time waiting on select. It is set to NULL in case all clients are intialized.
*   Note    : Output parameter to be freed by the calling function   */
struct timeval* Amil1_11GetTimeOut( void )
{
    /* Contains the  Timeout value based on the initialization of clients */
    uint8_t vl_Index;
    struct timeval  vl_Timeout;
    struct timeval* pl_Timeout;

    for(vl_Index =0; vl_Index< K_AMIL_MAX_DBCLIENTS; vl_Index++)
    {
        if(ClientDB[vl_Index].ClientState == K_AMIL_CLIENT_DEINITIALIZED)
            break;
    }

    /* all clients are initialized */
    if ( K_AMIL_MAX_DBCLIENTS == vl_Index)
    {
        pl_Timeout = NULL;
    }
    /* One or more clients need to be initialized yet*/
    else
    {
        pl_Timeout          = (struct timeval*)OSA_Malloc(sizeof(vl_Timeout));
        pl_Timeout->tv_sec  = K_AMIL_SELECT_TIMEOUT_CLIENT_NOT_INIT_SEC;
        pl_Timeout->tv_usec = 0;
    }

    return pl_Timeout;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 12
//Checks for the current state of the Modem
bool Amil1_12CheckModemReady(int32_t vl_NetlinkFD)
{


  /*Thread is blocked on select until MODEM_STATUS_ONLINE is received */
 while( vg_Modem_status != K_NETLINK_MODEM_READY )
  {
    int32_t vl_Ret = -1;
    struct timeval vl_NetlinkTimeout;
    fd_set  vl_NetLinkRfd;
    int32_t vl_SelectRet = -1;

    vl_NetlinkTimeout.tv_sec  = K_AMIL_SOCKET_SELECT_TIMEOUT_SEC;
    vl_NetlinkTimeout.tv_usec = 0;

    vl_Ret = netlnk_socket_send(vl_NetlinkFD, MODEM_QUERY_STATUS_REQ);

    if (vl_Ret < 0) {
      INF("%s :netlnk_socket_send() failed\n", __func__);
      return FALSE;

    }
    else {
      INF("%s : MODEM_QUERY_STATUS_REQ sent on Netlink socket\n", __func__);
    }

    FD_ZERO(&vl_NetLinkRfd);
    FD_SET(vl_NetlinkFD, &vl_NetLinkRfd );

    /* After Netlink socket has been created do select without timeout for modem ready */
    vl_SelectRet = select(vl_NetlinkFD+1, &vl_NetLinkRfd, NULL, NULL, &vl_NetlinkTimeout);
    INF("%s : Select return value : vl_SelectRet = %d \n", __func__, vl_SelectRet);
    if ( vl_SelectRet== -1 )
    {
      ERR("%s : Select on netlnk socket returned Error\n", __func__);
      return FALSE;
    }
    else if( vl_SelectRet )
    {
      INF("%s : Data is available now on netlnk socket.\n", __func__);
      /*Receiving Netlink Data */
      if(FD_ISSET(vl_NetlinkFD,&vl_NetLinkRfd))
      {
        Amil1_03RecvNetlinkData(vl_NetlinkFD);
      }
    }
    else
    {
      DBG("%s : No Data on Netlink socket\n", __func__);
    }

    OSA_Sleep(K_AMIL_SLEEP_BEFORE_MODEM_QUERY_MSEC);
  }


   INF("%s : MODEM_STATUS_ONLINE recvd, Initializing all clients\n", __func__);
     return TRUE;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 13

//Calls Client De-initialization Function
void Amil1_13DeInitializeClient(int8_t vp_handle)
{
    // Handle is the same as ClientDB index
    int8_t vl_Index = vp_handle;

    if ( vl_Index < 0 || vl_Index >= K_AMIL_MAX_DBCLIENTS )
    {
        ERR("%s : vl_Index (%d) out of range\n", __func__, vl_Index);
        return;
    }

    //Check whether the Client is in intialised state
    if ( ClientDB[vl_Index].ClientState != K_AMIL_CLIENT_INITIALIZED )
    {
        ERR("%s : Cannot de-initialize client with index %d\n", __func__, vl_Index);
        return;
    }

    if ( ClientDB[vl_Index].deinit != NULL )
    {
        ClientDB[vl_Index].deinit(vl_Index);

        ClientDB[vl_Index].ClientState = K_AMIL_CLIENT_DEINITIALIZED;
    }
}

