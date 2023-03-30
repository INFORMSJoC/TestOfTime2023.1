/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
 */

/*
MIT License

Copyright (c) 2023 David Applegate and William Cook

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <math.h>
#include "bottle.h"

jobtype job[MAXJOBS];
int njobs, nmachs;
char probname[100];
int magicnum = 0;
int cycflag;
int bestmake = BIGINT;
int simplebottle = 0;
filename etimesfilename;
char *fname, *processname = NULL;;

main (ac, av)
int ac;
char **av;
{
        parseargs (ac, av);
        setupnames (fname, processname);
        getprob (fname);
        showprob ();
        testbottle ();
        writesol ();
}

parseargs (ac, av)
int ac;
char **av;
{
        int c;
        extern int optind;
        extern char *optarg;

        while ((c = getopt (ac, av, "n:s")) != EOF)
                switch (c) {
                case 'n': processname = optarg; break;
                case 's': simplebottle++; break;
                case '?':
                default:
                        usage (av[0]);
                        break;
                }
        if (optind >= ac)
                usage (av[0]);
        fname = av[optind++];
}
 
usage (f)
char *f;
{
        fprintf (stderr, "Usage %s: [-n: process_name] [-s] jobshop_file\n",
                         f);  
        fprintf (stderr, "      -s simple run\n");
        exit (1);
}
