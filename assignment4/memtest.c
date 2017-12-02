#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

#ifndef __NR_get_slob_amt_claimed
#define __NR_get_slob_amt_claimed 353
#endif

#ifndef __NR_get_slob_amt_free
#define __NR_get_slob_amt_free 354
#endif

int main() {
        char * buffer0, buffer1, buffer2, buffer3, buffer4, buffer5, buffer6, buffer7;

        buffer0 = (char*) malloc (9000000);
        buffer1 = (char*) malloc (345555);
        buffer2 = (char*) malloc (1234);
        buffer3 = (char*) malloc (144445554);
        buffer4 = (char*) malloc (55630);
        buffer5 = (char*) malloc (10064666665);
        buffer6 = (char*) malloc (105555555550);
        buffer7 = (char*) malloc (1000000000);

        double psize = (double)syscall(__NR_get_slob_amt_claimed);
        printf("psize: %f\n",psize);
        double pused = (double)syscall(__NR_get_slob_amt_free);
        printf("pused: %f\n",pused);
        return 0;
}