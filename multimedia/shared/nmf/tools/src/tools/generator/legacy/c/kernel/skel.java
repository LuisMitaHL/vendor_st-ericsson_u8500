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
package tools.generator.legacy.c.kernel;

import java.io.PrintStream;

import tools.common.compilerexception;
import tools.idl.ast.*;
import tools.idl.generator.stubserver;

public class skel extends stubserver {
	boolean needheader;
	public skel(Interface _itf, boolean _needheader) throws compilerexception {
		setItf(_itf);
		needheader = _needheader;
	}
	
	public void Generate(PrintStream out) {
		if(needheader) {
			out.println("#include <cm/proxy/api/private/stub-requiredapi.h>");
			out.println();
		}

		out.println("/* Generated host skeleton for '" + itf.name + "' */");
		out.println("#include <host/" + itf.name.replace('.', '/') + ".h>");
		
		/*
		 * Generate all method skeleton for ARM side
		 */ 	
		int maxnb16 = 0;
		for(int j = 0; j < itf.methods.size(); j++) {
			Method md = itf.methods.get(j);
			
			out.println("static void sk_"+ itf.name_ + "_" + md.name + "(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {");
			out.println("  I" + itf.name_ + " *self = (I" + itf.name_ + " *)itfref;");
            for(int n = 0; n < needi[j]; n++)
                out.println("    int ___i" + n + ", ___j" + n +";");

			// Declare parameter
			for(int k = 0; k < md.parameters.size(); k++) {
				Parameter fp = (Parameter)md.parameters.get(k);
				out.println("  " + fp.type.declareLocalVariable(fp.name) + ";");
			}

			// Marshall parameter
			int nb16 = 0;
			for(int k = 0; k < md.parameters.size(); k++) {
				Parameter fp = md.parameters.get(k);
				out.println("    /* " + fp.name + " <" + fp.type.printType() + "> marshalling */");
				nb16 += readData(fp.type, fp.name, out, Integer.toString(nb16), "  ", 0);
			}
			if(nb16 > maxnb16)
				maxnb16 = nb16;
			
			// Call target
			out.println("  /* Server calling */");
			out.print("  self->" + md.name + "(self->THIS");
			for(int k = 0; k < md.parameters.size(); k++) {
				Parameter fp = (Parameter)md.parameters.get(k);
				out.print(", ");
				out.print(fp.type.passLocalVariableAsParameter(fp.name));
			}
			out.println(");");

			//out.println("  CM_INTERNAL_AcknowledgeEvent(THIS->mpc2hostId);");
			out.println("}");
			out.println();
		}
			
		out.println("static t_jump_method JT_" + itf.name_ + "[] = {");
		for(int j = 0; j < itf.methods.size(); j++) {
			Method md = itf.methods.get(j);
			out.println("  (t_jump_method)sk_" + itf.name_ + "_" + md.name + ",");
		}
		out.println("};");
	}
}