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
package tools.conf.graph;

import java.util.ArrayList;

import tools.common.compilerexception;
import tools.common.configuration;
import tools.conf.graph.binding.proxy;

public abstract class visitor extends configuration {

    protected boolean travelVirtual = true;
    protected boolean reserveOrder = false;
    
    /**
     * Traval a component (called before travelPrimitive or travelComposite)
     * @param component component to travel
     * @param data
     * @throws compilerexception
     */
    protected void travelCommon(component component, Object data) throws compilerexception {
    }

    /**
     * Travel a primitive component
     * @param primitive component to travel
     * @param data
     * @throws compilerexception
     */
    protected void travelPrimitive(primitive primitive, Object data) throws compilerexception {
        travelRawPrimitive(primitive, data);
    }

    protected void travelRawPrimitive(primitiveraw primitive, Object data) throws compilerexception {
    }

    /**
     * Travel a composite component
     * @param composite component to travel
     * @param data
     * @throws compilerexception
     */
    protected void travelComposite(composite composite, Object data) throws compilerexception {
        if(! reserveOrder)
        {
            for(final component sc : composite.getFcSubComponents()) {
                if(!(sc instanceof proxy) || travelVirtual)
                    travelComponent(sc, data);
            }
            
            for(final primitiveraw sc : composite.getFcControllerComponents()) {
                if(!(sc instanceof proxy) || travelVirtual)
                    travelComponent(sc, data);
            }
        } 
        else 
        {
            ArrayList<component> list = new ArrayList<component>();
            for(final component sc : composite.getFcSubComponents()) {
                if(!(sc instanceof proxy) || travelVirtual)
                    list.add(0, sc);
            }

            for(final primitiveraw sc : composite.getFcControllerComponents()) {
                if(!(sc instanceof proxy) || travelVirtual)
                    list.add(0, sc);
            }

            for(final component sc : list) {
                travelComponent(sc, data);
            }
        }
    }

    /**
     * Travel a component
     * @param component component to travel
     * @param data
     * @throws compilerexception
     */
    protected void travelComponent(component component, Object data) throws compilerexception {
        //System.out.println(getClass().getSimpleName() + " gen " +
        //        component.getPathName() + " <" + component.type.fullyname + ">");

        // Only first declaration of singleton component is configured, forgot other
        if(component.isFirstInstance) {
            travelCommon(component, data);
            if(component instanceof composite)
                travelComposite((composite)component, data);				
            else if(component instanceof primitive)
                travelPrimitive((primitive)component, data);
            else 
                travelRawPrimitive((primitiveraw)component, data);
        }
    }

    /**
     * Browse a top level component
     * @param component component to browse
     * @throws compilerexception
     */
    public void browseComponent(component component) throws compilerexception {
        //System.out.println(getClass().getSimpleName() + " gen " +
        //		component.pathname + " <" + component.typebase.fullyname + ">");
        browseComponent(component, null);				
    }

    /**
     * Browse a top level component
     * @param component component to browse
     * @param topData
     * @throws compilerexception
     */
    public void browseComponent(component component, Object topData) throws compilerexception {
        //System.out.println(getClass().getSimpleName() + " gen " +
        //		component.pathname + " <" + component.typebase.fullyname + ">");
        travelComponent(component, topData);				
    }

}
