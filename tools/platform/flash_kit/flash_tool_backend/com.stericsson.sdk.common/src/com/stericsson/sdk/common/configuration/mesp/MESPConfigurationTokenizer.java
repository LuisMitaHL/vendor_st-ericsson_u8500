package com.stericsson.sdk.common.configuration.mesp;

/**
 * Tokenizer for MESP configuration data
 * 
 * @author Tomas
 */
public class MESPConfigurationTokenizer {

    /** Unknown token */
    public static final int TOKEN_UNKNOWN = 0;

    /** ':' */
    public static final int TOKEN_COLON = 1;

    /** '=' */
    public static final int TOKEN_EQUALS = 2;

    /** '"' */
    public static final int TOKEN_CITATION = 3;

    /** Any character sequence */
    public static final int TOKEN_IDENTIFIER = 4;

    /** End of input (line) */
    public static final int TOKEN_EOI = 100;

    /** '{' */
    public static final int TOKEN_LEFT_BRACE = 8;

    /** '}' */
    public static final int TOKEN_RIGHT_BRACE = 9;

    private String input;

    private int position;

    private String value;

    private int token;

    /**
     * Constructor
     * 
     * @param line
     *            Line to tokenize
     */
    public MESPConfigurationTokenizer(String line) {
        input = line;
        position = 0;
        next();
    }

    /**
     * Only intended to be used by internal unit tests.
     */
    public MESPConfigurationTokenizer() {
    }

    /**
     * Move to next token
     */
    public void next() {
        int c;

        token = TOKEN_UNKNOWN;

        if (position >= input.length()) {
            token = TOKEN_EOI;
            return;
        }

        c = input.charAt(position);
        while (c == ' ' || c == '\t') {
            position = position + 1;
            if (position == input.length()) {
                token = TOKEN_EOI;
                return;
            }
            c = input.charAt(position);
        }

        if (Character.isLetter(c) || Character.isDigit(c)) {
            parseIdentifier();
        } else if (c == '"') {
            parseString();
        } else {
            parseCharacter(c);

            if (token != TOKEN_UNKNOWN) {
                position = position + 1;
            }
        }
    }

    /**
     * @return Token
     */
    public int getToken() {
        return token;
    }

    /**
     * @return Value of token (if any)
     */
    public String getValue() {
        return value;
    }

    private void parseIdentifier() {
        token = TOKEN_IDENTIFIER;

        int p1 = position;
        int c = input.charAt(position);

        while (Character.isLetter(c) || Character.isDigit(c) || isAcceptedCharacter(c)) {
            position = position + 1;
            c = input.charAt(position);
        }

        value = input.substring(p1, position);
    }

    private void parseString() {
        token = TOKEN_IDENTIFIER;

        int c = input.charAt(++position);
        int p1 = position;

        while (c != '"') {
            position = position + 1;
            if (position == input.length()) {
                token = TOKEN_EOI;
                return;
            }
            c = input.charAt(position);
        }

        value = input.substring(p1, position++);
    }

    /**
     * @param c
     *            TBD
     */
    public void parseCharacter(int c) {
        switch (c) {
            case '=':
                token = TOKEN_EQUALS;
                break;
            case '{':
                token = TOKEN_LEFT_BRACE;
                break;
            case '}':
                token = TOKEN_RIGHT_BRACE;
                break;
            default:
                break;
        }
    }

    private boolean isAcceptedCharacter(int c) {
        return ((c == '[') || (c == ']') || (c == '_'));
    }
}
