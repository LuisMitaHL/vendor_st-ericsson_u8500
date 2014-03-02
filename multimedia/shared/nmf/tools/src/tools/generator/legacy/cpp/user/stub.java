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
package tools.generator.legacy.cpp.user;

import tools.common.compilerexception;
import tools.idl.ast.Interface;
import tools.idl.ast.Method;
import tools.idl.ast.Parameter;
import tools.idl.generator.stubclient;

import java.io.PrintStream;

public class stub extends stubclient {    
    boolean needheader;
    public stub(Interface _itf, boolean _needheader) throws compilerexception {
        setItf(_itf);
        needheader = _needheader;
    }
    
    public void Generate(PrintStream out) { 
        if(needheader) {
            out.println("#include <cm/inc/cm.h>");
            out.println("#include <cm/proxy/api/private/stub-requiredapi.h>");
            out.println();
        }

        out.println("/* Generated C++ stub for '" + itf.name + "'");
        out.println("   Generator:" + stub.class.getName() + " */");
        out.println("#include <host/" + itf.name.replace('.', '/') + ".hpp>");
        out.println();
        
        out.println("class " + itf.name_ + ": public " + itf.name_ + "Descriptor");
        out.println("{");
        out.println("  public:");
        out.println("    t_cm_bf_host2mpc_handle    host2mpcId;");
        out.println();
        out.println("  public:");
  
        int maxnb16 = 0;
        for(int j = 0; j < itf.methods.size(); j++) 
        {
            Method md = (Method)itf.methods.get(j);

            out.println("    virtual " + md.printMethodCpp("", false) + " {");
            if(methnb16[j] > 0)
                out.println("      t_uint16 _xyuv_data[" + methnb16[j] + "];");
            for(int n = 0; n < needi[j]; n++)
                out.println("      int ___i" + n + ", ___j" + n +";");
            
            // Marshall parameter
            int nb16 = 0;
            for (int k = 0; k < md.parameters.size(); k++) 
            {
                Parameter fp = md.parameters.get(k);
                out.println("      /* " + fp.name + " <" + fp.type.printType() + "> marshalling */");
                nb16 += writeData(fp.type, fp.name, out, Integer.toString(nb16), "      ", 0);
            }
            if (nb16 > maxnb16)
                maxnb16 = nb16;

            out.println("      /* Event sending */");
            if(methnb16[j] > 0)
                out.println("      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, " + methnb16[j] + "*2, " + j + ");");
            else
                out.println("      CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, " + j + ");");
            out.println("    }");
        }

        out.println("};");

        out.println("static t_cm_bf_host2mpc_handle* INITstub_" + itf.name_ + "(t_nmf_interface_desc* itf) {");
        out.println("  " + itf.name_ + " *stub = new " + itf.name_ + "();");
        out.println("  if(stub == 0x0)");
        out.println("    return 0x0;");
        out.println("  *(NMF::InterfaceReference*)itf = stub;");
        out.println("  return &stub->host2mpcId;");
        out.println("}");
        out.println();
    }
}
