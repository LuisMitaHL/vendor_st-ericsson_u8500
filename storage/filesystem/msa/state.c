/************************************************************************
 *                                                                      *
 *  Copyright (C) 2010 ST-Ericsson                                      *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "convenience.h"
#include "fsa.h"
#include "log.h"
#include "state.h"

/* This state/cache will only be populated. Nothing will ever be removed as
the FSA protocol has no support for telling when it doesn't need a handle any
more. Once a PROC_LOOKUP has given a handle, we must be able to serve on it.

To uniquely identify a file i Linux (and UNIX?) one need the device id (dev_t) of the
file system and the inode (ino_t) within the file system. They are both 64 bit,
meaning it takes 16 bytes to identify a file. fsa_fhandle_t is only 4 bytes.

To make things worse we can't lookup/open a file given it's device id and inode
number. We must have the path. Finding the path from the device id and inode id
means scanning the entire file system. Very expensive.

This concludes that we need to give our own fhandle on a incrementing counter
and cache the path it belongs to. We can never remove the mapping as FSA can't
tell us when the handle is not needeed any more. It MIGHT be possible to return
ESTALE and maybe the modem will lookup the path for a new handle again but this
can not be relied on.

This is probably not really a problem as the number of files and directories
the modem would want to access is likely very low, so low that we never really
need to bother about the state/cache only can grow.
*/

//The hash table is static. But more than 1024 files can be handled, It
// will only load the hash table more. This figure should be the same as
// estimated number of max unique file/directory access the client/modem ever
// will do.

#ifdef DEBUG
//During debug we make this space really tight in order to stress test the state.
#define STATE_TABLESIZE 4
#else
#define STATE_TABLESIZE 1024
#endif

#define STATE_TOOSMALLWARNING \
	"Consider increasing STATE_TABLESIZE and recompile."

typedef struct Entry_st {
	fsa_fhandle_t    handle;
	char            *path;

	struct Entry_st  *nextHandle; //Next pointer in the bucket list
	struct Entry_st  *nextPath; //Next pointer in the bucket list
} Entry_t;

typedef struct State_st {
	Entry_t         *tableHandle[STATE_TABLESIZE]; //Lookup table via handle
	Entry_t         *tablePath[STATE_TABLESIZE]; //Lookup table via path

	fsa_fhandle_t    totalHandles;	//The number of entire in the table
	fsa_fhandle_t    nextFreeHandle;	//Next free fhandle
} State_t;

//Local and only state. It will be initialized to zero as it is placed in non
// inited memory.
static State_t state;

static unsigned hash_path(const char *path)
{
	unsigned hash = 0;
	unsigned iter = 0;

	//JenkinsOneAtATime hash function.
	//This is an open a free hash function which has a
	// very good distribution on strings.
	while ( path[iter] != '\0' )
	{
		hash += (unsigned char) path[iter];
		hash += (hash << 10);
		hash ^= (hash >> 6);
		iter++;
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash % STATE_TABLESIZE;
}


static inline unsigned hash_handle(fsa_fhandle_t handle)
{
	/*
	We could make for a smarter hash function, but the handles are allocated in
	sequential order, starting at one. Not until we hit STATE_TABLESIZE number
	of handles we will have our first collision.
	*/

	return handle % STATE_TABLESIZE;
}

const char *state_lookupHandle(fsa_fhandle_t handle)
{
	unsigned   buckets = 0;
	unsigned   slot;
	Entry_t   *entry;

	slot = hash_handle(handle);
	entry = state.tableHandle[slot];
	while ( entry != NULL )
	{
		//Debugging, Warn for overcrowded hash table
		buckets++;
		if ( unlikely(buckets > 3) )
		{
			logWARN("State: Table has %u buckets in slot=%u. TableSize=%u. "
				STATE_TOOSMALLWARNING,
		        buckets, slot, STATE_TABLESIZE);
		}

		if ( entry->handle == handle )
		{
			logDBG2("State lookup: handle=%u -> path='%s'.",
				entry->handle, entry->path);

			return entry->path;
		}

		entry = entry->nextHandle;
	}

	logDBG2("State lookup: handle=%u -> Not found.", handle);

	return NULL; //Not found
}

fsa_fhandle_t state_lookupPath(const char *path)
{
	unsigned   buckets = 0;
	unsigned   slot;
	Entry_t   *entry;

	slot = hash_path(path);
	entry = state.tablePath[slot];
	while ( entry != NULL )
	{
		//Debugging, Warn for overcrowded hash table
		buckets++;
		if ( unlikely(buckets > 3) )
		{
			logWARN("State: Table has %u buckets in slot=%u. TableSize=%u. "
				STATE_TOOSMALLWARNING,
		        buckets, slot, STATE_TABLESIZE);
		}

		if ( strcmp(entry->path, path) == 0 )
		{
			logDBG2("State lookup: path='%s' -> handle=%u.",
				entry->path, entry->handle);

			return entry->handle;
		}

		entry = entry->nextPath;
	}

	logDBG2("State lookup: path='%s' -> Not found.", path);

	return INVALID_HANDLE; //Not found
}

fsa_fhandle_t state_insertPath(const char *path)
{
	fsa_fhandle_t  handle;
	unsigned       slot_handle, slot_path;
	Entry_t       *entry;

	//Check that the path isn't already inserted
	handle = state_lookupPath(path);
	if ( handle != INVALID_HANDLE )
	{
		logDBG2("State insert: path='%s'. Already exists: handle=%u",
			path, handle);
		return handle;
	}

	//Next handle, start by increasing the state count. We don't want to use
	// fhandle=0. 0 invalid handle.
	state.nextFreeHandle++;

	//Alloc and populate new entry
	entry = easy_malloc(sizeof(Entry_t));
	entry->handle = state.nextFreeHandle;
	entry->path = easy_strdup(path);

	//Insert first in bucket list, for handle
	slot_handle = hash_handle(entry->handle);
	entry->nextHandle = state.tableHandle[slot_handle];
	state.tableHandle[slot_handle] = entry;

	//Insert first in bucket list, for path
	slot_path = hash_path(path);
	entry->nextPath = state.tablePath[slot_path];
	state.tablePath[slot_path] = entry;

	//Increase count we have in our state
	state.totalHandles++;

	if ( unlikely(state.totalHandles > STATE_TABLESIZE) )
	{
		logWARN("State: State has %u entries in it. Hash table size is %u. "
			STATE_TOOSMALLWARNING,
			state.totalHandles, STATE_TABLESIZE);
	}

	logDBG2("State insert: path='%s' -> handle=%u",
		path, entry->handle);

	return entry->handle;
}

