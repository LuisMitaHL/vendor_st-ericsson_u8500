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

import tools.common.compilerexception;
import tools.conf.ast.astcontent;
import tools.conf.graph.component;
import tools.conf.graph.binding.proxyhelper;
import tools.generator.cpp.state.automanaged;
import tools.generator.cpp.state.sharedvariable;
import tools.generator.cpp.state.undrawable;
import tools.generator.cpp.state.variable;

import java.io.PrintStream;
import java.util.HashSet;

public class proxybind extends proxyhelper implements automanaged, undrawable, sharedvariable, variable {    
    protected component client, server;
    protected String clientitfname, serveritfname;
    protected int size;
    protected boolean asynchronous;

    public proxybind(astcontent _content, 
            component _client, String _clientitfname, 
            component _server, String _serveritfname,
            int _size,
            boolean _asynchronous) throws compilerexception {
        super(_content);
        client = _client;
        clientitfname = _clientitfname;
        server = _server;
        serveritfname = _serveritfname;
        size = _size;
        asynchronous = _asynchronous;
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
        if(asynchronous) {
            out.println("  if((error = (t_nmf_error)CM_BindComponentAsynchronous(");
            out.println("        _xyuv_" + client.getPathName_() + ",");
            out.println("        \"" + clientitfname + "\",");
            out.println("        _xyuv_" + server.getPathName_() + ",");
            out.println("        \"" + serveritfname + "\",");
            out.println("        " + size +")) != CM_OK)");
            out.println("    goto out_on_error;");
        } else {
            out.println("  if((error = (t_nmf_error)CM_BindComponent(");
            out.println("        _xyuv_" + client.getPathName_() + ",");
            out.println("        \"" + clientitfname + "\",");
            out.println("        _xyuv_" + server.getPathName_() + ",");
            out.println("        \"" + serveritfname + "\")) != CM_OK)");
            out.println("    goto out_on_error;");
        }
    }

    public void start(PrintStream out) {
    }

    public void stop(PrintStream out) {
    }

    public void destroy(PrintStream out) {
        if(asynchronous) {
            out.println("    if(_xyuv_" + client.getPathName_() + " != 0x0)");
            out.println("      CM_UnbindComponentAsynchronous(");
            out.println("          _xyuv_" + client.getPathName_() + ",");
            out.println("          \"" + clientitfname + "\");");
        } else {
            out.println("    if(_xyuv_" + client.getPathName_() + " != 0x0)");
            out.println("      CM_UnbindComponent(");
            out.println("          _xyuv_" + client.getPathName_() + ",");
            out.println("          \"" + clientitfname + "\");");
        }
    }
}
