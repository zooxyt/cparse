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

#ifndef _ARGSPARSE_H_
#define _ARGSPARSE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Data structure for storing the parsing state */
typedef struct 
{
    int argc;
    char **argv;
    int idx;
} argparse_t;

/* Initialize argument parsing */
int argsparse_init(argparse_t *argparse, \
        int argc, char **argv);
/* Request an argument in char* style */
int argsparse_request(argparse_t *argparse, \
        char **p);

/* If argument available */
int argsparse_available(argparse_t *argparse);
/* Finished parsing current argument */
int argsparse_next(argparse_t *argparse);

/* Match String */
int argsparse_match_str(argparse_t *argparse, const char *pat);
/* Match Prefix */
int argsparse_match_prefix(argparse_t *argparse, const char *pat);
/* Fetch whole argument */
char *argsparse_fetch(argparse_t *argparse);
/* Fetch string after prefix */
char *argsparse_fetch_prefix_str(argparse_t *argparse, const char *pat);


#ifdef __cplusplus
}
#endif

#endif

