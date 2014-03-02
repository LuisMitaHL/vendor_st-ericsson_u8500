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

public class diesedefine extends definition {
    public String value;
    
    final int MAX_VALUE_SIZE = 255;
    
	public diesedefine(File file, tools.idl.parser.Token token, String _name, String _value) throws compilerexception {
		super(file, token, _name);
		value = _value;
		if(_value.length() > MAX_INDENTIFIER_SIZE)
		    throw new compilerexception(this, error.STRING_TOO_LARGE, _value, MAX_VALUE_SIZE);
	}
	
	/**
	 * Generate the structure in C; 
	 * @param variable
	 * @return
	 */
	public String printDefinition() {
		StringBuffer builded = new StringBuffer();
		builded.append("#define " + typeName + " " + value);
		return builded.toString(); 
	}
	
	public String printType() {
	    assert(false);
	    return null;
	}

}
