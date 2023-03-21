#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "shell-check.h"

static bool ready = false;
static Details m_Details;
static char* realShellPath = NULL;

// list of shells to check as a fallback if it doesn't match the user's SHELL variable.
// most commonly this is caused by a script running from a shebang interpeter
// and usually this is /bin/bash or /bin/sh, so that's all we *should* have to check.
static const char* shells[] = {"/bin/bash", "/bin/sh"};

void cleanup()
{
    if (realShellPath != NULL) {
        free(realShellPath);
        realShellPath = NULL;
    }
}

bool failed()
{
    cleanup();
    return false;
}

bool passed()
{
    cleanup();
    return true;
}

char* readProcFile(const char* file, pid_t process, ssize_t* fileSize)
{
    if (file == NULL) {
        return 0;
    }
    int readSize = 15 + strlen(file);
    char* filename = malloc(readSize);
    snprintf(filename, readSize, "/proc/%d/%s", process, file);

    FILE* cmdFile = fopen(filename, "r");
    free(filename);
    if (cmdFile == NULL) {
        return 0;
    }
    struct stat s;
    int fd = fileno(cmdFile);

    int allocSize = 100;
    char* tmp = malloc(allocSize);

    ssize_t fsize = 0, tmpSize = 0;
    int offset = 0;
    char* tmpBuffer[100];
    while ((tmpSize = pread(fd, tmp+offset, 100, offset)) == 100) {
        allocSize += 100;
        offset += 100;
        tmp = realloc(tmp, allocSize);
        fsize += tmpSize;
    }
    fsize += tmpSize;
    fclose(cmdFile);

    if (fileSize != NULL) {
        *fileSize = fsize;
    }
    return tmp;
}

char* followProcLink(const char* file, pid_t process)
{
    if (file == NULL) {
        return NULL;
    }
    int readSize = 15 + strlen(file);
    char* buffer = malloc(readSize);
    snprintf(buffer, readSize, "/proc/%d/%s", process, file);
    int fileNameLen = strlen(buffer);
    return realloc(buffer, fileNameLen);
}

bool comparePathToShell(char* path)
{
    char* resolvedPath = realpath(path, NULL);
    bool result = (strcmp(realShellPath, resolvedPath) == 0);
    if (result) {
        // matches user's specified SHELL variable
        free(resolvedPath);
        return result;
    }

    //
    int shellCount = (sizeof (shells)/sizeof (char*));
    for (int i = 0 ; i < shellCount; ++i) {
        char* shellPath = realpath(shells[i], NULL);
        result = (strcmp(shellPath, resolvedPath) == 0);
        free(shellPath);
    }

    free(resolvedPath);
    return result;
}

bool checkIfPidIsAShell(pid_t pid)
{
    char* processPath = followProcLink("exe", m_Details.parent);
    bool result = comparePathToShell(processPath);
    free(processPath);
    if (result) {
        return true;
    }

    char* fileContents = readProcFile("cmdline", m_Details.parent, NULL);
    result = comparePathToShell(fileContents);
    free(fileContents);

    return result;
}

// Checks first if the program is running in a TTY.
// If that fails (i.e. because i/o are being captured),
// then it checks if the parent process or session owner
// is a shell.
bool CheckIfRunningFromShell()
{
    m_Details.procees = 0;
    m_Details.parent = 0;
    m_Details.session = 0;
    m_Details.parent_session = 0;

    if (isatty(fileno(stdin)) && isatty(fileno(stdout))) {
        // stdin and stdout are interactive - definitely in a shell
        return passed();
    }

    m_Details.procees = getpid();
    m_Details.parent = getppid();
    pid_t session = getsid(m_Details.procees);
    if (session == -1) {
        if (errno == EPERM) {
            fprintf(stderr, "Insufficient permissions to get sessions.");
        } else {
            fprintf(stderr, "Unknown error getting session.");
        }
        return failed();
    }
    m_Details.session = session;
    pid_t parentSession = getsid(m_Details.parent);
    if (parentSession == -1) {
        if (errno == EPERM) {
            fprintf(stderr, "Insufficient permissions to get parent session.");
        } else {
            fprintf(stderr, "Unknown error geting parent session.");
        }
        return failed();
    }
    m_Details.parent_session = parentSession;

    if (realShellPath == NULL) {
        char* shell = getenv("SHELL");
        realShellPath = realpath(shell, NULL);
        if (realShellPath == NULL) {
            fprintf(stderr, "Invalid SHELL variable.");
            return failed();
        }
    }

    if (checkIfPidIsAShell(m_Details.parent) || checkIfPidIsAShell(m_Details.session)) {
        return passed();
    }

    return failed();
}
