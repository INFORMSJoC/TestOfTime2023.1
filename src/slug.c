/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
 */

#include <stdio.h>
#include "bottle.h"

int njobs, nmachs;
jobtype job[MAXJOBS];
sequencetype seq[MAXMACHS];
char *probfile, *timefile;
char probname[100];
int subset = 0;
int wanted = 0;
int bound = -1;
int cycflag, magicnum = 0;
int usemax = 0;
int choice = -1;

main (ac, av)
int ac;
char **av;
{
        int i, j, k, val, make, round = 0;
        int  select[MAXMACHS], order[MAXJOBS], avail[MAXMACHS];

        parseargs (ac, av);
        getprob (probfile);
        gettimes (timefile);

        if (wanted > nmachs) {
                fprintf (stderr, "Sorry pal, you can get %d machs\n", wanted);
                exit (1);
        }
        if (choice != -1)
                wanted = 1;
        if (!wanted)
                wanted = nmachs/3;
        for (i = 0; i < nmachs; i++)
                avail[i] = 1;

        do {
                printf ("\nRound %d ....\n", round);
                buildsequences ();
                make = evaluate (seq);
                printf ("MAKESPAN: %d\n", make);
                selectmachs (wanted, select, avail);
                if (wanted <= nmachs/2) {
                        for (i = 0; i < nmachs; i++)
                                avail[i] = 1;
                        for (i = 0; i < wanted; i++)
                                avail[select[i]] = 0;
                }
                buildsequences ();
                french_loadprob (njobs, nmachs, job);
                for (i = 0; i < wanted; i++) {
                        j = select[i];
                        for (k = 0; k < njobs; k++)
                                order[k] = seq[j].job[k];
                        french_schedmach (j, order);
                }
                if (round == 0 && bound != -1)
                        val = french_finish (job, bound);
                else        
                        val = french_finish (job, make - 1);
                round++;
        } while (val != -1 && choice == -1);

        printf ("No more improvement. Number of rounds: %d\n", round);
        exit (1);
}

selectmachs (k, select, avail)
int k, *select, *avail;
{
        int i, j, l, min, minj, make, chosen[MAXMACHS], order[MAXJOBS];

        if (choice != -1) {
                select[0] = choice;
                clearseq (seq, choice, NULL);
                return;
        }

        for (j = 0; j < nmachs; j++)
                chosen[j] = 0;

        for (l = 0; l < k; l++) {
                min = usemax ? 0 : BIGINT;
                for (j = 0; j < nmachs; j++) {
                        if (!chosen[j] && avail[j]) {
                                clearseq (seq, j, order);
                                make = evaluate (seq);
                                setseq (seq, j, order);
                                if ((usemax && make > min) ||
                                    (!usemax && make < min)) {
                                        min = make;
                                        minj = j;        
                                }
                        }
                }
                printf ("%s: %d  make: %d\n", usemax ? "max" : "min",
                        min, minj);
                chosen[minj] = 1;
                select[l] = minj;
                clearseq (seq, minj, NULL);
        }
}
                        

gettimes (f)
char *f;
{
        FILE *in;
        int i, j;

        if ((in = fopen (f, "r")) == NULL) {
                fprintf (stderr, "Unable to open %s for input\n", f);
                exit (1);
        }

        fscanf (in, "%d %d", &njobs, &nmachs);
        for (i = 0; i < njobs; i++) 
                for (j = 0; j < nmachs; j++)
                        fscanf (in, "%d ", &(job[i].start[j]));
        fclose (in);
}

getprob (f)
char *f;
{
        FILE *in;
        int i, j, ptime, machine;
 
        if ((in = fopen (f, "r")) == NULL) {
                fprintf (stderr, "Unable to open %s for input\n", f);
                exit (1);
        }
 
        fgets (probname, 100, in);
        fscanf (in, "%d %d", &njobs, &nmachs);
 
        for (i = 0; i < njobs; i++)
                for (j = 0; j < nmachs; j++) {
                        fscanf (in, "%d %d", &machine, &ptime);
                        job[i].order[j] = machine;
                        job[i].proctime[machine] = ptime;
                        job[i].step[machine] = j;
                }
        fclose (in);
}


buildsequences ()
{
        int i, j, k, min, mini, current, order[MAXJOBS];

        for (j = 0; j < nmachs; j++) {
                current = -1;
                for (i = 0; i < njobs; i++) {
                        min = BIGINT;
                        for (k = 0; k < njobs; k++) {
                                if (job[k].start[j] > current &&
                                    job[k].start[j] < min) {
                                        min = job[k].start[j];
                                        mini = k;
                                }
                        }
                        order[i] = mini;
                        current = min;
                }
                setseq (seq, j, order);        
        }
}

dumpsequences (wanted, select)
int wanted, *select;
{        
        FILE *out;
        int i, j, k;

        if ((out = fopen ("seq.out", "w")) == NULL) {
                fprintf (stderr, "Unable to open seq.out for output\n");
                exit (1);
        }

        for (i = 0; i < wanted; i++) {
                j = select[i];
                fprintf (out, "%2d ", j);
                for (k = 0; k < njobs; k++)
                        fprintf (out, "%2d ", seq[j].job[k]);
                fprintf (out, "\n");
        }

        fclose (out);
}

printsequences ()
{
        int i, j;

        for (j = 0; j < nmachs; j++) {
                printf ("%d: ", j);
                for (i = 0; i < njobs; i++)
                        printf ("%d ", seq[j].job[i]);
                printf ("\n");
        }
}

printstarts ()
{
        int i, j;

        for (i = 0; i < njobs; i++) {
                printf ("%d:", i);
                for (j = 0; j < nmachs; j++)
                        printf (" %d", job[i].start[j]);
                printf ("\n");
        }
}

parseargs (ac, av)
int ac;
char **av;
{
        int c;
        extern int optind;
        extern char *optarg;

        while ((c = getopt (ac, av, "n:b:ro:")) != EOF)
                switch (c) {
                case 'n': wanted = atoi(optarg); break;
                case 'b': bound = atoi(optarg); break;
                case 'r': usemax++; break;
                case 'o': choice = atoi(optarg); break;
                case '?':
                default:
                        usage (av[0]);
                        break;
                }
        if (optind >= ac - 1)
                usage (av[0]);
        probfile = av[optind++];
        timefile = av[optind++];
}

usage (f)
char *f;
{
        fprintf (stderr, "Usage %s: [-n integer] [-b integer] [-o integer] \
[-r] prob_file etimes_file\n", f);
        fprintf (stderr, "    -n: number of machines to fix\n");
        fprintf (stderr, "    -b: bound on the solution wanted\n");
        fprintf (stderr, "    -o: fix only this machine\n");
        fprintf (stderr, 
            "    -r: reverse the rule for choosing the machines to fix\n");
        exit (1);
}
