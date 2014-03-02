package com.stericsson.sdk.loader.communication;

/**
 * @author xtomlju
 */
public interface IAuthenticate {

    /**
     * Get control keys object.
     * 
     * @return Control keys object
     */
    Object[] getControlKeys();

    /**
     * Get response for a specified challenge.
     * 
     * @param challenge
     *            Challenge data
     * @return Response data for challenge data
     */
    byte[] getChallengeResponse(byte[] challenge);
}
