package com.stericsson.sdk.cli;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.brp.ServerResponse;

/**
 * local test server
 * 
 * @author xdancho
 * 
 */
public class LocalServer {

    static boolean started = false;

    static final int PORT = 8088;

    Socket clientSocket = null;

    ServerSocket server = null;

    static boolean forceNAK = false;

    static boolean forceError = false;

    private volatile Queue<String> replies = new ConcurrentLinkedQueue<String>();

    private static void setStarted(boolean value) {
        started = value;
    }

    private static void setForceNAK(boolean value) {
        forceNAK = value;
    }

    private static void setForceError(boolean value) {
        forceError = value;
    }

    /**
     * start local test server
     * 
     */
    public LocalServer() {

        if (started) {
            return;
        }
        setStarted(true);
        BufferedReader input = null;
        BufferedReader in = null;
        PrintWriter out = null;

        try {
            server = new ServerSocket(PORT);
        } catch (IOException e) {
            System.out.println(e);
            return;
        }
        try {
            System.out.println("started test server on port: 8088");
            while (true) {

                clientSocket = server.accept();
                input = new BufferedReader(new InputStreamReader(System.in, "UTF-8"));
                if (clientSocket == null) {
                    throw new Exception("Failed to setup communication.");
                }
                out =
                    new PrintWriter(
                        new BufferedWriter(new OutputStreamWriter(clientSocket.getOutputStream(), "UTF-8")), true);
                in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream(), "UTF-8"));

                String fromClient = in.readLine();

                if (fromClient == null) {
                    throw new Exception("Received null.");
                }
                AbstractCommand aCmd = CommandFactory.createCommand(fromClient);
                if (aCmd == null) {
                    throw new Exception("Failed to parse command: " + fromClient + ".");
                }

                if (forceNAK) {
                    out.println(aCmd.getCommandString() + ";" + ServerResponse.NAK.name());
                    setForceNAK(false);
                    continue;
                }
                if (forceError) {
                    out.println(aCmd.getCommandString() + ";" + ServerResponse.ERROR.name() + ";" + "ERROR desc");
                    setForceError(false);
                    continue;
                }
                // send ACK
                out.println(aCmd.getCommandString() + ";" + ServerResponse.ACK.name());

                if (aCmd.isCancellable()) {
                    sendProgress(out, aCmd);
                }

                sendResult(out, aCmd);

            }
        } catch (Exception e) {
            System.out.println(e.getMessage());
        } finally {
            try {
                cleanup(input, in, out);
            } catch (IOException e) {
                System.out.println(e.getMessage());
            }

        }

    }

    private void cleanup(BufferedReader input, BufferedReader in, PrintWriter out) throws IOException {
        if (input != null) {
            input.close();
        }
        if (out != null) {
            out.close();
        }
        if (in != null) {
            in.close();
        }
    }

    private void sendProgress(PrintWriter out, AbstractCommand aCmd) throws InterruptedException {
        // send progress every second
        int progress = 0;
        while (progress != 100) {
            Thread.sleep(100);

            out.println(aCmd.getCommandString() + ";" + ServerResponse.PROGRESS.name() + ";" + progress + ";" + "1234");
            progress += 20;
        }
    }

    /**
     * @param msg
     *            Message to be replayed.
     */
    public void setReply(String msg) {
        replies.add(msg);
    }

    private void sendResult(PrintWriter out, AbstractCommand aCmd) {
        if (aCmd.getCommandName().equals(CommandName.BACKEND_GET_CONNECTED_EQUIPMENTS)) {
            out.println(getComplexResultString(aCmd, "DB8500;USB1;DB8500;USB2"));
        } else if (aCmd.getCommandName().equals(CommandName.BACKEND_GET_AUTO_SENSE_STATUS)) {
            out.println(getComplexResultString(aCmd, "Auto sense ON"));
        } else if (aCmd.getCommandName().equals(CommandName.BACKEND_GET_NEXT_CONNECTED_EQUIPMENT)) {
            out.println(getComplexResultString(aCmd, "DB8500;USB1;ProfileAlias;StateName;State"));
        } else if (!replies.isEmpty()) {
            out.println(replies.poll());
        } else {
            // send command complete
            out.println(aCmd.getCommandString() + ";" + ServerResponse.SUCCESS.name());
        }
    }

    private String getComplexResultString(AbstractCommand ac, String simulatedBackendResponse) {
        return ac.getCommandString() + ";" + ServerResponse.SUCCESS.name() + ";" + simulatedBackendResponse;
    }

    /**
     * force server to reply with Error
     */
    public static void forceError() {
        forceError = true;
    }

    /**
     * force the server to reply with NAK
     */
    public static void forceNAK() {
        forceNAK = true;
    }

    /**
     * 
     */
    public void stopServer() {
        try {
            if (server != null) {
                server.setReuseAddress(true);
                server.close();

            }
            setStarted(false);
        } catch (IOException e) {
            System.out.println(e.getMessage());
        }
    }

}
