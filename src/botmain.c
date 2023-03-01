/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
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
