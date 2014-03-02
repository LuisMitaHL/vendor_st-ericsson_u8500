package com.stericsson.sdk.backend.remote.io;

/**
 * 
 * @author xolabju
 * 
 */
public final class ServerProperties {

    private ServerProperties() {
    }

    /** The default port number used if no port is specified */
    public static final int DEFAULT_PORT_NUMBER = 8088;

    /** The system property to set to change the server listen port */
    public static final String PORT_PROPERTY_NAME = "backend.remote.brp.server.port";
}
