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

import tools.idl.parser.Token;

public class typeprimitiveunmanaged extends Type {
	public typeprimitiveunmanaged(File file, 
	        Token _name1, Token _name2, Token _name3, Token _name4) {
	    super(file, _name1 != null ? _name1 : _name2);
	    typeName = null;
	    if(_name1 != null)
	        typeName = (typeName != null ? typeName + " " : "") +_name1.image;
	    if(_name2 != null)
	        typeName = (typeName != null ? typeName + " " : "") + _name2.image;
	    if(_name3 != null)
	        typeName = (typeName != null ? typeName + " " : "") + _name3.image;
	    if(_name4 != null)
	        typeName = (typeName != null ? typeName + " " : "") + _name4.image;
	}

	public String printType() {
		return typeName;
	}
}
