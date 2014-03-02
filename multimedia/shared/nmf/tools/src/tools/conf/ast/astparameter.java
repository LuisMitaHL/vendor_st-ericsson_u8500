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
package tools.conf.ast;

import java.io.File;

import tools.common.astbasefile;;

public class astparameter extends astbasefile {
    public String type;
    public String name;
    public String initvalue;

    public astparameter(File _file, int line, int column, String _type, String _name) {
        super(_file, line, column);
        type = _type;
        name = _name;
    }
    public astparameter(File _file, int line, int column, String _type, String _name, String _initvalue) {
        this(_file, line, column, _type, _name);
        initvalue = _initvalue;
    }
}
