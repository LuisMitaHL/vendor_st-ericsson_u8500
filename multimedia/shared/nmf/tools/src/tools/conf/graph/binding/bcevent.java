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
package tools.conf.graph.binding;

import java.io.IOException;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.conf.ast.astprimitive;
import tools.idl.ast.Interface;

public class  bcevent extends bc {
    protected Interface itf;
    protected int size = 0;

    public bcevent(astbasefile _declaration, 
            Interface _itf, 
            astprimitive _type,
            int _size,             
            SchedPriority _priority) throws compilerexception, IOException {
        super(_declaration, _priority, _type);
        itf = _itf;
        size = _size;
    }

    public bcevent(astbasefile _declaration, 
            Interface _itf, 
            astprimitive _type) throws compilerexception, IOException {
        this(_declaration, _itf, _type, 0, SchedPriority.NotSetted);
    }

    public bcevent(astbasefile _declaration, 
            Interface _itf, 
            String _name,
            astprimitive _type) throws compilerexception, IOException {
        super(_declaration, _name, SchedPriority.NotSetted, _type);
        itf = _itf;
    }
}
