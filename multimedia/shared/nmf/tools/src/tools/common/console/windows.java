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
public class windows implements console {
    public static final int FOREGROUND_BLUE = 0x0001;
    public static final int FOREGROUND_GREEN = 0x0002;
    public static final int FOREGROUND_RED = 0x0004;
    public static final int FOREGROUND_INTENSITY = 0x0008;
    public static final int BACKGROUND_BLUE = 0x0010;
    public static final int BACKGROUND_GREEN = 0x0020;
    public static final int BACKGROUND_RED = 0x0040;
    public static final int BACKGROUND_INTENSITY = 0x0080;
    public static final int COMMON_LVB_LEADING_BYTE = 0x0100;
    public static final int COMMON_LVB_TRAILING_BYTE = 0x0200;
    public static final int COMMON_LVB_GRID_HORIZONTAL = 0x0400;
    public static final int COMMON_LVB_GRID_LVERTICAL = 0x0800;
    public static final int COMMON_LVB_GRID_RVERTICAL = 0x1000;
    public static final int COMMON_LVB_REVERSE_VIDEO = 0x4000;
    public static final int COMMON_LVB_UNDERSCORE = 0x8000;

    /** Map of foreground colors. */
    private Map<foregroundcolor, Integer> fcolors = new HashMap<foregroundcolor, Integer>();

    /** Map of background colors. */
    private Map<backgroundcolor, Integer> bcolors = new HashMap<backgroundcolor, Integer>();

    protected static native int init();
    protected static native void setColor(int foreground);
    protected static native int getColor();

    PrintStream out;
    static int initialColors = 0x0007;
    
    static {
		 System.loadLibrary("windows");
		 initialColors = init();
	}
    

    /**
     * Create new unix console access.
     */
    protected windows(PrintStream out) {
    	this.out = out;
        fcolors.put(foregroundcolor.BLACK, new Integer(0));
        fcolors.put(foregroundcolor.DARK_RED, new Integer(FOREGROUND_RED));
        fcolors.put(foregroundcolor.DARK_GREEN, new Integer(FOREGROUND_GREEN));
        fcolors.put(foregroundcolor.DARK_YELLOW, new Integer(FOREGROUND_RED | FOREGROUND_GREEN));
        fcolors.put(foregroundcolor.DARK_BLUE, new Integer(FOREGROUND_BLUE));
        fcolors.put(foregroundcolor.DARK_MAGENTA, new Integer(FOREGROUND_RED | FOREGROUND_BLUE));
        fcolors.put(foregroundcolor.DARK_CYAN, new Integer(FOREGROUND_GREEN | FOREGROUND_BLUE));
        fcolors.put(foregroundcolor.GREY, new Integer(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE));
        fcolors.put(foregroundcolor.LIGHT_RED, new Integer(FOREGROUND_RED | FOREGROUND_INTENSITY));
        fcolors.put(foregroundcolor.LIGHT_GREEN, new Integer(FOREGROUND_GREEN | FOREGROUND_INTENSITY));
        fcolors.put(foregroundcolor.LIGHT_YELLOW, new Integer(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY));
        fcolors.put(foregroundcolor.LIGHT_BLUE, new Integer(FOREGROUND_BLUE | FOREGROUND_INTENSITY));
        fcolors.put(foregroundcolor.LIGHT_MAGENTA, new Integer(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY));
        fcolors.put(foregroundcolor.LIGHT_CYAN, new Integer(FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY));
        fcolors.put(foregroundcolor.WHITE, new Integer(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY));

        bcolors.put(backgroundcolor.BLACK, new Integer(0));
        bcolors.put(backgroundcolor.DARK_RED, new Integer(BACKGROUND_RED));
        bcolors.put(backgroundcolor.DARK_GREEN, new Integer(BACKGROUND_GREEN));
        bcolors.put(backgroundcolor.DARK_YELLOW, new Integer(BACKGROUND_RED | BACKGROUND_GREEN));
        bcolors.put(backgroundcolor.DARK_BLUE, new Integer(BACKGROUND_BLUE));
        bcolors.put(backgroundcolor.DARK_MAGENTA, new Integer(BACKGROUND_RED | BACKGROUND_BLUE));
        bcolors.put(backgroundcolor.DARK_CYAN, new Integer(BACKGROUND_GREEN | BACKGROUND_BLUE));
        bcolors.put(backgroundcolor.GREY, new Integer(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE));
    }

    public void setForegroundColor(foregroundcolor color) {
    	out.flush();
    	setColor((getColor() & 0xFFF0) | ((Integer) fcolors.get(color)).intValue());
    }

    public void setBackgroundColor(backgroundcolor color) {
    	out.flush();
    	setColor((getColor() & 0xFF0F) | ((Integer) bcolors.get(color)).intValue());
    }

    public void resetColors() {
    	out.flush();
    	setColor(initialColors);
    }

}
