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

public class pipelet 
{
    protected void rawDispatch(BufferedReader br) throws IOException, compilerexception 
    {
        String line;
        while ( (line = br.readLine()) != null) 
        {
            synchronized (System.out) 
            {
                System.out.println(line);
            }   
        }
    }
    
    protected void dispatch(Process process) 
    {
        InputStream is = process.getInputStream();
        InputStreamReader isr = new InputStreamReader(is);
        BufferedReader br = new BufferedReader(isr);

        try 
        {
            rawDispatch(br);
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
        } 
        catch (compilerexception e) 
        {
            configuration.error(e);
            System.exit(e.exitNumber);
        } 

        // Close as soon as possible stream in order to not consume to much resources
        try { br.close(); } catch (IOException e) {} ;
        try { isr.close(); } catch (IOException e) {} ;
        try { is.close(); } catch (IOException e) {} ;
        try { process.getOutputStream().close(); } catch (IOException e) {} ;
        try { process.getInputStream().close(); } catch (IOException e) {} ;
        try { process.getErrorStream().close(); } catch (IOException e) {} ;
    }

}
