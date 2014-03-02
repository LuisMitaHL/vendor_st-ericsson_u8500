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

import tools.common.targetfactory;

public class typereference extends Type{
	public Type type;
	
	public typereference(File file, tools.idl.parser.Token token, Type _type) {
		super(file, token);
		this.type = _type;
	}

	public String declareParameter(String variable) {
	    switch (targetfactory.style) {
	    case C:
	        return type.printType() + "* " + variable;
	    case CPP:
            return type.printType() + "& " + variable;
	    default:
	        assert(false) : "Unexpected language style";
	        return null;
	    }
	}
	
    public String readParameter(String variable) {
        switch (targetfactory.style) {
        case C:
            return "(*" + variable + ")";
        case CPP:
            return variable;
        default:
            assert(false) : "Unexpected language style";
            return null;
        }
    }

	public String declareLocalVariable(String variable) {
        return type.printType() + " " + variable;
	}

    public String passLocalVariableAsParameter(String variable) {
        switch (targetfactory.style) {
        case C:
            return "&" + variable;
        case CPP:
            return variable;
        default:
            assert(false) : "Unexpected language style";
            return null;
        }
    }

	public String printType() {
	    switch (targetfactory.style) {
	    case C:
	        return type.printType() + "*";
	    case CPP:
            return type.printType() + "&";
	    default:
	        assert(false) : "Unexpected language style";
	        return null;
	    }
	}
}
