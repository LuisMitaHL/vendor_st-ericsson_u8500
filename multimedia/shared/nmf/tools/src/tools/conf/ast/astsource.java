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

import tools.common.astbasefile;
import java.io.File;

public class astsource extends astbasefile {
    public enum SourceType {
        CSource, CPPCource
    };
    
    public File file;
    public SourceType sourceType;
    
    public String CFLAGS;
    
    public astsource(File _file, tools.conf.parser.Token token, File _srcfile) {
        super(_file, token.beginLine, token.beginColumn);
        setSource(_srcfile);        
    }
    public astsource(File _srcfile) {
        super(null, 0, 0);
        setSource(_srcfile);
    }
    
    private void setSource(File _file) {
        this.file = _file;
        if(file.getAbsolutePath().endsWith(".cpp"))
            sourceType = SourceType.CPPCource;
        else
            sourceType = SourceType.CSource;
    }
}
