/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \brief   resource_sharer ARM-NMF component class implementation
*/
#include <resource_sharer.nmf>

resource_sharer::resource_sharer()
{
	for (int i=0 ; i<NB_CLIENT ; i++)
	{
		for (int j=0 ; j<REQUESTER_MAX ; j++)
		{
			G_RequestQueue[j][i] = 0;
		}
	}

	G_Lock = REQUESTER_MAX;     //resource is free
    G_Client = 0;
}

void METH(Free)(t_uint16 res, t_uint8 collectionIdx) {
    alert[!collectionIdx].freed();
}


void METH(Ping)(t_uint16 res,t_uint8 collectionIdx) {
    alert[collectionIdx].pong();
}


void METH(Lock)(t_uint16 requesterId,t_uint8 collectionIdx) {
    
	if(G_Lock==REQUESTER_MAX) // resource is free
	{
		//take it
		G_Lock=requesterId;
		
		// send granted
		alert[collectionIdx].accepted(requesterId, requesterId);
	}
	else // resource is not free
	{
        /* a higher priority requester wants to lock the resource */
        if (requesterId < G_Lock) {
            /* ask client that already has resource to release it */
            alert[G_Client].freed();
        }
        else {
		//store the request in request queue
		G_RequestQueue[requesterId][collectionIdx] ++;
		//send denied
		alert[collectionIdx].denied(requesterId);
        }
	}
	
}

void METH(Release)(t_uint16 requesterId,t_uint8 collectionIdx) {
    t_uint16 col;
	
	// resource is freed
	G_Lock=REQUESTER_MAX;
	
	alert[collectionIdx].released(requesterId);
	
	// implement the priority management

		for (col=0;col<NB_CLIENT;col++) 
		{
			if(G_RequestQueue[CAMERA_RAW][col] != 0) {
				//take it
				G_Lock=CAMERA_RAW;	
			    G_Client=col;
                 
				//decrement the request in request queue
				G_RequestQueue[CAMERA_RAW][col] --;
				
				// send granted
				alert[col].accepted(CAMERA_RAW, requesterId);	
				return	;			

			}

		}
		for (col=0;col<NB_CLIENT;col++) 
		{
			if(G_RequestQueue[ISPPROC_SINGLE_STILL][col] != 0) {
			//take it
			G_Lock=ISPPROC_SINGLE_STILL;	
			G_Client=col;
			
			//decrement the request in request queue
			G_RequestQueue[ISPPROC_SINGLE_STILL][col] --;
			
			// send granted
			alert[col].accepted(ISPPROC_SINGLE_STILL, requesterId);	
			return	;	
			}	
		}
		
		for (col=0;col<NB_CLIENT;col++) 
		{
		
			if(G_RequestQueue[ISPPROC_SINGLE_SNAP][col] != 0) {
			//take it
			G_Lock=ISPPROC_SINGLE_SNAP;	
			G_Client=col;
			
			//decrement the request in request queue
			G_RequestQueue[ISPPROC_SINGLE_SNAP][col] --;		
			
			// send granted
			alert[col].accepted(ISPPROC_SINGLE_SNAP, requesterId);		
			return ;
			}	
		}	

		for (col=0;col<NB_CLIENT;col++) 
		{		
			if(G_RequestQueue[ISPPROC_BURST][col] != 0) {
				//take it
				G_Lock=ISPPROC_BURST;	
			    G_Client=col;
				
				//decrement the request in request queue
				G_RequestQueue[ISPPROC_BURST][col] --;				
				
				// send granted
				alert[col].accepted(ISPPROC_BURST, requesterId);	
				return;	
			}
		}	
		for (col=0;col<NB_CLIENT;col++) 
		{				
			if(G_RequestQueue[CAMERA_VF][col] != 0) {
			//take it
			G_Lock=CAMERA_VF;	
			G_Client=col;
			
			//decrement the request in request queue
			G_RequestQueue[CAMERA_VF][col] --;				
			
			// send granted
			alert[col].accepted(CAMERA_VF, requesterId);	
			return ;
			}	
		}
	
	//else do nothing.	
	
}

