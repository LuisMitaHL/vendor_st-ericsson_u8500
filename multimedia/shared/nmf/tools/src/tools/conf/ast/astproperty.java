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
package tools.conf.ast;

import java.io.File;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.common.error;

/**
 * Associate property with value.
 * 
 * @author fassino
 */
public class astproperty extends astbasefile {
	public String name;
	protected String value;
	
    final int MAX_PROPERTY_NAME_LENGTH = 32;
    final int MAX_PROPERTY_VALUE_LENGTH = 128;

	public astproperty(File _file, tools.conf.parser.Token token, String _name, String _value)  throws compilerexception{
		super(_file, token.beginLine, token.beginColumn);
		this.name = _name;
		this.value = _value;
		
        if(name.length() > MAX_PROPERTY_NAME_LENGTH)
            throw new compilerexception(this, error.IDENTIFIER_TOO_LARGE,
                    name, "property", MAX_PROPERTY_NAME_LENGTH);
        if(value.length() > MAX_PROPERTY_VALUE_LENGTH)
            throw new compilerexception(this, error.VALUE_TOO_LARGE,
                    value, "property", MAX_PROPERTY_VALUE_LENGTH);
	}
	
	/**
	 * Return the value formated for C
	 * @return
	 */
	public String getCValue() {
	    return value;
	}
    
    public void setCValue(String _value) {
        value = _value;
    }
	
	/**
	 * Return the value formated for binary dumping
	 * @return
	 */
	public String getBinValue() {
	    if(value.startsWith("\""))
		return value.substring(1, value.length() - 1); 
	    else
		return value;
	}
}
