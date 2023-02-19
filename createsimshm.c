#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <poll.h>
#include <termios.h>



#ifdef ASSETTOCORSA
#include "simapi/simapi/ac.h"
#include "simapi/include/acdata.h"
#include "simapi/simmap/mapacdata.h"
#endif
#ifdef RFACTOR2
#include "simapi/simapi/rf2.h"
#include "simapi/include/rf2data.h"
#include "simapi/simmap/maprf2data.h"
#endif

#define DEFAULT_UPDATE_RATE      60.0


int main(int argc, char* argv[])
{

#ifdef ASSETTOCORSA

    char* memfile1 = AC_PHYSICS_FILE;
    char* memfile2 = AC_GRAPHIC_FILE;
    char* memfile3 = AC_STATIC_FILE;
    char* memfile4 = AC_CREWCHIEF_FILE;

    int datasize1 = sizeof(struct SPageFilePhysics);
    int datasize2 = sizeof(struct SPageFileGraphic);
    int datasize3 = sizeof(struct SPageFileStatic);
    int datasize4 = sizeof(struct SPageFileCrewChief);

#define MEMFILECNT1
#define MEMFILECNT2
#define MEMFILECNT3
#define MEMFILECNT4

#endif
#ifdef RFACTOR2

    char* memfile1 = RF2_TELEMETRY_FILE;
    char* memfile2 = RF2_SCORING_FILE;

    int datasize1 = sizeof(struct rF2Telemetry);
    int datasize2 = sizeof(struct rF2Scoring);

#define MEMFILECNT1
#define MEMFILECNT2

#endif

#ifdef MEMFILECNT1
    int fd1 = shm_open(memfile1, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd1 == -1)
    {
        printf("open");
        return 10;
    }
    int res1 = ftruncate(fd1, datasize1);
    if (res1 == -1)
    {
        printf("ftruncate");
        return 20;
    }

    void* addr1 = mmap(NULL, datasize1, PROT_WRITE, MAP_SHARED, fd1, 0);
    if (addr1 == MAP_FAILED)
    {
        printf("mmap");
        return 30;
    }
#endif
#ifdef MEMFILECNT2
    int fd2 = shm_open(memfile2, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd2 == -1)
    {
        printf("open");
        return 10;
    }
    int res2 = ftruncate(fd2, datasize2);
    if (res2 == -1)
    {
        printf("ftruncate");
        return 20;
    }

    void* addr2 = mmap(NULL, datasize2, PROT_WRITE, MAP_SHARED, fd2, 0);
    if (addr2 == MAP_FAILED)
    {
        printf("mmap");
        return 30;
    }
#endif
#ifdef MEMFILECNT3
    int fd3 = shm_open(memfile3, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd3 == -1)
    {
        printf("open");
        return 10;
    }
    int res3 = ftruncate(fd3, datasize3);
    if (res3 == -1)
    {
        printf("ftruncate");
        return 20;
    }

    void* addr3 = mmap(NULL, datasize3, PROT_WRITE, MAP_SHARED, fd3, 0);
    if (addr3 == MAP_FAILED)
    {
        printf("mmap");
        return 30;
    }
#endif
#ifdef MEMFILECNT4
    int fd4 = shm_open(memfile4, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd4 == -1)
    {
        printf("open");
        return 10;
    }
    int res4 = ftruncate(fd4, datasize4);
    if (res4 == -1)
    {
        printf("ftruncate");
        return 20;
    }

    void* addr4 = mmap(NULL, datasize4, PROT_WRITE, MAP_SHARED, fd4, 0);
    if (addr4 == MAP_FAILED)
    {
        printf("mmap");
        return 30;
    }
#endif

    fprintf(stderr, "Press Q after all processes using shared memory are closed, to release the shared memory files.");

    struct termios newsettings, canonicalmode;
    tcgetattr(0, &canonicalmode);
    newsettings = canonicalmode;
    newsettings.c_lflag &= (~ICANON & ~ECHO);
    newsettings.c_cc[VMIN] = 1;
    newsettings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &newsettings);
    char ch;
    struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };

    double update_rate = DEFAULT_UPDATE_RATE;
    int t=0;
    int go = true;
    while (go == true)
    {

        if( poll(&mypoll, 1, 1000.0/update_rate) )
        {
            scanf("%c", &ch);
            if(ch == 'q')
            {
                go = false;
            }
        }
    }
    tcsetattr(0, TCSANOW, &canonicalmode);

#ifdef MEMFILECNT1
    munmap(addr1, datasize1);
#endif
#ifdef MEMFILECNT2
    munmap(addr2, datasize2); 
#endif
#ifdef MEMFILECNT3
    munmap(addr3, datasize3);
#endif
#ifdef MEMFILECNT4
    munmap(addr4, datasize4);
#endif

#ifdef MEMFILECNT1
    shm_unlink(memfile1);
#endif
#ifdef MEMFILECNT2
    shm_unlink(memfile2);
#endif
#ifdef MEMFILECNT3
    shm_unlink(memfile3);
#endif
#ifdef MEMFILECNT4
    shm_unlink(memfile4);
#endif

#ifdef MEMFILECNT1
    close(fd1);
#endif
#ifdef MEMFILECNT2
    close(fd2);
#endif
#ifdef MEMFILECNT3
    close(fd3);
#endif
#ifdef MEMFILECNT4
    close(fd4);
#endif


    return 0;
}
