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
import tools.idl.ast.Interface;

import java.io.PrintStream;

public class mpctoout implements bindto {    

    protected component client;
    protected String clientitfname;
    protected Interface itf;

    public mpctoout(Interface _itf, component _client, String _clientitfname) {
        itf = _itf;
        client = _client;
        clientitfname = _clientitfname;
    }

    public String bindToUser(String itfname, PrintStream out) 
    {
            // This is a binding outside composite, take client as priority
            out.println("    signalComponentCreation(" + client.priority.getValue() + ");");

            out.println("    if((error = (t_nmf_error)CM_BindComponentToUser(");
            out.println("         (t_nmf_channel)EEgetDistributionChannel(" + client.priority.getValue() + "),");
            out.println("         _xyuv_" + client.getPathName_() + ", ");
            out.println("         \"" + clientitfname + "\", ");
            out.println("         cbl,");
            out.println("         size)) != CM_OK) {");
            out.println("      signalComponentDestruction(" + client.priority.getValue() + ");");
            out.println("    }");
            
            return client.getPathName_() + "[" + client.type.fullyname + "]";
    }

    public void unbindToUser(String itfname, PrintStream out) 
    {
        out.println("      CM_UnbindComponentToUser(");
        out.println("           (t_nmf_channel)EEgetDistributionChannel(" + client.priority.getValue() + "),");
        out.println("           _xyuv_" + client.getPathName_() + ", ");
        out.println("           \"" + clientitfname + "\", ");
        out.println("           (void**)0x0);");
        out.println("      signalComponentDestruction(" + client.priority.getValue() + ");");
    }

    public boolean bindToSMP(String itfname, PrintStream out) 
    {
        
        // YES, here we shortcut event created between yourself and server 
        
        out.println("    {");
        out.println("      I" + itf.name_ + " output;");
        out.println("      signalComponentCreation(" + client.priority.getValue() + ");");
        out.println("      target->getInterface(targetname, &output);");
        out.println("      if((error = (t_nmf_error)CM_BindComponentToUser(");
        out.println("           (t_nmf_channel)EEgetDistributionChannel(" + client.priority.getValue() + "),");
        out.println("           _xyuv_" + client.getPathName_() + ", ");
        out.println("           \"" + clientitfname + "\", ");
        out.println("           output.getReference(),");
        out.println("           size)) != CM_OK) {");
        out.println("        signalComponentDestruction(" + client.priority.getValue() + ");");
        out.println("      }");
        out.println("    }");

        return true;
    }

    public void unbindToSMP(PrintStream out) 
    {
        out.println("    {");
        out.println("      void *cbl;");
        out.println("      CM_UnbindComponentToUser(");
        out.println("           (t_nmf_channel)EEgetDistributionChannel(" + client.priority.getValue() + "),");
        out.println("           _xyuv_" + client.getPathName_() + ", ");
        out.println("           \"" + clientitfname + "\", &cbl);");
        out.println("      signalComponentDestruction(" + client.priority.getValue() + ");");
        out.println("    }");
    }

    public void bindToMPC(PrintStream out)
    {
        out.println("    error = (t_nmf_error)CM_BindComponentAsynchronous(");
        out.println("          _xyuv_" + client.getPathName_() + ",");
        out.println("          \"" + clientitfname + "\",");
        out.println("          server,");
        out.println("          serveritfname,");
        out.println("          size);");
    }
    public void unbindToMPC(PrintStream out) 
    {
        out.println("      CM_UnbindComponentAsynchronous(");
        out.println("            _xyuv_" + client.getPathName_() + ",");
        out.println("            \"" + clientitfname + "\");");
    }

}
