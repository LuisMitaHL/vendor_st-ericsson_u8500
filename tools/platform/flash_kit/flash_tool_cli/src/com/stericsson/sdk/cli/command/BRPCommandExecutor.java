package com.stericsson.sdk.cli.command;

import java.io.IOException;
import java.util.Locale;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.ServerResponse;
import com.stericsson.sdk.brp.util.Constant;
import com.stericsson.sdk.cli.ClientSocket;
/**
 * class that executes actions for a BRPCommand
 * 
 * @author xdancho
 * 
 */
public class BRPCommandExecutor {
    boolean error = false;

    boolean verbose = false;

    String completeCommandString = "";

    String commandString = "";

    boolean isWaiting;

    boolean isSilent;

    Thread waitingThread;

    boolean missingLibrary = false;
    // Thread scannerThread;
    //
    // boolean isScanning;
    //
    // FileChannel inScanner;
    //
    // AbstractCommand cancelCommand;

    BRPPrintFormatter formatter = BRPPrintFormatter.getInstance();

    private boolean bare;

    /**
     * construct a BRPCommandExecutor
     * 
     * @param completeCmdString
     *            the command to use
     * @param silent
     *            true if there should be no print from the command
     */
    public BRPCommandExecutor(String completeCmdString, boolean silent) {

        try {
            this.completeCommandString = completeCmdString;
            if (completeCmdString.indexOf(AbstractCommand.DELIMITER) > 0) {
                this.commandString =
                    completeCmdString.substring(0, completeCmdString.indexOf(AbstractCommand.DELIMITER)).toLowerCase(
                        Locale.getDefault());
            } else {
                this.commandString = completeCmdString.toLowerCase(Locale.getDefault());
            }
            isSilent = silent;
        } catch (Exception e) {
            System.out.println(e.getMessage());
            System.out.print("\nException technical details:\n");
            e.printStackTrace(System.out);
        }
    }

    /**
     * execute a command
     * 
     * @return true on error execution
     */
    public boolean execute() {
        // try {
        // cancelCommand = CommandFactory.createCancelCommand(completeCommandString);
        // System.out.println("To cancel the command, press q and then enter");
        // startScanner();
        // } catch (InvalidSyntaxException e) {
        // e.getMessage();
        // }
        boolean finished = false;
        ClientSocket.getInstance().sendCommand(completeCommandString);
        startProgress();
        try {
            while (!finished) {
                String received = ClientSocket.getInstance().receiveCommand();
                received = removeEscapeSequence(received);
                if (received == null) {
                    System.out.println("Received null. Terminating...");
                    break;
                }
                finished = proccesResults(received);
                if(received.contains(Constant.No64BitLcdLcmDriver.getValue())){
                    missingLibrary = true;
                }
            }
        } catch (IOException ioe) {
            System.out.println(ioe.getMessage());
            System.out.print("\nException technical details:\n");
            ioe.printStackTrace(System.out);
        } finally {
            joinProgress();
            // stopScanner();
        }
        return error;

    }

    /**
     * Method processing results and printing them if there is no silent option
     * 
     * @param received
     *            Received output from backend
     * @return True if there will be no additional data from backend
     */
    private boolean proccesResults(String received) {

        if (verbose && !getStatus(received).equalsIgnoreCase(ServerResponse.PROGRESS.name())) {
            System.out.println(received.toLowerCase(Locale.getDefault()));
        }
        if (getStatus(received).equalsIgnoreCase(ServerResponse.PROGRESS.name())) {
            if (!isSilent) {
                formatter.printProgress(commandString, received);
            }
        }else if (getStatus(received).equalsIgnoreCase(ServerResponse.PROGRESSDEBUGMESSAGE.name())) {
            if (!isSilent) {
                formatter.printProgressMessage(commandString, received);
            }
        } else if (getStatus(received).equalsIgnoreCase(ServerResponse.ERROR.name())) {
            error = true;
            if (!isSilent) {
                formatter.printError(commandString, received);
            }
            return true;
        } else if (getStatus(received).equalsIgnoreCase(ServerResponse.NAK.name())) {
            error = true;
            if (!isSilent) {
                formatter.printNak(commandString);
            }
            return true;
        } else if (getStatus(received).equalsIgnoreCase(ServerResponse.SUCCESS.name())) {
            joinProgress();
            if (bare) {
                System.out.println(formatter.getResponseData(received, ServerResponse.SUCCESS));
            } else {
                if (!isSilent) {
                    formatter.printResult(commandString, received);
                }
            }
            return true;
        }
        return false;
    }

    private String removeEscapeSequence(String pReceived) {
        if (pReceived != null && !pReceived.trim().equals("")) {
            return pReceived.replace("<newLine>", "\n");
        }
        return pReceived;
    }

    // private void stopScanner() {
    // if (inScanner != null) {
    // try {
    // inScanner.close();
    // } catch (IOException e) {
    // // ignore
    // e.getMessage();
    // }
    // }
    //
    // }

    // private void startScanner() {
    // if (scannerThread == null) {
    // scannerThread = new Thread(new Runnable() {
    //
    // @Override
    // public void run() {
    // inScanner = new FileInputStream(FileDescriptor.in).getChannel();
    // ByteBuffer b = ByteBuffer.allocate(1);
    // boolean scan = true;
    // while (scan) {
    // try {
    // inScanner.read(b);
    // String next = new String(b.array());
    // b.rewind();
    // if (next != null && next.equalsIgnoreCase("q")) {
    // try {
    // scan = false;
    // ClientSocket.getInstance().sendCommand(cancelCommand.getCommandString());
    // } catch (Exception e) {
    // e.printStackTrace();
    // }
    // }
    // } catch (Exception e) {
    // scan = false;
    // }
    // }
    //
    // }
    //
    // }, "Scanner");
    // scannerThread.start();
    // }
    //
    // }

    private void joinProgress() {
        isWaiting = false;
        try {
            if (waitingThread != null) {
                waitingThread.join();
            }
        } catch (InterruptedException e) {
            System.out.print("\nException technical details:\n");
            e.printStackTrace(System.out);
        }
        waitingThread = null;

    }

    private void startProgress() {
        isWaiting = true;

        if (waitingThread == null) {
            // start on new line
            if (!isSilent) {
                System.out.print("\n");
            }
            waitingThread = new Thread(new Runnable() {

                public void run() {
                    while (isWaiting) {
                        if (!isSilent) {
                            formatter.printIndeterminateProgress();
                        }
                        try {
                            Thread.sleep(100);
                        } catch (InterruptedException e) {
                            System.out.print("\nException technical details:\n");
                            e.printStackTrace(System.out);
                        }
                    }
                }

            }, "Indeterminate progress");
            waitingThread.start();
        }

    }

    /**
     * set the execution to verbose
     * 
     * @param v
     *            true/false
     */
    public void setVerbose(boolean v) {
        this.verbose = v;
    }

    /**
     * set the execution to bare
     * 
     * @param b
     *            true/false
     */
    public void setBare(boolean b) {
        this.bare = b;
    }

    String getStatus(String inString) {
        String[] split = inString.split(AbstractCommand.DELIMITER);

        for (String s : split) {
            for (ServerResponse sr : ServerResponse.values()) {
                if (sr.name().equalsIgnoreCase(s)) {
                    return s;
                }
            }
        }
        return null;
    }

    /**
     * @return if a library is missing or not
     */
    public boolean isMissingLibrary(){
        return missingLibrary;
    }
}
