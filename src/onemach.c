/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
 */

#include <stdio.h>
#include "bottle.h"

/*                                                */
/*   CARLIER'S ONE-MACHINE SEQUENCING ALGORITHM   */
/*                                                */

onemach (one, bestorder)
onemach_times one;
int *bestorder;
{
        onemach_times currentone, bbnodes[ONEMACH_BBNODES];
        onemach_bbpoint bbpoint[ONEMACH_BBNODES];
        int order[MAXJOBS];
        int make, onemake, lbound, lbound2, bestbound, d, e;
        int active_cjob, active_pjob;
        int jset[MAXJOBS], jset_size;
        int cjob, pjob, pt, ppt, last, i, got_c;

        for (i = 0; i < njobs; i++)
                bestorder[i] = 0;
        for (i = 0; i < ONEMACH_BBNODES; i++)
                bbpoint[i].active = bbpoint[i].bound = 0;
        onemake = BIGINT;
        last = 0;
        bbpoint[0].active = 1;
        bbpoint[0].bound = 0;
        copy_onemach_times (&(bbnodes[0]), &one);
MUNCHNODE:
        bestbound = onemake;
        pt = 0;
        for (ppt = 0; ppt <= last; ppt++)
                if (bbpoint[ppt].active)
                        if (bbpoint[ppt].bound < bestbound) {
                                bestbound = bbpoint[ppt].bound;
                                pt = ppt;
                        }
        if (bestbound == onemake)
                return onemake;

        copy_onemach_times (&currentone, &(bbnodes[pt]));

        bbpoint[pt].active = 0;
        mwr_schedule (currentone, order);
        got_c = makespan (currentone, order, jset, &jset_size, &cjob,
                                                               &pjob, &make);
        if (make < onemake) {
                onemake = make;
                for (i = 0; i < njobs; i++)
                        bestorder[i] = order[i];
        }
        if (!got_c)
                goto MUNCHNODE;
        lbound = lowerbound (currentone, jset, jset_size);
        jset[jset_size] = order[cjob];
        jset_size++;
        lbound2 = lowerbound (currentone, jset, jset_size);
        lbound = MAX(lbound2, lbound);
        jset_size--;
        if (lbound >= onemake)
                goto  MUNCHNODE;
        
        active_cjob = order[cjob];
        active_pjob = order[pjob];

        for (ppt = cjob; ppt <= pjob - 1; ppt++)
                order[ppt] = order[ppt + 1];

        order[pjob] = active_cjob;
        make = make_value (currentone, order);
        if (make < onemake) {
                for (i = 0; i < njobs; i++)
                        bestorder[i] = order[i];
                onemake = make;
                if (onemake == bestbound)
                        return onemake;
                if (lbound >= onemake)
                        goto MUNCHNODE;
        }
        bbpoint[pt].active = 1;
        bbpoint[pt].bound = MAX(lbound, bestbound);
        copy_onemach_times (&(bbnodes[pt]), &currentone);
        
        d = currentone.ftime[active_pjob];
        for (ppt = 0; ppt < jset_size; ppt++)
                d += currentone.proctime[jset[ppt]];
        if (currentone.ftime[active_cjob] < d)
                bbnodes[pt].ftime[active_cjob] = d;
        pt = 0;
        while (bbpoint[pt].active)
                pt++;
        bbpoint[pt].active = 1;
        bbpoint[pt].bound = MAX(lbound, bestbound);
        copy_onemach_times (&(bbnodes[pt]), &currentone);

        d = currentone.etime[jset[0]];
        e = currentone.proctime[jset[0]];
        for (ppt = 1; ppt < jset_size; ppt++) {
                d = MAX(currentone.etime[jset[ppt]], d);
                e += currentone.proctime[jset[ppt]];
        }

        if (currentone.etime[active_cjob] < d + e)
                bbnodes[pt].etime[active_cjob] = d + e;
        if (last < pt)
                last = pt;
        goto MUNCHNODE;
}

/*                                              */
/*    MOST WORK REMAINING SCHEDULE (SCHRAGE)    */
/*                                              */

mwr_schedule (one, order)
onemach_times one;
int *order;
{
        int ready[MAXJOBS], scheduled[MAXJOBS];
        int i, t, q, nscheduled = 0, next;

        for (i = 0; i < njobs; i++) 
                ready[i] = scheduled[i] = 0;        

        t = one.etime[0];
        for (i = 1; i < njobs; i++) 
                if (one.etime[i] < t)
                        t = one.etime[i];

        do {
                for (i = 0; i < njobs; i++)
                        if (one.etime[i] <= t)
                                ready[i] = 1;

                q = -1;
                for (i = 0; i < njobs; i++)
                        if (ready[i] && !scheduled[i])
                                if (q < one.ftime[i]) {
                                        q = one.ftime[i];
                                        next = i;
                                }
                order[nscheduled++] = next;
                scheduled[next] = 1;
                if (nscheduled < njobs) {
                        t = t + one.proctime[next];
                        q = BIGINT;
                        for (i = 0; i < njobs; i++)
                                if (!scheduled[i])
                                        if(q > one.etime[i])
                                                q = one.etime[i];
                        if (t < q)
                                t = q;
                }
        } while (nscheduled < njobs);
}

