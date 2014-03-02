#include <stdio.h>
#include <sys/inotify.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <pthread.h>
#include <dumpstate.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#include "stedump.h"
#define SUMMARY_HEADER "################################## STEDUMP INSTRUCTIONS ##################"
pthread_t sia_reader_thread;
pthread_t sva_reader_thread;
pthread_t sia_dump_thread;
pthread_t sva_dump_thread;

int verbose=0;
int write_instructions(void) ;
static int inotify_fd = -1;
char filename[FILENAME_MAX+1];
static char* circ_buf_sia=0;
static char* circ_buf_sva=0;

int circ_buf_offset_sia=0;
int circ_buf_offset_sva=0;
static int circ_buf_size=0;
// This variable is set when the circular buffer has been full at least once
// and thus contains data from beginning to end
int write_to_file=0;

#define __DO_LOG_BSIZE 1024
// This mutext protects rolling buffer and write pointer offset
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t dump_cond = PTHREAD_COND_INITIALIZER;
static int copy(char *source, char *dest);

int fdlock;

int get_lock(void)
{
  struct flock fl;

  fl.l_type = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = 1;

  if((fdlock = open("/data/ste-debug/singlestedump.lock", O_WRONLY|O_CREAT, 0666)) == -1)
    return 0;

  if(fcntl(fdlock, F_SETLK, &fl) == -1)
    return 0;

  return 1;
}

void *mpc_dumper_threadentry(void *arg) {

#ifdef PR_SET_NAME
    prctl(PR_SET_NAME, (unsigned long)"stedump-mpcdump", 0, 0, 0);
#endif
    mpc_ring_buf * struct_p = (mpc_ring_buf*)arg;

    while(1) {
        int s = pthread_cond_wait(&dump_cond,NULL);
        if (s == -1) {
            fprintf(stderr,"Error in dump pthread_cond_wait : %s\n",strerror(errno));
        }
        else{
            fprintf(stderr,"Dumping %s buffer on request\n",struct_p->name);
            char filename[FILENAME_MAX+1];
            sprintf(filename,"/data/ste-debug/%s_dump.bin",struct_p->name);
            if (dump_mpc_circularbuffer(struct_p) == 0) {
                fprintf(stderr,"Dumped %s circular buffer\n",struct_p->name);
            }
        }
    }
    return (void*)0;
}


void *mpc_reader_threadentry(void *arg) {
 #ifdef PR_SET_NAME
	prctl(PR_SET_NAME, (unsigned long)"stedump-mpcreader", 0, 0, 0);
#endif
    mpc_ring_buf * struct_p = (mpc_ring_buf*)arg;

    char filename[FILENAME_MAX+1];
    char outfilename[FILENAME_MAX+1];
    int write_count=0,tot=0,read_count=0;
    FILE* out=0;
    int mpc_trace_fd ;
    char *buf;

    fprintf(stderr,"Entering %s reader thread\n",struct_p->name);
    snprintf(filename,FILENAME_MAX,"/dev/cm_%s_trace",struct_p->name);
    mpc_trace_fd = open(filename,O_RDONLY );
    if (mpc_trace_fd == -1) {
        fprintf(stderr,"%s log reader: Could not open %s for reading: (%s)\n",struct_p->name,filename,strerror(errno));
        return (void*)0;
    }

    // Should we output to a file ?
    if (write_to_file) {
        snprintf(outfilename,FILENAME_MAX,STE_DEBUG_DIR "/%s_trace",struct_p->name);
        out = fopen(outfilename, "wb");
        if (out == NULL) {
            fprintf(stderr,"%s log reader: Could not open %s for writing: (%s)\n",struct_p->name,outfilename,strerror(errno));
            return (void*)0;
        }

    }
    buf= (char*) malloc(CHUNK_SIZE*sizeof(char));

    int message_cntr=0;
    while (1) {
        read_count = read(mpc_trace_fd,buf,CHUNK_SIZE);
        if (read_count == -1) {
            //fprintf(stderr,"READ ERROR : %s \n",strerror(errno));
            fprintf(stderr,"%s log reader: Could not read from %s %d (%s)\n",struct_p->name,filename,errno,strerror(errno));
            break;
        }
        else {
            if (verbose) fprintf(stderr,"%s log reader: Read %d bytes from %s, msg=%d\n",struct_p->name,read_count,filename,message_cntr++);
        }
        if (read_count) {



            ////////////////////////////////////////////////////////////////////////////////
            // LOG TO CIRCULAR BUFFER
            ////////////////////////////////////////////////////////////////////////////////

            pthread_mutex_lock(&mutex); // to protect write_ptr_offset
            log_to_ring_buffer(struct_p,buf,read_count);
            pthread_mutex_unlock(&mutex); // to protect write_ptr_offset



            if (write_to_file && out !=0) {
                write_count = fwrite(buf,sizeof(char),read_count,out);
                if (write_count == 0) {
                    fprintf(stderr,"%s log reader: Error while writing %d bytes to %s : %d (%s)\n",struct_p->name,read_count,outfilename,errno,strerror(errno));
                    break;
                }
                else {
                    tot+=write_count;
                    fprintf(stderr,"%s log reader: Wrote %d bytes to %s\n",struct_p->name,write_count,outfilename);
                }
            }
        }
    }
    free(buf);

    fprintf(stderr,"%s log reader: exiting reader thread\n",struct_p->name);
    return (void*)0;

}

