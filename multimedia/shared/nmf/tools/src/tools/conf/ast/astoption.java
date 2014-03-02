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
package tools.conf.ast;

import java.io.File;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.common.error;
import tools.common.targetfactory.TargetNames;

public class astoption extends astbasefile {
    public enum OptionNames {
        CPPFLAGS, 
        CFLAGS,
        OVERWRITTEN_CFLAGS,
        FIXED_CFLAGS,
        LDFLAGS,
        LIBLDFLAGS,
        GROUPLDFLAGS,
        stack,
        incdir,
        memories
    };

    public TargetNames target; // Could be null
    public OptionNames name;
    public String value;

    public astoption(TargetNames _target, OptionNames _name, String _value) {
        super(null, 0, 0);
        
        // Cast name
        target = _target;
        name = _name;
        value = _value;
    }
    public astoption(OptionNames _name, String _value) {
        this(null, _name, _value);
    }
    public astoption(File _file, int line, int column, String _target, String _name, String _value) throws compilerexception {
        this(_file, line, column, _name, _value);
        
        // Cast target
        try {
            target = TargetNames.valueOf(_target);
        } catch(IllegalArgumentException e) {
            throw new compilerexception(this, error.BAD_OPTION_TARGET, 
                    _target);
        }

     }
    public astoption(File _file, int line, int column, String _name, String _value) throws compilerexception {
        super(_file, line, column);

        // Cast name
        try {
            name = OptionNames.valueOf(_name);
        } catch(IllegalArgumentException e) {
            throw new compilerexception(this, error.BAD_OPTION, 
                    _name);
        }
        
        value = _value;
        target = null;
    }
}
