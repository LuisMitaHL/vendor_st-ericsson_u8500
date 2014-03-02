package com.stericsson.ftc;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.StreamTokenizer;
import java.io.StringReader;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

/**
 * Console for flash-tool CLI. Console is executed by -console command.
 * 
 * @author Tsikor01
 * 
 */
public class FTCReader {

    private BufferedReader br;

    FTCApplicationContext context;

    /**
     * Default constructor
     * 
     * @param args
     *            Input parameters
     */
    public FTCReader(String[] args) {
        int consolePort = 8088;
        String consoleHost = "localhost";

        for (int i = 1; i < args.length - 1; i++) {
            if (args[i].equals("port")) {
                consolePort = Integer.valueOf(args[i + 1]);
            }
            if (args[i].equals("host")) {
                consoleHost = args[i + 1];
            }
        }
        context = new FTCApplicationContext(consoleHost, consolePort);
        System.out.println("-- Starting Flash Tool Console --");
        try {
            br = new BufferedReader(new InputStreamReader(System.in, "UTF8"));
        } catch (UnsupportedEncodingException e1) {
            System.out.println("Unsupported coding UTF8");
            e1.printStackTrace();
        }
    }

    /**
     * Method for reading user input
     * 
     */
    public void start() {
        String line = null;
        try {
            while (read(line)) {
                System.out.print("\nFTC: <" + context.getWorkingDirectory() + "> ");
                line = br.readLine();
            }
        } catch (Exception e) {
            System.out.print("Error reading user input!\n");
            e.printStackTrace();
        }
    }

    /**
     * Method handling logic of console
     * 
     * @param currentLine
     *            String with input parameters
     * @return true if console instance should continue in reading
     * @throws Exception
     *             FTC Parser Exception if there is error in input
     */
    boolean read(String currentLine) throws Exception {
        String tempLine = currentLine;
        if (tempLine != null) {

            if (tempLine.trim().toUpperCase(Locale.getDefault()).equals("EXIT")) {
                return false;
            }

            List<String> words = parse(tempLine);
            if (words == null) {
                return true;
            }

            FTCAbstractCommand command = FTCCommandFactory.createCommand(words.get(0));
            if (command != null) {
                words.remove(0);// command name
                command.setContext(context);
                if (command.setParameters(words)) {
                    command.execute();
                } else {
                    command.printHelp();
                }
            } else {
                System.out.println("Command " + words.get(0) + " not found!");
            }
        }
        return true;
    }

    /**
     * Method parses line of user input to the output list, which is used later.
     * 
     * @param inputLine
     *            Line of user input.
     * @return List of parsed user commands or null on error.
     * 
     */
    public List<String> parse(String inputLine) {
        if (inputLine == null) {
            return null;
        }
        inputLine = inputLine.replaceAll("\\\\", "\\\\\\\\");

        List<String> parsedWords = new ArrayList<String>();

        try {
            StreamTokenizer words = new StreamTokenizer(new StringReader(inputLine));
            words.ordinaryChars(0x0, 0xff);

            words.wordChars(0x0, 0xff);

            words.whitespaceChars(' ', ' ');

            words.quoteChar('"');

            while (words.nextToken() != StreamTokenizer.TT_EOF) {
                // This is replace caused by strange behavior of stream tokenizer with quotations
                // and without them
                // if there are quotations present \\ slash is deleted by its regexp
                // if quotations are not here slashes are accepted without problems
                parsedWords.add(words.sval.replaceAll("\\\\\\\\", "\\\\"));

            }
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
        return parsedWords;
    }
}