int get_buffer_size(char * mpc) {
    FILE *trace_size_file;
    char filename[FILENAME_MAX+1];
    int size_buf=0;
    snprintf(filename,FILENAME_MAX,"/sys/module/cm/parameters/%s_osttrace_nb",mpc);
    if (!(trace_size_file = fopen(filename,"r"))) {
        fprintf(stderr,"Could NOT reopen trace size %s file for checking\n",filename);
        return -1;
    }
    fscanf(trace_size_file,"%d",&size_buf);
    fclose(trace_size_file);
    return size_buf;
}
int set_buffer_size(char * mpc,int requested_size) {
    FILE * trace_size_file;
    char filename[FILENAME_MAX+1];
    snprintf(filename,FILENAME_MAX,"/sys/module/cm/parameters/%s_osttrace_nb",mpc);
    trace_size_file = fopen(filename,"w");
    if(trace_size_file) {
        fprintf(trace_size_file,"%d\n",requested_size);
        fclose(trace_size_file);
        fprintf(stderr,"Modify %s trace buffer size to %d\n",mpc,requested_size);        
        return 0;
    }
    else {
        fprintf(stderr,"Could NOT modify %s trace buffer size\n",mpc);
        return -1;
    }

}

void resize_buffer(char* mpc,int size) {

    int current_size = get_buffer_size(mpc);
    if (current_size != -1 && current_size != size) {
        set_buffer_size(mpc,size);
    }

}

