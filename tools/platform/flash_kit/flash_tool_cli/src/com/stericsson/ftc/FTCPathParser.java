package com.stericsson.ftc;

import com.stericsson.sdk.brp.AbstractCommand;
import com.stericsson.sdk.brp.CommandName;
import com.stericsson.sdk.cli.ClientSocket;
import com.stericsson.sdk.cli.CommandParser;
import com.stericsson.sdk.cli.command.BRPCommandExecutor;

/**
 * Class for parsing path from user input. It changes slashes to linux one during parsing. It also
 * removes slash on end of path. And it remove parent("..")/current(".") directory symbols in the path.
 * 
 * @author tsikor01
 * @author Vit Sykala
 * 
 */
public final class FTCPathParser {

    private FTCPathParser() {
    }

    private static final String SEPARATOR = "/";

    private static final String LEVELU_UP_PATTERN = "..";

    private static final String REMOVE_WILDCARD = ".";

    /**
     * @param context
     *            application context containing working directory and connection information to
     *            backend.
     * @param path
     *            user typed string which should be parsed
     * @return initialized {@link FTCPath} object.
     */
    public static FTCPath parse(FTCApplicationContext context, String path) {
        String prefix = null;
        String suffix = null;
        int separatorIndex = path.indexOf(FTCPath.PREFIX_SEPARATOR);

        if (separatorIndex < 0) {// try as relative address
            return parse(context, getAbsolutePath(context.getWorkingDirectory(), path));
        } else {
            prefix = path.substring(0, separatorIndex).trim();
            try {
                if (!FTCPath.PC_PREFIX.equals(prefix) && !isConnectedEquipment(context, prefix)) {
                    // try as relative address
                    return parse(context, getAbsolutePath(context.getWorkingDirectory(), path));
                } else {
                    suffix = path.substring(separatorIndex + 1).trim();
                    suffix = normalizePathSuffix(suffix);
                }
            } catch (Exception e) {
                return null;
            }
        }
        return new FTCPath(prefix, suffix);
    }

    private static boolean isConnectedEquipment(FTCApplicationContext context, String prefix) throws Exception {
        CommandParser cmdParser = new CommandParser();

        BRPCommandExecutor cmd =
            cmdParser.parse(new String[] {
                CommandName.getCLISyntax(CommandName.FLASH_LIST_DEVICES.name()),
                "-" + AbstractCommand.PARAMETER_EQUIPMENT_ID, prefix}, true);

        if (cmd != null) {
            ClientSocket.getInstance().startSocket(context.getHost(), context.getPort());
            return !cmd.execute();
        }

        return false;
    }

    /**
     * change folder separator from \ to /<br />
     * remove double separators (// to /)<br />
     * remove end separator if it is not also first one on volume. (from c:/dir/ to c:/dir but not
     * from c:/) remove actual and parent directory from path (from c:/./dirA/dirB/.././.. to c:/)
     * 
     * @param path
     *            Input path
     * @return Normalized path suffix
     */
    private static String normalizePathSuffix(String path) {
        String[] pathSplited = path.split(":", 2);
        String volume = "";
        if (pathSplited.length == 2) {
            volume = pathSplited[0] + ":";
            path = pathSplited[1];
        }

        path = path.replace("\\", SEPARATOR); // use linux separators only

        path = removeDualSlash(path);

        path = removeSeparatorOnEnd(path);

        pathSplited = path.split(SEPARATOR);

        for (int i = 0; i < pathSplited.length; i++) {
            if (isLevelUpPattern(pathSplited[i])) {
                pathSplited[i] = REMOVE_WILDCARD;
                pathSplited = removeWildcard(pathSplited, i);
            }
        }
        return volume + stringFromStringArray(pathSplited);
    }

    /**
     * Removes dual separator character sometimes used by windows users
     * 
     * @param path
     *            Input path
     * @return path without dual slashes
     */
    private static String removeDualSlash(String path) {
        String doubleSeparator = SEPARATOR + SEPARATOR;
        while (path.contains(doubleSeparator)) {
            path = path.replaceAll(doubleSeparator, SEPARATOR);
        }
        return path;
    }

    /**
     * Removes separators on the end of string
     * 
     * @param path
     *            Input path
     * @return path not ending by file separators
     */
    private static String removeSeparatorOnEnd(String path) {
        if (path.endsWith(SEPARATOR)) {
            path = removeLastCharacter(path);
        }
        return path;
    }

    private static String removeLastCharacter(String path) {
        return path.substring(0, path.length() - 1);
    }

    private static String stringFromStringArray(String[] pathSplited) {
        StringBuffer result = new StringBuffer();
        for (int i = 0; i < pathSplited.length; i++) {
            if (!pathSplited[i].equals(REMOVE_WILDCARD)) {
                result.append((i > 0) ? SEPARATOR : "");
                result.append(pathSplited[i]);
            }
        }
        if (result.length() == 0) {
            result.append(SEPARATOR);
        }
        return result.toString();
        // do so here
    }

    private static String[] removeWildcard(String[] pathSplited, int i) {
        if (isRemoveWildcard(pathSplited[i - 1])) {
            pathSplited = findNextRemove(pathSplited, i - 1);
        } else {
            pathSplited[i - 1] = REMOVE_WILDCARD;
        }
        return pathSplited;
    }

    private static boolean isRemoveWildcard(String pathSplited) {
        return pathSplited.equals(REMOVE_WILDCARD);
    }

    private static boolean isLevelUpPattern(String pathSplited) {
        return pathSplited.equals(LEVELU_UP_PATTERN);
    }

    private static String[] findNextRemove(String[] pathSplited, int i) {
        for (; i >= 0; i--) {
            if (!isRemoveWildcard(pathSplited[i])) {
                pathSplited[i] = REMOVE_WILDCARD;
                break;
            }
        }
        return pathSplited;
    }

    private static String getAbsolutePath(FTCPath workingDirectory, String relativePath) {
        return workingDirectory + SEPARATOR + relativePath;
    }
}
