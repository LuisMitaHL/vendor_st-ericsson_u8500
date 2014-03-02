package com.stericsson.sdk.backend.remote.io;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;

import com.stericsson.sdk.brp.AbstractCommand;

/**
 * Implementation of the backend client
 * 
 * @author xolabju
 * 
 */
public class BackendServerConnection implements IBackendServerConnection {

    private Socket socket;

    private BufferedReader reader;

    private BufferedWriter writer;

    private String address;

    /**
     * Constructor
     * 
     * @param s
     *            the client socket
     * @throws IOException
     *             on errors
     */
    public BackendServerConnection(Socket s) throws IOException {
        socket = s;
        address = socket.getInetAddress().getHostAddress() + ":" + socket.getPort();
        reader = new BufferedReader(new InputStreamReader(socket.getInputStream(), "UTF-8"));
        writer = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
    }

    /**
     * {@inheritDoc}
     */
    public String getAddress() {
        return address;
    }

    /**
     * {@inheritDoc}
     */
    public String readLine() throws IOException {
        if (reader == null) {
            throw new IOException("reader stream is closed");
        }
        return reader.readLine();
    }

    /**
     * {@inheritDoc}
     */
    public void write(String line) throws IOException {
        if (writer == null) {
            throw new IOException("output stream is closed");
        }
        writer.write(line + AbstractCommand.LINE_SEPARATOR);
        writer.flush();
    }

    /**
     * {@inheritDoc}
     */
    public void close() {
        if (writer != null) {
            try {
                writer.close();
                writer = null;
            } catch (IOException ioe) {
                // ignore
                ioe.getMessage();
            }
        }
        if (reader != null) {
            try {
                reader.close();
                reader = null;
            } catch (IOException ioe) {
                // ignore
                ioe.getMessage();
            }
        }
        if (socket != null) {
            try {
                socket.close();
                socket = null;
            } catch (IOException ioe) {
                // ignore
                ioe.getMessage();
            }
        }

    }

}
