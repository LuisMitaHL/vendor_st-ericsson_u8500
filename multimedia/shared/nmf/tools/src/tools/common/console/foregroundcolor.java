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
 * Common foreground colors.
 */
public class foregroundcolor {

    /** Black foreground. */
    public static final foregroundcolor BLACK
        = new foregroundcolor("black");

    /** Dark red foreground. */
    public static final foregroundcolor DARK_RED
        = new foregroundcolor("dark red");

    /** Dark green foreground. */
    public static final foregroundcolor DARK_GREEN
        = new foregroundcolor("dark green");

    /** Dark yellow foreground. */
    public static final foregroundcolor DARK_YELLOW
        = new foregroundcolor("dark yellow");

    /** Dark blue foreground. */
    public static final foregroundcolor DARK_BLUE
        = new foregroundcolor("dark blue");

    /** Dark magenta foreground. */
    public static final foregroundcolor DARK_MAGENTA
        = new foregroundcolor("dark magenta");

    /** Dark cyan foreground. */
    public static final foregroundcolor DARK_CYAN
    = new foregroundcolor("dark cyan");

    /** Grey foreground ("dark white"). */
    public static final foregroundcolor GREY
        = new foregroundcolor("grey");

    /** Bright red foreground. */
    public static final foregroundcolor LIGHT_RED
        = new foregroundcolor("light red");

    /** Bright green foreground. */
    public static final foregroundcolor LIGHT_GREEN
        = new foregroundcolor("light green");

    /** Bright yellow foreground. */
    public static final foregroundcolor LIGHT_YELLOW
        = new foregroundcolor("light yellow");

    /** Bright blue foreground. */
    public static final foregroundcolor LIGHT_BLUE
        = new foregroundcolor("light blue");

    /** Bright magenta foreground. */
    public static final foregroundcolor LIGHT_MAGENTA
        = new foregroundcolor("light magenta");

    /** Bright cyan foreground. */
    public static final foregroundcolor LIGHT_CYAN
        = new foregroundcolor("light cyan");

    /** White foreground. */
    public static final foregroundcolor WHITE
        = new foregroundcolor("white");

    /** Name of color. */
    private final String name;

    /**
     * Constructor.
     * 
     * @param   name    Color name.
     */
    private foregroundcolor(final String name) {
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
