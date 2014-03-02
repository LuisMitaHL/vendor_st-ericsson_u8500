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

import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.concurrent.Callable;

import tools.common.configuration;
import tools.common.configuration.Debug;
import tools.common.exec.pipecolorlet.ErrorType;

public class task implements Callable<Void>  {
    ErrorType type;
    
    Args arg;

    enum tstate {WAIT, RUN, FINISH};
    tstate state;

    ArrayList<task> predecessors;

    protected String printArgc() 
    {
        String msg = "";
        if(configuration.verbose)
        {
            for(Iterator<String> i = arg.args.iterator(); i.hasNext(); )
                msg += i.next() + " ";
        } 
        else
        {
            msg = String.format(" %-10s%s", arg.name, arg.output);
        }
        return msg;
        
    }

    public task(Args _arg, ArrayList<task> _predecessors, ErrorType _type) 
    {
        arg = _arg;
        predecessors = _predecessors;
        type = _type;
        state = tstate.WAIT;
    }

    public boolean checkPredecessor() 
    {
        if(predecessors != null) 
        {
            while(! predecessors.isEmpty()) 
            {
                task predecessor = predecessors.get(0); 

                if(predecessor.state != tstate.FINISH) 
                    return false;

                // In order to not travel many time same element, remove it when finish
                predecessors.remove(0);
            }
            
            predecessors = null;
        }
        return true;
    }

    public Void call() 
    {
        configuration.debug(Debug.always, printArgc());

        try {
            // Start process
            ProcessBuilder pb = new ProcessBuilder(arg.args);
            pb.redirectErrorStream(true);
            
            Process process = pb.start();
            state = tstate.RUN;

            new pipecolorlet(type).dispatch(process);
            
            // Get return value (use waitFor since we assume here that process have finish)
            if(process.waitFor() != 0) 
                System.exit(255);
            
            state = tstate.FINISH;
            process.destroy();
        } 
        catch (InterruptedException e) 
        {
            configuration.error(e.toString());
            System.exit(255);
        } 
        catch (IOException e) 
        {
            configuration.error(e.toString());
            System.exit(255);
        }
        
        return null;
    }
}
    
