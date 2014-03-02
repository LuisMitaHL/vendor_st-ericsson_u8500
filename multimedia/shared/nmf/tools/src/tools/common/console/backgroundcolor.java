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

/**
 * All common background colors.
 */
public class backgroundcolor {

    /** Black background. */
    public static final backgroundcolor BLACK = 
    	new backgroundcolor("black");

    /** Red background. */
    public static final backgroundcolor DARK_RED
        = new backgroundcolor("red");

    /** Green background. */
    public static final backgroundcolor DARK_GREEN
        = new backgroundcolor("dark green");

    /** Yellow background. */
    public static final backgroundcolor DARK_YELLOW 
        = new backgroundcolor("dark yellow");

    /** Blue background. */
    public static final backgroundcolor DARK_BLUE 
        = new backgroundcolor("dark blue");

    /** Magenta background. */
    public static final backgroundcolor DARK_MAGENTA 
        = new backgroundcolor("dark magenta");

    /** Cyan background. */
    public static final backgroundcolor DARK_CYAN 
        = new backgroundcolor("dark cyan");

    /** Grey background. */
    public static final backgroundcolor GREY 
        = new backgroundcolor("grey");

    /** Color name. */
    private final String name;

    /**
     * Constructor.
     * 
     * @param   name    Color name.
     */
    private backgroundcolor(final String name) {
        this.name = name;
    }  
    
    /**
     * Get color name.
     * 
     * @return  Color name.
     */
    public final String getName() {
        return name;
    }
}
