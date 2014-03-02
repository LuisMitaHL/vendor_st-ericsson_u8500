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
import java.util.ArrayList;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.conf.ast.astprimitive;
import tools.conf.graph.component;
import tools.conf.graph.binding.bcevent;
import tools.generator.cpp.state.automanaged;
import tools.generator.cpp.state.undrawable;
import tools.generator.cpp.state.variable;
import tools.idl.ast.*;

public class callbackbase extends bcevent implements undrawable, automanaged, variable {
    protected String itfname;
    protected boolean optional;
    
    protected String name;
    protected int indextodeclare;   // Just here to allow array declaration

    public static class subclient {
        component client; 
        String clientitfname;
        public subclient(component _client, String _clientitfname)
        {
            client = _client;
            clientitfname = _clientitfname;
        }
    }

    public ArrayList<subclient> clients = new ArrayList<subclient>();

    public callbackbase(astbasefile _declaration, 
            Interface _itf, 
            String _itfname,
            boolean _optional, 
            String _name, 
            int _indextodeclare,
            astprimitive _type) throws compilerexception, IOException {
        super(_declaration, _itf, _itfname, _type);
        itfname = _itfname;
        optional = _optional;
        name = _name;
        indextodeclare = _indextodeclare;
    }
    
    public void declarePrivate(PrintStream out) 
    {
        if(indextodeclare != -1)
        {
            if(itfname.contains("["))
            {
                out.println("    I" + itf.name_ + " _xyitfcb_" + name + "[" + indextodeclare+ "]; // Callback");
                out.println("    enum bindType _xyitfcbbt_" + name + "[" + indextodeclare+ "]; // Callback");
            }
            else
            {
                out.println("    I" + itf.name_ + " _xyitfcb_" + name + "; // Callback");
                out.println("    enum bindType _xyitfcbbt_" + name + "; // Callback");
            }
        }
    }

    public void cpppreconstruct(PrintStream out) 
    {
    }

    public void cppconstruct(PrintStream out) 
    {
        out.println("  _xyitfcb_" + itfname + " = 0x0;");
    }

    public void construct(PrintStream out) {
    }

    public void start(PrintStream out) {
    }

    public void stop(PrintStream out) {
    }

    public void destroy(PrintStream out) {
    }

    public void bindComponent(PrintStream out)
    {
        out.println("  if(compositeStrcmp(name, \"" + itfname + "\") == 0) {");

        if(! optional) 
        {
            out.println("    if(startNumber > 0)");
            out.println("      return NMF_COMPONENT_NOT_STOPPED;");
        }
        out.println("    if(! _xyitfcb_" + itfname + ".IsNullInterface())");
        out.println("      return NMF_INTERFACE_ALREADY_BINDED;");

        out.println("    if((error = target->getInterface(targetname, &_xyitfcb_" + itfname + ")) != NMF_OK)");
        out.println("      return error;");

        for(subclient client : clients)
        {
            out.println("    _xyitfcbbt_" + itfname + "= BINDSYNC;");
            out.println("    _xyuv_" + client.client.getPathName_() + "." + client.clientitfname + " = _xyitfcb_" + itfname + ";");
        }
        
        out.println("    return NMF_OK;");
        out.println("  }");
    }

    public void unbindComponent(PrintStream out) 
    {
        out.println("  if(compositeStrcmp(name, \"" + itfname + "\") == 0) {");
        
        if(! optional) 
        {
            out.println("    if(startNumber > 0)");
            out.println("      return NMF_COMPONENT_NOT_STOPPED;");
        }
        out.println("    if(_xyitfcb_" + itfname + ".IsNullInterface())");
        out.println("      return NMF_INTERFACE_NOT_BINDED;");

        for(subclient client : clients)
        {
            out.println("    _xyuv_" + client.client.getPathName_() + "." + client.clientitfname + " = 0x0;");
        }

        out.println("    _xyitfcb_" + itfname + " = 0x0;");        
        out.println("    return NMF_OK;");
        out.println("  }");
    }

}
