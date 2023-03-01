/*
 * This is part of JOBSHOP, a set of codes for job-shop scheduling by
 * David Applegate and Bill Cook.  The codes are based on "A computational
 * study of the job-shop scheduling problem" to appear in the ORSA Journal
 * on Computing.
 */

#define FSIZE 4
#define FVALUES {4,3,2,1}
#define MAXJOBS 30
#define MAXMACHS 30
#define ONEMACH_BBNODES 300
#define BIGINT 10000
#define MAX(a,b) ((a) > (b) ? (a) : (b))

typedef struct jobtype {
        int     order[MAXMACHS];
        int     proctime[MAXMACHS];
        int         step[MAXMACHS];
        int        start[MAXMACHS];
        int        etime[MAXMACHS];
        int     ftime[MAXMACHS];
        int     magic[MAXMACHS];
        int        next[MAXMACHS];
        int     prev[MAXMACHS];
} jobtype;

typedef struct sequencetype {
        int        job[MAXJOBS];
} sequencetype;

typedef struct blisttype {
        int        mach;
        int     make;
        int        order[MAXJOBS];
} blisttype;

typedef struct neighbor {
        int        next[MAXMACHS];
        int        prev[MAXMACHS];
} neighbor;

typedef struct motype {
        int        size;
        int     machs[MAXMACHS];
} motype;

typedef struct onemach_times {
        int        etime[MAXJOBS];
        int        proctime[MAXJOBS];
        int        ftime[MAXJOBS];
} onemach_times;

typedef struct onemach_bbpoint {
        int         active;
        int        bound;
} onemach_bbpoint;

typedef struct job_mach {
        int     job;
        int        mach;
} job_mach;

#define FILENAMESIZE 100
typedef char filename[FILENAMESIZE];

extern jobtype job[MAXJOBS];
extern int njobs, nmachs;
extern char probname[100];
extern int magicnum;
extern filename etimesfilename;
extern int cycflag, bestmake, simplebottle;
