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
import tools.common.util;
import tools.common.compiler.ccompiler;
import tools.common.compiler.compilerfactory;
import tools.common.targetfactory;

public class astattribute extends astbasefile {
    public String name;
    public String ctype;
    public String initvalue;
    
    final int MAX_ATTRIBUTE_NAME_LENGTH = 32;

    public astattribute(File _file, int line, int column, String _name, String _ctype, String _initvalue) throws compilerexception {
        super(_file, line, column);
        this.name = _name;
        this.ctype = _ctype;
        this.initvalue = _initvalue;

        if(name.length() > MAX_ATTRIBUTE_NAME_LENGTH)
            throw new compilerexception(this, error.IDENTIFIER_TOO_LARGE,
                    name, "attribute", MAX_ATTRIBUTE_NAME_LENGTH);
        
        if(ctype != null) {
            ccompiler cc = compilerfactory.getRawComponent();
            String ptype = ctype.replace("*", "");
            if(cc.isAvailabletype(ptype) == false) 
                throw new compilerexception(this, error.TYPE_NOT_HANDLE_BY_TARGET, 
                        ctype, targetfactory.TARGET);
        }
    }

    public astattribute(File _file, int line, int column, String _name) {
        super(_file, line, column);
        this.name = _name;
        this.ctype = null;
        this.initvalue = null;
    }
    
    public void checkValue(astbasefile decl, String value) throws compilerexception {
        checkValue(decl, ctype, value);
    }
    
    public static void checkValue(astbasefile decl, String ctype, String value) throws compilerexception {
        if(ctype == null)
            throw new compilerexception(decl, error.ASSIGN_UNTYPED_ATTRIBUTE);

        if(ctype.equals("char*") && ! value.startsWith("\""))
            throw new compilerexception(decl, error.CAST_NOTSTRING_TO_STRING, value);

        if(! ctype.equals("char*") && value.startsWith("\""))
            throw new compilerexception(decl, error.CAST_STRING_TO_NOTSTRING);

        if(ctype.equals("char") && ! value.startsWith("\'"))
            throw new compilerexception(decl, error.CAST_NOTCHAR_TO_CHAR);

        if(! ctype.equals("char") && value.startsWith("\'"))
            throw new compilerexception(decl, error.CAST_CHAR_TO_NOTCHAR);

        try {
            if(ctype.endsWith("*") && (ctype.startsWith("t_uint") || ctype.startsWith("t_sint"))) {
                if(value.startsWith("-"))
                    throw new compilerexception(decl, error.CAST_NEGATIVE_TO_UNSIGNED);

                long integersize = compilerfactory.getRawComponent().sizeOfWord();
                long intvalue = util.StringToDecimal(value);
                if(intvalue > (1L << integersize) - 1L)
                    throw new compilerexception(decl, error.INTEGER_TOO_BIG, (1L << integersize) - 1L, ctype);
            } else if(ctype.startsWith("t_uint")) {
                if(value.startsWith("-"))
                    throw new compilerexception(decl, error.CAST_NEGATIVE_TO_UNSIGNED);

                long integersize = Integer.parseInt(ctype.substring(6));
                long intvalue = util.StringToDecimal(value);
                if(intvalue > (1L << integersize) - 1L)
                    throw new compilerexception(decl, error.INTEGER_TOO_BIG, (1L << integersize) - 1L, ctype);
            } else if(ctype.startsWith("t_sint")) {
                long integersize = Integer.parseInt(ctype.substring(6));
                long intvalue = util.StringToDecimal(value);
                if(intvalue > (1L << (integersize - 1L)) - 1L)
                    throw new compilerexception(decl, error.INTEGER_TOO_BIG, (1L << (integersize - 1L)) - 1L, ctype);
                if(intvalue < -(1L << (integersize - 1L)))
                    throw new compilerexception(decl, error.INTEGER_TOO_BIG, -(1L << (integersize - 1L)), ctype);
            }
        } catch (NumberFormatException e) {
            throw new compilerexception(decl, error.BAD_INTEGER, value);
        }
    }
}
