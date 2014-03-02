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
package tools.generator.api;

import java.io.IOException;

import tools.common.astbasefile;
import tools.common.compilerexception;
import tools.conf.ast.astprimitive;
import tools.conf.graph.binding.bc;
import tools.idl.ast.Interface;

public interface SkeletonGeneratorInterface 
{
    public void setItf(Interface itf) throws compilerexception;
    
    public bc getPrimitive(
            astbasefile _declaration, 
            String _name) throws compilerexception;
    
    public astprimitive registerAst() throws compilerexception, IOException;
}
