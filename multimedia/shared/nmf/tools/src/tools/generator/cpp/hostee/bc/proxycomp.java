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
import tools.conf.ast.astcomponent;
import tools.conf.ast.astcontent;
import tools.conf.graph.binding.proxyhelper;
import tools.generator.cpp.state.automanaged;
import tools.generator.cpp.state.sharedvariable;
import tools.generator.cpp.state.variable;

import java.io.PrintStream;
import java.util.HashSet;

public class proxycomp extends proxyhelper implements automanaged, sharedvariable, variable {    
    public proxycomp(astcontent _content, astcomponent _type) throws compilerexception {
        super(_content, _type);
    }
    

    public void includeSharedFile(HashSet<String> include) {
    }

    public void declareShared(HashSet<String> variables) {
        variables.add("t_cm_error cm_error;");
        variables.add("t_cm_domain_id " + getMPC() + ";");
        if(priority.getOrdinal() == -1)
        {
            variables.add("t_sint32 " + priority.getValue() +";");
        }
    }

    public void declarePrivate(PrintStream out) {
        out.println("    t_cm_instance_handle _xyuv_" + getPathName_() + ";");
    }

    public void cpppreconstruct(PrintStream out) {
        out.println(",");
        out.print("    _xyuv_" + getPathName_() + "(0)");
    }

    public void cppconstruct(PrintStream out) {
        if(priority.getOrdinal() == -1)
        {
            out.println("  " + priority.getValue() + " = -1;");
        }
    }

    public void construct(PrintStream out) {
        out.println("  if((error = (t_nmf_error)CM_InstantiateComponent(");
        out.println("        \"" + type.fullyname +"\",");
        out.println("        " + getMPC() + ",");
        out.println("        " + priority.getValue() + ","); // TODO
        out.println("        \"" + nameinowner +"\",");
        out.println("        &_xyuv_" + getPathName_() +")) != CM_OK)");
        out.println("    goto out_on_error;");
    }

    public void start(PrintStream out) {
        out.println("  CM_StartComponent(_xyuv_" + getPathName_() +");");
    }

    public void stop(PrintStream out) {
        out.println("  CM_StopComponent(_xyuv_" + getPathName_() +");");
    }

    public void destroy(PrintStream out) {
        out.println("    CM_DestroyComponent(_xyuv_" + getPathName_() +");");
    }

    public void getMPCreference(PrintStream out)
    {
        out.println("  if(compositeStrcmp(\"" + nameinowner + "\", nameinowner) == 0)");
        out.println("    return _xyuv_" + getPathName_() +";");
    }
}
