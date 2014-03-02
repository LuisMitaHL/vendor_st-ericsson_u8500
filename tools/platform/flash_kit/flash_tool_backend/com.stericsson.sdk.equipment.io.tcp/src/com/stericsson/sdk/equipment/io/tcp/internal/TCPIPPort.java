package com.stericsson.sdk.equipment.io.tcp.internal;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.InterfaceAddress;
import java.net.NetworkInterface;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.util.Enumeration;
import java.util.List;

import org.apache.log4j.Logger;

import com.stericsson.sdk.equipment.io.AbstractPort;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * @author xtomzap
 * 
 */
public class TCPIPPort extends AbstractPort {

    private Logger logger = Logger.getLogger(TCPIPPort.class.getName());

    /** Stream to write to */
    private OutputStream outputStream = null;

    /** Stream to read from */
    private InputStream inputStream = null;

    private Socket clientSocket;

    /** Unique system identifier of current Ethernet interface connection */
    private int scopeId;

    private static int uniqueID = 0;

    /**
     * Constructor
     * 
     * @param socket
     *            instance
     */
    public TCPIPPort(Socket socket) {
        super("TCP" + Integer.toString(uniqueID));
        clientSocket = socket;
        uniqueID++;

        Inet6Address inetAddr = (Inet6Address) socket.getInetAddress();
        scopeId = inetAddr.getScopeId();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void open() throws PortException {
        try {
            clientSocket.setSoTimeout(1000);

            outputStream = clientSocket.getOutputStream();
            inputStream = clientSocket.getInputStream();
            super.open();
        } catch (IOException e) {
            close();
            throw new PortException(e);
        }
    }

    /**
    * {@inheritDoc}
    */
    @Override
    public void close() throws PortException {
        try {
            if (outputStream != null) {
                try {
                    outputStream.close();
                    outputStream = null;
                } catch (IOException e) {
                    logger.error("Error when closing client socket output stream!", e);
                }
            }

            if (inputStream != null) {
                try {
                    inputStream.close();
                    inputStream = null;
                } catch (IOException e) {
                    logger.error("Error when closing client socket input stream!", e);
                }
            }

            if (clientSocket != null) {
                try {
                    clientSocket.close();
                    clientSocket = null;
                } catch (IOException e) {
                    logger.error("Error when closing client socket!", e);
                }
            }
        } finally {
            super.close();
        }
    }

    /**
     * {@inheritDoc}
     */
    public int read(byte[] buffer, int offset, int length, int timeout) throws PortException {
        int read = 0;
        int totallyRead = 0;

        if (!isOpen()) {
            throw new PortException("Cannot read from port. TCP\\IP port is not opened.");
        }
        try {
            while (totallyRead < length) {
                try {
                    read = inputStream.read(buffer, totallyRead, (length - totallyRead));
                    if (read < 0) {
                        throw new PortException("Reached end of stream before all required data has been read.");
                    }

                    if (read >= 0) {
                        totallyRead += read;
                    }
                } catch (SocketTimeoutException e) {
                    checkUsbEthInterface();
                }
            }
        } catch (IOException e) {
            throw new PortException(e);
        }

        return totallyRead;
    }

    /**
     * This method goes through all network connections and tries to find out if current connection is still present.
     * If it is not, then it means that device has been disconnected.
     *
     * @throws PortException
     */
    private void checkUsbEthInterface() throws PortException {
        boolean found = false;
        Enumeration<NetworkInterface> en;
        try {
            en = NetworkInterface.getNetworkInterfaces();

            while (en.hasMoreElements()) {
                NetworkInterface netInterface = en.nextElement();
                List<InterfaceAddress> addrList = netInterface.getInterfaceAddresses();

                for (InterfaceAddress interfaceAddr : addrList) {
                    InetAddress ia = interfaceAddr.getAddress();
                    if (ia instanceof Inet6Address) {
                        Inet6Address ia6 = (Inet6Address) ia;
                        if (ia6.getScopeId() == scopeId) {
                            found = true;
                        }
                    }
                }
            }

            if (!found) {
                throw new PortException("USB Ethernet device " + getPortName() + " disconected.");
            }
        } catch (SocketException e) {
            throw new PortException("Not able to get available network interfaces", e);
        }
    }

    /**
     * {@inheritDoc}
     */
    public int write(byte[] buffer, int offset, int length, int timeout) throws PortException {
        try {
            outputStream.write(buffer, offset, length);
            outputStream.flush();
        } catch (IOException e) {
            throw new PortException(e);
        }
        return length;
    }

}
