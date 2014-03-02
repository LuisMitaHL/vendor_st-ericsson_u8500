package com.stericsson.sdk.signing.cli;

import java.io.BufferedWriter;
import java.io.OutputStreamWriter;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;

import org.apache.commons.cli.HelpFormatter;
import org.apache.commons.cli.Options;

/**
 * @author xtomlju
 */
public final class SignHelper {

    /**
     * Will write help description for the application to a specific stream.
     * 
     * @param commandLineOptions
     *            TBD
     * @param signingType
     *            Target platform string.
     * @param ps
     *            the print stream that we write the help description to. Use System.out as argument
     *            if you need to write the help to stdout.
     */
    public static void printHelp(Options commandLineOptions, String signingType, PrintStream ps) {
        HelpFormatter formatter = new HelpFormatter();
        PrintWriter pw = null;
        try {
            pw = new PrintWriter(new BufferedWriter(new OutputStreamWriter(ps, "UTF8")));
        } catch (UnsupportedEncodingException e) {
            System.out.print("\nException technical details:\n");
            e.printStackTrace();
            return;
        }
        formatter.printHelp(pw, 100, "sign-tool-" + signingType + " [OPTIONS] input output", null, commandLineOptions,
            4, 4, null);
        pw.flush();
    }

    private SignHelper() {

    }
}