int main() {


    if(!get_lock())
    {
        fprintf(stderr,"stedump already running (error while getting lock : %s). assuming required action is to trigger a dump through %s and exit\n", strerror(errno),STE_DEBUG_DUMP_TRIGGER);
        FILE *file = fopen(STE_DEBUG_DUMP_TRIGGER, "w");
        fprintf(file,"dump\n");
        fclose(file);
        return 0;
    }


    resize_buffer("sia",1024);
    resize_buffer("sva",512);
    char value[PROPERTY_VALUE_MAX];	
        
    property_get("ste.syslog.buffersize", value, "1048576" /*1024*1024*/);
    int _size = atoi(value);
    circ_buf_size = _size*sizeof(char) ;


    circ_buf_sia = (char*)malloc(circ_buf_size);
    if (circ_buf_sia==0) {
        circ_buf_size = 0;
        fprintf(stderr,"Could not allocate circular buffer of size %d for sia!!\n",_size);
        return 1;
    }
    else {
        memset(circ_buf_sia,0,circ_buf_size);
        circ_buf_offset_sia=0;
    }


    circ_buf_sva = (char*)malloc(circ_buf_size);
    if (circ_buf_sva==0) {
        circ_buf_size = 0;
        fprintf(stderr,"Could not allocate circular buffer of size %d for sva !!\n",_size);
        return 1;
    }
    else {
        memset(circ_buf_sva,0,circ_buf_size);
        circ_buf_offset_sva=0;
    }


    mpc_ring_buf * p_sia =(mpc_ring_buf *)malloc(sizeof(mpc_ring_buf));
    strcpy(p_sia->name , "sia");
    p_sia->circ_buf_ptr = circ_buf_sia;
    p_sia->write_ptr_offset = &circ_buf_offset_sia;
    p_sia->circular_buffer_full=0;
       
    int ret = pthread_create(&sia_reader_thread, NULL,mpc_reader_threadentry,p_sia);
    if (ret) {
        fprintf(stderr,"Error creating sia trace reader thread : %s",strerror(errno));
    }

    
    mpc_ring_buf * p_sva =(mpc_ring_buf *)malloc(sizeof(mpc_ring_buf));
    strcpy(p_sva->name , "sva");
    p_sva->circ_buf_ptr = circ_buf_sva;
    p_sva->write_ptr_offset = &circ_buf_offset_sva;
    p_sva->circular_buffer_full=0;
       
    ret = pthread_create(&sva_reader_thread, NULL,mpc_reader_threadentry,p_sva);
    if (ret) {
        fprintf(stderr,"Error creating sva trace reader thread : %s",strerror(errno));
    }

    
    ret = pthread_create(&sia_dump_thread, NULL,mpc_dumper_threadentry,p_sia);
    if (ret) {
        fprintf(stderr,"Error creating dump thread : %s",strerror(errno));
    }
    ret = pthread_create(&sva_dump_thread, NULL,mpc_dumper_threadentry,p_sva);
    if (ret) {
        fprintf(stderr,"Error creating dump thread : %s",strerror(errno));
    }



    inotify_fd = inotify_init();
    if (inotify_fd < 0) {
        printf("Init inotify_init failed: %s", strerror(errno));
        return 1;
    }

    // let's try to create the debug dir until
    int sleep_amount=1;
    while (1) {
        if (mkdir(STE_DEBUG_DIR, S_IRWXU | S_IRWXG | S_IRWXO)) {
            if (errno != EEXIST) {
                printf("mkdir of '" STE_DEBUG_DIR "' failed: %s",
                        strerror(errno));
                sleep(sleep_amount);
                sleep_amount*=10;
                continue;
            }

        }

        if (write_instructions()) {
            printf("Error writing %s : %s\n",STE_DEBUG_DUMP_TRIGGER,strerror(errno));
            return 1;
        }
        printf("Wrote %s \n",STE_DEBUG_DUMP_TRIGGER);


        // Watch the file
        int wd = inotify_add_watch(inotify_fd,STE_DEBUG_DUMP_TRIGGER , IN_CLOSE_WRITE);
        if (wd < 0) {
            printf("Init inotify_add_watch failed wd=%d, errno=%s, trigger=%s",
                    wd, strerror(errno), STE_DEBUG_DUMP_TRIGGER);
            inotify_fd = -1;
            sleep(sleep_amount);
            sleep_amount*=10;
        }
        break;
    }

    /** let's wait for file events. */
    while (1) {
        fd_set fds;
        static char buffer[10 * sizeof(struct inotify_event)];

        FD_ZERO(&fds);
        FD_SET(inotify_fd, &fds);

        /* Wait file event */
        if (select(inotify_fd + 1, &fds, NULL, NULL, 0) <= 0) {
            printf("Exiting loop\n");
            continue;
        }

        /* Retrieve information about the file event */
        int length = read(inotify_fd, buffer, sizeof(buffer));

        if (verbose) printf("Processing events\n");

        if (length <= 0) {
            printf("Thread read from inotify_fd failed: %s", strerror(errno));
            continue;
        }
        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (verbose) {
                printf("i=%d/%d file event wd=%d, mask=0x%08x len=%d, name=%s length=%d sizeof strcut : %d\n",
                    i,length,event->wd, event->mask, event->len, (event->len ? event->name : "<none>"),event->len,sizeof (struct inotify_event));
            }
            FILE *file = fopen(STE_DEBUG_DUMP_TRIGGER, "r");
            if (file == NULL) {
                printf("open of '%s' for reading failed: %s", STE_DEBUG_DUMP_TRIGGER, strerror(errno));
            } else {
                char actionBuffer[1024];

                int got_nb_char = fread(actionBuffer, 1, 1024, file);
                fclose(file);
                if (strncmp(actionBuffer, SUMMARY_HEADER, strlen(SUMMARY_HEADER))) {
                    
                    if (verbose) {
                        printf("Action buffer : %s\n",actionBuffer);
                    }
              /* user action */
                    if (strncmp(actionBuffer, ADMSRV , strlen(ADMSRV)) == 0) {

                        printf("#### ADM SERVER STEDUMP REQUESTED ####\n");
                        mm_stedump(1,0,0);
                        printf("done\n");

                    }
                    else if (strncmp(actionBuffer, MEDIASERVER , strlen(MEDIASERVER)) == 0) {

                        printf("#### MEDIASERVER SERVER STEDUMP REQUESTED ####\n");
                        mm_stedump(0,1,0);
                        printf("done\n");
                    }
                    else if (strncmp(actionBuffer, "dump" , strlen("dump")) == 0) {

                        printf("#### MEDIASERVER AND ADMSRV STEDUMP REQUESTED ####\n");
                        mm_stedump(1,1,0);
                        printf("done\n");
                    }
                    else if (strncmp(actionBuffer, "fatal" , strlen("fatal")) == 0) {

                        printf("#### MEDIASERVER AND ADMSRV STEDUMP FATAL ERROR REQUESTED ####\n");
                        mm_stedump(1,1,1);
                        printf("done\n");
                    }
                    else {
                        printf("#### UKNOWN ACTION %s ! ####\n",actionBuffer);
                    }
                    printf("#### RETRIEVING DSP TRACE DICS ####\n");
                    copy("/system/usr/share/ste_dicos.tgz","/data/ste-debug/ste_dicos.tgz");
                    printf("#### DUMPING DSP LOGS ####\n");
                    int s = pthread_cond_broadcast(&dump_cond);
                    


                    if (write_instructions()) {
                        printf("Error re-writing instructions in %s : %s\n",STE_DEBUG_DUMP_TRIGGER,strerror(errno));
                        return 1;
                    }                    
                    printf("#### DUMPED DATA IN /data/ste-debug ####\n");
                }

            }
            i += event->len + sizeof (struct inotify_event);

        }
    }
        





    return 0;
}

