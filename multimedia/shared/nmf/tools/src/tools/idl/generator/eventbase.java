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
package tools.idl.generator;

import java.io.PrintStream;

import tools.idl.ast.Interface;
import tools.idl.ast.Type;
import tools.idl.ast.typearray;
import tools.idl.ast.typeconst;
import tools.idl.ast.typedef;
import tools.idl.ast.typereference;

public class eventbase {
    protected Interface itf;
    
    public void setItf(Interface _itf) {
        this.itf = _itf;
    }
    
    public int checkData(Type type) {
        if(type.typedefinition != null) {
            return checkData(type.typedefinition);
        } else if (type instanceof typereference) {
            typereference tr = (typereference) type;
            return checkData(tr.type);
        } else if (type instanceof typedef)  {
            typedef td = (typedef)type;
            return checkData(td.type);
        } else if (type instanceof typearray)  {
            typearray array = (typearray) type;
            return 1 + checkData(array.type);
        } else if (type instanceof typeconst)  {
            typeconst cst = (typeconst)type;
            return checkData(cst.type);
        } 
        return 0;
    }

    protected void readData(Type type, String name, String struct, PrintStream out, String dec, int arraylevel) {
        if(type.typedefinition != null) {
            readData(type.typedefinition, name, struct, out, dec, arraylevel);
        } else if (type instanceof typereference) {
            typereference tr = (typereference) type;
            readData(tr.type, name, struct, out, dec, arraylevel);
        } else if (type instanceof typearray) {
            typearray array = (typearray) type;
            out.println(dec + "for(___i" + arraylevel + " = 0; " +
                    "___i" + arraylevel + " < " + array.sizeInteger + "; " + "" +
                    "___i" + arraylevel + "++) {");
            readData(array.type, name + "[___i" + arraylevel + "]", struct, out, dec + "  ", arraylevel + 1);
            out.println(dec + "}");
        } else if (type instanceof typedef)  {
            typedef td = (typedef)type;
            readData(td.type, name, struct, out, dec, arraylevel);
        } else if (type instanceof typeconst)  {
            typeconst cst = (typeconst)type;
            readData(cst.type, name, struct, out, dec, arraylevel);
        } else {
            out.println(dec + name + " = " + struct + name + ";");
        }
    }

    protected void writeData(Type type, String leftname, String rightname, String struct, PrintStream out, String dec, int arraylevel) {
        if(type.typedefinition != null) {
            writeData(type.typedefinition, leftname, rightname, struct, out, dec, arraylevel);
        } else if (type instanceof typereference) {
            typereference tr = (typereference) type;
            writeData(tr.type, leftname, tr.readParameter(rightname), struct, out, dec, arraylevel);
        } else if (type instanceof typearray) {
            typearray array = (typearray) type;
            out.println(dec + "for(___i" + arraylevel + " = 0; " +
                    "___i" + arraylevel + " < " + array.sizeInteger + "; " + "" +
                    "___i" + arraylevel + "++) {");
            writeData(array.type, leftname + "[___i" + arraylevel + "]", rightname + "[___i" + arraylevel + "]", struct, out, dec + "  ", arraylevel + 1);
            out.println(dec + "}");
        } else if (type instanceof typedef)  {
            typedef td = (typedef)type;
            writeData(td.type, leftname, rightname, struct, out, dec, arraylevel);
        } else if (type instanceof typeconst)  {
            typeconst cst = (typeconst)type;
            writeData(cst.type, leftname, rightname, struct, out, dec, arraylevel);
        } else {
            out.println(dec + struct + leftname + " = " + rightname + ";");
        }
    }

}
