/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   main.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "testapp.h"
#include <assert.h>
#include "ENS_DBC.h"
#include "ee/api/ee.h"
#include "OMX_Core.h"
#include <cscall/common/include/modem_audio_isi.h>
#include "cscall/nmf/host/composite/cscall.hpp"
#include <timer/timerWrapped.hpp>
#include "FileBuffer.hpp"
#include "IdleTime.h"

#ifdef __SYMBIAN32__
#include "e32std.h"
#else
#define idle_start() 
#define idle_stop() 
#endif

#include "audiomodem_helper.hpp"
#define NB_BUFF_IN  (4)
#define NB_BUFF_OUT (4)

OMX_BUFFERHEADERTYPE modem_buff_in[NB_BUFF_IN];
OMX_BUFFERHEADERTYPE modem_buff_out[NB_BUFF_OUT];

OMX_BUFFERHEADERTYPE * modem_buff_in_ptr[NB_BUFF_IN];
OMX_BUFFERHEADERTYPE * modem_buff_out_ptr[NB_BUFF_OUT];

void dbc_assert(int a, DBCError_e errorId, const char * file, int line)
{
    if (!a) {
        TESTAPP_PRINTF("DBC_ASSERT: %d in %s@%d\n", errorId,file,line);
        assert(0);
    }
}

t_cm_error EnsWrapper_cm_bindComponentToUser(void*, unsigned long, const char*, t_cm_interface_desc*, unsigned long)
{
    return NMF_INVALID_PARAMETER;
}

t_cm_error EnsWrapper_cm_unbindComponentToUser(void*, unsigned long, const char*, t_cm_interface_desc**)
{
    return NMF_INVALID_PARAMETER;
}

#define STRING(x) #x
const char * OMX_EVENTTYPE_NAME[]=
{
    STRING(OMX_EventCmdComplete),
    STRING(OMX_EventError),
    STRING(OMX_EventMark),
    STRING(OMX_EventPortSettingsChanged),
    STRING(OMX_EventBufferFlag),
    STRING(OMX_EventResourcesAcquired),
    STRING(OMX_EventComponentResumed),
    STRING(OMX_EventDynamicResourcesAvailable),
    STRING(OMX_EventPortFormatDetected),
    STRING(OMX_EventMax)
};

const char* OMX_STATETYPE_NAME[]=
{
    STRING(OMX_StateInvalid),
    STRING(OMX_StateLoaded),
    STRING(OMX_StateIdle),
    STRING(OMX_StateExecuting),
    STRING(OMX_StatePause),
    STRING(OMX_StateTransientToIdle),
    STRING(OMX_StateWaitForResources),
    STRING(OMX_StateTransient),
    STRING(OMX_StateLoadedToIdleHOST),
    STRING(OMX_StateLoadedToIdleDSP),
    STRING(OMX_StateIdleToLoaded),
    STRING(OMX_StateIdleToExecuting),
    STRING(OMX_StateIdleToPause),
    STRING(OMX_StateExecutingToIdle),
    STRING(OMX_StateExecutingToPause),
    STRING(OMX_StatePauseToIdle),
    STRING(OMX_StatePauseToExecuting),
    STRING(OMX_StateMax)
};

const char* OMX_COMMANDTYPE_NAME[]=
{
    STRING(OMX_CommandStateSet),
    STRING(OMX_CommandFlush),
    STRING(OMX_CommandPortDisable),
    STRING(OMX_CommandPortEnable),
    STRING(OMX_CommandMarkBuffer),
    STRING(OMX_CommandMax)
};

const char * t_sample_freq_name[]= 
{
    STRING(FREQ_UNKNOWN),
    STRING(FREQ_192KHZ),
    STRING(FREQ_176_4KHZ),
    STRING(FREQ_128KHZ),
    STRING(FREQ_96KHZ),
    STRING(FREQ_88_2KHZ),
    STRING(FREQ_64KHZ),
    STRING(FREQ_48KHZ),
    STRING(FREQ_44_1KHZ),
    STRING(FREQ_32KHZ),
    STRING(FREQ_24KHZ),
    STRING(FREQ_22_05KHZ),
    STRING(FREQ_16KHZ),
    STRING(FREQ_12KHZ),
    STRING(FREQ_11_025KHZ),
    STRING(FREQ_8KHZ),
    STRING(FREQ_7_2KHZ),
    STRING(FREQ_LAST_IN_LIST)
};


