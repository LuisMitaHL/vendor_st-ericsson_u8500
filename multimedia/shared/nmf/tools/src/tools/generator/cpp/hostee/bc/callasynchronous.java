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
import tools.conf.graph.binding.bcevent;
import tools.generator.cpp.state.automanaged;
import tools.generator.cpp.state.bindfrom;
import tools.generator.cpp.state.variable;
import tools.idl.ast.*;

public class callasynchronous extends bcevent implements variable, automanaged {
    protected String itfname;
    protected String name;
    protected int indextodeclare;

    protected bindfrom server;

    public callasynchronous(astbasefile _declaration, 
            Interface _itf, 
            String _itfname, String _name, int _indextodeclare,
            bindfrom _server) throws compilerexception, IOException 
    {
        super(_declaration, _itf, _itfname, new tools.generator.cpp.hostee.callgenerator(_itf).registerAst());
        itfname = _itfname;
        name = _name;
        indextodeclare = _indextodeclare;
        server = _server;
    }

    public void declarePrivate(PrintStream out) 
    {
        if(indextodeclare != -1)
        {
            if(itfname.contains("["))
            {
                out.println("    " + type.fullyname_ + " _xybc_" + name + "[" + indextodeclare+ "]; // Call");
                out.println("    NMF::InterfaceReference *_xyitfc_" + name + "[" + indextodeclare+ "]; // Call");
            }
            else
            {
                out.println("    " + type.fullyname_ + " _xybc_" + name + "; // Call");
                out.println("   NMF::InterfaceReference *_xyitfc_" + name + "; // Call");
            }
        }
    }

    public void cpppreconstruct(PrintStream out) 
    {
    }

    public void cppconstruct(PrintStream out) {
        out.println("   _xyitfc_" + itfname + " = 0;");
    }

    public void construct(PrintStream out) {
    }

    public void start(PrintStream out) {
    }

    public void stop(PrintStream out) {
    }

    public void destroy(PrintStream out) {
    }

    public void getInterface(PrintStream out) 
    { 
        out.println("  if(compositeStrcmp(name, \"" + itfname + "\") == 0) {");
        server.getInterface(itfname, out);
        out.println("  }");
    }

    public void getComponentInterface(PrintStream out) 
    {
        out.println("  if(compositeStrcmp(compositeitfname, \"" + itfname + "\") == 0) {");
        server.getComponentInterface(out);
        out.println("  }");
    }

    public void bindFromUser(PrintStream out) {
        out.println("  if(compositeStrcmp(name, \"" + itfname + "\") == 0) {");
        server.bindFromUser(itfname, out);
        out.println("    return NMF_OK;");
        out.println("  }");
    }

    public void unbindFromUser(PrintStream out)
    {
        out.println("  if(compositeStrcmp(name, \"" + itfname + "\") == 0) {");
        server.unbindFromUser(itfname, out);
        out.println("  }");
    }
    
    public void bindFromMPC(PrintStream out) 
    {
        out.println("  if(compositeStrcmp(compositeitfname, \"" + itfname + "\") == 0) {");
        server.bindFromMPC(itfname, out);
        out.println("  }");
    }

    public void unbindFromMPC(PrintStream out) 
    {
        out.println("  if(compositeStrcmp(compositeitfname, \"" + itfname + "\") == 0) {");
        server.unbindFromMPC(itfname, out);
        out.println("  }");
    }

}

        