package com.stericsson.sdk.signing.mockup;

/**
 * Helper class to store what message should be expected from client and what message to send back
 * from server to client to satisfy its request.
 * 
 * @author emicroh
 */
public class MockProtocolMessage {

    private FakeMessage clientMessage;

    private FakeMessage serverResponse;

    private boolean serverInitiated = false;

    private long timeout = -1;

    /**
     * Will create instance of this class with given arguments.
     * 
     * @param clientMsg
     *            Message expected to be recieved from client.
     *
     * @param response
     *            Appropriate response messages to be sent back to client.
     */
    public MockProtocolMessage(FakeMessage clientMsg, FakeMessage response) {
        this.clientMessage = clientMsg;
        serverResponse = response;
    }

    /**
     * Will create instance of this class with given arguments.
     * 
     * @param clientMsg
     *            Message expected to be recieved from client.
     *
     * @param response
     *            Appropriate response messages to be sent back to client.
     *
     * @param msgTimeout
     *          Timeout for waiting after this mock message is handled.
     */
    public MockProtocolMessage(FakeMessage clientMsg, FakeMessage response, long msgTimeout) {
        clientMessage = clientMsg;
        serverResponse = response;
        timeout = msgTimeout;
    }

    /**
     * Will create instance of this class with given arguments.
     * 
     * @param clientMsg
     *            Message expected to be recieved from client.
     *
     * @param response
     *            Appropriate response messages to be sent back to client.
     *
     * @param initByServer
     *          First should be sent message by server then wait for client response.
     */
    public MockProtocolMessage(FakeMessage clientMsg, FakeMessage response, boolean initByServer) {
        this(clientMsg, response);
        this.serverInitiated = initByServer;
    }

    /**
     * Will create instance of this class with given arguments.
     * 
     * @param clientMsg
     *            Message expected to be recieved from client.
     *
     * @param response
     *            Appropriate response messages to be sent back to client.
     *
     * @param initByServer
     *          First should be sent message by server then wait for client response.
     * 
     * @param msgTimeout
     *          Timeout for waiting after this mock message is handled.
     */
    public MockProtocolMessage(FakeMessage clientMsg, FakeMessage response, boolean initByServer, long msgTimeout) {
        this(clientMsg, response, initByServer);
        timeout = msgTimeout;
    }

    /**
     * @return Message which is expected from client.
     */
    public FakeMessage getClientMessage() {
        return clientMessage;
    }

    /**
     * @return Messages from server to be sent back to client as response for client message.
     */
    public FakeMessage getServerResponse() {
        return serverResponse;
    }

    /**
     * @return True if first should be sent message from server to client, false will start with sending message
     * from client to server which is default behavior.
     * 
     */
    public boolean isServerInitiated() {
        return serverInitiated;
    }

    /**
     * @param serverInit Set behavior for this message. True means that firstly should be sent message from server
     * to client, false cause opposite behavior.
     */
    public void setServerInitiated(boolean serverInit) {
        this.serverInitiated = serverInit;
    }

    /**
     * @return Timeout set for wait after this message is satisfied.
     */
    public long getTimeout() {
        return timeout;
    }

}
