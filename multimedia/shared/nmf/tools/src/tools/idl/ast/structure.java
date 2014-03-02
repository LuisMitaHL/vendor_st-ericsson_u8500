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
package tools.idl.ast;

import java.io.File;
import java.util.ArrayList;

import tools.common.compilerexception;
import tools.common.configuration;

public class structure extends definition {
	public ArrayList<structentry> members;

	public structure(File file, tools.idl.parser.Token token) throws compilerexception {
		super(file, token, null);
		this.members = new ArrayList<structentry>();
	}
	
	public void setName(String _name) {
	    super.setName(_name);
	       for(final structentry entry : members) 
	       {
	        if(entry.type.typeName == null) {
	            if(typeName.startsWith("t_xyuv_"))
	                entry.type.typeName = typeName + "_" + entry.name;
	            else
	                entry.type.typeName = "t_xyuv_" + typeName + "_" + entry.name;
	        }
	    }
	}
	
	private void printMembers(StringBuffer builded) {
		level++;
        for(final structentry param : members) 
        {
			shift(builded);
			builded.append(param.type.printAsStructField(param.name) + ";" + configuration.eol);
		}
		level--;
	}

	/**
	 * Generate the structure in C; 
	 * @param variable
	 * @return
	 */
	public String printDefinition() {
		StringBuffer builded = new StringBuffer();
		builded.append("struct " + typeName + " {" + configuration.eol);
		printMembers(builded);
		builded.append("}");
		return builded.toString();
	}
	
	public String printType() {
		return "struct " + typeName;
	}

}
