package com.stericsson.sdk.brp;

/**
 * Remote server responses
 * 
 * @author xolabju
 * 
 */
public enum ServerResponse {

    /** ACK Response, sent as reply if the requested command is supported */
    ACK,

    /** ACK Response, sent as reply if the requested command is NOT supported */
    NAK,

    /** SUCCESS Response, sent as reply if the execution of the requested command succeeded */
    SUCCESS,

    /** ERROR Response, sent as reply if the execution of the requested command failed */
    ERROR,

    /** PROGRESS Response, sent to the client during long-running processes */
    PROGRESS,

    /** PROGRESSDEBUGMESSAGE Response, sent to the client during long-running processes in order to easier trace problems */
    PROGRESSDEBUGMESSAGE;

}
