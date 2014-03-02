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

import java.io.OutputStream;
import java.io.PrintStream;
import java.util.Locale;

/**
 * Create platform specific.
 */
public class factory {

    private static console getRawConsole(PrintStream stream) {
        String term = System.getenv("ANSI_COLORS_DISABLED");
        String os = System.getProperty("os.name").toLowerCase(Locale.US);
        if("1".equals(term)) {
        } else if (os.indexOf("windows") != -1) {
            try {
                return new windows(stream);
            } catch (UnsatisfiedLinkError e) {
                System.err.println(e);
                System.err.println("java.library.path = " + System.getProperty("java.library.path"));
            } catch (NoClassDefFoundError e) {
                System.err.println(e);
                System.err.println("java.library.path = " + System.getProperty("java.library.path"));
            }
        } else {
            return new escaped(stream);
        }
        return new normal();
    }

	protected static console consoleErr = null;
	protected static console consoleOut = null;
	 
	/**
     * Get console access. 
     */
	public static final console getErrConsole() {
        if(consoleErr == null) {
            consoleErr = getRawConsole(System.err);
        }
        return consoleErr;
	}
	public static final console getOutConsole() {
	    if(consoleOut == null) {
	        consoleOut = getRawConsole(System.out);
        }
        return consoleOut;
	}

	public static final console getConsole(OutputStream os) {
		if(os == System.out)
			return getOutConsole();
		else
			return getErrConsole();
	}
}
