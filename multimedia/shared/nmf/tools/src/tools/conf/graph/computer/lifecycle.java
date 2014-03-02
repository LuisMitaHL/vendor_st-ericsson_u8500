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
package tools.conf.graph.computer;

import tools.conf.graph.primitive;
import tools.conf.graph.visitor;
import tools.generator.api.state.constructor;

public class lifecycle extends visitor {
    public boolean needConstuctor;
    public boolean needStarter;
    public boolean needStopper;
    public boolean needDestructor;

    public lifecycle() { 
        needConstuctor = false;
        needStarter = false;
        needStopper = false;
        needDestructor = false;
    }
    
    protected void travelPrimitive(primitive primitive, Object data) {
        // Check is _sb_bases_section required by generated code and if constructor is required
        if(primitive instanceof constructor) {
            needConstuctor = true;
            needDestructor = true;
        }
        if (primitive.getProvide("constructor") != null)
            needConstuctor = true;
        if (primitive.getProvide("starter") != null)
            needStarter = true;
        if (primitive.getProvide("stopper") != null)
            needStopper = true;
        if(primitive.getProvide("destructor") != null)
            needDestructor = true;
    }
}
