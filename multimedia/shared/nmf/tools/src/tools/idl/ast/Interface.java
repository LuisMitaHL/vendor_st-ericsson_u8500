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
package tools.idl.ast;

import java.util.ArrayList;
import java.io.File;

import tools.common.util;

public class Interface extends definedtype{
	  public ArrayList<Method> methods;
	  
	  public String name, name_, nameslash;
	  public String smallname;
	  public boolean unmanaged = false;
	  
	  public void setName(String _name) {
          name = _name;
          name_ = _name.replace('.', '_');
          nameslash = _name.replace('.', '/');
          
          smallname = util.getLastName(name);
	  }
	  
	  public Interface(File _file, String _name) {
	      super(_file);
		  this.methods = new ArrayList<Method>();
		  setName(_name);
	  }
	  
	  /*
	   * This method return the directory relative to the root sbources directory
	   * where the interface reside (in fact, the package).
	   */
	  public String getParentDirectory() {
		  if(name.lastIndexOf('.') == -1)
			  return "";
		  else
			  return name.substring(0, name.lastIndexOf('.')).replace('.', File.separatorChar) + File.separator;
	  }
	  
	  /*
	   * True if a no method of the interface return value
	   */
	  public boolean isAsynchronous() {
		  for(int i = 0; i < methods.size(); i++ ) {
			Method md = methods.get(i);
			if(! (md.type instanceof TypeVoid))
				return false;
		  }
		  return true;
	  }
}
