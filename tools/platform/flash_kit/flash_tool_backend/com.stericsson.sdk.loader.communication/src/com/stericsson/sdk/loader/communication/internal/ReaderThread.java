package com.stericsson.sdk.loader.communication.internal;

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

import org.apache.log4j.Logger;

import com.stericsson.sdk.equipment.io.port.IPort;
import com.stericsson.sdk.equipment.io.port.PortException;

/**
 * 
 * @author xdancho
 * 
 */
public class ReaderThread extends Thread {

    private static Logger logger = Logger.getLogger(ReaderThread.class.getName());

    /**
     * reader task queue
     */
    private BlockingQueue<Task> readerQueue = new ArrayBlockingQueue<Task>(10);

    private boolean terminate;

    /**
     * @param task
     *            reader task to add
     */
    public void addToQueue(Task task) {
        readerQueue.add(task);
    }

    /**
     * 
     */
    public void terminate() {
        terminate = true;
        readerQueue.add(new ReaderTask(0, 0, 0, null));
    }

    /**
     * (non-Javadoc)
     * 
     * @see java.lang.Thread#run()
     */
    public void run() {

        while (true) {
            Task readTask = null;
            try {
                readTask = readerQueue.take();

                if(terminate) {
                    break;
                }

                readTask.process();

                if (readTask.isDone()) {
                    logger.debug("Read task done (" + readTask.getLength() + " bytes read)");
                    LoaderCommunicationInterface.rReply(readTask.getData(), readTask.getLength(), readTask
                        .getDataPointer(), readTask.getInstancePointer());
                } else {
                    addToQueue(readTask);
                }
            } catch (InterruptedException ie) {
                logger.warn("Read task was interrupted");
            } catch (PortException e) {
                logger.error(e.getMessage());
                logger.error("De-initializing loader communication for instance: " + readTask.getInstancePointer());
                LoaderCommunicationInterface.deInit(readTask.getInstancePointer());
                LoaderCommunicationInterface.notifyLoaderCommunicationError((IPort)readTask.getPort(), e.getMessage());
            }
        }
    }

}
