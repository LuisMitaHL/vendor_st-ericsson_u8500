/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * This code is part of the NMF Toolset.
 *
 * The NMF Toolset is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The NMF Toolset is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the NMF Toolset. If not, see <http://www.gnu.org/licenses/>.
 *
 */
package tools.common.console;

import java.util.HashMap;
import java.util.Map;
import java.io.PrintStream;

/**
 * Unix console colors with ESC sequences.
 */
public class escaped implements console {
    
    /** Resets the color to default. */
    public static final String RESET = "\u001B[0m";

    /** Black foreground. */
    public static final String BLACK = "\u001B[22;30m";

    /** Normal intensity red. */
    public static final String RED = "\u001B[22;31m";

    /** Normal intensity green. */
    public static final String GREEN = "\u001B[22;32m";

    /** Normal intesity yellow (brown). */
    public static final String YELLOW = "\u001B[22;33m";

    /** Normal intensity blue. */
    public static final String BLUE = "\u001B[22;34m";

    /** Normal intensity magenta (purple). */
    public static final String MAGENTA = "\u001B[22;35m";

    /** Normal intensity cyan. */
    public static final String CYAN = "\u001B[22;36m";

    /** Normal intesity white (grey). */
    public static final String WHITE = "\u001B[22;37m";

    /** Bright black (grey). */
    public static final String GREY = "\u001B[1;30m";

    /** Bright red. */
    public static final String LIGHT_RED = "\u001B[1;31m";

    /** Bright green. */
    public static final String LIGHT_GREEN = "\u001B[1;32m";

    /** Bright yellow. */
    public static final String LIGHT_YELLOW = "\u001B[1;33m";

    /** Bright blue. */
    public static final String LIGHT_BLUE = "\u001B[1;34m";

    /** Bright magenta. */
    public static final String LIGHT_MAGENTA = "\u001B[1;35m";

    /** Bright cyan. */
    public static final String LIGHT_CYAN = "\u001B[1;36m";

    /** Bright white (white). */
    public static final String LIGHT_WHITE = "\u001B[1;37m";

    /** Black background. */
    public static final String BACKGROUND_BLACK = "\u001B[40m";

    /** Red background. */
    public static final String BACKGROUND_RED = "\u001B[41m";

    /** Green background. */
    public static final String BACKGROUND_GREEN = "\u001B[42m";

    /** Yellow background. */
    public static final String BACKGROUND_YELLOW = "\u001B[43m";

    /** Blue background. */
    public static final String BACKGROUND_BLUE = "\u001B[44m";

    /** Magenta background. */
    public static final String BACKGROUND_MAGENTA = "\u001B[45m";

    /** Cyan background. */
    public static final String BACKGROUND_CYAN = "\u001B[46m";

    /** White background (grey). */
    public static final String BACKGROUND_WHITE = "\u001B[47m";

    /** Map of foreground colors. */
    private Map<foregroundcolor, String> fcolors = new HashMap<foregroundcolor, String>();

    /** Map of background colors. */
    private Map<backgroundcolor, String> bcolors = new HashMap<backgroundcolor, String>();

    PrintStream out;
    
    /**
     * Create new unix console access.
     */
    protected escaped(PrintStream out) {
    	this.out = out;
        fcolors.put(foregroundcolor.BLACK, BLACK);
        fcolors.put(foregroundcolor.DARK_RED, RED);
        fcolors.put(foregroundcolor.DARK_GREEN, GREEN);
        fcolors.put(foregroundcolor.DARK_YELLOW, YELLOW);
        fcolors.put(foregroundcolor.DARK_BLUE, BLUE);
        fcolors.put(foregroundcolor.DARK_MAGENTA, MAGENTA);
        fcolors.put(foregroundcolor.DARK_CYAN, CYAN);
        fcolors.put(foregroundcolor.GREY, WHITE);
        fcolors.put(foregroundcolor.LIGHT_RED, LIGHT_RED);
        fcolors.put(foregroundcolor.LIGHT_GREEN, LIGHT_GREEN);
        fcolors.put(foregroundcolor.LIGHT_YELLOW, LIGHT_YELLOW);
        fcolors.put(foregroundcolor.LIGHT_BLUE, LIGHT_BLUE);
        fcolors.put(foregroundcolor.LIGHT_MAGENTA, LIGHT_MAGENTA);
        fcolors.put(foregroundcolor.LIGHT_CYAN, LIGHT_CYAN);
        fcolors.put(foregroundcolor.WHITE, LIGHT_WHITE);

        bcolors.put(backgroundcolor.BLACK, BACKGROUND_BLACK);
        bcolors.put(backgroundcolor.DARK_RED, BACKGROUND_RED);
        bcolors.put(backgroundcolor.DARK_GREEN, BACKGROUND_GREEN);
        bcolors.put(backgroundcolor.DARK_YELLOW, BACKGROUND_YELLOW);
        bcolors.put(backgroundcolor.DARK_BLUE, BACKGROUND_BLUE);
        bcolors.put(backgroundcolor.DARK_MAGENTA, BACKGROUND_MAGENTA);
        bcolors.put(backgroundcolor.DARK_CYAN, BACKGROUND_CYAN);
        bcolors.put(backgroundcolor.GREY, BACKGROUND_WHITE);
    }

    public void setForegroundColor(foregroundcolor color) {
        out.print(fcolors.get(color));
    }

    public void setBackgroundColor(backgroundcolor color) {
        out.print(bcolors.get(color));
    }

    public void resetColors() {
        out.print(RESET);
    }

}
