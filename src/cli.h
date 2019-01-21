#ifndef _CLI_H_
#define _CLI_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "const.h"
#include "get-tmux.h"
#include "dir.h"

void tmux(const char*, char*);
void cleaner(char*);

void usage(char*);
void err_msg(char*, int);

void defaultize();
void customize(int, char**);

#endif // _CLI_H_