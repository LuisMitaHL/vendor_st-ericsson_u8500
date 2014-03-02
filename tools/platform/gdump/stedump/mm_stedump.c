#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/inotify.h>

#include <dumpstate.h>
#include "stedump.h"

int nblk_copyfile(const char * from, const char * to,int try_count) ;
static int __verbose=0;
static int inotify_fd = -1;


void mm_stedump(int admsrv,int mediaserver,int isfatal){
    int fd,out_fd;
    int bytes_read=0;
    int tot=0;
    char buf[1024];

    printf("STEDUMP-MM: Retrieving DSP trace buffers...\n");
    //nblk_copyfile("/dev/cm_sia_trace","/data/ste-debug/sia_trace",2);
    //nblk_copyfile("/dev/cm_sva_trace","/data/ste-debug/sva_trace",2);
    printf("STEDUMP-MM: Done retrieving dsp buffers\n");


    if (admsrv) {
        printf("STEDUMP-MM:  ### writing %s in %s\n",DUMP_ADMSRV,isfatal?"fatal":"dump");

        FILE* ctx_trace_admsrv = fopen(DUMP_ADMSRV, "w");
        if(ctx_trace_admsrv==0) {
            printf("STEDUMP-MM: Could not open %s for writing : (%s)!\n",DUMP_ADMSRV,strerror(errno));
        }
        else {

            fprintf(ctx_trace_admsrv,isfatal?"fatal":"dump");
            fclose(ctx_trace_admsrv);
        }
    }


    if (mediaserver) {
        printf("STEDUMP-MM:  ### writing %s in %s\n",DUMP_MEDIASERVER,isfatal?"fatal":"dump");

        FILE* ctx_trace_mediaserver = fopen(DUMP_MEDIASERVER, "w");
        if(ctx_trace_mediaserver==0) {
            printf("STEDUMP-MM: Could not open %s for writing : (%s)!\n",DUMP_MEDIASERVER,strerror(errno));
        }
        else {

            fprintf(ctx_trace_mediaserver,isfatal?"fatal":"dump");
            fclose(ctx_trace_mediaserver);
        }
    }
}

int nblk_copyfile(const char * from, const char * to,int try_count) {

    int fd,out_fd;
    int bytes_read=0;
    int totr=0,totw=0,tot_try=0;
    int try=0;
    char buf[65536];
    int errsv;
    int err=0;

    unlink(to);
    fd = open(from, O_RDONLY | O_NONBLOCK);
    errsv = errno;
    if (fd == -1) {
        printf("STEDUMP-MM: Could not open %s : (%s)\n",from,strerror(errsv));
        return 1;

    }
    FILE* out = fopen(to, "wb");
    if(fd < 0) {
        printf("STEDUMP-MM: Could not open %s for writing : (%s)!\n",from,strerror(errsv));
        return 1;
    }
    while(try++ < try_count) {
        int errsv;
        int read_count=0,write_count=0;

        read_count = read(fd,buf,CHUNK_SIZE);
        errsv = errno;
        if (read_count == -1) {
            printf("STEDUMP-MM: Could not read from %s %d (%s)\n",from,errsv,strerror(errsv));
            break;
        } else {
            if (read_count ==0) {
                if (__verbose) printf("read 0 bytes from %s\n",from);
                break;
            }
            if (__verbose) printf("STEDUMP-MM: read %d bytes from %s, reseting try count\n",read_count,from);
            tot_try++;
            try=0;
            totr+=read_count;
            write_count = fwrite(buf,sizeof(char),read_count,out);
            int errsv = errno;
            if (write_count == 0) {
                printf("STEDUMP-MM: Error while writing to %s : %d (%s)\n",to,errsv,strerror(errsv));
                break;
            }
            else {
                totw+=write_count;
                if (__verbose) printf("STEDUMP-MM: Wrote %d bytes to %s\n",write_count,to);
            }
        }
        if (__verbose) printf("STEDUMP-MM:  - Read %d bytes from %s and wrote %d to %s at try %d\n",read_count,from, write_count,to,try);
    }
    printf("STEDUMP-MM:  ### Read %d bytes from %s and wrote %d to %s (tried %d)\n",totr,from, totw,to,tot_try);
    close(fd);
    fclose(out);

    return 0;


}

