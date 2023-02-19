#include <windows.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#ifdef ASSETTOCORSA
#include "simapi/simapi/ac.h"
#include "simapi/include/acdata.h"

LPCSTR filefind = "acpmf_*";

int getmemfilesize(const char* filename)
{
    if(strcmp(filename, AC_CREWCHIEF_FILE) == 0)
    {
        return sizeof(struct SPageFileCrewChief);
    }

    return 2048;
}
#endif
#ifdef RFACTOR2
#include "simapi/simapi/rf2.h"
#include "simapi/include/rf2data.h"

LPCSTR filefind = "rFactor*";

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

int main(int argc, char** argv)
{
    WIN32_FIND_DATA fdata;
    HANDLE ffhandle;
    if (chdir("/dev/shm") != 0)
    {
        printf("Could not change directory to /dev/shm: %s\n", strerror(errno));
        return 1;
    }
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

    do
    {
        HANDLE maph;
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
    }
    while (FindNextFile(ffhandle, &fdata));

    printf("Done! Sleeping forever to keep objects alive, press Ctrl-C to stop\n");
    while (1)
    {
        Sleep(1000000);
    }
    return 0;
}
