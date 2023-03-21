#ifndef SHELL_CHECK_INCLUDE
#define SHELL_CHECK_INCLUDE
#include <stdbool.h>
#include <sys/types.h>
#include "shellcheck_export.h"

typedef struct {
    pid_t procees;
    pid_t parent;
    pid_t session;
    pid_t parent_session;
} Details;

SC_EXPORT bool CheckIfRunningFromShell();
//TODO: add an interface for getting details about the guess.
//SC_EXPORT bool GetDetails(Details* output);
//SC_EXPORT bool ParentName(Details* details, char* output, int outSize);
//SC_EXPORT bool SessionName(Details* details, char* output, int outSize);
//SC_EXPORT bool ParentSessionName(Details* details, char* output, int outSize);

#endif
