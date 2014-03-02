#include <stdio.h>
#include <stdlib.h>
#include <dumpstate.h>
#include <errno.h>

void dumpstate_board()
{
    printf("Requesting dump\n");
    extern char* __progname;
    FILE* f=fopen("/data/ste-debug/stedump", "w");
    if (f==0) {
        fprintf(stderr,"ERROR opening /data/ste-debug/stedump for writing : %s\n",strerror(errno));
    }
    else {
        fprintf(f,"%s","dump");
        fclose(f);
    }
}
