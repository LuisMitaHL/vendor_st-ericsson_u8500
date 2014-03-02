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

import tools.common.astbasefile;

public abstract class Type extends astbasefile {
    public String typeName;    
    
    public Type typedefinition;

    public Type(File _file, tools.idl.parser.Token token) {
        super(_file, token.beginLine, token.beginColumn);
    }
    public Type(File _file, int line, int column) {
        super(_file, line, column);
    }

    /**
     * Declare method parameter 
     */	
    public String declareParameter(String variable) {
        return printType() + " " + variable;
    }
    
    /**
     * Read a method parameter
     */
    public String readParameter(String variable) {
        return variable;
    }

    
    /**
     * Declare type used inside another type
     * @param fieldName
     * @return
     */
    public String printAsStructField(String fieldName) {
        return printType() + " " + fieldName;
    }
 
    //public abstract String printDeclaration(String variable);
    public abstract String printType();

    /**
     * Declare local variable inside a method or structure.
     */
    public String declareLocalVariable(String variable) {
        return printType() + " " + variable;
    }

    /**
     * Pass a variable previously declared as declareLocalVariable as a method parameter.
     */
    public String passLocalVariableAsParameter(String variable) {
        return variable;
    }

}
