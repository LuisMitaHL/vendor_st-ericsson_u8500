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
import tools.common.error;

public class enumeration extends definition {
	public ArrayList<enumentry> members;

	public enumeration(File file, tools.idl.parser.Token token, String _name) throws compilerexception {
		super(file, token, _name);
		this.members = new ArrayList<enumentry>();
	}
	
	private void printMembers(StringBuffer builded) {
		boolean second = false;
		level++;
		for(final enumentry en : members) 
		{
			if(second)
				builded.append("," + configuration.eol);
			shift(builded);
			builded.append(en.getName());
			if(en.value != null) {
			    enumentry env = hasMember(en.value);
			    if(env != null)
				builded.append("="+env.getName());
			    else
				builded.append("="+en.value);
			}
			second = true;
		}
		level--;
	}
	
	public void addEnumerator(enumentry newenum) throws compilerexception {
        for(final enumentry e : members) 
        {
	        if(e.name.equals(newenum.name))
                throw new compilerexception(error.IDT_ENUMERATOR_ALREADY_DEFINED, newenum.name);
	    }
	    
	    members.add(newenum);
	}
	
	private enumentry hasMember(String name) {
	    for(final enumentry en : members) 
	    {
	        if(name.equals(en.name))
	            return en;
	    }
	    return null;
	}
	/**
	 * Generate the structure in C; 
	 * @param variable
	 * @return
	 */
	public String printDefinition() {
		StringBuffer builded = new StringBuffer();
		builded.append("enum " + typeName + " {" + configuration.eol);
		printMembers(builded);
		builded.append("}");
		return builded.toString();
	}
	
	public String printType() {
		return "enum " + typeName;
	}

}
