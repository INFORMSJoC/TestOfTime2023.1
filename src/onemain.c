/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
 */

#include <stdio.h>
#include "bottle.h"

int njobs;
int ready[MAXJOBS];
int scheduled[MAXJOBS];
int readynum = 0, schedulednum = 0;

main (ac, av)
int ac;
char **av;
{
        onemach_times test;
        int order[MAXJOBS], val, i;

        if (ac > 1)
                getonemach (av[1], &test);
        else
                getonemach ((char *) NULL, &test);

        for (i = 0; i < njobs; i++)
                ready[i] = scheduled[i] = 0;

        val = onemach (test, order);
        showonemach (order, val, test);
}
