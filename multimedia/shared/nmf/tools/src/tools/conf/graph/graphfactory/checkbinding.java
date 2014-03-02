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
package tools.conf.graph.graphfactory;

import tools.common.compilerexception;
import tools.common.error;
import tools.conf.ast.astprovide;
import tools.conf.ast.astrequire;
import tools.conf.graph.*;

public class checkbinding extends visitor {
    
    private void checkRequire(component component, astrequire require, String itfName) throws compilerexception {
        interfacereference itfref = component.lookupFcWithoutError(itfName);

        // This required interface is bound outside top level component; no check could be done
        if(itfref instanceof externalinterfacereference)
            return;
        
        // Check if not optional required interface bound
        if (itfref == null && !require.optional && !require.isIntrinsec) {
            throw new compilerexception(component.declaration, error.REQUIRED_NOT_BINDED, 
                    component.nameinowner, 
                    component.type.fullyname, 
                    require.name, 
                    require.type);
        } 
        
        // Check if static required interface bound to skeleton
        if(require.isStatic && (itfref != null) && (! itfref.server.type.singleton)) {
            throw new compilerexception(require, error.STATIC_REQUIRED_NOT_BIND_TO_SINGLETON);
        }
    }
    
    protected void travelCommon(component component, Object data) throws compilerexception {
        
        for (astrequire require : component.getRequires()) 
        {
            if(require.collection) {
                for(int i = 0; i < require.numberOfElement; i++)
                    checkRequire(component, require, require.name + "[" + i + "]");
            } else {
                checkRequire(component, require, require.name);
            }
        }
    }

    private void checkProvide(composite composite, astprovide provide, String itfName) throws compilerexception {
        // Check that a provide has really been connected to a sub-component
        if(composite.getFcInterface(itfName) == null) {
            throw new compilerexception(composite.type,
                    error.TEMPLATE_NOT_BINDED, composite.type.fullyname, provide.name, provide.type);
        }
    }
    
    protected void travelComposite(composite composite, Object data) throws compilerexception {
        super.travelComposite(composite, data);
        
        for(astprovide provide : composite.getProvides()) {
            if(provide.collection) {
                for(int i = 0; i < provide.numberOfElement; i++)
                    checkProvide(composite, provide, provide.name + "[" + i + "]");
            } else {
                checkProvide(composite, provide, provide.name);
            }
        }
        
        // Check that required really bound to sub component
        for (astrequire require : composite.getRequires()) 
        {
            for(int i = 0; i < require.numberOfElement; i++) {
                String itfname = (require.collection ? require.name + "[" + i + "]" : require.name);
                try {
                    composite.lookupFc(itfname);
                } catch(illegalbindingexception e) {
                    throw new compilerexception(require, error.COMPOSITE_REQUIRE_NOT_BINDED, itfname);
                }
            }
        }
    }
    
}
