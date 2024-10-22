/*
*
* Copyright 2006, The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#define LOG_TAG "ImgEnsWrapper_DumpMemInfo"
//#include "ImgEns_WrapperLog.h"
#include "img_mapinfo.h"

extern "C" void get_malloc_leak_info (uint8_t** info, size_t* overallSize, size_t* infoSize, size_t* totalMemory, size_t* backtraceSize );
extern "C" void free_malloc_leak_info(uint8_t* info);

// Use the String-class below instead of String8 to allocate all memory
// beforehand and not reenter the heap while we are examining it...
struct MyString8 
{
	static const size_t MAX_SIZE = 256 * 1024;

	MyString8()
		: mPtr((char *)malloc(MAX_SIZE)) {
			*mPtr = '\0';
	}

	~MyString8() {
		free(mPtr);
	}

	void append(const char *s) {
		strcat(mPtr, s);
	}

	void clear() {
		*mPtr = '\0';
	}

	const char *string() const {
		return mPtr;
	}

	size_t size() const {
		return strlen(mPtr);
	}

private:
	char *mPtr;

	MyString8(const MyString8 &);
	MyString8 &operator=(const MyString8 &);
};

void Img_EnsWrapper__memStatus(void)
{
	const size_t SIZE = 1024;
	char buffer[SIZE];
	MyString8 result;
	int fd;
	time_t t;
	char filename[SIZE];

	static const char * filenameFormat = "/data/memstatus-%d.txt";

	typedef struct 
	{
		size_t size;
		size_t dups;
		intptr_t * backtrace;
	} AllocEntry;

	uint8_t *info = NULL;
	size_t overallSize = 0;
	size_t infoSize = 0;
	size_t totalMemory = 0;
	size_t backtraceSize = 0;

	img_mapinfo * milist = get_process_mapinfo();

	sprintf(filename, filenameFormat, getpid());

	fd = open(filename, O_CREAT|O_RDWR|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
	if (fd == -1) {
		LOGE("Unable to open file %s : %s\n", filename, strerror(errno));
		free_mapinfo(milist);
		return;
	}

	LOGI("Starting memStatus dump to %s", filename);

	t = time(NULL);
	ctime_r(&t, buffer);
	write(fd, "################################################################################\n", 81);
	write(fd, "Date: ", 6);
	write(fd, buffer, strlen(buffer));
	write(fd, "\n\n", 2);

	get_malloc_leak_info(&info, &overallSize, &infoSize, &totalMemory, &backtraceSize);
	if (info) {
		uint8_t *ptr = info;
		size_t count = overallSize / infoSize;

		snprintf(buffer, SIZE, " Allocation count %i\n", count);
		result.append(buffer);
		snprintf(buffer, SIZE, " Total memory %i\n", totalMemory);
		result.append(buffer);

		AllocEntry * entries = new AllocEntry[count];

		for (size_t i = 0; i < count; i++) 
		{
			// Each entry should be size_t, size_t, intptr_t[backtraceSize]
			AllocEntry *e = &entries[i];

			e->size = *reinterpret_cast<size_t *>(ptr);
			ptr += sizeof(size_t);

			e->dups = *reinterpret_cast<size_t *>(ptr);
			ptr += sizeof(size_t);

			e->backtrace = reinterpret_cast<intptr_t *>(ptr);
			ptr += sizeof(intptr_t) * backtraceSize;
		}

		// Now we need to sort the entries.  They come sorted by size but
		// not by stack trace which causes problems using diff.
		bool moved;
		do {
			moved = false;
			for (size_t i = 0; i < (count - 1); i++) 
			{
				AllocEntry *e1 = &entries[i];
				AllocEntry *e2 = &entries[i+1];

				bool swap = e1->size < e2->size;
				if (e1->size == e2->size) 
				{
					for(size_t j = 0; j < backtraceSize; j++) 
					{
						if (e1->backtrace[j] == e2->backtrace[j]) 
						{
							continue;
						}
						swap = e1->backtrace[j] < e2->backtrace[j];
						break;
					}
				}
				if (swap) 
				{
					AllocEntry t = entries[i];
					entries[i] = entries[i+1];
					entries[i+1] = t;
					moved = true;
				}
			}
		} while (moved);

		for (size_t i = 0; i < count; i++) 
		{
			AllocEntry *e = &entries[i];
			unsigned offset;
			const char * libname;

			snprintf(buffer, SIZE, "size %8i, dup %4i, ", e->size, e->dups);
			result.append(buffer);
			for (size_t ct = 0; (ct < backtraceSize) && e->backtrace[ct]; ct++) 
			{
				if (ct) 
				{
					result.append(", ");
				}
				offset = 0;
				libname = map_to_name(milist, e->backtrace[ct], "", &offset);
				snprintf(buffer, SIZE, "0x%08x %s", offset ? offset : e->backtrace[ct], libname);
				result.append(buffer);
			}
			result.append("\n");
			write(fd, result.string(), result.size());
			result.clear();
		}

		delete[] entries;
		free_malloc_leak_info(info);
	}
	free_mapinfo(milist);

	write(fd, result.string(), result.size());
	write(fd, "\n\n", 2);
	close(fd);
	LOGI("memStatus dump done\n");
}
