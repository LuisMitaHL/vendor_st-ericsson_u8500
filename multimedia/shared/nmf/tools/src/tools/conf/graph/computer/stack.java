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

import tools.common.util;
import tools.common.targetfactory;
import tools.conf.ast.astoption.OptionNames;
import tools.conf.graph.component;
import tools.conf.graph.visitor;

public class stack extends visitor {
    public int minstack = 0;
    
    protected void travelCommon(component component, Object data) {   
        // Compute stack
        String msstr = component.getOption(targetfactory.TARGET, OptionNames.stack);
        int _minstack = (msstr == null) ? 0 : (int)util.StringToDecimal(msstr);

        // Compute minimum required stack
        if(minstack < _minstack)
            minstack = _minstack;
    }
}
