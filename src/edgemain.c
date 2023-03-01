/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
 */

#include <stdio.h>
#include "jobdefs.h"
#include "bottle.h"

char probname[100];

static char *in_filename;
static char *sched_file;
static int first_machine = 1;
static int inlimit;

int njobs, nmachs;
jobtype job[MAXJOBS];

main(ac, av)
int ac;
char **av;
{
    int newlimit;
    int i,j;
    
    parseargs (ac, av);

    printf ("filename: %s\n",in_filename);
    newgetprob (in_filename);
    french_loadprob(njobs,nmachs,job);

    if (sched_file) {
            getsched (sched_file);
    }

    newlimit = french_finish (job, inlimit);

    if (newlimit == -1) {
        printf ("lower bound = %d\n",inlimit+1);
    } else {
        printf ("optimal solution = %d\n",newlimit);
        for (i=0; i<num_of_jobs; i++) {
            for (j=0; j<num_of_machines; j++) {
                printf ("%d ",job[i].start[j]);
            }
            printf ("\n");
        }
    }
    exit (1);
}

parseargs(ac, av)
int ac;
char **av;
{
    int c;
    extern int optind;
    extern char *optarg;

    while ((c = getopt (ac, av, "s:0")) != EOF) switch (c) {
        case 's':
            sched_file = optarg;
            break;
        case '0':
            first_machine = 0;
            break;
        default:
            usage (*av);
    }
    if (optind >= ac)
        usage (*av);
    in_filename = av[optind++];
    if (optind >= ac)
        usage (*av);
    inlimit = atoi(av[optind++]);
    if (optind < ac)
        usage (*av);
}

usage (prog)
char *prog;
{
    fprintf (stderr, "Usage: %s [-s schedule_file] [-0] probfile bound\n",
                     prog);
    fprintf (stderr,
            "    -s: a file containing a list of machines to pre-schedule\n");
    fprintf (stderr, "    -0: machine and job #'s start from 0, not 1\n");
    exit (1);
}

getsched (f)
char *f;
{
    FILE *fin;
    int m;
    int i;
    int sched[MAXJOBS];

    if ((fin = fopen (f, "r")) == NULL) {
        perror (f);
        fprintf (stderr, "Unable to open schedule file for input\n");
        exit (1);
    }
    while (fscanf (fin, " %d ", &m) == 1) {
        m -= first_machine;
        for (i = 0; i < njobs; i++) {
            if (fscanf (fin, " %d ", &sched[i]) != 1) {
                fprintf (stderr, "Syntax error in schedule file\n");
                exit (1);
            }
            sched[i] -= first_machine;
        }
        french_schedmach (m, sched);
        printf ("machine %d pre-scheduled\n", m);
    }
    fclose (fin);
}

newgetprob (fname)
char *fname;
{
        FILE *in;
        int i, j, ptime, achine;

        if ((in = fopen (fname, "r")) == NULL) {
                fprintf (stderr, "Unable to open %s for input\n", fname);
                exit (1);
        }

        fgets (probname, 100, in);
        fscanf (in, "%d %d", &njobs, &nmachs);

        for (i = 0; i < njobs; i++)
                for (j = 0; j < nmachs; j++) {
                        fscanf (in, "%d %d", &achine, &ptime);
                        job[i].order[j] = achine - first_machine;
                        job[i].proctime[achine - first_machine] = ptime;
                        job[i].step[achine-first_machine] = j;
                }
        fclose (in);
}
