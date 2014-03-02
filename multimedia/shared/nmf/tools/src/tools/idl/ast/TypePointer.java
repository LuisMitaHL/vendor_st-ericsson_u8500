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

public class TypePointer extends Type{
    public Type type;
    public String memQualifier;

    public TypePointer(File file, tools.idl.parser.Token token, Type _type, String _memQualifier) {
        super(file, token);
        this.type = _type;
        this.memQualifier = _memQualifier;
    }

    public String printType() {
        if(memQualifier == null)
            return type.printType() + "*";
        else
            return type.printType() + " " + memQualifier + "*";
    }
}