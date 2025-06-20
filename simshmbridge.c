#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int go = 0;

#ifdef DEBUG
void hexDump(char *desc, void *addr, int len);
#endif

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        go = 1;
        return TRUE;
    default:
        return FALSE;
    }
}





int pids[4];
/**************
 * Game Variables
 * ***********/

#ifdef ASSETTOCORSA
#include "simapi/simapi/ac.h"
#include "simapi/include/acdata.h"

LPCSTR filefind = "acpmf_*";
// define a single file to use as a test to see if the game has initialized the shared memory
LPCSTR file1 = AC_PHYSICS_FILE;

int numfiles = 4;
const char* files[] =
{
    AC_STATIC_FILE,
    AC_GRAPHIC_FILE,
    AC_CREWCHIEF_FILE,
    AC_PHYSICS_FILE
};


typedef struct SPageFilePhysics SharedMemory1;

size_t getmemfilesize(const char* filename)
{
    if(strcmp(filename, AC_PHYSICS_FILE) == 0)
    {
        return AC_PHYSICS_SIZE;
    }
    if(strcmp(filename, AC_STATIC_FILE) == 0)
    {
        return AC_STATIC_SIZE;
    }
    if(strcmp(filename, AC_GRAPHIC_FILE) == 0)
    {
        return AC_GRAPHIC_SIZE;
    }
    if(strcmp(filename, AC_CREWCHIEF_FILE) == 0)
    {
        return AC_CREWCHIEF_SIZE;
    }
    if(strcmp(filename, "acpmf_secondMonitor") == 0)
    {
        return AC_CREWCHIEF_SIZE;
    }

    return AC_CREWCHIEF_SIZE;
}
#endif

#ifdef RFACTOR2
#include "simapi/simapi/rf2.h"
#include "simapi/include/rf2data.h"

typedef struct rF2Scoring SharedMemory1;

LPCSTR filefind = "$rFactor2SMMP_*";

int numfiles = 13;
const char* files[] =
{
    RF2_TELEMETRY_FILE,
    RF2_SCORING_FILE
};

size_t getmemfilesize(const char* filename)
{
    //if(strcmp(filename, RF2_TELEMETRY_FILE) == 0)
    //{
    //    return sizeof(struct rF2Telemetry);
    //}
    //if(strcmp(filename, RF2_SCORING_FILE) == 0)
    //{
    //    return sizeof(struct rF2Scoring);
    //}
    //return 4096;
    return 250000;
}
#endif

#ifdef PROJECTCARS2
#include "simapi/simapi/pcars2.h"
#include "simapi/include/pcars2data.h"

typedef struct pcars2APIStruct SharedMemory1;

LPCSTR filefind = PCARS2_FILE;
LPCSTR file1 = PCARS2_FILE;

int numfiles = 1;
const char* files[] =
{
    PCARS2_FILE,
};

size_t getmemfilesize(const char* filename)
{
    if(strcmp(filename, PCARS2_FILE) == 0)
    {
        return PCARS2_SIZE;
    }

    return 100000;
}
#endif





/*************************
 * Helper Process First
 *
 ************************/