#define INPUT_BUFFER_SIZE  256
#define OUTPUT_BUFFER_SIZE 512
#define BUFFER_UNIT_SIZE  sizeof(int)
int bufft1[INPUT_BUFFER_SIZE * BUFFER_UNIT_SIZE];
int bufft2[OUTPUT_BUFFER_SIZE * BUFFER_UNIT_SIZE];

#ifdef __SYMBIAN32__
char inputFile[256] = "z:\\nmf\\T04.INP";
char outputFile[256] = "c:\\CS3-testOutput.pcm";
#else
char inputFile[256]  = "/streams/T04.INP";
char outputFile[256] = "CS3-testOutput.pcm";
#endif

t_nmf_channel	nmfCallBackChannel;
int		_end_of_example = 0;

#define INPUT_BLOCK_SIZE_MS   1
#define INPUT_BLOCK_SIZE_BYTE INPUT_BLOCK_SIZE_MS * 16


class eventhandlerCB : public eventhandlerDescriptor {
    unsigned int			 id ;
    Isendcommand		*cmd ;
    Iarmnmf_emptythisbuffer	*etbCscallIn;
    Iarmnmf_fillthisbuffer	*ftbCscallOut;
    OMX_BUFFERHEADERTYPE		*buff_in;
    OMX_BUFFERHEADERTYPE		*buff_out;
    int init;

    virtual void eventHandler(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2) 
    {
        TESTAPP_PRINTF("eventhandlerCB(%d) - %s - %s (0x%x)- %s (0x%x)\n", id, OMX_EVENTTYPE_NAME[ev], 
                (ev == OMX_EventCmdComplete)?OMX_COMMANDTYPE_NAME[data1]:NULL, data1, 
                (ev == OMX_EventCmdComplete)?OMX_STATETYPE_NAME[data2]:NULL, data2);

        if (data2 == OMX_StateIdle) {
            if (init == 0) {
                cmd->sendCommand(OMX_CommandStateSet, OMX_StateExecuting);
                init = 1 ;
            } else _end_of_example = 1;
        }
        else if (data2 == OMX_StateExecuting) {
            FileBuffer *filebuffer = (FileBuffer *)buff_in->pAppPrivate;
            filebuffer->read(buff_in);
            etbCscallIn->emptyThisBuffer(buff_in);
            ftbCscallOut->fillThisBuffer(buff_out);
        }
    }

    public:
    eventhandlerCB(unsigned int id, Isendcommand *cmd, 
            Iarmnmf_emptythisbuffer *etbCscallIn, Iarmnmf_fillthisbuffer *ftbCscallOut,
            OMX_BUFFERHEADERTYPE *buf, OMX_BUFFERHEADERTYPE *buf2) :
        id(id), cmd(cmd), etbCscallIn(etbCscallIn), ftbCscallOut(ftbCscallOut), buff_in(buf), buff_out(buf2),init(0)
    {}
};


class fillthisbufferCB : public armnmf_fillthisbufferDescriptor {
    private:
        unsigned int id ;
        Iarmnmf_emptythisbuffer *cmd;

        virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer)
        {
            //    TESTAPP_PRINTF("fillthisbuffer (cscall Input Port) CallBack\n");
            OMX_BUFFERHEADERTYPE *omx_buffer = (OMX_BUFFERHEADERTYPE *)buffer;
            FileBuffer           *filebuffer = (FileBuffer *)omx_buffer->pAppPrivate;

            if (! filebuffer->is_eos())
            {
                filebuffer->read(omx_buffer);
                cmd->emptyThisBuffer(buffer);
            }
        }

    public:
        fillthisbufferCB(unsigned int id, Iarmnmf_emptythisbuffer *cmd, OMX_BUFFERHEADERTYPE *buf): 
            id(id), cmd(cmd)
    {}
};


