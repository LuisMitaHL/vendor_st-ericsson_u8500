/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

package com.stericsson.atservice;

import java.io.IOException;

import android.net.LocalSocket;
import android.util.Log;

/**
 * Container class for objects needed to receive AT commands, handle them and
 * send responses.
 */
public class AtHandler {

    /**
     * The local socket used to send responses and receive commands.
     */
    private LocalSocket socket;

    /**
     * The at service.
     */
    private AtService atService;

    /**
     * The command reader, used to read the AT commands.
     */
    private CommandReader commandReader;

    /**
     * The at processor, used to process the commands.
     */
    private AtProcessor processor;

    /**
     * The identity number of this at handler.
     */
    private Integer id;

    /**
     * Constructor for an AtHandler.
     *
     * @param socket
     *            the socket
     * @param parser
     *            the AtParser
     * @param atService
     *            the at service
     * @param id
     *            the identity number
     * @throws IOException
     *             if the socket is faulty
     */
    public AtHandler(LocalSocket socket, AtParser parser, AtService atService,
                     Integer id) throws IOException {
        this.socket = socket;
        this.atService = atService;
        this.id = id;
        setUpHandler(parser);
    }

    /**
     * Creates and setup an AtParser, a CommandMonitor, a CommandReader, a
     * ResponseWriter and an AtProcessor.
     *
     * @param parser
     *            the AtParser
     *
     * @throws IOException
     *             if the socket is faulty
     */
    private void setUpHandler(AtParser parser) throws IOException {
        CommandMonitor commandMonitor = new CommandMonitor(parser);
        try {
            commandReader = new CommandReader(commandMonitor, socket
                                              .getInputStream());

        } catch (IOException e) {
            Log.e(AtService.LOG_TAG,
                  "AtHandler: faulty socket, cannot set up communication");
            commandReader = null;
            throw new IOException(
                "Athandler could not be created due to socket problem, "
                + e.getMessage());
        }
        commandReader.start();
        ResponseWriter responseWriter = new ResponseWriter(socket);
        processor = new AtProcessor(parser, commandMonitor, responseWriter,
                                    this);
        processor.start();
    }

    /**
     * Closes the at handler. Is called from AtProcessor.
     */
    public void close() {
        // should already be closed but do it anyway
        closeSocket();
        commandReader = null;
        processor = null;
        atService.removeHandler(id);
        Log.d(AtService.LOG_TAG, "AtHandler closed");
    }

    /**
     * Used to trigger an indirect close down of the at handler and all classes
     * connected to it. Is called from AtService.
     */
    public void closeSocket() {
        try {
            // Not certain that this is handled correctly in android
            socket.close();
        } catch (IOException e) {
            Log.d(AtService.LOG_TAG, "AtHandler.close: Socket already closed");
        }
    }
}
