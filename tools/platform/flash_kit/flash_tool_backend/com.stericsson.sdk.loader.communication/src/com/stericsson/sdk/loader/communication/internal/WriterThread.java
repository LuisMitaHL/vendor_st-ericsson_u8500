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
public class WriterThread extends Thread {

    private static Logger logger = Logger.getLogger(WriterThread.class.getName());

    /**
     * the reader task queue
     */
    private BlockingQueue<Task> writerQueue = new ArrayBlockingQueue<Task>(10);

    private boolean terminate;

    /**
     * @param task
     *            the task to add to queue
     */
    public void addToQueue(Task task) {
        writerQueue.add(task);
    }

    /**
     * 
     */
    public void terminate() {
        terminate = true;
        writerQueue.add(new WriterTask(0 ,0, 0, null));
    }

    /**
     * (non-Javadoc)
     * 
     * @see java.lang.Thread#run()
     */
    public void run() {

        while (true) {
            Task writeTask = null;
            try {
                writeTask = writerQueue.take();

                // TODO: check if instance is active or not
                if (terminate) {
                    break;
                }

                writeTask.process();
                if (writeTask.isDone()) {
                    logger.debug("Write task done");
                    LoaderCommunicationInterface.wReply(writeTask.getLength(), writeTask.getDataPointer(), writeTask
                        .getInstancePointer());
                } else {
                    writerQueue.add(writeTask);
                }
            } catch (InterruptedException ie) {
                logger.debug("write task was interrupted");
            } catch (PortException e) {
                logger.error(e.getMessage());
                logger.error("De-initializing loader communication for instance: " + writeTask.getInstancePointer());
                LoaderCommunicationInterface.deInit(writeTask.getInstancePointer());
                LoaderCommunicationInterface.notifyLoaderCommunicationError((IPort)writeTask.getPort(), e.getMessage());
            }
        }
    }
}
