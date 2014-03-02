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

public class Method extends TypeMethod {
	public String name;
	public String qualifier;
	
    public Method(File file, tools.idl.parser.Token token, String _name, Type _type) {
        this(file, token, _name, _type, null);
    }
	public Method(File file, tools.idl.parser.Token token, String _name, Type _type, String _qualifier) {
		super(file, token, _type);
		this.name = _name;
		this.qualifier = _qualifier;
	}

	public String printMethod(String component, String methprefix, boolean thisAsParameter, boolean collectionIndexAsParameter, boolean errorForAsynchronous) {
		StringBuffer builded = new StringBuffer();
		if(errorForAsynchronous)
		    builded.append("t_cm_error");
		else
		    builded.append(type.printType());
		builded.append(" ");
		builded.append(component + "_" + methprefix + name);
		printParameters(thisAsParameter, builded, collectionIndexAsParameter);
		return builded.toString();
	}
	
	public String printReference(String variable, boolean thisAsParameter, String typeOverload) {
		StringBuffer builded = new StringBuffer();
		if(typeOverload != null)
            builded.append(typeOverload);
        else
            builded.append(type.printType());
		builded.append(" (*");
		builded.append(variable);
		builded.append(")");
		printParameters(thisAsParameter, builded, false);
		return builded.toString();
	}

	public String printMethodMETH(boolean errorForAsynchronous) {
	    StringBuffer builded = new StringBuffer();
        if(errorForAsynchronous)
            builded.append("t_cm_error");
        else
            builded.append(type.printType());
	    builded.append(" ");
	    builded.append("METH(" + name + ")");
	    printParameters(false, builded, false);
	    return builded.toString();
	}
	public String printMethodMETH() {
	    return printMethodMETH(false);
	}
	public String printMethodLibrary() {
	    StringBuffer builded = new StringBuffer();
	    // TO DO mettre isi
	    if(qualifier != null) {
	        builded.append(qualifier);
	        builded.append(" ");
	    }
        builded.append(type.printType());
        builded.append(" ");
        builded.append(name);
        printParameters(false, builded, false);
        return builded.toString();
    }
    public String printMethodCpp(String methprefix, boolean collectionIndexAsParameter) {
        StringBuffer builded = new StringBuffer();
        builded.append(type.printType());
        builded.append(" ");
        builded.append(methprefix + name);
        printParameters(false, builded, collectionIndexAsParameter);
        return builded.toString();
    }

}
