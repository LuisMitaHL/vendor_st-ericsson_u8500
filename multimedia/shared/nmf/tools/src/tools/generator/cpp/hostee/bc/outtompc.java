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

import tools.conf.graph.interfacereference;
import tools.generator.cpp.state.bindfrom;
import tools.idl.ast.Interface;

import java.io.PrintStream;

public class outtompc extends bindfrom 
{
    public outtompc(Interface _itf, interfacereference _ir)
    {
        super(_itf, _ir);
    }

    public void bindFromUser(String itfname, PrintStream out)
    {
        out.println("    t_nmf_error error = (t_nmf_error)CM_BindComponentFromUser(");
        out.println("       _xyuv_" + ir.server.getPathName_() + ", ");
        out.println("       \"" + ir.getInterfaceName() + "\", ");
        out.println("       size, ");
        out.println("       reference);");
        out.println("    if(error != NMF_OK)");
        out.println("      return error;");
        out.println("    _xyitfc_" + itfname + " = reference;");
    }
    
    public void unbindFromUser(String itfname, PrintStream out)
    {
        out.println("    return CM_UnbindComponentFromUser(_xyitfc_" + itfname  +");");
    }
    

    public void getComponentInterface(PrintStream out) 
    {
        out.println("       compositeCpycmp(realitfname, \"" + ir.getInterfaceName() + "\", MAX_INTERFACE_NAME_LENGTH);");
        out.println("       return _xyuv_" + ir.server.getPathName_() + ";");
    }

    public void bindFromMPC(String itfname, PrintStream out) {
        out.println("    return (t_nmf_error)CM_BindComponentAsynchronous(");
        out.println("          client,");
        out.println("          clientitfname,");
        out.println("         _xyuv_" + ir.server.getPathName_() + ", ");
        out.println("         \"" + ir.getInterfaceName() + "\", ");
        out.println("          size);");
    }

    public void unbindFromMPC(String itfname, PrintStream out) {
        out.println("      return CM_UnbindComponentAsynchronous(");
        out.println("            client,");
        out.println("            clientitfname);");        
    }

    
}
