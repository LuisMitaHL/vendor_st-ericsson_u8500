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
import java.util.HashSet;

import tools.common.targetfactory;
import tools.common.targetfactory.CodingStyle;
import tools.idl.ast.Interface;
import tools.idl.ast.Method;

public class predefinemethod {

    public static void GenerateClass(Interface itf, PrintStream out) {
       for(int j = 0; j < itf.methods.size(); j++) {
           Method md = (Method)itf.methods.get(j);
           out.println("extern  " + md.printMethodLibrary() + ";");
       }
           
   }
   public static void GenerateUnanbiguous(Interface itf,
             PrintStream out) {
        for(int j = 0; j < itf.methods.size(); j++) {
            Method md = (Method)itf.methods.get(j);
            out.println("extern  " + md.printMethodLibrary() + ";");
        }
            
    }
    
    protected static void Generate(Interface itf,
            boolean errorForAsynchronous,
            String component, String methprefix,
            boolean useAsACollection, 
            String modifier,
            PrintStream out) {
        for(int j = 0; j < itf.methods.size(); j++) {
            Method md = (Method)itf.methods.get(j);
            out.println(modifier + " " + md.printMethod(component, methprefix, false, useAsACollection, errorForAsynchronous) + ";");
        }
	}
	
	public static void Generate(Interface itf,
			String component, String methprefix,
			boolean useAsACollection,
			PrintStream out) {
		Generate(itf,
		        component.startsWith("_stl"),
				component, 
				methprefix,
                useAsACollection,
				"extern",
				out);
	}

	/**
	 * 
	 * @param itf
	 * @param out
	 * @param incls
	 * @return true if interface added in include
	 */
	public static boolean GenerateInclude(Interface itf, PrintStream out, HashSet<Interface> incls) {
        if(! incls.contains(itf)) {
            if(targetfactory.style == CodingStyle.CPP)
                out.println("#include <" + itf.nameslash + ".hpp>");
            else
                out.println("#include <" + itf.nameslash + ".h>");
            incls.add(itf);
            return true;
        }
        return false;
	}

}
