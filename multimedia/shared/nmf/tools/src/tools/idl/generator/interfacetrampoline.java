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
package tools.idl.generator;

import java.io.PrintStream;

import tools.idl.ast.*;
import tools.conf.graph.*;

public class interfacetrampoline {

	public static void GenerateManualThisSwitch(Interface itf, String itfname,
			primitive to, 
			PrintStream out) {
	    String methprefix = to.getProvide(itfname).methprefix;

	    for(int j = 0; j < itf.methods.size(); j++) {
	        Method md = (Method)itf.methods.get(j);
	        String topathname_ = to.getPathName_();
	        out.println("static " + md.printMethod("T" + topathname_, methprefix, false, false, false) + " {");
	        out.println("  void *oldTHIS = THIS;");
	        out.println("  THIS = &" + topathname_ + ";");
	        out.print("  " + to.type.fullyname_ + "_" + methprefix + md.name + "(");
	        if(md.parameters.size() != 0) {
	            for(int k = 0; k < md.parameters.size(); k++) {
	                if(k >= 1) out.print(", ");
	                Parameter fp = md.parameters.get(k);
	                out.print(fp.name);
	            }
	        }
	        out.println(");");
	        out.println("  THIS = oldTHIS;");

	        out.println("}");
	    }
	}
	
}
