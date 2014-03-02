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
/*
 * Generated ELF header component description
 */
package tools.conf.generator;

import tools.common.compilerexception;
import tools.common.targetfactory;
import tools.conf.graph.*;

public class wrapper extends visitor 
{
    tools.generator.api.WrapGeneratorInterface wrapper;
    
    public wrapper() throws compilerexception 
    {
        travelVirtual = false;
        wrapper = targetfactory.getWrapper();
    }

    protected void travelPrimitive(primitive primitive, Object data) throws compilerexception 
    {
        wrapper.generate(primitive);
    }
}