#ifdef HELPERPROCESSFIRST
int main(int argc, char** argv)
{

    WIN32_FIND_DATA fdata;
    HANDLE ffhandle;
    if (chdir("/dev/shm") != 0)
    {
        printf("Could not change directory to /dev/shm: %s\n", strerror(errno));
        return 1;
    }
    // TODO: change this to a loop until it's up and running
    if ((ffhandle = FindFirstFile(filefind, &fdata)) == INVALID_HANDLE_VALUE)
    {
        DWORD err = GetLastError();
        if (err == 2) {
            printf("Could not find any OVR SHM objects: is service running?\n");
        } else {
            printf("FindFirstFile error: %s\n", strerror(err));
        }
        return 1;
    }

    unsigned char* pMem = NULL;
    HANDLE maph = NULL;
    HANDLE shmhandles[numfiles];
    HANDLE fdhandles[numfiles];
    int i = 0;;
    do
    {
        fdhandles[i] = CreateFile(fdata.cFileName, GENERIC_READ|GENERIC_WRITE|GENERIC_EXECUTE, FILE_SHARE_READ|FILE_SHARE_WRITE,
                NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (fdhandles[i] == INVALID_HANDLE_VALUE)
        {
            printf("warning: could not open %s: %s\n", fdata.cFileName, strerror(GetLastError()));
            continue;
        }
        int filesize = getmemfilesize(fdata.cFileName);

        maph = CreateFileMapping(fdhandles[i], NULL, PAGE_EXECUTE_READWRITE, 0, filesize, fdata.cFileName);
        if (maph == NULL)
        {
            printf("warning: could not create mapping for %s: %s\n", fdata.cFileName, strerror(GetLastError()));
            CloseHandle(fdhandles[i]);
            continue;
        }
        shmhandles[i] = maph;;
        // TODO: save all these handles to close them later
        printf("Bridged /dev/shm/%s of %i size to Win32 named mapping \"%s\"\n", fdata.cFileName, filesize, fdata.cFileName);


        i++;
    }
    while (FindNextFile(ffhandle, &fdata));
    printf("Done! Sleeping forever to keep objects alive, press Shift-E to stop\n");


#ifdef DEBUG
    pMem = (unsigned char*)MapViewOfFile(maph, FILE_MAP_READ, 0, 0, getmemfilesize(files[0]));
#endif


    while(1)
    {

#ifdef DEBUG
        hexDump( "MEM: ", pMem, sizeof(SharedMemory1));
#endif
        if(kbhit()) {
            char ch = getch();
            if(ch == 'E')
            {
                break;
            }
        }
        continue;
    }


#ifdef DEBUG
        CloseHandle(pMem);
#endif

    for(int j = 0; j < numfiles; j++)
    {
        // if there is some kind of error these will either not exist or already be closed, if you're nitpicking
        CloseHandle(shmhandles[j]);
        CloseHandle(fdhandles[j]);
    }
    return 0;
}
#endif







/*************************
 * Helper Process Second
 *
 ************************/

#ifdef HELPERPROCESSSECOND
int main(int argc, char** argv) {
    char *access_mode;
    DWORD access = 0;
    STARTUPINFO si;
    PROCESS_INFORMATION pi[numfiles];
    HANDLE shmhandles[numfiles];
    HANDLE maph = NULL;


    fprintf(stderr, "Waiting to map first shared memory file.");
    while(maph == NULL) {
        maph = OpenFileMapping(FILE_MAP_READ, FALSE, file1);
        Sleep(3000);
    }
    CloseHandle(maph);


#ifdef DEBUG
    unsigned char* mapfb;
    mapfb = (unsigned char*)MapViewOfFile(maph, FILE_MAP_READ, 0, 0, sizeof(SharedMemory1));
    SharedMemory1* b = malloc(sizeof(SharedMemory1));
    b = (SharedMemory1*)mapfb;


    //fprintf(stderr, "buf contains: %i\n", b->mVersion);
    //fprintf(stderr, "buf contains: %i\n", b->mBuildVersionNumber);
    //fprintf(stderr, "buf contains: %i\n", b->mGameState);
    //fprintf(stderr, "buf contains: %i\n", b->mSessionState);
    //fprintf(stderr, "buf contains: %i\n", b->mRaceState);

    FILE *outfile;

#endif


    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.hStdInput = maph;
    si.dwFlags |= STARTF_USESTDHANDLES;


    for(int i = 0; i < numfiles; i++)
    {
        maph = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, FILE_MAP_READ, 0, getmemfilesize(files[i]), files[i]);
        ZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));
        size_t size = snprintf(NULL, 0, "%s %s", argv[1], files[i]);
        char* ProcessString = malloc(size + 1);
        sprintf(ProcessString, "%s %s", argv[1], files[i]);
        SetStdHandle(STD_INPUT_HANDLE, maph);
        si.hStdInput = maph;
        shmhandles[i] = maph;
        CreateProcess(NULL, ProcessString, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi[i]);
        fprintf(stderr, "started process %s at pid %li\n", ProcessString, pi[i].dwProcessId);
        free(ProcessString);
    }

    SetStdHandle(STD_INPUT_HANDLE, "CONIN$");
    SetConsoleCtrlHandler(CtrlHandler, TRUE);

    fprintf(stderr, "Mapped and sleeping forever Press Ctrl-C to stop\n");
    int key = 0;
    while(go == 0)
    {

        sleep(300);
        continue;
    }

    fprintf(stderr, "Closing child processes\n");
    GenerateConsoleCtrlEvent(0, 0);
    for(int i = 0; i < numfiles; i++)
    {
        TerminateProcess(pi[i].hProcess, 0);
        CloseHandle(shmhandles[i]);
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
    }
    return 0;
}
#endif








/*********************
 * DEBUG
 * *******************/


#ifdef DEBUG
void hexDump(char *desc, void *addr, int len)
{
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != NULL)
        printf ("%s:\n", desc);

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf("  %s\n", buff);

            // Output the offset.
            printf("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf(" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
            buff[i % 16] = '.';
        } else {
            buff[i % 16] = pc[i];
        }

        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf("  %s\n", buff);
}
#endif
