#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

//#define DEBUG


#ifdef ASSETTOCORSA
#include "simapi/simapi/ac.h"
#include "simapi/include/acdata.h"

LPCSTR filefind = "acpmf_*";
LPCSTR file1 = AC_PHYSICS_FILE;

typedef struct SPageFilePhysics SharedMemory1;

int getmemfilesize(const char* filename)
{


    if(strcmp(filename, AC_PHYSICS_FILE) == 0)
    {
        return sizeof(struct SPageFilePhysics);
    }
    if(strcmp(filename, AC_STATIC_FILE) == 0)
    {
        return sizeof(struct SPageFileStatic);
    }
    if(strcmp(filename, AC_GRAPHIC_FILE) == 0)
    {
        return sizeof(struct SPageFileGraphic);
    }
    if(strcmp(filename, AC_CREWCHIEF_FILE) == 0)
    {
        return sizeof(struct SPageFileCrewChief);
    }
    if(strcmp(filename, "acpmf_secondMonitor") == 0)
    {
        return sizeof(struct SPageFileCrewChief);
    }

    return 4096;
}
#endif

#ifdef RFACTOR2
#include "simapi/simapi/rf2.h"
#include "simapi/include/rf2data.h"

LPCSTR filefind = "$rFactor2*";

typedef struct rF2Scoring SharedMemory1;

int getmemfilesize(const char* filename)
{
    if(strcmp(filename, RF2_TELEMETRY_FILE) == 0)
    {
        return sizeof(struct rF2Telemetry);
    }
    if(strcmp(filename, RF2_SCORING_FILE) == 0)
    {
        return sizeof(struct rF2Scoring);
    }
    return 4096;
}
#endif

#ifdef PROJECTCARS2
#include "simapi/simapi/pcars2.h"
#include "simapi/include/pcars2data.h"

typedef struct pcars2APIStruct SharedMemory1;

LPCSTR filefind = PCARS2_FILE;
LPCSTR file1 = PCARS2_FILE;

int getmemfilesize(const char* filename)
{


    if(strcmp(filename, PCARS2_FILE) == 0)
    {
        return sizeof(struct pcars2APIStruct);
    }

    return 10000;
}
#endif

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

    void* pMem;
    void* pMem2;
    HANDLE maph;
    HANDLE mapha;
    SharedMemory1* a;
    SharedMemory1* b;
    do
    {
        HANDLE fd = CreateFile(fdata.cFileName, GENERIC_READ|GENERIC_WRITE|GENERIC_EXECUTE, FILE_SHARE_READ|FILE_SHARE_WRITE,
                NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (fd == INVALID_HANDLE_VALUE)
        {
            printf("warning: could not open %s: %s\n", fdata.cFileName, strerror(GetLastError()));
            continue;
        }
        int filesize = getmemfilesize(fdata.cFileName);

        maph = CreateFileMapping(fd, NULL, PAGE_EXECUTE_READWRITE, 0, filesize, fdata.cFileName);
        if (maph == NULL)
        {
            printf("warning: could not create mapping for %s: %s\n", fdata.cFileName, strerror(GetLastError()));
            CloseHandle(fd);
            continue;
        }
        printf("Bridged /dev/shm/%s of %i size to Win32 named mapping \"%s\"\n", fdata.cFileName, filesize, fdata.cFileName);

#ifdef DEBUG
        pMem = (unsigned char*)MapViewOfFile(maph, FILE_MAP_READ, 0, 0, filesize);
#endif
    }
    while (FindNextFile(ffhandle, &fdata));


    printf("Done! Sleeping forever to keep objects alive, press Shift-E to stop\n");
#ifndef DEBUG
    int key = 0;
    while(1)
    {
        if (_kbhit())
        {
            key =_getch();

            if (key == 'E')
                break;
        }
#endif
#ifdef DEBUG
    for (;;)
    {
        a = malloc(sizeof(SharedMemory1));
        hexDump( NULL, a, sizeof(SharedMemory1));
#endif
    }
    return 0;
}
#endif

#ifdef HELPERPROCESSSECOND
int main(int argc, char** argv) {
    char *access_mode;
    DWORD access = 0;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    HANDLE maph = NULL;


    fprintf(stderr, "Waiting to map first shared memory file.");
    while(maph == NULL) {
        maph = OpenFileMapping(FILE_MAP_READ, FALSE, file1);
        Sleep(3000);
    }
    CloseHandle(maph);
    maph = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, FILE_MAP_READ, 0, sizeof(SharedMemory1), file1);
    fprintf(stderr, "Mapped first shared memory file, mapping any necessary remaining shared memory files.");


#ifdef DEBUG
    unsigned char* mapfb;
    mapfb = (unsigned char*)MapViewOfFile(maph, FILE_MAP_READ, 0, 0, sizeof(SharedMemory1));
    SharedMemory1* b = malloc(sizeof(SharedMemory1));
    b = (SharedMemory1*)mapfb;

    //fprintf(stderr, "buf contains: %d\n", b->mNumVehicles);
    //fprintf(stderr, "buf contains: %d\n", b->mVehicles[0].mLapNumber);
    //fprintf(stderr, "buf contains: %lf\n", b->mVehicles[0].mLocalVel.x);
    //fprintf(stderr, "buf contains: %lf\n", b->mVehicles[0].mLocalVel.y);
    //fprintf(stderr, "buf contains: %lf\n", b->mVehicles[0].mLocalVel.z);
    //fprintf(stderr, "buf contains: %d\n", b->mVehicles[0].mGear);
    //fprintf(stderr, "buf contains: %lf\n", b->mVehicles[0].mEngineRPM);

    fprintf(stderr, "buf contains: %i\n", b->mVersion);
    fprintf(stderr, "buf contains: %i\n", b->mBuildVersionNumber);
    fprintf(stderr, "buf contains: %i\n", b->mGameState);
    fprintf(stderr, "buf contains: %i\n", b->mSessionState);
    fprintf(stderr, "buf contains: %i\n", b->mRaceState);

    FILE *outfile;

    //outfile = fopen ("/home/racerx/telemetry.dat", "w");
    //if (outfile == NULL)
    //{
    //    fprintf(stderr, "\nError opened file\n");
    //    exit (1);
    //}

    //fwrite (b, sizeof(SharedMemory1), 1, outfile);

    //if(fwrite != 0)
    //    printf("contents to file written successfully !\n");
    //else
    //    printf("error writing file !\n");

    //fclose (outfile);
#endif

    SetStdHandle(STD_INPUT_HANDLE, maph);
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.hStdInput = maph;
    si.dwFlags |= STARTF_USESTDHANDLES;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    if (!CreateProcess(argv[1], NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        fprintf(stderr, "failed to launch second helper process: %s\n", strerror(GetLastError()));
    }

    fprintf(stderr, "Mapped and sleeping forever Press Shift-E to stop");
    int key = 0;
    while(1)
    {
        if (_kbhit())
        {
            key =_getch();

            if (key == 'E')
                break;
        }
    }
}
#endif
