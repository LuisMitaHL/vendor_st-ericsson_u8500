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
import tools.common.compiler.ccompiler;
import tools.common.compiler.compilerfactory;
import tools.common.targetfactory;

public class TypePrimitive extends Type {
	public int packedsize;
	public boolean signed;
	
	public TypePrimitive(File file, tools.idl.parser.Token token, String _name) throws compilerexception  {
		super(file, token);
		this.typeName = _name;

		if("t_bool".equals(_name)) {
			this.packedsize = 1; 
			this.signed = false;
		} else if("char".equals(_name)) {
            this.packedsize = 8;
            this.signed = false;
		} else if(_name.startsWith("t_uint")) {
		    this.packedsize = Integer.parseInt(_name.substring(6));
	        if(this.packedsize % 8 != 0)
	            throw new compilerexception(this, error.TYPE_UNKNOWN_UINT);
		    this.signed = false;
		} else if(_name.startsWith("t_sint")) {
            this.packedsize = Integer.parseInt(_name.substring(6));
            if(this.packedsize % 8 != 0)
                throw new compilerexception(this, error.TYPE_UNKNOWN_SINT);
            this.signed = true;
		} else if("t_uword".equals(_name)) {
		    this.packedsize = 32;
            this.signed = false;
		} else if("t_sword".equals(_name)) {
            this.packedsize = 32;
            this.signed = true;
		} else if("t_physical_address".equals(_name)) {
            this.packedsize = 32;
            this.signed = false;
		} else
		    throw new compilerexception(this, error.TYPE_UNKOWN);
		
		ccompiler cc = compilerfactory.getRawComponent();
		if(cc != null && cc.isAvailabletype(_name) == false) 
		    throw new compilerexception(this, error.TYPE_NOT_HANDLE_BY_TARGET,
		            _name, targetfactory.TARGET);
	}

	public String printType() {
		return typeName;
	}
}
