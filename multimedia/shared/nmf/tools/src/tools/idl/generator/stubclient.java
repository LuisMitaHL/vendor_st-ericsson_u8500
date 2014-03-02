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
import tools.common.targetfactory;

public abstract class stubclient extends stubbase
{
	protected int writeData(Type type, String name, PrintStream out, String nb16, String dec, int arraylevel) 
	{
		int computed = 0;
		if(type.typedefinition != null) 
		{
            computed = writeData(type.typedefinition, name, out, nb16, dec, arraylevel);
        }
		else if (type instanceof TypePrimitive)
		{
			TypePrimitive primitive = (TypePrimitive) type;
			out.println(dec + "_xyuv_data[" + nb16 + "] = (t_uint16)" + name + ";");
			computed = 1;
			if (primitive.packedsize > 16) {
				out.println(dec + "_xyuv_data[" + nb16 + "+1] = (t_uint16)(" + name + " >> 16);");
				computed++;
			}
			if (primitive.packedsize > 32){
				out.println(dec + "_xyuv_data[" + nb16 + "+2] = (t_uint16)(" + name + " >> 32);");
				computed++;
			}
			if (primitive.packedsize > 48) {
				out.println(dec + "_xyuv_data[" + nb16 + "+3] = (t_uint16)(" + name + " >> 48);");
				computed++;
			}
		} 
		else if (type instanceof TypePointer)
		{
			out.println(dec + "_xyuv_data[" + nb16 + "] = (t_uint16)((unsigned int)" + name + " & 0xFFFFU);");
			out.println(dec  +"_xyuv_data[" + nb16 + "+1] = (t_uint16)((unsigned int)" + name + " >> 16);");
			computed = 2;
		} 
		else if (type instanceof typereference) 
		{
			typereference tr = (typereference) type;
	        switch (targetfactory.style) {
	        case C:
	            computed = writeData(tr.type, "(*" +name + ")", out, nb16, dec, arraylevel);
	            break;
	        case CPP:
	            computed = writeData(tr.type, name, out, nb16, dec, arraylevel);
	            break;
	        default:
	            assert false: "Compiler target not supported: " + targetfactory.style;
	        }
		} 
		else if (type instanceof typearray) 
		{
			typearray array = (typearray) type;
			out.println(dec + "for(___i" + arraylevel + " = ___j" + arraylevel + " = 0; ___i" + arraylevel + " < " + array.sizeInteger + "; ___i" + arraylevel + "++) {");
			int elemcomputed  = writeData(array.type, name + "[___i" + arraylevel + "]", out, nb16 + "+___j" + arraylevel + "", dec + "  ", arraylevel+1);
			out.println(dec + "  ___j" + arraylevel + " += " + elemcomputed + ";");
			computed = elemcomputed * array.sizeInteger;
			out.println(dec + "}");
		} 
		else if (type instanceof typedef) 
		{
			typedef td = (typedef)type;
			computed = writeData(td.type, name, out, nb16, dec, arraylevel);
		}
		else if (type instanceof structure) 
		{
			structure ts = (structure)type;
			computed = 0;
            for(final structentry se : ts.members) 
            {
				computed += writeData(se.type, name + "." + se.name, out, nb16 + "+" + Integer.toString(computed), dec, arraylevel);
			}
		}
		else if (type instanceof enumeration)  
		{
			// Handle as uint32
            out.println(dec + "_xyuv_data[" + nb16 + "] = (t_uint16)((unsigned int)" + name + " & 0xFFFFU);");
            out.println(dec  +"_xyuv_data[" + nb16 + "+1] = (t_uint16)((unsigned int)" + name + " >> 16);");
            computed = 2;
		}
		else if (type instanceof typeconst) 
		{
		    typeconst cst = (typeconst)type;
		    computed = writeData(cst.type, name, out, nb16, dec, arraylevel);
		} 
		return computed;
	}

}
