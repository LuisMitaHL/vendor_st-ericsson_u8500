/**
 * 
 */
package com.stericsson.ftc.command;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

import org.junit.Assert;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandFactory;
import com.stericsson.sdk.brp.ServerResponse;

/**
 * @author Vit Sykala <vit.sykala@tieto.com>
 * 
 */
public class DummyBackend extends Thread {

    private class Reply {
        ServerResponse type;

        String text;

        /**
         * 
         */
        public Reply(ServerResponse pType, String pText) {
            text = pText;
            type = pType;
        }
    }

    private int serverPort;

    private volatile Queue<Reply> replies = new ConcurrentLinkedQueue<Reply>();

    private AssertionError assertionError;

    private ServerSocket ssocket;

    /**
         * 
         */
    public void startServer() {
        ssocket = createServerSocket();
        start();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void run() {
        Socket socket = null;
        try {
            while (true) {
                try {
                    socket = ssocket.accept();
                    handleClient(socket);
                } catch (SocketTimeoutException ste) {
                    continue;
                } catch (IOException e) {
                    if (!e.getMessage().equals("socket closed") && !e.getMessage().equals("Socket is closed")) {
                        e.printStackTrace();
                    }
                    break;
                } catch (AssertionError e) {
                    assertionError = e;
                }
            }
        } finally {
            if (ssocket != null) {
                try {
                    ssocket.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            ssocket = null;
        }
    }

    /**
     * @param socket
     *            communication socket.
     */
    public void handleClient(Socket socket) {
        if (socket != null && socket.isConnected()) {
            Reply reply = replies.poll();
            Assert.assertNotNull(reply);
            try {
                PrintWriter out =
                    new PrintWriter(new BufferedWriter(new OutputStreamWriter(socket.getOutputStream(), "UTF-8")), true);
                BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream(), "UTF-8"));

                String fromClient = in.readLine();

                if (fromClient == null) {
                    throw new Exception("Received null.");
                }
                AbstractCommand aCmd = CommandFactory.createCommand(fromClient);
                if (aCmd == null) {
                    throw new Exception("Failed to parse command: " + fromClient + ".");
                }

                out.println(aCmd.getCommandString() + ";" + reply.type.name());
                if (reply.type != ServerResponse.ACK) {
                    return;
                }
                if (aCmd.isCancellable()) {
                    sendProgress(out, aCmd);
                }
                if (reply.text != null) {
                    out.println(aCmd.getCommandString() + ";" + ServerResponse.SUCCESS.name() + ";" + reply.text);
                } else {
                    out.println();
                }
                out.flush();
            } catch (Exception e) {
                e.printStackTrace();
                Assert.fail(e.getMessage());
            }
        }
    }

    /**
     * @return
     */
    private ServerSocket createServerSocket() {
        ServerSocket ss = null;
        try {
            ss = new ServerSocket(0);
            serverPort = ss.getLocalPort();
        } catch (IOException e) {
            e.printStackTrace();
            Assert.fail(e.getMessage());
        }
        return ss;
    }

    /**
     * @throws IOException
     *             server couldn't be stop.
     */
    public void shutDown() throws IOException {
        ssocket.close();
    }

    /**
     * add new dummy communication to queue.
     * 
     * @param reply
     *            reply for this request.
     */
    public void addDummyCommunication(String reply) {
        replies.add(new Reply(ServerResponse.ACK, reply));
    }

    /**
     * @param pResponseType
     *            the responseType to set
     */
    public void addDummyCommunication(ServerResponse pResponseType) {
        replies.add(new Reply(pResponseType, ""));
    }

    /**
         * 
         */
    public void test() {
        Assert.assertTrue(replies.isEmpty());
        if (assertionError != null) {
            AssertionError error = assertionError;
            assertionError = null;
            throw error;
        }
    }

    /**
     * @return the serverPort
     */
    public int getServerPort() {
        return serverPort;
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
}
