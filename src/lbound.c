/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
 */

#include "jobdefs.h"

lbound (bndloc)
machine **bndloc;
{
    int i, bnd = 0, v;
    machine *m;

    for (i=0, m=schedule; i<num_of_machines; i++,m++) {
        v = preemptive(m);
        if (v > bnd) {
            bnd = v;
            if (bndloc) *bndloc = m;
        }
    }
    return (bnd);
}
