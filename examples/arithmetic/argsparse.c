/* Arguments Parsing
   Copyright (c) 2014-2015 Cheryl Natsu 
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   3. The name of the authors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   */

#include <stdio.h>
#include <string.h>

#include "argsparse.h"


int argsparse_init(argparse_t *argparse, \
        int argc, char **argv)
{
    argparse->idx = 1;
    argparse->argc = argc;
    argparse->argv = argv;

    return 0;
}

int argsparse_request(argparse_t *argparse, \
        char **p)
{
    if (argparse->idx >= argparse->argc)
    {
        return -1;
    }
    else
    {
        *p = (char *)argparse->argv[(size_t)argparse->idx];
        argparse->idx++;
    }

    return 0;
}


/* If argument available */
int argsparse_available(argparse_t *argparse)
{
    return (argparse->idx < argparse->argc) ? 1 : 0;
}

/* Finished parsing current argument */
int argsparse_next(argparse_t *argparse)
{
    argparse->idx++;

    return 0;
}


/* Match String */
int argsparse_match_str(argparse_t *argparse, const char *pat)
{
    char *s;
    size_t s_len, pat_len;

    s = (char *)argparse->argv[(size_t)argparse->idx];
    s_len = strlen(s);
    pat_len = strlen(pat);
    /* Length Comparing */
    if (s_len != pat_len) return 0;
    /* Content Comparing */
    if (strncmp(pat, s, pat_len) != 0) return 0;

    /* Match */
    return 1;
}


/* Match Prefix */
int argsparse_match_prefix(argparse_t *argparse, const char *pat)
{
    char *s;
    size_t s_len, pat_len;

    s = (char *)argparse->argv[(size_t)argparse->idx];
    s_len = strlen(s);
    pat_len = strlen(pat);
    /* Length Comparing */
    if (s_len < pat_len) return 0;
    /* Content Comparing */
    if (strncmp(pat, s, pat_len) != 0) return 0;

    /* Match */
    return 1;
}

/* Fetch whole argument */
char *argsparse_fetch(argparse_t *argparse)
{
    char *s;
    s = (char *)argparse->argv[(size_t)argparse->idx];
    return s;
}

/* Fetch string after prefix */
char *argsparse_fetch_prefix_str(argparse_t *argparse, const char *pat)
{
    char *s;
    size_t pat_len;

    s = (char *)argparse->argv[(size_t)argparse->idx];
    pat_len = strlen(pat);

    return s + pat_len;
}

