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

public class proxyskel extends proxyhelper implements automanaged, sharedvariable, variable {    

    protected component client, server;
    protected String clientitfname, serveritfname;
    Interface itf;
    protected int size;

    public proxyskel(astbasefile _declaration, 
            Interface _itf, 
            component _client, String _clientitfname,
            component _server, String _serveritfname,
            int _size) throws compilerexception, IOException {
        super(_declaration, "_skl." + _itf.name_);
        itf = _itf;
        size = _size;
        client = _client;
        server = _server;
        clientitfname = _clientitfname;
        serveritfname = _serveritfname;
        
        // Populate proxy !!!
        astrequire require = new astrequire(null, 0, 0, itf.name, "target");
        require.virtualInterface = true;
        type.addRequire(require);
        
        astprovide provide = new astprovide(null, 0, 0, itf.name, "target");
        provide.virtualInterface = true;
        type.addProvide(provide); 
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

    public void construct(PrintStream out) {
        out.println("  signalComponentCreation(" + server.priority.getValue() + ");");

        out.println("  if((error = (t_nmf_error)CM_BindComponentToUser(");
        out.println("       (t_nmf_channel)EEgetDistributionChannel(" + server.priority.getValue() + "),");
        out.println("       _xyuv_" + client.getPathName_() + ", ");
        out.println("       \"" + clientitfname + "\", ");
        out.println("       &_xyuv_" + server.getPathName_()/* +"." + serveritfname */ +",");
        out.println("       " + size + ")) != CM_OK)");
        out.println("    goto out_on_error;");
    }

    public void start(PrintStream out) {
    }

    public void stop(PrintStream out) {
    }

    public void destroy(PrintStream out) {
        out.println("    void *handle" + hashCode() + ";");
        out.println("    CM_UnbindComponentToUser(");
        out.println("       (t_nmf_channel)EEgetDistributionChannel(" + server.priority.getValue() + "),");
        out.println("       _xyuv_" + client.getPathName_() + ", ");
        out.println("       \"" + clientitfname + "\", ");
        out.println("       &handle" + hashCode() + ");");

        out.println("    signalComponentDestruction(" + server.priority.getValue() + ");");
    }
}
