package com.stericsson.sdk.equipment;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import org.apache.log4j.Logger;

/**
 * @author xtomzap
 * 
 */
public class RuntimeExecutor implements IRuntimeExecutor {

    private StreamGobbler s1;

    private StreamGobbler s2;

    private Logger logger = Logger.getLogger(RuntimeExecutor.class.getName());

    /**
     * {@inheritDoc}
     */
    public int execute(String pCommand) throws IOException, InterruptedException {
        logger.debug(pCommand);
        Process exec = Runtime.getRuntime().exec(pCommand);
        s1 = new StreamGobbler(RuntimeExecutor.class.getName() + "stdin", exec.getInputStream());
        s2 = new StreamGobbler(RuntimeExecutor.class.getName() + "stderr", exec.getErrorStream());
        s1.start();
        s2.start();

        int toReturn;
        logger.debug("Waiting for process");
        toReturn = exec.waitFor();
        logger.debug("Process finished");
        return toReturn;
    }

    private class StreamGobbler implements Runnable {
        String name;

        InputStream is;

        Thread thread;

        Logger logger;

        /**
         * @param newName
         *            threat name
         * @param newIs
         *            input stream
         */
        public StreamGobbler(String newName, InputStream newIs) {
            this.name = newName;
            this.is = newIs;
            this.logger = Logger.getLogger(name);
        }

        /** */
        public void start() {
            thread = new Thread(this);
            thread.start();
        }

        /** */
        public void run() {
            try {
                InputStreamReader isr = new InputStreamReader(is, "UTF-8");
                BufferedReader br = new BufferedReader(isr);

                while (true) {
                    String s = br.readLine();
                    if (s == null) {
                        break;
                    }
                    System.out.println("[" + name + "] " + s);
                    logger.debug(s);
                }

                is.close();

            } catch (Exception ex) {
                logger.debug(ex);
                System.out.println("Problem reading stream " + name + "... :" + ex);
                ex.printStackTrace();
            }
        }
    }
}