class emptythisbufferCB : public armnmf_emptythisbufferDescriptor {
    private:
        unsigned int id ;
        Iarmnmf_fillthisbuffer *cmd;

        virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer)
        {
            OMX_BUFFERHEADERTYPE *omx_buffer = (OMX_BUFFERHEADERTYPE *)buffer;
            FileBuffer           *filebuffer = (FileBuffer *)omx_buffer->pAppPrivate;

            filebuffer->write(omx_buffer);

            if(omx_buffer->nFlags & OMX_BUFFERFLAG_EOS)
            {
                filebuffer->save(outputFile);
                NMF_PANIC("Test Over !\n");
            }
            else
            {
                omx_buffer->nFilledLen = 0;
                omx_buffer->nFlags = 0;
                cmd->fillThisBuffer(buffer);
            }
        }


    public:
        emptythisbufferCB(unsigned int id, Iarmnmf_fillthisbuffer *cmd):
            id(id), cmd(cmd)
    {}
};



class networkupdateCB : public cscall_nmf_host_protocolhandler_network_updateDescriptor
{
    private:
        bool						 is_call_active;
        FileBuffer					*filebuffer;

        cscall_nmf_host_composite_cscall		*cscall ;

        Icscall_nmf_host_wrapper_dec_configure *configureDecWrapper;
        Icscall_nmf_host_wrapper_enc_configure *configureEncWrapper;
        Ihst_common_pcmsettings		*encoderSettings;
        Isendcommand *sendcommand;

        const char *get_network_name(CsCallNetwork_t network)
        {
            switch(network)
            {
                case NETWORK_NONE:	return "NONE";
                case NETWORK_WCDMA:	return "3G";
                case NETWORK_GSM:   return "GSM";
                default:			return "Unknown Network";
            }
        }

        const char *get_codec_name(CsCallCodec_t speech_codec)
        {
            switch(speech_codec)
            {
                case CODEC_NONE:   return "NONE";
                case CODEC_GSM_FR: return "GSM FR";
                case CODEC_GSM_HR: return "GSM HR";
                case CODEC_EFR:    return "EFR";
                case CODEC_AMR_NB: return "AMR NB";
                case CODEC_AMR_WB: return "AMR WB";
                default : return "Unknown codec";
            }
        }

        virtual void newCodecReq(CsCallCodecReq_t config)
        {

            //	TESTAPP_PRINTF("NewCodecReq received - codec = %s, network = %s\n",
            //		   get_codec_name(config.speech_codec),
            //		   get_network_name(config.network));


            if(config.speech_codec != CODEC_NONE)
            {
                if(is_call_active)
                {
                    TESTAPP_PRINTF("Codec change Not supported\n");
                }
                else
                {
                    if(config.speech_codec == CODEC_AMR_NB)
                    {
                        is_call_active = true;
                        idle_start() ;
                        encoderSettings->newFormat(FREQ_8KHZ, 1, 16);
                    }
                }
            }
            else if(is_call_active)
            {
                is_call_active = false;
                filebuffer->save(outputFile);
                idle_stop() ;

#ifdef __SYMBIAN32__
                User::After(100000) ;  
#endif 
                sendcommand->sendCommand(OMX_CommandStateSet, OMX_StateIdle);
            }
        }

    public:
        networkupdateCB(FileBuffer *filebuffer, cscall_nmf_host_composite_cscall *cscalltest,
                Icscall_nmf_host_wrapper_dec_configure *dec_wrapper,
                Icscall_nmf_host_wrapper_enc_configure *enc_wrapper,
                Ihst_common_pcmsettings *pcm,
                Isendcommand *send)
            :filebuffer(filebuffer),cscall(cscalltest),configureDecWrapper(dec_wrapper),
            configureEncWrapper(enc_wrapper), encoderSettings(pcm), sendcommand(send)
    {is_call_active=false;}

};


