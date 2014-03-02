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
package tools.common.exec;

import java.io.*;

import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.error;
import tools.common.console.console;
import tools.common.console.factory;
import tools.common.console.foregroundcolor;

public class pipecolorlet extends pipelet {
    static console cons = factory.getConsole(System.out);

    public enum ErrorType {AlaGnu, AlaArm};
    private enum t_state {None, Warning, Error};
    
    ErrorType type;

    protected pipecolorlet(ErrorType _type) {
        type = _type;
    }
    
    protected void rawDispatch(BufferedReader br) throws IOException, compilerexception
    {
        t_state state = t_state.None;
        String line;

        while ( (line = br.readLine()) != null) 
        {
            if(type == ErrorType.AlaArm) 
            {
                // RVCT compiler
                if(line.indexOf(": Error:") != -1)
                    state = t_state.Error;
                else if(line.indexOf(": Warning:") != -1) 
                    state = t_state.Warning;
            } else 
            {
                // Temporary PATCH for removing check
                if(line.contains(".map: mem") && line.contains(": checked"))
                    continue;

                // GCC compiler
                if((line.indexOf("Warning:") != -1) || (line.indexOf("warning:") != -1)) {
                    state = t_state.Warning;
                } else if(line.indexOf("In function") != -1) {
                    String nextline = br.readLine();
                    if((nextline.indexOf("Warning:") != -1) || (nextline.indexOf("warning:") != -1)) 
                        state = t_state.Warning;
                    else 
                        state = t_state.Error;
                    line = line + configuration.eol + nextline;
                } else {
                    state = t_state.Error;
                }
            }
            
            synchronized (System.out) 
            {
                cons.setForegroundColor((state ==  t_state.Error) ? foregroundcolor.LIGHT_RED : foregroundcolor.LIGHT_GREEN);
                System.out.println(line);
                cons.resetColors();
            }

            if(state == t_state.Warning && configuration.stoponwarning)
                throw new compilerexception(error.ERROR_ON_WARNING);
        }
    }
}
