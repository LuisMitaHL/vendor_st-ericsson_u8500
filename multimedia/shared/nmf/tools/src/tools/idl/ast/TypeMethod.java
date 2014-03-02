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
import tools.common.error;

public class TypeMethod extends Type {
	public Type type;
	public ArrayList<Parameter> parameters;
	
	public TypeMethod(File file, tools.idl.parser.Token token, Type _type) {
		super(file, token);
		this.type = _type;
		this.parameters = new ArrayList<Parameter>();
	}
	
	public void addParameter(Parameter newparam) throws compilerexception {
	    for(final Parameter param : parameters) 
	    {
	        if(param.name.equals(newparam.name))
	            throw new compilerexception(error.IDT_PARAM_ALREADY_DEFINED, param.name);
	    }
	    parameters.add(newparam);
	}
	
	protected void printParameters(boolean thisAsParameter, StringBuffer builded, boolean collectionIndexAsParameter) {
		boolean paramGived = false;
        builded.append("(");
		if(thisAsParameter) { 
			builded.append("void* THIS");
            paramGived = true;
		}
        for(int k = 0; k < parameters.size(); k++) {
            if(paramGived) builded.append(", ");
            Parameter fp = parameters.get(k);
            builded.append(fp.type.declareParameter(fp.name));
            paramGived = true;
        }
        if(collectionIndexAsParameter) {
            if(paramGived) builded.append(", ");
            builded.append("t_uint8 collectionIndex");
        }
        // No one parameter has been gives, put void to be C compliant
        if(paramGived == false)
            builded.append("void");
		builded.append(")");
	}

	public String declareParameter(String variable) {
		StringBuffer builded = new StringBuffer();
		builded.append(type.printType());
		builded.append(" (*");
		builded.append(variable);
		builded.append(")");
		printParameters(false, builded, false);
		return builded.toString();
	}
	public String declareLocalVariable(String variable) {
		return declareParameter(variable);
	}
	public String printType() {
		StringBuffer builded = new StringBuffer();
		builded.append(type.printType());
		builded.append(" (*)");
		printParameters(false, builded, false);
		return builded.toString();
	}
}
