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
package tools.common;

import java.io.File;


public class compilerexception extends Exception {
    static final long serialVersionUID = 1L;
    public int exitNumber;
    
    public File parsedFile;
    public int line;
    public int column;

    // Don't remove it ; use in nmf_util
    public compilerexception(String message) {
        super(message);
        exitNumber = 255;
    }
    public compilerexception(astbasefile _basefile, String message) {
        this(message);
        if(_basefile != null) {
            parsedFile = _basefile.parsedFile;
            line = _basefile.line;
            column = _basefile.column;
        }
    }

    public compilerexception(error id, Object ... args) {
        super(id.getFormatString(args));
        exitNumber = id.id;
    }
    public compilerexception(astbasefile _basefile, error id, Object ... args) {
        this(id, args);
        if(_basefile != null) {
            parsedFile = _basefile.parsedFile;
            line = _basefile.line;
            column = _basefile.column;
        }
    }
}
