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

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.conf.ast.astprovide;
import tools.conf.ast.astrequire;
import tools.conf.graph.component;
import tools.conf.graph.binding.proxyhelper;
import tools.generator.cpp.state.automanaged;
import tools.generator.cpp.state.sharedvariable;
import tools.generator.cpp.state.variable;
import tools.idl.ast.Interface;

import java.io.IOException;
import java.io.PrintStream;
import java.util.HashSet;

/**
 * This class generate code for:
 *  - ARM TOP composite -> DSP (client == null)
 *  - ARM composite -> DSP
 *  
 * @author fassino
 *
 */
public class proxystub extends proxyhelper implements automanaged, sharedvariable, variable {    
    
    protected component client, server;
    protected String clientitfname, serveritfname;
    Interface itf;
    protected int size;

	public proxystub(astbasefile _declaration, 
            Interface _itf, 
            component _client, String _clientitfname,
            component _server, String _serveritfname,
            int _size) throws compilerexception, IOException {
        super(_declaration, "_stl." + _itf.name_);
        itf = _itf;
        client = _client;
        server = _server;
        clientitfname = _clientitfname;
        serveritfname = _serveritfname;
        size = _size;
                
        // Populate proxy !!!
        astprovide provide = new astprovide(null, 0, 0, itf.name, "target");
        provide.virtualInterface = true;
        type.addProvide(provide); 
        
        astrequire require = new astrequire(null, 0, 0, itf.name, "target");
        require.virtualInterface = true;
        type.addRequire(require); 
	}
	
    public void includeSharedFile(HashSet<String> include) {
    }

    public void declareShared(HashSet<String> variables) {
        variables.add("t_cm_error cm_error;");
    }

    public void declarePrivate(PrintStream out) {
    }

    public void cpppreconstruct(PrintStream out) {}

    public void cppconstruct(PrintStream out) {
    }

    public void construct(PrintStream out) 
    {
        out.println("  if((error = (t_nmf_error)CM_BindComponentFromUser(");
        out.println("       _xyuv_" + server.getPathName_() + ", ");
        out.println("       \"" + serveritfname + "\", ");
        out.println("       " + size + ", ");
        out.println("       &_xyuv_" + client.getPathName_() +"." + clientitfname  +")) != CM_OK)");
        out.println("    goto out_on_error;");
    }

    public void start(PrintStream out) {
    }

    public void stop(PrintStream out) {
    }

    public void destroy(PrintStream out) 
    {
        out.println("    CM_UnbindComponentFromUser(");
        out.println("         &_xyuv_" + client.getPathName_() +"." + clientitfname  +");");
    }
}