int copy(char *source, char *dest)
{
    FILE *from, *to;
    char ch;

    /* open source file */
    if((from = fopen(source, "rb"))==NULL) {
        printf("Cannot open source file %s\n",source);
        return 1;
    }

    /* open destination file */
    if((to = fopen(dest, "wb"))==NULL) {
        printf("Cannot open destination file %s\n",dest);
        return 1;
    }

    /* copy the file */
    while(!feof(from)) {
        ch = fgetc(from);
        if(ferror(from)) {
            printf("Error reading source file %s\n",source);
            return 1;
        }
        if(!feof(from)) fputc(ch, to);
        if(ferror(to)) {
            printf("Error writing destination file %s\n",dest);
            return 1;
        }
    }

    if(fclose(from)==EOF) {
        printf("Error closing source file %s\n",source);
        return 1;
    }

    if(fclose(to)==EOF) {
        printf("Error closing destination file %s\n",dest);
        return 1;
    }
    return 0;

} 


int write_instructions(void) {
    FILE *file = fopen(STE_DEBUG_DUMP_TRIGGER, "w");
    if (file == NULL) {
        printf("open of '%s' for writing failed: %s", filename, strerror(errno));
        return 1;
    } else {
        fprintf(file,SUMMARY_HEADER);
        fprintf(file, "\n# Write \"dump\" or \"fatal\" in this file to trig a dump\n");
        fclose(file);
    }
    return 0;

}

