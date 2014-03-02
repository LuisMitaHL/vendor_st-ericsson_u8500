/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef ATC_COMMAND_LIST_H
#define ATC_COMMAND_LIST_H

/*************************************************************************
 * Includes
 *************************************************************************/

/* ATC Common related header files */
#include "atc_common.h"

/*************************************************************************
 * Types, constants and external variables
 *************************************************************************/

/* Enumeration of AT commands: (list all commands we have had in SB, PAS and
 * AT Core) */
typedef enum {
    AT_NO_COMMAND, /* No AT commands on command line. */
    AT_NO_OVERRIDE, /* Used to flag that the current command shall not be overridden when calling Parser_SendResponse. */
    AT_UNKNOWN_COMMAND, /* Unknown AT command. */
    AT_PENDING, /* Pending AT command. */
    AT_, /* Command mode test. */
    AT_A, /* Answer. */
    AT_AND_C, /* Carrier detect signal behavior description. */
    AT_AND_D, /* DCE response control. */
    AT_AND_F, /* Similar to ATZ. */
    AT_D, /* Dial. */
    AT_STAR, /* List all implemented commands. */
    AT_H, /* Hangup. */
    AT_L, /* Monitor speaker loudness control. */
    AT_M, /* Monitor speaker mute. */

    /* Ensemble S21 (Test Commands) */
    AT_STAR_ZAPC, /* Area Pixel Checksum. */
    AT_STAR_ZIPI, /* Inquire Row Pixel Information. */
    AT_STAR_ZFSC, /* Create File */
    AT_STAR_ZFSD, /* Delete File */
    AT_STAR_ZFSI, /* Shows information about a given file */
    AT_STAR_ZFSL, /* Creates a link to a given file */
    AT_STAR_ZFST, /* List File System Directory */
    AT_STAR_ZFSP, /* Set Privilegies on a file */
    AT_STAR_ZFSR, /* Read File */
    AT_STAR_ZFSW, /* Write To File */
    AT_STAR_ZORRO, /* Format the File System */

    /*GSM specific AT commands (according to GSM 07.07). */
    AT_PLUS_BLDN,
    AT_PLUS_BINP,
    AT_PLUS_BSIR,
    AT_PLUS_CACM, /* Accumulated call meter. */
    AT_PLUS_CAOC, /* Advice of charge. */
    AT_PLUS_CCFC, /* Call Forwarding. */
    AT_PLUS_CALA, /* Alarm. */
    AT_PLUS_CALD, /* Alarm. */
    AT_PLUS_CALV, /* Alarm notification. */
    AT_PLUS_CAPD, /* Alarm. */
    AT_PLUS_CAMM, /* Accumulated call meter maximum. */
    AT_PLUS_CBC,  /* Report battery charge status. */
    AT_PLUS_CBST, /* Type of bearer service. */
    AT_PLUS_CSQ,  /* Report signal quality. */
    AT_PLUS_CPIN, /* Enter PIN code(s). */
    AT_PLUS_CCLK, /* Clock (time and date). */
    AT_PLUS_CDEV, /* Display event. */
    AT_PLUS_CEER, /* Extended Error Reporting (most recent cause) */
    AT_PLUS_CFUN, /* Phone functionality. */
    AT_PLUS_CGACT,/* Activate pdp context. */
    AT_PLUS_CGATT,/* Packet Service Attach or Detach */
    AT_PLUS_CGCMOD, /* PDP context modify. */
    AT_PLUS_CGDCONT,/* Define pdp context. */
    AT_PLUS_CGEQMIN,/* 3G Quality of Service Profile (Minimum Acceptable) */
    AT_PLUS_CGEQNEG,/*3G Quality of Service Profile (Negotiated)*/
    AT_PLUS_CGEQREQ,/* 3G Quality of Service Profile (Requested) */
    AT_PLUS_CGQMIN,/* 2G Quality of Service Profile (Minimum Acceptable) */
    AT_PLUS_CGQREQ,/* 2G Quality of Service Profile (Requested) */
    AT_PLUS_CGEREP, /* Set/unset reports on unsolicited events in ps. */
    AT_PLUS_CGMI, /* Manufacture Identification. */
    AT_PLUS_CGMM, /* Model Identification. */
    AT_PLUS_CGMR, /* Revision Identification. */
    AT_PLUS_CGPADDR,/* Show PDP Address. */
    AT_PLUS_CGREG,
    AT_PLUS_CGSN, /* Serial number request. */
    AT_PLUS_CHLD, /* Call Hold and Multiparty */
    AT_PLUS_CHSC, /* Current HSCSD call parameters. */
    AT_PLUS_CHSD, /* HSCSD features supported by ME/TA. */
    AT_PLUS_CHSN, /* Parameters for non-transparent HSCSD calls. */
    AT_PLUS_CHUP, /* Hang up call (ALS). */
    AT_PLUS_CIND, /* MS indicator control. */
    AT_PLUS_CIMI, /* Read IMSI. */
    AT_PLUS_CLAC, /* List all available AT commands. */
    AT_PLUS_CLAN, /* MMI language selection command. */
    AT_PLUS_CLCK, /* Facillity lock. */
    AT_PLUS_CLCC, /* List Current Calls. */
    AT_PLUS_CLIP, /* Calling line identification presentation. */
    AT_PLUS_CLIR, /* Calling line identification restriction. */
    AT_PLUS_CMAR, /* Master reset. */
    AT_PLUS_CMEE, /* GSM error codes enable and disable. */
    AT_PLUS_CMER, /* Mobile equipment event reporting. */
    AT_PLUS_CMGC, /* Send SMS command. */
    AT_PLUS_CMGD, /* Delete message. */
    AT_PLUS_CMGF, /* Message format. */
    AT_PLUS_CMGL, /* List messages. */
    AT_PLUS_CMGR, /* Read messages. */
    AT_PLUS_CMGS, /* Send message. */
    AT_PLUS_CMGW, /* Write message to memory. */
    AT_PLUS_CMOD, /* Call mode. */
    AT_PLUS_CMT,  /* Unsolicited SMS is directly routed to TE. */
    AT_PLUS_CMTI, /* Unsolicited SMS received indication. */
    AT_PLUS_CBM,  /* Unsolicited cell broadcast message. */
    AT_PLUS_CDS,  /* Unsolicited SMS status report is directly routed to TE. */
    AT_PLUS_CDSI, /* Unsolicited SMS status report received indication. */
    AT_PLUS_CMUX, /* TS 07.10 support. */
    AT_PLUS_CKPD, /* Keypad emulation command. */
    AT_PLUS_CMSS, /* Send SMS message from storage. */
    AT_PLUS_CMUT,
    AT_PLUS_CLVL, /* Master volume set */
    AT_PLUS_CNAP, /* Calling name identification presentation. */
    AT_PLUS_CNMA,
    AT_PLUS_CNMI, /* New message indications. */
    AT_PLUS_CNUM, /* Subscriber MSISDN number. */
    AT_PLUS_COLP, /* Connected line identification presentation. */
    AT_PLUS_COLR, /* Connected line identification presentation. */
    AT_PLUS_COPN, /* Operator Names. */
    AT_PLUS_COPS, /* Operator selection. */
    AT_PLUS_CPAS, /* Phone activity status. */
    AT_PLUS_CPBF, /* List phone book entries in alphabetical order. */
    AT_PLUS_CPBR, /* Read phone book entries. */
    AT_PLUS_CPBW, /* write phone book entries. */
    AT_PLUS_CPMS, /* Preferred message storage. */
    AT_PLUS_CSVM, /* Set voice mail number */
    AT_PLUS_CPOL, /* Preferred Operator List. */
    AT_PLUS_CPUC, /* Price per unit and currency table. */
    AT_PLUS_CPWD, /* Change facillity lock password. */
    AT_PLUS_CR,   /* Service reporting control. */
    AT_PLUS_CRC,  /* Cellular result codes. */
    AT_PLUS_CREG, /* Network registration. */
    AT_PLUS_CRES, /* Restore service profile. */
    AT_STAR_EEMPAGE, /* Engineer Mode Page control */
    AT_STAR_EREG, /* Network registration. */
    AT_STAR_ESMSFULL,/* SMS full. */
    AT_STAR_EVIB, /* Vibrator manipulation, on/off */
    AT_STAR_ECHD, /* USB charging settings for current */
    AT_PLUS_CCHC, /* Close Logical Channel */
    AT_PLUS_CCHO, /* Open Logical Channel */
    AT_PLUS_CGLA, /* Generic UICC Logical Channel Access */
    AT_PLUS_CRSM, /* Restricted SIM access */
    AT_PLUS_CSIM, /* Generic SIM access */
    AT_PLUS_CSAS, /* Save service profile. */
    AT_PLUS_CSCA, /* Service center address. */
    AT_PLUS_CSCB, /* Select cell broadcast message types. */
    AT_PLUS_CSCC, /* Secure control command. */
    AT_PLUS_CSCS, /* Select TE character set. */
    AT_PLUS_CSDF, /* Setting date format. */
    AT_PLUS_CSIL, /* Silence command. */
    AT_PLUS_CSMS, /* Select message service. */
    AT_PLUS_CSSN, /* Enable unsolicited SS response codes. */
    AT_PLUS_CSTF, /* Time format. */
    AT_PLUS_CTFR, /* Call Transfer */
    AT_PLUS_CUSD, /* Unstructured supplementary services. */
    AT_PLUS_CUUS1, /* User-to-User signalling supplementary service 1. */
    AT_PLUS_CVIB, /* ... vibrator status. */
    AT_PLUS_DS,   /* CS data compression */
    AT_PLUS_GCAP, /* Request Modem Capabilities List */
    AT_PLUS_GMI,  /* ... request manufacturer information. */
    AT_PLUS_GMM,  /* ... request model identification. */
    AT_PLUS_GMR,  /* ... request revision identification. */
    AT_PLUS_IFC,  /* Flowcontrol for online data mode */
    AT_PLUS_NREC,
    AT_PLUS_VTD,  /* DTMF tone duration.. */
    AT_PLUS_VTS,  /* DTMF tone generation. */
    AT_PLUS_WS46, /* Mode selection. */

    /* Ericsson specific AT commands. */
    AT_STAR_EACE, /* Extended call events reporting enable and disable. */
    AT_STAR_EALS, /* Request ALS status. */
    AT_STAR_EBAT, /* Extended call events reporting enable and disable. */
    AT_STAR_EBCA, /* Battery and charging algorithm. */
    AT_STAR_ECAM, /* Extended call monitoring enable and disable. */
    AT_STAR_ECME, /* CME unsolicited error code reporting enable and disable. */
    AT_STAR_ECPSCHBLKR,   /* COPS get RSA challenge data */
    AT_STAR_ECPSAUTHPE,    /* COPS authenticate permanent engineering mode */
    AT_STAR_ECPSDEAUTHPE, /* COPS deauthenticate permanent engineering mode */
    AT_STAR_ECPSLOCKBOOTP, /* COPS locks bootpartition permanently */
    AT_STAR_ECPSINITARB, /* COPS initializes the antirollback table */
    AT_STAR_ECPSDTW, /* COPS write Debug and Testing hash-keys */
    AT_STAR_ECPSAUTHU, /* Trigger COPS authenticate */
    AT_STAR_ECPSAUTHDU, /* COPS Write authentication data */
    AT_STAR_ECPSIMEIW, /* COPS write IMEI */
    AT_STAR_ECPSLCW, /* COPS write SIMLock keys */
    AT_STAR_ECPSSIMLOCK, /* COPS trigger SIMLock data */
    AT_STAR_ECPSSIMLOCKD, /* COPS write SIMLock data */
    AT_STAR_CPI,  /* Extended call monitoring enable and disable. */
    AT_STAR_ECSP, /* Read Customer Service Profile. */
    AT_STAR_ECSPSAW, /* CSPSA write parameter */
    AT_STAR_ECSPSAR, /* CSPSA read parameter */
    AT_STAR_ECRAT,   /* Current RAT Name */
    AT_STAR_ECUR, /* External current consumption. */
    AT_STAR_EDIF, /* Divert function enable. */
    AT_STAR_EDIS, /* Divert set. */
    AT_STAR_EDST, /* Daylight saving. */
    AT_STAR_EHSTACT, /* Host Activity. */
    AT_STAR_EIAAUW, /* Write authentication parameters */
    AT_STAR_EICO, /* ... icon command. */
    AT_STAR_EKSC, /* ... key sound change report. */
    AT_STAR_EKSP, /* ... ericsson key sound playback. */
    AT_STAR_EKSR, /* ... key sound change enable/disable. */
    AT_STAR_ELAN, /* ... MMI language selection command. */
    AT_STAR_ELAT, /* Unsolitited local comfort tone generation indication */
    AT_STAR_ELIN, /* Set Line. */
    AT_STAR_EMSR, /* MSR command */
    AT_STAR_EMAR, /* ... master reset. */
    AT_STAR_EMMI, /* ... MMI mode. */
    AT_STAR_EMME, /* ... enable Mail menue. */
    AT_STAR_ENAP, /* Network Access Point */
    AT_STAR_ERIN, /* ... set and get ring signal. */
    AT_STAR_ERIP, /* ... play ring signal. */
    AT_STAR_EPEE, /* PIN event enable. */
    AT_STAR_EPNR, /* ... read SIM preferred networks. */
    AT_STAR_EPNW, /* ... write SIM preferred networks. */
    AT_STAR_EPRR, /* ... phonebook personal ring-type read. */
    AT_STAR_EPRW, /* ... phonebook personal ring-type write. */
    AT_STAR_EPWRRED, /* TX Power Reduction */
    AT_STAR_ESAM, /* ... answer mode. */
    AT_STAR_ESAP, /* Set acoustic properties (apf) file */
    AT_STAR_ESBL, /* Back light mode. */
    AT_STAR_ESCN, /* ... set credit card number. */
    AT_STAR_ESDF, /* ... date format. */
    AT_STAR_EQVL, /* Volume level report. */
    AT_STAR_ESHLVOCR, /* Response for STK setup call pro-active command*/
    AT_STAR_ESIL, /* ... silence command. */
    AT_STAR_ESIMSR,
    AT_STAR_ESIMRF, /* Enable/disable SIM refresh events */
    AT_STAR_ESKL, /* Key lock mode. */
    AT_STAR_ESKS, /* Key sound mode. */
    AT_STAR_ESLEEP, /* Modem Sleep mode. */
    AT_STAR_ESLN, /* Line name (ALS). */
    AT_STAR_ESMA, /* Mail alert sound. */
    AT_STAR_ESMM, /* Minute minder mode. */
    AT_STAR_ESNU, /* Set phone/fax/data number. */
    AT_STAR_ESOM, /* Own melody. */
    AT_STAR_ESIMR,
    AT_STAR_ESTF, /* Time format. */
    AT_STAR_ESVM, /* Set voice mail number. */
    AT_STAR_ESVN, /* Read IMEISV */
    AT_STAR_ESZS, /* Alarm snooze. */
    AT_STAR_ETCH, /* Trickle charges rear slot. */
    AT_STAR_ETTY, /* Set acoustic properties (apf) file tty/handheld and start/stop tty mode for voice */
    AT_STAR_ETXT, /* Greeting text. */
    AT_STAR_EVOLC, /* Volume change report. */
    AT_STAR_EUPLINK, /* Send data uplink */
    AT_STAR_EXVC, /* External volume control. */
    AT_STAR_ECAP, /* ... camera button pressed */
    AT_STAR_ECAR, /* ... read allowed callers */
    AT_STAR_ECAS, /* Set allowed callers. */
    AT_STAR_ECAW, /* ... write allowed callers */
    AT_STAR_ETZR, /* time zone change event reporting */
    AT_STAR_ECLKM, /* Clock (time and date) in monotonic time */
    AT_STAR_EPSB, /* Current Packet Switched Bearer */
    AT_STAR_ENNIR, /* nitz name change event reporting */
    AT_STAR_EFSIM, /* Get full SIM status / content */
    AT_STAR_ERFSTATE, /* Set RF state reporting */
    AT_STAR_EGNCI, /* Request neighbouring 2G cell info */
    AT_STAR_EWNCI, /* Request neighbouring 3G cell info */
    AT_STAR_EGSCI, /* Request Serving 2G cell info */
    AT_STAR_EWSCI, /* Request Serving 3G cell info */
    AT_STAR_ECEXPIN, /* Extended pin control */


    /* Ensemble A27 */
    AT_STAR_EGPSC, /* ... MS configuration */
    AT_STAR_EGPSP, /* ... transfer current Position */

    /* Ensemble AC58 */
    AT_STAR_EAPP, /* Application Start. */
    AT_STAR_EKSE, /* Keystroke Send. */

    /* Ensemble AC62 */
    AT_STAR_EIMR, /* ... Input Method Change Report */

    /* Ensemble S11 */
    AT_STAR_ESTL, /* ... SMS Template List Edit. */

    /* Ensemble S16 */
    AT_STAR_ESDG, /* Delete Group. */
    AT_STAR_ESGR, /* Group Read. */
    AT_STAR_ESAG, /* Add to Group. */
    AT_STAR_EGIR, /* Group Item Read. */
    AT_STAR_ESDI, /* Group Delete Item. */

    /* Ensemble S24 */
    AT_STAR_EFOS, /* ... Font Size Setting */

    /* Ensemble S26 */

    /* Ensemble C20 */
    AT_STAR_EALR, /* ... audio line request. */
    AT_STAR_EALV, /* ... unsolicited audio line request. */
    AT_STAR_EAPM, /* ... audio path mode set. */
    AT_STAR_EARS, /* ... analog ring signal request. */
    AT_STAR_EASY,
    AT_STAR_EASI, /* Unsolicited. */
    AT_STAR_EMIR, /* ... mute indication request. */
    AT_STAR_EMIV, /* ... unsolicited mute indication request. */
    AT_STAR_ELAM, /* ... local audio mode. */
    AT_STAR_EVAD, /* ... VAD function mode selection. */
    AT_STAR_EVAV, /* ... unsolicited VAD function mode request. */
    AT_STAR_EAMS, /* ... audio mode selection. */
    AT_STAR_EPHD, /* ... portable handsfree detection. */
    AT_STAR_EPPSD,
    AT_STAR_ECBP, /* ... CHF button pushed. */
    AT_STAR_EAPN, /* ... rename active profile */
    AT_STAR_EAPS, /* ... set active profile */
    AT_STAR_EPED, /* ... remove accessory association from profile */
    AT_STAR_EPEW, /* ... add accessory association from profile */

    /* Ensemble C21 */
    AT_STAR_EAM, /* ... add accessory menu item. */
    AT_STAR_EAST, /* ... accessory status text. */
    AT_STAR_EASM, /* ... accessory sub menu. */
    AT_STAR_EAID, /* ... accessory input dialog. */

    /* Ensemble C24 */
    AT_STAR_EVA, /* ... answer. */
    AT_STAR_EVH, /* ... hook. */
    AT_STAR_EVD, /* ... dial. */

    /* Ensemble C26 */
    AT_STAR_EINA, /* ... returns the active interface */

    /* Ensemble S6 */
    AT_STAR_EIPS, /* ... identity presentation set */

    /* Ensemble S26 */
    AT_STAR_EACS, /* ... notifies MS that an accessory has been connected */
    AT_STAR_ECSCA, /* ... CSCA functionality */

    /* Ensemble S29 */
    AT_STAR_EWIL, /* ... image load. */
    AT_STAR_EWHP, /* ... homepage. */
    AT_STAR_EWDT, /* ... download timeout. */
    AT_STAR_EWCT, /* ... connection timeout. */
    AT_STAR_EWLI, /* ... login. */
    AT_STAR_EWCL, /* ... connection login. */
    AT_STAR_EWPB, /* ... preferred bearer. */
    AT_STAR_EWCG, /* ... CSD gateway. */
    AT_STAR_EWIP, /* ... IP-network phonenumber. */
    AT_STAR_EWSA, /* ... SMSC address. */
    AT_STAR_EWSG, /* ... SMS gateway. */
    AT_STAR_EWPR, /* ... profiles. */
    AT_STAR_EWPN, /* ... profile name. */
    AT_STAR_EWBA, /* ... bookmark add/delete */
    AT_STAR_EWBR, /* ... read bookmark */

    /* Ensemble C30 */
    AT_STAR_EYPE, /* ... play entry. */
    AT_STAR_EYPP, /* ... play prompt. */
    AT_STAR_EYRZ, /* ... recognize. */
    AT_STAR_EYTN, /* ... train name. */
    AT_STAR_EYPT, /* ... play training recording. */
    AT_STAR_EYRR, /* ... recording result. */
    AT_STAR_EYDE, /* ... delete entry. */
    AT_STAR_EYSR, /* ... save recording. */
    AT_STAR_EYRE, /* ... recognize entry. */
    AT_STAR_EYAB, /* ... abort. */
    AT_STAR_EYDO, /* ... done. */
    AT_STAR_EYRV, /* ... register VAD. */
    AT_STAR_EYGP, /* ... get phones. */
    AT_STAR_EYPI, /* ... phone info. */
    AT_STAR_EYDP, /* ... delete phone. */
    AT_STAR_EYSS, /* ... start synchronize. */
    AT_STAR_EYPV, /* ... Play VAD. */

    /* Ensemble C31 */
    AT_STAR_EMLR, /* ... read menu candidates list. */
    AT_STAR_ECMW, /* ... write customize menu. */

    /* Ensemble C38 */
    AT_STAR_EIBA,
    AT_PLUS_VGM, /* Store microphone gain received from a Bluetooth HF */
    AT_PLUS_VGS, /* Store speaker gain received from a Bluetooth HF */

    /* Parser specific commands. */
    AT_E,  /* ... echo enable and disable. */
    AT_S0, /* ... Automatic answer control. */
    AT_S3, /* ... termination character parameter. */
    AT_S4, /* ... response formatting character parameter. */
    AT_S5, /* ... deletion character parameter. */
    AT_S6, /* ... pause before blind dialling. */
    AT_S7, /* ... connection completion timeout. */
    AT_S8, /* ... comma dial modifier time. (dummy) */
    AT_S10,/* ... automatic disconnect delay control. */
    AT_V,  /* ... verbose responses enable & disable. */
    AT_Q,  /* ... response enable and disable. */
    AT_X,  /* ... call progress monitoring control. */
    AT_Z,  /* Reset to Default Configuration. */
    AT_PLUS_RADIOVER, /*get modem version*/
    /* AT result codes. */
    AT_TEXT_RESPONSE, /* Response is an undefined text string. */
    AT_OK, /* Response when successful. */
    AT_ABORT_OK, /* Response when successful abort. */
    AT_ERROR, /* Response when not successful. */
    AT_CONNECT, /* Intermediate response when dialing. */
    AT_NO_CARRIER, /* Response when failing to connect. */
    AT_NO_DIALTONE, /* Response when failing to dial. */
    AT_BUSY, /* Response when called party is busy. */

    AT_PLUS_CAOV, /* AoC unsolicited result code. */
    AT_PLUS_CCCM, /* Unsolicited advice of charge message. */
    AT_PLUS_CIEV, /* Indicator event. */
    AT_PLUS_CSSI, /* Supplementary service intermediate result code. */
    AT_PLUS_CSSU, /* Supplementary service unsolicited result code. */
    AT_PLUS_CUUS1U, /* User-to-User signalling service 1 unsolicited result code. */
    AT_STAR_ECAV, /* Extended call monitoring unsolicited result code. */
    AT_STAR_ECWV, /* Call meter warning event. */
    AT_STAR_EPEV, /* PIN event. */
    AT_STAR_EAAI, /* Accessory additional indication. */
    AT_STAR_EAMI, /* Accessory menu indication. */
    AT_STAR_EAII, /* Accessory input indication. */
    RING,         /* Incoming Call Indication. */
    CGEV,         /* Packet Domain Event Reporting. */

    AT_NO_ANSWER, /* Response when called party does not answer. */
    AT_CME_ERROR, /* Response with mobile equipment error information. */
    AT_CMS_ERROR, /* Response with SMS error information. */

    AT_PLUS_CKEV, /* Unsolicited response when event reporting is on. */
    AT_STAR_ELIP, /* Unsolicited response if caller identification req. */
    AT_STAR_EOLP, /* Unsolicited response if called identification req. */
    AT_STAR_ENAD,
    AT_STAR_EIMV, /* Unsolicited input method event. */
    AT_STAR_ETZV, /* Unsolicited response when time changes and at network registration */

    /* Ensemble S15 (GPRS) */
    AT_PLUS_CGSMS, /* Select service for MO SMS messages */
    AT_PLUS_CMMS,

    AT_STAR_ZPPI, /* Read parameters during production. */
    AT_STAR_STKC, /* Sim Toolkit Configuration */
    AT_STAR_STKN, /* UR code */
    AT_STAR_STKI, /* UR code */
    AT_STAR_STKEND, /* UR code */
    AT_STAR_STKE, /* Sim Toolkit Envelope Command */
    AT_STAR_STKR, /* Sim Toolkit Command Response */
    AT_PLUS_CMEC,
    AT_PLUS_CCWA,
    AT_PLUS_CEAP,
    AT_PLUS_CERP,
    AT_PLUS_CUAD,
    AT_STAR_EHNET,
    AT_STAR_EPINR, /* STE get number of retries for different pin/puk  */
    AT_PLUS_CCWE,
    AT_PLUS_CCWV, /* Unsolicited AOC 30sec warning message. */
    AT_STAR_EPBC,
    /* Customer specific commands */

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <inc_atc_extended_command_list.h>
#endif
    AT_THE_END
} AT_Command_e;

/*************************************************************************/
#endif /* ATC_COMMAND_LIST_H */
