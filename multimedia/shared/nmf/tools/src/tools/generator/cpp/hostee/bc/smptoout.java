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

import tools.conf.graph.component;
import tools.generator.cpp.state.bindto;

import java.io.PrintStream;

public class smptoout implements bindto {    

    protected component client;
    protected String clientitfname;

    public smptoout(component _client, String _clientitfname) {
        client = _client;
        clientitfname = _clientitfname;
    }

    public String bindToUser(String itfname, PrintStream out) 
    {
        out.println("    _xyuv_" + client.getPathName_() + "." + clientitfname + " = cbl;");
        
        return client.getPathName_() + "[" + client.type.fullyname + "]";
    }

    public void unbindToUser(String itfname, PrintStream out) 
    {
        out.println("    _xyuv_" + client.getPathName_() + "." + clientitfname + " = 0x0;");
    }

    public boolean bindToSMP(String itfname, PrintStream out) 
    {
        out.println("    _xyuv_" + client.getPathName_() + "." + clientitfname + " = _xyitfcb_" + itfname + ";");
        
        return false;
    }

    public void unbindToSMP(PrintStream out)
    {
        out.println("    _xyuv_" + client.getPathName_() + "." + clientitfname + " = 0x0;");
    }

    public void bindToMPC(PrintStream out) 
    {
        out.println("    error = (t_nmf_error)CM_BindComponentFromUser(");
        out.println("         server, ");
        out.println("         serveritfname, ");
        out.println("         size, ");
        out.println("         &_xyuv_" + client.getPathName_() + "." + clientitfname + ");");
    }
    public void unbindToMPC(PrintStream out) 
    {
        out.println("    CM_UnbindComponentFromUser(&_xyuv_" + client.getPathName_() + "." + clientitfname + ");");
        out.println("    _xyuv_" + client.getPathName_() + "." + clientitfname + " = 0;");
   }

}
