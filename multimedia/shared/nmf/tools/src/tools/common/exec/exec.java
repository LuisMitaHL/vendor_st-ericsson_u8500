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

import java.util.ArrayList;
import java.util.concurrent.ExecutorCompletionService;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import tools.common.compilerexception;
import tools.common.configuration;
import tools.common.exec.pipecolorlet.ErrorType;
import tools.common.exec.Args;

public class exec 
{
    public static void runArgc(Args args, ErrorType type) throws compilerexception 
    {
        // Start process
        task waiter = new task(args, null, type);
    
        waiter.call();
    }
	
    private static ArrayList<task> waitingTasks = new ArrayList<task>();
    private static ExecutorService pool = Executors.newFixedThreadPool(configuration.numberOfJobs);
    private static ExecutorCompletionService<Void> runner = new ExecutorCompletionService<Void>(pool);

    /*
     * Start task with all predecessor finished
     */
    private static void scheduleTasks() 
    {
        for (int i = 0; i < waitingTasks.size(); ) 
        {
            task waiter = waitingTasks.get(i);

            if(waiter.checkPredecessor()) 
            {
                waitingTasks.remove(i);

                runner.submit(waiter);
            } else
                i++;
        }
    }
    
    public static task startArgc(Args args, ErrorType type) throws compilerexception 
    {
        return startArgc(args, type, (ArrayList<task>)null);
    }

    public static task startArgc(Args args, ErrorType type, task predecessor) throws compilerexception
    {
        ArrayList<task> predecessors = new ArrayList<task>();
        predecessors.add(predecessor);

        return startArgc(args, type, predecessors);
    }
    
    public static task startArgc(Args args, ErrorType type, ArrayList<task> predecessors) throws compilerexception 
    {   
        if(configuration.numberOfJobs == 1)
        {
            runArgc(args, type);
            
            return null;
        } 
        else
        {
            // Start process
            task task = new task(args, predecessors, type);

            waitingTasks.add(task);

            scheduleTasks();

            return task;
        }
    }
    
    public static void waitProcesses() throws InterruptedException 
    {

        while(waitingTasks.size() > 0) 
        {
            // Wait first terminate runner
            runner.take();
            
            // Schedule another one.
            scheduleTasks();
        }

        // shutdown worker threads
        pool.shutdown(); 
        //pool.awaitTermination(5, TimeUnit.MINUTES);
    }	
}