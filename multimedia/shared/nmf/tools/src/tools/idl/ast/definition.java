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

import tools.common.compilerexception;
import tools.common.error;

public abstract class definition extends Type {
    final int MAX_INDENTIFIER_SIZE = 255;
    
	public definition(File file, tools.idl.parser.Token token, String _name) throws compilerexception {
		super(file, token);
		typeName = _name;
        if(_name != null && _name.length() > MAX_INDENTIFIER_SIZE)
            throw new compilerexception(this, error.IDENTIFIER_TOO_LARGE, _name, "C", MAX_INDENTIFIER_SIZE);
	}
	
	public void setName(String _name) {
	    typeName = _name;
	}
	
	/*
	 * Just for formating well in generated code
	 */
	protected static int level = 0;
	protected void shift(StringBuffer builded) {
		for(int i = 0; i < level; i++) {
			builded.append("  ");
		}
	}	

	public String printAsStructField(String fieldName) {
	    if(typedefinition != null)
	        return super.printAsStructField(fieldName);
	    else
	        return printDefinition() + " " + fieldName;
	}


	/**
	 * Generate the structure in C; 
	 * @param variable
	 * @return
	 */
	public abstract String printDefinition();
}
