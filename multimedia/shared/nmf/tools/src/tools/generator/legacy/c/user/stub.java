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
package tools.generator.legacy.c.user;

import java.io.PrintStream;

import tools.idl.ast.*;
import tools.idl.generator.stubclient;
import tools.common.compilerexception;

public class stub extends stubclient {
	boolean needheader;
	public stub(Interface _itf, boolean _needheader) throws compilerexception {
		setItf(_itf);
		needheader = _needheader;
	}
	
	public void Generate(PrintStream out) {	
		if(needheader) {
            out.println("#include <cm/proxy/api/private/stub-requiredapi.h>");
			out.println();
		}

		out.println("/* Generated host stub for '" + itf.name + "' */");
        out.println("#include <host/" + itf.name.replace('.', '/') + ".h>");
		out.println();
		
		int maxnb16 = 0;
		for(int j = 0; j < itf.methods.size(); j++) {
			Method md = (Method)itf.methods.get(j);
			
			out.println("static " + md.printMethod("st_" + itf.name_, "", true, false, true) + " {");
			out.println("  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;");
            if(methnb16[j] > 0)
                out.println("  t_uint16 _xyuv_data[" + methnb16[j] + "];");
            for(int n = 0; n < needi[j]; n++)
                out.println("    int ___i" + n + ", ___j" + n +";");
			
			// Marshall parameter
			int nb16 = 0;
			for (int k = 0; k < md.parameters.size(); k++) {
				Parameter fp = md.parameters.get(k);
				out.println("    /* " + fp.name + " <" + fp.type.printType() + "> marshalling */");
				nb16 += writeData(fp.type, fp.name, out, Integer.toString(nb16), "  ", 0);
			}
			if (nb16 > maxnb16)
				maxnb16 = nb16;
			
			out.println("  /* Event sending */");
            if(methnb16[j] > 0)
                out.println("  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, " + methnb16[j] + "*2, " + j + ");");
            else
                out.println("  return CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, " + j + ");");
            out.println("}");
			out.println();
		}
		
        out.println("static t_cm_bf_host2mpc_handle* INITstub_" + itf.name_ + "(t_nmf_interface_desc* itf) {");
        for (int j = 0; j < itf.methods.size(); j++) {
            Method md = itf.methods.get(j);
            out.println("  ((I" + itf.name_ + " *)itf)->" + md.name + " = st_" + itf.name_ + "_" + md.name + ";");
        }
        out.println("  return (t_cm_bf_host2mpc_handle*)&((I" + itf.name_ + " *)itf)->THIS;");
		out.println("}");
		out.println();
		
	}

}
