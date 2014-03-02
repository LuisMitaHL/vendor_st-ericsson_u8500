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

import tools.idl.ast.*;

public abstract class stubserver extends stubbase 
{
	protected int create32(PrintStream out, String typein, String typeout, int packedsize, boolean signed, String nb16) 
	{
		if(typein != typeout) 
		{
			out.print("(" + typeout + ")");
		}
		int computed = 1;
		out.print("((" + typein + ")_xyuv_data[" + nb16 + "]");
		if(packedsize > 16) 
		{
			out.print(" | ((" + typein + ")_xyuv_data[" + nb16 + "+1] << 16)");
			computed++;
		}
		if(packedsize > 32) 
		{ 
			out.print(" | ((" + typein + ")_xyuv_data[" + nb16 + "+2] << 32)");
			computed++;
		}
		if(packedsize > 48) 
		{
			out.print(" | ((" + typein + ")_xyuv_data[" + nb16 + "+3] << 48)");
			computed++;
		}
		out.print(")");
		return computed;
	}
	
	protected int readData(Type type, String name, PrintStream out, String nb16, String dec, int arraylevel) 
	{
	    int computed = 0;
	    if(type.typedefinition != null) 
	    {
            computed = readData(type.typedefinition, name, out, nb16, dec, arraylevel);
        } 
	    else if (type instanceof TypePrimitive) 
	    {
	        TypePrimitive primitive = (TypePrimitive)type;
	        String typestr = type.printType();
	        out.print(dec + name + " = ");
	        computed = create32(out, typestr, typestr, primitive.packedsize, primitive.signed, nb16);
	        out.println(";");
	    } 
	    else if (type instanceof TypePointer) 
	    {
	        out.print(dec + name + " = ");
	        computed = create32(out, "t_uint16", type.printType(), 32, false, nb16);
	        out.println(";");
	    } 
	    else if (type instanceof typereference) 
	    {
	        typereference tr = (typereference) type;
	        computed = readData(tr.type, name, out, nb16, dec, arraylevel);
	    } 
	    else if (type instanceof typearray) 
	    {
	        typearray array = (typearray) type;
	        out.println(dec + "for(___i" + arraylevel + " = ___j" + arraylevel + " = 0; " +
	                "___i" + arraylevel + " < " + array.sizeInteger + "; " + "" +
	                "___i" + arraylevel + "++) {");
	        int elemcomputed  = readData(array.type, name + "[___i" + arraylevel + "]", out, nb16 + "+___j" + arraylevel + "", dec + "  ", arraylevel + 1);
	        out.println(dec + "  ___j" + arraylevel + " += " + elemcomputed + ";");
	        computed = elemcomputed * array.sizeInteger;
	        out.println(dec + "}");
	    } 
	    else if (type instanceof typedef)  
	    {
	        typedef td = (typedef)type;
	        if(td.type instanceof enumeration) 
	        {
	            // This is a special patch that allow to convert to a typedef enum that has no name
	            // itself (e.g.  typedef enum {...} t_e)
	            out.print(dec + name + " = ");
	            computed = create32(out, "t_uint16", td.printType(), 32, false, nb16);
	            out.println(";");
	        } 
	        else
	            computed = readData(td.type, name, out, nb16, dec, arraylevel);
	    } 
	    else if (type instanceof structure)  
	    {
	        structure ts = (structure)type;
	        for(structentry se : ts.members) 
	        {
	            computed += readData(se.type, name + "." + se.name, out, nb16 + "+" + Integer.toString(computed), dec, arraylevel);
	        }
	    } 
	    else if (type instanceof enumeration)  
	    {
	        out.print(dec + name + " = ");
	        computed = create32(out, "t_uint16", type.printType(), 32, false, nb16);
	        out.println(";");
        }
	    else if (type instanceof typeconst)  
	    {
            typeconst cst = (typeconst)type;
            computed = readData(cst.type, name, out, nb16, dec, arraylevel);
	    }
	    return computed;
	}
}
