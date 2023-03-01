/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
 */

#include <stdio.h>
#include "bottle.h"

evaluate (seq)
sequencetype *seq;
{
        job_mach qu[MAXJOBS * MAXMACHS];
        int i, j, ft, mach, newmach, nj, oj, evalcnt, make;

        magicnum++;
        evalcnt = ft = 0;
        for (i = 0; i < njobs; i++)
                for (j = 0; j < nmachs; j++)
                        job[i].etime[j] = job[i].ftime[j] = BIGINT;

        for (i = 0; i < njobs; i++) {
                mach = job[i].order[0];
                if (job[i].prev[mach] == -1) {
                        ft++;
                        qu[ft].job = i;
                        qu[ft].mach = mach;
                        job[i].etime[mach] = 0;
                        evalcnt++;
                } else
                        job[i].magic[mach] = magicnum;
        }
        for (j = 0; j < nmachs; j++)
                if (seq[j].job[0] == -1)
                        for (i = 0; i < njobs; i++)
                                job[i].magic[j] = magicnum;
                else
                        job[seq[j].job[0]].magic[j] = magicnum;
 
        while (ft > 0) {
                oj = qu[ft].job;
                mach = qu[ft].mach;
                ft--;
                nj = job[oj].next[mach];
                if (nj != -1) {
                        if (job[nj].magic[mach] == magicnum) {
                                ft++;
                                qu[ft].job = nj;
                                qu[ft].mach = mach;
                                if (job[nj].step[mach] > 0) {
                                        newmach = job[nj].order[job[nj].
                                                  step[mach] - 1];
                                        job[nj].etime[mach] =
                                           MAX(job[oj].etime[mach] +
                                               job[oj].proctime[mach],
                                               job[nj].etime[newmach] +
                                               job[nj].proctime[newmach]);
                                } else
                                       job[nj].etime[mach]=job[oj].etime[mach]
                                                     + job[oj].proctime[mach];
                                evalcnt++;
                        } else
                                job[nj].magic[mach] = magicnum;
                }
                if (job[oj].step[mach] < nmachs - 1) {
                        newmach = job[oj].order[job[oj].step[mach] + 1];
                        if (job[oj].magic[newmach] == magicnum) {
                                ft++;
                                qu[ft].job = oj;
                                qu[ft].mach = newmach;
                                nj = job[oj].prev[newmach];
                                if (nj != -1)
                                        job[oj].etime[newmach] =
                                           MAX(job[nj].etime[newmach] +
                                               job[nj].proctime[newmach],
                                               job[oj].etime[mach] +
                                               job[oj].proctime[mach]);
                                else
                                        job[oj].etime[newmach] =
                                                job[oj].etime[mach] +
                                           job[oj].proctime[mach];
                                evalcnt++;
                        } else
                                job[oj].magic[newmach] = magicnum;
                }
        }
        cycflag = evalcnt != njobs * nmachs;

        /* now for the f times */

        magicnum++;
        ft = 0;
        for (i = 0; i < njobs; i++) {
                mach = job[i].order[nmachs - 1];
                if (job[i].next[mach] == -1) {
                        ft++;
                        qu[ft].job = i;
                        qu[ft].mach = mach;
                        job[i].ftime[mach] = 0;
                } else
                        job[i].magic[mach] = magicnum;
        }
        for (j = 0; j < nmachs; j++)
                if (seq[j].job[njobs - 1] == -1)
                        for (i = 0; i < njobs; i++)
                                job[i].magic[j] = magicnum;
                else
                        job[seq[j].job[njobs - 1]].magic[j] = magicnum;

        while (ft > 0) {
                oj = qu[ft].job;
                mach = qu[ft].mach;
                ft--;
                nj = job[oj].prev[mach];
                if (nj != -1) {
                        if (job[nj].magic[mach] == magicnum) {
                                ft++;
                                qu[ft].job = nj;
                                qu[ft].mach = mach;
                                if (job[nj].step[mach] < nmachs - 1) {
                                        newmach = job[nj].order[job[nj].
                                                  step[mach] + 1];
                                        job[nj].ftime[mach] =
                                           MAX(job[nj].ftime[newmach] +
                                               job[nj].proctime[newmach],
                                               job[oj].ftime[mach] +
                                               job[oj].proctime[mach]);
                                } else
                                       job[nj].ftime[mach]=job[oj].ftime[mach]
                                                     + job[oj].proctime[mach];
                        } else
                                job[nj].magic[mach] = magicnum;
                }
                if (job[oj].step[mach] > 0) {
                        newmach = job[oj].order[job[oj].step[mach] - 1];
                        if (job[oj].magic[newmach] == magicnum) {
                                ft++;
                                qu[ft].job = oj;
                                qu[ft].mach = newmach;
                                nj = job[oj].next[newmach];
                                if (nj != -1)
                                        job[oj].ftime[newmach] =
                                           MAX(job[nj].ftime[newmach] +
                                               job[nj].proctime[newmach],
                                               job[oj].ftime[mach] +
                                               job[oj].proctime[mach]);
                                else
                                        job[oj].ftime[newmach] =
                                           job[oj].ftime[mach] +
                                           job[oj].proctime[mach];
                        } else
                                job[oj].magic[newmach] = magicnum;
                }
        }
        make = job[0].etime[job[0].order[nmachs - 1]] +
                job[0].proctime[job[0].order[nmachs - 1]];
        for (i = 1; i < njobs; i++)
                make = MAX(make, job[i].etime[job[i].order[nmachs - 1]] +
                                 job[i].proctime[job[i].order[nmachs - 1]]);

        return make;
}

setseq (seq, mach, order)
sequencetype *seq;
int mach, *order;
{
        int i;

        for (i = 0; i < njobs - 1; i++) {
                seq[mach].job[i] = order[i];
                job[order[i]].next[mach] = order[i+1];
                job[order[i+1]].prev[mach] = order[i];
        }
        seq[mach].job[njobs-1] = order[njobs-1];
        job[order[njobs-1]].next[mach] = -1;
        job[order[0]].prev[mach] = -1;
}

clearseq (seq, mach, save)
sequencetype *seq;
int mach, *save;
{
        int i;

        for (i = 0; i < njobs; i++) {
                if (save)
                        save[i] = seq[mach].job[i];
                seq[mach].job[i] = -1;
                job[i].next[mach] = -1;
                job[i].prev[mach] = -1;
        }
}

