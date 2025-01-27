#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <stdbool.h>
#include <termios.h>
#include <ctype.h>
#include <signal.h>

#include "simapi/include/acdata.h"
#include "simapi/simapi/ac.h"
#include "simapi/include/pcars2data.h"
#include "simapi/simapi/pcars2.h"

double UPDATE_RATE = 980;
int go = 0;

int strcicmp(char const *a, char const *b)
{
    for (;; a++, b++) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}

size_t getSharedMemorySize(char* MemFileName)
{
    if(strcicmp(MemFileName, PCARS2_FILE) == 0)
    {
    	return sizeof(struct pcars2APIStruct);
    }
    if(strcicmp(MemFileName, AC_PHYSICS_FILE) == 0)
    {
    	return sizeof(struct SPageFilePhysics);
    }
    if(strcicmp(MemFileName, AC_GRAPHIC_FILE) == 0)
    {
    	return sizeof(struct SPageFileGraphic);
    }
    if(strcicmp(MemFileName, AC_STATIC_FILE) == 0)
    {
    	return sizeof(struct SPageFileStatic);
    }
    if(strcicmp(MemFileName, AC_CREWCHIEF_FILE) == 0)
    {
    	return sizeof(struct SPageFileCrewChief);
    }

    return 0;
}

int main(int argc, char** argv) {

    size_t SharedMemorySize = 0;
    SharedMemorySize = getSharedMemorySize(argv[1]);

    if(SharedMemorySize == 0)
    {
        return 1;
    }

    int fd;
    fd = shm_open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
    	perror("open");
    	return 10;
    }


    void *addr;
    ftruncate(fd, SharedMemorySize);
    addr = mmap(NULL, SharedMemorySize, PROT_WRITE, MAP_SHARED, fd, 0);



    if (addr == MAP_FAILED)
    {
    	perror("mmap");
    	return 30;
    }

    int duplicated_stdin = dup(0);
    ftruncate(duplicated_stdin, SharedMemorySize);
    read(duplicated_stdin, addr, SharedMemorySize);


    double update_rate = UPDATE_RATE;
    while (go == 0)
    {

        duplicated_stdin = dup(0);
        lseek(duplicated_stdin, 0, SEEK_SET);

        ftruncate(duplicated_stdin, SharedMemorySize);
        read(duplicated_stdin, addr, SharedMemorySize);

	usleep((unsigned long)(1000000.0/update_rate));
    }

    close(duplicated_stdin);
    //fd = shm_unlink(MEM_FILE_LOCATION);
    //if (fd == -1)
    //{
    //	perror("unlink");
    //	return 100;
    //}

    exit(0);
}