int dump_mpc_circularbuffer(mpc_ring_buf * struct_p)
{
    char*name = struct_p->name;
    char path[FILENAME_MAX+1];

    snprintf(path,FILENAME_MAX,"/data/ste-debug/%s_trace_dump",name);
    fprintf(stderr,"Dumping %s circular buffer in %s\n",name,path);

    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        fprintf(stderr,"%s: %s\n", path, strerror(errno));
        return 1;
    }

    int initial_offset,count=0;
    int tot=0;
    int still_to_write = circ_buf_size;
    char * dump_write_ptr;
    int circ_buf_filled_length;

    pthread_mutex_lock(&mutex); // to protect write_ptr_offset

    if (struct_p->circular_buffer_full) {
         // let's look for the beginning of next trace : 
         int found=0;
         int offset = *struct_p->write_ptr_offset;
         int pos=0;
         int value;
         char * ptr = struct_p->circ_buf_ptr;
         while(!found && count < circ_buf_size) {
             if (*((int*)(ptr+offset))==0x7c4c101c) {
                found=1;
                fprintf(stderr,"Found beginning of oldest trace at offset %d (after %d trials)\n",offset,count);
                break;
             }
             offset++;
             if (offset>=circ_buf_size-1) {
                 offset=0;
             }

             count++;
           }
         if (!found) {
                fprintf(stderr,"Could not find beginning of a trace in buffer after %d increments\n",offset-*struct_p->write_ptr_offset);

             // Not really normal...
             dump_write_ptr = struct_p->circ_buf_ptr+*(struct_p->write_ptr_offset);
             circ_buf_filled_length = circ_buf_size - (struct_p->discardable_bytecount);
         } else {
                if (verbose) fprintf(stderr,"found beginning of a trace in buffer after %d increments\n",offset);
             int todiscard=(offset-*(struct_p->write_ptr_offset));
             dump_write_ptr=struct_p->circ_buf_ptr+*(struct_p->write_ptr_offset)+todiscard;
             circ_buf_filled_length =
             /*overall size*/ circ_buf_size
             -/*end of buffer where there was no room to store a message*/ (struct_p->discardable_bytecount)
             -/*offset to next message beginning*/ todiscard;
         }
     } else {
        if (verbose)  fprintf(stderr,"%s buffer was not yet filled\n",name);
        circ_buf_filled_length = *(struct_p->write_ptr_offset);
        dump_write_ptr = struct_p->circ_buf_ptr;
    }


    initial_offset = dump_write_ptr-struct_p->circ_buf_ptr;
    if (verbose)  fprintf(stderr,"Initial offset = %d, circ_buf_filled_length=%d, overall size=%d\n",initial_offset,circ_buf_filled_length,circ_buf_size);

    if (circ_buf_filled_length) {
        // write from write pointer till end of the circ buffer
        count += write(fd,dump_write_ptr,circ_buf_filled_length-initial_offset-1);
        if (count>0) {
            still_to_write -= count;
            tot+=count;
        }
        // now write from buffer beginning till initial_offset
        count += write(fd,struct_p->circ_buf_ptr,initial_offset+1);

        pthread_mutex_unlock(&mutex);

        fprintf(stderr,"Dumped %d bytes to  %s (buffer size : %d)\n", tot, path,circ_buf_size);
        close(fd);

        pid_t gzip_pid = -1;

        int gzip_level=5;
        if (gzip_level > 0) {
            gzip_pid = fork();
            if (gzip_pid < 0) {
                fprintf(stderr, "fork: %s\n", strerror(errno));
                exit(1);
            }

            if (gzip_pid == 0) {

                char level[10];
                int size = snprintf(level, sizeof(level), "-%d", gzip_level);
                execlp("gzip", "gzip", level, path,NULL);
                fprintf(stderr,"exec(gzip): %s\n", strerror(errno));
                return(1);
            }
            else {
                fprintf(stderr,"gzipping %s circular buffer to %s.gz\n",name,path);
            }

        }
        else {
            fprintf(stderr,"Dumped  %s circular buffer to %s\n",name,path);
        }
    }
    else {
        pthread_mutex_unlock(&mutex);
        close(fd);
        fprintf(stderr,"%s EMPTY buffer will not be dumped\n",name);
        return 1;
    }

    return 0;

}

void log_to_ring_buffer(mpc_ring_buf * struct_p,const char * buf,int read_count) {
    // -> is there enough room to hold our newly created message ?
    if (*(struct_p->write_ptr_offset)+read_count+1>=circ_buf_size) {
        int ret;
        // we erase the unfilled content of the buffer
        (struct_p->discardable_bytecount) = circ_buf_size-*(struct_p->write_ptr_offset)-1;
        memset((char*)(struct_p->circ_buf_ptr+*(struct_p->write_ptr_offset)),0,(struct_p->discardable_bytecount));
        // back to the beginning of the circular buffer
        *(struct_p->write_ptr_offset)=0;
        struct_p->circular_buffer_full=1;
    }

    // actually copy to the circular buffer
    memcpy((char*)(struct_p->circ_buf_ptr+*(struct_p->write_ptr_offset)),buf,read_count);
    *(struct_p->write_ptr_offset)+=read_count;

    if (verbose>2) {
        fprintf(stderr,"offset=%d\n",*(struct_p->write_ptr_offset));
    }
}


