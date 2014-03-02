package com.stericsson.sdk.equipment.io.tcp;

import static com.stericsson.sdk.equipment.Activator.getServiceObject;

import java.io.IOException;
import java.io.InterruptedIOException;
import java.net.ServerSocket;
import java.net.Socket;

import org.apache.log4j.Logger;

import com.stericsson.sdk.equipment.io.port.IPortReceiver;
import com.stericsson.sdk.equipment.io.port.PortDeliveryState;
import com.stericsson.sdk.equipment.io.tcp.internal.TCPIPPort;

/**
 * Server is listening on specified TCP\IP port and setting up connections to mobile devices
 * 
 * @author xtomzap
 * 
 */
public class TCPIPServer implements Runnable {

    private boolean killed = false;

    private ServerSocket serverSocket;

    private int serverPort;

    private Logger logger = Logger.getLogger(TCPIPServer.class.getName());

    /**
     * Constructor
     * 
     * @param port
     *            number
     */
    public TCPIPServer(int port) {
        serverPort = port;
    }

    /**
     * new thread starts here
     */
    public void run() {
        try {
            serverSocket = new ServerSocket(serverPort);
            logger.info("Started TCP/IP Server on port " + serverPort);

            while (!killed) {
                Socket socket = null;
                try {
                    socket = serverSocket.accept();
                    deliverPort(socket);
                } catch (InterruptedIOException ex) {
                    continue;
                } catch (IOException e) {
                    logger.error("Failed to accept socket connection: " + e.getMessage());
                }
            }
        } catch (IOException e) {
            logger.error("Failed to start TCP/IP Server on port " + serverPort + ": " + e.getMessage());
        } finally {
            stop();
        }
    }

    private void deliverPort(final Socket socket) {
        try {
            IPortReceiver portReciever = getServiceObject(IPortReceiver.class);
            if (portReciever != null) {
                portReciever.deliverPort(new TCPIPPort(socket), PortDeliveryState.SERVICE, false);
            }
        } catch (IOException e) {
            logger.error("Ethernet port delivery failed!", e);
        }
    }

    /**
     * Stop TCP/IP server
     */
    public void stop() {
        killed = true;
        if (serverSocket != null) {
            try {
                serverSocket.close();
                serverSocket = null;
            } catch (IOException ioe) {
                logger.error("Error when closing server socket!", ioe);
            }
        }
    }
}
