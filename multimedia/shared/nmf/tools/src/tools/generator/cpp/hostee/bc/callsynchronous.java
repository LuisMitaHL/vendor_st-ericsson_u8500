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
package tools.generator.cpp.hostee.bc;

import java.io.IOException;
import java.io.PrintStream;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.conf.ast.astprovide;
import tools.conf.graph.interfacereference;
import tools.conf.graph.binding.proxyhelper;
import tools.generator.cpp.state.automanaged;
import tools.generator.cpp.state.undrawable;
import tools.idl.ast.*;

public class callsynchronous extends proxyhelper implements automanaged, undrawable 
{
    protected Interface itf;
    protected String itfname;

    protected interfacereference ir;

    public callsynchronous(astbasefile _declaration, 
            Interface _itf, 
            String _itfname,
            interfacereference _ir) throws compilerexception, IOException
    {
        super(_declaration, "cs." + _itf.name_, _itfname);
        itf = _itf;
        itfname = _itfname;
        ir = _ir;
    }

    public void getInterface(PrintStream out) 
    { 
        astprovide providesc = ir.server.getProvide(ir.getInterfaceName());

        out.println("  if(compositeStrcmp(name, \"" + itfname + "\") == 0) {");
        if(providesc.collection || ! "".equals(providesc.methprefix)) 
        {
            out.println("    *reference = (" +  itf.name_ + "Descriptor*)&_xyuv_" + ir.server.getPathName_() + "._xywrap_" + ir.getInterfaceName() + ";");        
        } 
        else 
        {
            out.println("    *reference = (" +  itf.name_ + "Descriptor*)&_xyuv_" + ir.server.getPathName_() + ";");        
        }
        out.println("    return NMF_OK;");
        out.println("  }");
    }
}
