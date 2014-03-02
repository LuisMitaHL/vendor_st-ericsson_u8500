/**
 * Copyright ST-Ericsson 2010. All rights reserved.
 */
package com.stericsson.RefMan.USDoc;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Class for listening to streams from processes.
 * 
 * @author Thomas Palmqvist
 * 
 */
public class StreamGobbler extends Thread {

    /** The logger */
    final static Logger logger = LoggerFactory.getLogger(StreamGobbler.class);

    /**
     * The <code>InputStream</code> for <code>StreamGobbler</code> to listen to.
     */
    InputStream is;

    /**
     * The output will look like this <code>prefix</code> <code>type</code> >
     * text from <code>is</code>.
     * <p>
     * <code>type</code> is generally OUTPUT or ERROR.
     */
    String type;

    /**
     * The output will look like this <code>prefix</code> <code>type</code> >
     * text from <code>is</code>.
     * <p>
     * <code>prefix</code> can be anything for example myprog.
     */
    String prefix;

    /**
     * Constructor for {@link StreamGobbler}
     * 
     * @param is
     *            The <code>InputStream</code> for <code>StreamGobbler</code> to
     *            listen to.
     * @param type
     *            The output will look like this <code>prefix</code>
     *            <code>type</code> > text from <code>is</code>.
     *            <p>
     *            <code>type</code> is generally OUTPUT or ERROR.
     * @param prefix
     *            The output will look like this <code>prefix</code>
     *            <code>type</code> > text from <code>is</code>.
     *            <p>
     *            <code>prefix</code> can be anything for example myprog.
     */
    StreamGobbler(InputStream is, String type, String prefix) {
        this.is = is;
        this.type = type;
        this.prefix = prefix;
    }

    /**
     * Prints the stream to the logger.
     * <p>
     * Method that starts when {@link Thread#start()} is invoked.
     */
    public void run() {
        try {
            InputStreamReader isr = new InputStreamReader(is);
            BufferedReader br = new BufferedReader(isr);
            String line = null;
            while ((line = br.readLine()) != null) {
                logger.info("{} {}> {}", new Object[]{prefix, type, line});
            }

        } catch (IOException e) {
            logger.error("Something went wrong while reading stream ", e);
        }
    }
}
