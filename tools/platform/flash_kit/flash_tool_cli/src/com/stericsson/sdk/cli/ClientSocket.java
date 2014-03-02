package com.stericsson.sdk.cli;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;

/**
 * 
 * a socket class that handles read and writes
 * 
 * @author xdancho
 * 
 */
public final class ClientSocket {

    private static ClientSocket instance = new ClientSocket();

    PrintWriter out = null;

    BufferedReader in = null;

    Socket clientSocket = null;

    private ClientSocket() {

    }

    /**
     * get the only instance
     * 
     * @return the instance
     */
    public static ClientSocket getInstance() {
        return instance;
    }

    /**
     * starts the socket
     * 
     * @param host
     *            the host
     * @param port
     *            the port
     * @throws Exception
     *             socket exception
     */
    public void startSocket(String host, int port) throws Exception {

        clientSocket = new Socket(host, port);
        out =
            new PrintWriter(new BufferedWriter(new OutputStreamWriter(clientSocket.getOutputStream(), "UTF-8")), true);
        in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream(), "UTF-8"));

    }

    /**
     * stops the socket
     */
    public void stopSocket() {
        try {
            if (out != null) {
                out.close();
            }
            if (in != null) {
                in.close();
            }
            if (clientSocket != null) {
                clientSocket.close();
            }
        } catch (IOException ioe) {
            System.out.println(ioe.getMessage());
            System.out.print("\nException technical details:\n");
            ioe.printStackTrace(System.out);
        }
    }

    /**
     * sends a command
     * 
     * @param command
     *            the command to send
     */
    public void sendCommand(String command) {
        out.println(command);
    }

    /**
     * receive a command
     * 
     * @return the command
     * @throws IOException
     *             on errors
     */
    public String receiveCommand() throws IOException {
        return in.readLine();
    }

}
