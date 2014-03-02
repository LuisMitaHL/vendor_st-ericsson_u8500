package com.stericsson.ftc;


/**
 * Application context including crucial information about current state of application.
 * 
 * @author TSIKOR01
 * 
 */
public class FTCApplicationContext {

    private FTCPath workingDirectory;

    private final String host;

    private final int port;

    /**
     * Default constructor of application context, console is also reading port and host from this
     * instance of Flash tool cli.
     * 
     * @param pHost
     *            Host address
     * @param pPort
     *            Communication port
     */
    public FTCApplicationContext(String pHost, int pPort) {
        this.host = pHost;
        this.port = pPort;
        workingDirectory= new FTCPath(FTCPath.PC_PREFIX, System.getProperty("user.dir"));
    }

    /**
     * Getter for address where Flash tool backend is executed.
     * 
     * @return Host address
     */
    public String getHost() {
        return host;
    }

    /**
     * Getter for communication port of current CLI instance.
     * 
     * @return Communication port
     */
    public int getPort() {
        return port;
    }

    /**
     * Getter for current working directory path.
     * 
     * @return Working directory
     */
    public FTCPath getWorkingDirectory() {
        return workingDirectory;
    }

    /**
     * Setter for current working directory path.
     * 
     * @param path
     *            Parsed path to change working directory.
     */
    public void setWorkingDirectory(FTCPath path) {
        this.workingDirectory = path;
    }
}
