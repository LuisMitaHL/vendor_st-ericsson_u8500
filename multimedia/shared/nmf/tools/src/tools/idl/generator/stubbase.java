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

import tools.idl.ast.*;
import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.error;

public class stubbase 
{
    protected Interface itf;
    
    protected int needi[];
    protected int maxnb16;
    protected int methnb16[];

    protected int checkData(Type type, int meth, int arrayindex) throws compilerexception 
    {
        if(type.typedefinition !=null)
        {
            return checkData(type.typedefinition, meth, arrayindex);
        }
        else if (type instanceof TypePrimitive) 
        {
            TypePrimitive primitive = (TypePrimitive) type;
            if(primitive.packedsize <= 16)
                return 1;
            else if(primitive.packedsize <= 32)
                return 2;
            else {
                throw new compilerexception(type, error.COMS_TOO_BIG_TYPE);
            }
        }
        else if (type instanceof TypePointer) 
        {
            configuration.warning("Warning: pointer used in distributed communication!");
            return 2;
        } 
        else if (type instanceof typereference) 
        {
            typereference tr = (typereference) type;
            return checkData(tr.type, meth, arrayindex);
        }
        else if (type instanceof structure) 
        {
            int nb16 = 0;
            structure ts = (structure)type;
            for(structentry se : ts.members) {
                nb16 += checkData(se.type, meth, arrayindex);
            }
            return nb16;
        } 
        else if (type instanceof enumeration)
        {
            // Handle enum as 32bits
            return 2;
        }
        else if (type instanceof typedef) 
        {
            typedef td = (typedef)type;
            return checkData(td.type, meth, arrayindex);
        } 
        else if (type instanceof typearray) 
        {
            typearray array = (typearray) type;
            needi[meth] = Math.max(needi[meth], arrayindex + 1);
            return checkData(array.type, meth, arrayindex+1) * array.sizeInteger;
        } 
        else if (type instanceof typeconst) 
        {
            typeconst cst = (typeconst)type;
            return checkData(cst.type, meth, arrayindex);
        } 
        else 
        {
            throw new compilerexception(type, error.COMS_UNKNOWN_TYPE, type);
        }
    }

    public void setItf(Interface _itf) throws compilerexception 
    {
        this.itf = _itf;

        needi = new int[itf.methods.size()];
        methnb16 = new int[itf.methods.size()];
        maxnb16 = 0;
        
        for(int j = 0; j < itf.methods.size(); j++) 
        {
            Method md = (Method)itf.methods.get(j);

            // Check return type
            if(!( md.type instanceof TypeVoid))
                throw new compilerexception(md.type, error.COMS_RETURN_VALUE_NOT_HANDLED);

            // Compute maximum data for this method and check if an index is require for array copying!
            needi[j] = 0;
            methnb16[j] = 0;
            for(int k = 0; k < md.parameters.size(); k++)
            {
                Parameter fp = (Parameter)md.parameters.get(k);
                methnb16[j] += checkData(fp.type, j, 0);
            }
            
            // Compute maximum data for each methods
            if(methnb16[j] > maxnb16)
                maxnb16 = methnb16[j];
        }
    }
}