class newformatCB : public hst_common_pcmsettingsDescriptor {
    unsigned int id ;

    virtual void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size)
    {
        //    TESTAPP_PRINTF("New Format Call Back : samplingFreq: %s, chan nb: %d, sample size: %d bits\n", t_sample_freq_name[sample_freq], chans_nb, sample_size);
    }

    public:
    newformatCB(unsigned int id){}
};



#ifdef __SYMBIAN32__
int mainL(int argc, char ** argv) ;
int main(int argc, char ** argv)
{
    TRAPD(error, mainL(argc, argv));
    __ASSERT_ALWAYS(!error, User::Invariant());
}

int mainL(int argc, char ** argv)
{
    RThread thread;
    thread.SetPriority(EPriorityAbsoluteRealTime4);
#else
    int main(int argc, char ** argv)
    {
#endif

        OMX_BUFFERHEADERTYPE	buff_in;
        buff_in.pBuffer   = (OMX_U8*)&bufft1;
        buff_in.nAllocLen = INPUT_BUFFER_SIZE;
        buff_in.nFlags    = 0;
        buff_in.nOffset   = 0;

        OMX_BUFFERHEADERTYPE	buff_out;
        buff_out.pBuffer   = (OMX_U8*)&bufft2;
        buff_out.nAllocLen = OUTPUT_BUFFER_SIZE;
        buff_out.nFlags    = 0;
        buff_out.nOffset   = 0;

        //OMX_ERRORTYPE omx_error;
        t_nmf_error	error;


        if (EE_CreateChannel(NMF_CHANNEL_PRIVATE, &nmfCallBackChannel) != NMF_OK) NMF_PANIC("PANIC - Creating channel\n") ; 

#ifdef __SYMBIAN32__

        NMF_LOG("Waiting initialization of HostEE...\n") ;
        User::After(800000) ;  
#endif 

        /* read parameters */
        if(argc > 1)
        {
            int i=0;
            while(argv[1][i] != '\0')
            {
                inputFile[i]=argv[1][i];
                i++;
            }
            inputFile[i]='\0';
        }
        if(argc > 2)
        {
            int i=0;
            while(argv[2][i] != '\0')
            {
                outputFile[i]=argv[2][i];
                i++;
            }
            outputFile[i]='\0';
        }

        buff_in.pAppPrivate  = (void *)new FileBuffer(inputFile, 16, true);
        buff_out.pAppPrivate = (void *)new FileBuffer(128000);

        /* Init Components */
        cscall_nmf_host_composite_cscall * cscall = cscall_nmf_host_composite_cscallCreate();
        if(cscall == NULL)  NMF_PANIC("PANIC - cscall is NULL\n") ;
        if (cscall->construct() != NMF_OK) NMF_PANIC("PANIC - CsCallTest3 Construct Error\n");

        // Bindings .....
        // Send command
        Isendcommand sendcommand;
        error = cscall->bindFromUser("sendcommand" ,2, &sendcommand);
        if (error != NMF_OK) NMF_PANIC("PANIC - bindFromUser sendcommand\n") ;

        // FSM init
        Ifsminit fsminit;
        error = cscall->bindFromUser("fsminit", 1, &fsminit);
        if (error != NMF_OK) NMF_PANIC("PANIC - fsminit \n");

        // Cscall input port to the host
        Iarmnmf_emptythisbuffer etbCscallIn;
        error = cscall->bindFromUser("emptythisbuffer" ,2, &etbCscallIn);
        if (error != NMF_OK) NMF_PANIC("PANIC - etbCscallIn \n");

        fillthisbufferCB *fillthisbuffer = new  fillthisbufferCB(1, &etbCscallIn, &buff_in);
        if (fillthisbuffer == NULL) NMF_PANIC("PANIC - fillthisbuffer is NULL");
        error = cscall->bindToUser(nmfCallBackChannel, "inputport", fillthisbuffer, 4);
        if (error != NMF_OK) NMF_PANIC("PANIC - bindtoUser inputport \n");

        // Cscall output port to the host
        Iarmnmf_fillthisbuffer ftbCscallOut;
        error = cscall->bindFromUser("fillthisbuffer" ,3, &ftbCscallOut);
        if (error != NMF_OK) NMF_PANIC("PANIC - ftbCscallOut \n");

        emptythisbufferCB *emptythisbuffer = new  emptythisbufferCB(1, &ftbCscallOut);
        if (emptythisbuffer == NULL) NMF_PANIC("PANIC - emptythisbuffer is NULL");
        error = cscall->bindToUser(nmfCallBackChannel, "outputport", emptythisbuffer, 4);
        if (error != NMF_OK) NMF_PANIC("PANIC - bindtoUser outputport\n");

        newformatCB *format = new newformatCB(4);
        if (format == NULL) NMF_PANIC("PANIC - format is NULL");
        error = cscall->bindToUser(nmfCallBackChannel, "outputsettings", format, 2);
        if (error != NMF_OK) NMF_PANIC("PANIC - bindtoUser outputsettings\n");

        Icscall_nmf_host_wrapper_dec_configure configureDecWrapper;
        error = cscall->bindFromUser("configure_decoder_wrapper", 1, &configureDecWrapper);
        if (error != NMF_OK) NMF_PANIC("PANIC - configureDecWrapper \n");

        Icscall_nmf_host_wrapper_enc_configure configureEncWrapper;
        error = cscall->bindFromUser("configure_encoder_wrapper", 1, &configureEncWrapper);
        if (error != NMF_OK) NMF_PANIC("PANIC - configureEncWrapper \n");

        Icscall_nmf_host_protocolhandler_configure configurePH;
        error = cscall->bindFromUser("configure_protocol", 1, &configurePH);
        if (error != NMF_OK) NMF_PANIC("PANIC - configurePH \n");

        Icscall_nmf_host_ulrate_configure configureUlRate;
        error = cscall->bindFromUser("configure_ulrate", 1, &configureUlRate);
        if (error != NMF_OK) NMF_PANIC("PANIC - configureUlRate \n");

        Ihst_common_pcmsettings encoderSettings;
        error = cscall->bindFromUser("pcmsettings", 1, &encoderSettings);
        if (error != NMF_OK) NMF_PANIC("PANIC - pcmsettings \n");


        networkupdateCB *networkupdate = new  networkupdateCB((FileBuffer *)buff_out.pAppPrivate, cscall,
                &configureDecWrapper, &configureEncWrapper,
                &encoderSettings, &sendcommand);
        if (networkupdate == NULL) NMF_PANIC("PANIC - networkupdate is NULL");
        error = cscall->bindToUser(nmfCallBackChannel, "networkUpdate", networkupdate, 2);
        if (error != NMF_OK) NMF_PANIC("PANIC - bindtoUser networkUpdate\n");

        // the callback should come fron the CSCALL (later to controller)
        eventhandlerCB *eventhandler = new  eventhandlerCB(1, &sendcommand, &etbCscallIn, &ftbCscallOut, &buff_in, &buff_out);
        if (eventhandler == NULL) NMF_PANIC("PANIC - eventhandler is NULL");
        error = cscall->bindToUser(nmfCallBackChannel, "proxy", eventhandler, 4);
        if (error != NMF_OK) NMF_PANIC("PANIC - bindtoUser proxy\n");

		timerWrapped *timer = timerWrappedCreate();
		if (timer->construct() != NMF_OK)  NMF_PANIC("PANIC - Timer Construct Error\n");

		cscall->bindComponent("timer", timer, "timer");
		timer->bindAsynchronous("alarm", 10, cscall, "alarm", 0);
		
        cscall->start();
		timer->start();
        TESTAPP_PRINTF("exemple started!...\n");

        // Get modem buffers
#ifdef __SYMBIAN32__    

        audiomodem_helper * helper = audiomodem_helper::getHandle() ; // get handle after init
        if (helper == 0) NMF_PANIC("PANIC - audiomodem_helper::getHandle()\n");

        for (int i=0; i<NB_BUFF_IN ;i++)  modem_buff_in_ptr[i]  = &modem_buff_in[i];
        for (int i=0; i<NB_BUFF_OUT ;i++) modem_buff_out_ptr[i] = &modem_buff_out[i];

        helper->allocateRxBuffersL(modem_buff_in_ptr, NB_BUFF_IN) ; 
        helper->allocateTxBuffersL(modem_buff_out_ptr, NB_BUFF_OUT) ; 

        configurePH.setModemBuffer(NB_BUFF_OUT, (OMX_BUFFERHEADERTYPE_p *)modem_buff_out_ptr,
                NB_BUFF_IN, (OMX_BUFFERHEADERTYPE_p *)modem_buff_in_ptr);

#elif __ARM_LINUX

        audiomodem_helper * amh = audiomodem_helper::getHandle();

        ModemBufferList * ulList = amh->getUplinkModemBufferList();
        ModemBufferList * dlList = amh->getDownlinkModemBufferList();

        unsigned int ulBuffercount = ulList->getBufferCount();
        unsigned int dlBuffercount = dlList->getBufferCount();

        for (unsigned int i=0; i<dlBuffercount ;i++)
        {  modem_buff_in_ptr[i] = dlList->getBuffer(i);
            NMF_LOG("DL Buff %d addr %x\n",i, modem_buff_in_ptr[i]); 
        }
        for (unsigned int i=0; i<ulBuffercount ;i++)
        {  modem_buff_out_ptr[i] = ulList->getBuffer(i);
            NMF_LOG("UL Buff %d addr %x\n",i, modem_buff_out_ptr[i]); 
        }

        // configure the protocol handler
        configurePH.setModemBuffer((t_uint16)ulBuffercount, modem_buff_out_ptr,
                (t_uint16)dlBuffercount, modem_buff_in_ptr);
#endif

        // configure ulrate
        RateAdaptUlConfig_t ulrate_params = {INPUT_BLOCK_SIZE_MS,false};
        configureUlRate.setParameter(ulrate_params);

        // start FSM
        fsmInit_t init      = {0, 0xff, 0, 0};
        init.traceInfoAddr  = new TraceInfo_t;
        init.id1            = 0x100;
        fsminit.fsmInit(init);

        /* Do CM job */
        while (_end_of_example==0) {
            char *message ;
            NMF::InterfaceReference *ctxt;

            error = EE_GetMessage(nmfCallBackChannel, (void**)&ctxt, &message, 1);
            if (error != NMF_OK) NMF_PANIC("PANIC - EE_GetMessage\n") ;

            EE_ExecuteMessage(ctxt, message);
        } /* while (_end_of_example==0) */

        //sendcommand.sendCommand(OMX_CommandStateSet, OMX_StateIdle);
#if 0  
        // Get last message
        {
            char *message ;
            NMF::InterfaceReference *ctxt ;

            EE_GetMessage(nmfCallBackChannel, (void**)&ctxt, &message, 1);
            EE_ExecuteMessage(ctxt, message);
        }
#endif  

        NMF_LOG("End of the test\n") ;

        /* Try to shutdown everything */
        cscall->stop();
		timer->stop();
		
#ifdef __SYMBIAN32__
        User::After(100000) ;  
#endif 

#ifdef __SYMBIAN32__
        helper->freeRxBuffers() ;
        helper->freeTxBuffers() ;
#endif
        delete(init.traceInfoAddr);

		cscall->unbindComponent("timer", timer, "timer");
		timer->unbindAsynchronous("alarm", cscall, "alarm");
		
		timer->destroy();
		timerWrappedDestroy(timer);
        cscall->destroy() ;
        cscall_nmf_host_composite_cscallDestroy(cscall);

        EE_CloseChannel(nmfCallBackChannel) ;

        return 0;
    } /* main */