/*                                         */
/*    MAKESPAN AND CRITICAL SET AND JOB    */
/*                                         */

makespan (one, order, jset, jset_size, cjob, pjob, make)
onemach_times one;
int *order, *jset, *jset_size, *cjob, *pjob, *make;
{
        int length[MAXJOBS];
        int i, j, pt;

        length[0] = one.etime[order[0]] + one.proctime[order[0]];
        *make = length[0] + one.ftime[order[0]];
        *pjob = 0;
        for (i = 1; i < njobs; i++) {
                length[i] = MAX(length[i-1], one.etime[order[i]]) +
                                             one.proctime[order[i]];
                if (*make <= length[i] + one.ftime[order[i]]) {
                        *make = length[i] + one.ftime[order[i]];
                        *pjob = i;
                }
        }
        for (pt = *pjob - 1; pt >= 0; pt--) {
                if (one.etime[order[pt+1]] > length[pt]) {
                        return 0;
                }
                if (one.ftime[order[pt]] < one.ftime[order[*pjob]])
                        break;
        }
        if (pt >= 0) {
                *cjob = pt;
                *jset_size = 0;
                for (i = *cjob + 1; i <= *pjob; i++) {
                        jset[*jset_size] = order[i];
                        (*jset_size)++;
                }
                return 1;
        } else
                return 0;
}

lowerbound (one, jset, jset_size)
onemach_times one;
int *jset, jset_size;
{
        int t, i, lb, mine, minf;

        if (!jset_size)
                return 0;
        t = jset[0];
        lb = one.proctime[t];
        mine = one.etime[t];
        minf = one.ftime[t];

        for (i = 1; i < jset_size; i++) {
                t = jset[i];
                if (one.etime[t] < mine)
                        mine = one.etime[t];
                if (one.ftime[t] < minf)
                        minf = one.ftime[t];
                lb += one.proctime[t];
        }
        return (lb + mine + minf);
}

make_value (one, order)
onemach_times one;
int *order;
{
        int head, value, i;

        head = one.etime[order[0]];
        value = head + one.proctime[order[0]] + one.ftime[order[0]];

        for (i = 1; i < njobs; i++) {
                head = MAX(head + one.proctime[order[i-1]],
                           one.etime[order[i]]);
                value = MAX(value, head + one.proctime[order[i]] +
                                                   one.ftime[order[i]]);
        }
        return value;
}

copy_onemach_times (first, second)
onemach_times *first, *second;
{
        int i;

        for (i = 0; i < njobs; i++) {
                first->etime[i] = second->etime[i];
                first->proctime[i] = second->proctime[i];
                first->ftime[i] = second->ftime[i];
        }
}
        
getonemach (f, one)
char *f;
onemach_times *one;
{
        FILE *in;
        int i;

        if (f == NULL) {
                printf ("Enter number of jobs: ");
                scanf ("%d", &njobs);
                printf ("Enter etimes: ");
                for (i = 0; i < njobs; i++)
                        scanf ("%d", &(one->etime[i]));
                printf ("Enter proctimes: ");
                for (i = 0; i < njobs; i++)
                        scanf ("%d", &(one->proctime[i]));
                printf ("Enter ftimes: ");
                for (i = 0; i < njobs; i++)
                        scanf ("%d", &(one->ftime[i]));
                printf ("\n");
        } else {
                if ((in = fopen (f, "r")) == NULL) {
                        fprintf (stderr, "Unable to open %s for input\n", f);
                        exit (1);
                }
                fscanf (in, "%d", &njobs);
                for (i = 0; i < njobs; i++)
                        fscanf (in, "%d", &(one->etime[i]));
                for (i = 0; i < njobs; i++)
                        fscanf (in, "%d", &(one->proctime[i]));
                for (i = 0; i < njobs; i++)
                        fscanf (in, "%d", &(one->ftime[i]));

                fclose (in);
        }
}

showonemach (order, value, one)
int *order, value;
onemach_times one;
{
        int i;

        printf ("MAKESPAN: %d\n", value);
        printf ("JOB ORDER: ");
        for (i = 0; i < njobs; i++)
                printf ("%d ", order[i]);
        printf ("\n");
        printf ("ORIGINAL DATA:\n");
        for (i = 0; i < njobs; i++)
                printf ("%3d ", one.etime[i]);
        printf ("\n");
        for (i = 0; i < njobs; i++)
                printf ("%3d ", one.proctime[i]);
        printf ("\n");
        for (i = 0; i < njobs; i++)
                printf ("%3d ", one.ftime[i]);
        printf ("\n");
}
        
dumporder (order)
int *order;
{
        int i;

        printf ("ORDER: ");
        for (i = 0; i < njobs; i++)
                printf ("%d ", order[i]);
        printf ("\n");
}
