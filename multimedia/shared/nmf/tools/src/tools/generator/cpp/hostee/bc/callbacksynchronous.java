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
package tools.generator.cpp.hostee.bc;

import java.io.IOException;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.conf.ast.astprimitive;
import tools.conf.graph.binding.proxy;
import tools.idl.ast.*;

public class callbacksynchronous extends callbackbase implements proxy {
    
    public callbacksynchronous(astbasefile _declaration, 
            Interface _itf, 
            String _itfname, String _name, int _indextodeclare,
            boolean _optional) throws compilerexception, IOException {
        super(_declaration, _itf, _itfname, _optional, _name, _indextodeclare, new astprimitive(null, "cbs." + _itf.name));
    }

}
