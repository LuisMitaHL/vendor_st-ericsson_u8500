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

public class astrequire extends astinterface {
    public boolean optional = false;
    public boolean isStatic = false;
    public boolean isIntrinsec = false;

    public astrequire(File _file, int line, int column, String _type, String _name) {
        super(_file, line, column, _type, _name);
        
        if(type.equals("rtos.api.ee"))
            isStatic = isIntrinsec = true;
        if(type.equals("rtos.api.panic"))
            isStatic = isIntrinsec = true;
        if(type.equals("rtos.api.runtime") || type.equals("ee.api.debug"))
            isStatic = isIntrinsec = true;
        if(type.equals("communication.api.communication"))
            isStatic = isIntrinsec = true;
        if(type.equals("ee.api.panic"))
            isStatic = isIntrinsec = true;
        if(type.equals("ee.api.sleep"))
            isStatic = isIntrinsec = true;
        if(type.equals("ee.api.sleepRegister"))
            isStatic = isIntrinsec = true;
        if(type.equals("trace.api.trace"))
            isStatic = isIntrinsec = true;
    }
}
