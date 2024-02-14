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

#ifdef PROJECTCARS2
#include "simapi/include/pcars2data.h"
#define MEM_FILE_LOCATION "$pcars2"
typedef struct pcars2APIStruct SharedMemory1;
#define SHAREDMEMORY1
#endif


#define SOCKET_FD 1
#define SEND_FD 0

int main(int argc, char** argv) {


    int fd;
	fd = shm_open(MEM_FILE_LOCATION, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd == -1)
	{
		perror("open");
		return 10;
	}


	void *addr;
#ifdef SHAREDMEMORY1
    ftruncate(fd, sizeof(SharedMemory1));
	addr = mmap(NULL, sizeof(SharedMemory1), PROT_WRITE, MAP_SHARED, fd, 0);
#endif



	if (addr == MAP_FAILED)
	{
		perror("mmap");
		return 30;
	}

    int duplicated_stdin = dup(0);
#ifdef SHAREDMEMORY1
    SharedMemory1* b = malloc(sizeof(SharedMemory1));
    ftruncate(duplicated_stdin, sizeof(SharedMemory1));
    read(duplicated_stdin, addr, sizeof(SharedMemory1));
#endif



    double update_rate = 480;
    int go = 0;

    while (go == 0)
    {

        duplicated_stdin = dup(0);
        lseek(duplicated_stdin, 0, SEEK_SET);

#ifdef SHAREDMEMORY1
        ftruncate(duplicated_stdin, sizeof(SharedMemory1));
        read(duplicated_stdin, addr, sizeof(SharedMemory1));
#endif


		usleep((unsigned long)(1000000.0/update_rate));
	}

    close(duplicated_stdin);
    fd = shm_unlink(MEM_FILE_LOCATION);
	if (fd == -1)
	{
		perror("unlink");
		return 100;
	}

    return 0;
}
