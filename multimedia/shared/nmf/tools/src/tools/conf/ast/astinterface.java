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
import tools.common.util;

public class astinterface extends astbasefile {
    public String type;
    public String name;
    
    public boolean virtualInterface = false;

    // For Collection management
    public boolean collection = false;
    public int numberOfElement = 1;
    
    public astinterface(File _file, int line, int column, String _type, String _name) {
        super(_file, line, column);
        this.type = _type;
        this.name = _name;
    }
    
    public void setCollection(String max) {
        this.collection = true;
        try {
            this.numberOfElement = (int)util.StringToDecimal(max);
        } catch (java.lang.NumberFormatException e) {
            // Simulate max value in order to not generate an exception, but allow checker to detect it!
            this.numberOfElement = Integer.MAX_VALUE;
        }
    }
    
    public String getInterfaceName() {
        if(collection) {
            return name + "[" + numberOfElement + "]";
        } else {
            return name;
        }
    }

    public String getInterfaceName(int index) {
        if(collection) {
            return name + "[" + index + "]";
        } else {
            return name;
        }
    }
    
    public boolean isNMFLifeCycleInterface() {
        if(type.startsWith("lifecycle.constructor") ||
                type.startsWith("lifecycle.destructor") ||
                type.startsWith("lifecycle.starter") ||
                type.startsWith("lifecycle.stopper"))
            return true;
        return false;
    }


    
    public static String getNameOfInterfaceCollection(String itfname) {
        if(itfname.contains("[")) {
            return itfname.substring(0, itfname.indexOf("["));
        }
        return itfname;
    }
    
    public static int getIndexOfInterfaceCollection(String itfname) {
        if(itfname.contains("[")) {
            return (int)util.StringToDecimal(itfname.substring(itfname.indexOf("[") + 1, itfname.indexOf("]")));
        }
        return -1;
    }

}
