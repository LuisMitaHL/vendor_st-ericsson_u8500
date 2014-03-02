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
package tools.generator.c;

import tools.idl.ast.Interface;
import tools.idl.ast.Method;
import tools.common.targetfactory;
import tools.common.targetfactory.DomainNames;
import tools.conf.ast.astprovide;
import tools.conf.graph.interfacereference;

public class InterfaceReferenceStatic {
    public static StringBuffer GenerateNull(Interface itf) {
        StringBuffer out = new StringBuffer();

        out.append("{(void*)0x0");        // What you what here
        for(final Method md : itf.methods) 
        {
            out.append(", (" +  md.printReference("", false, null) + ")0x0");  // 0x0 since IS_NULL_INTERFACE rely on it
        }
        out.append("}       /* unbinded */");

        return out;
    }

    public static StringBuffer GenerateNotNull(Interface itf, 
            interfacereference itfref) {
        StringBuffer out = new StringBuffer();

        astprovide provide = itfref.server.getProvide(itfref.itfname);

        if(targetfactory.domain == DomainNames.MMDSP)
            out.append("{(void*)&_sb_bases_section");
        else
            out.append("{(void*)0xFEEDU");

        for(final Method md : itf.methods) 
        {
            out.append(", " + itfref.server.type.fullyname_ + "_"
                    + (provide.collection ? itfref.collectionIndex : "")
                    + provide.methprefix + md.name);
        }
        out.append("}         /* -> '" + itfref.server.getPathName() + "'." + itfref.itfname + "*/");

        return out;
    }
}
