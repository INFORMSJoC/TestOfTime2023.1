#
# This is part of JOBSHOP, a set of codes for job-shop scheduling by
# David Applegate and Bill Cook.  The codes are based on "A computational
# study of the job-shop scheduling problem" to appear in the ORSA Journal
# on Computing.
#

CC=cc
CFLAGS = -O

NEWJOBOBJS = edgemain.o edgesub.o edge.o jobsort.o preempt.o lbound.o \
             printp.o init.o edges.o
DUMBOBJS = dumbo.o jobsort.o preempt.o lbound.o getprob.o printp.o \
           init.o edges.o
SLUGOBJS = slug.o jobsort.o preempt.o lbound.o printp.o init.o edges.o \
           edge.o edgesub.o boteval.o
ONEMACHOBJS=onemain.o onemach.o 
BOTTLEOBJS=botmain.o bottle.o boteval.o onemach.o

all: dumbo slug edge onemach bottle

edge: ${NEWJOBOBJS}
	${CC} ${CFLAGS} -o edge ${NEWJOBOBJS}

dumbo: ${DUMBOBJS}
	${CC} ${CFLAGS} -o dumbo ${DUMBOBJS}

slug: $(SLUGOBJS)
	${CC} ${CFLAGS} -o slug ${SLUGOBJS}

onemach: $(ONEMACHOBJS)
	$(CC) $(CFLAGS) -o onemach $(ONEMACHOBJS)

bottle: $(BOTTLEOBJS)
	$(CC) $(CFLAGS) -o bottle $(BOTTLEOBJS) -lm

dumbo.o: jobdefs.h
edges.o: jobdefs.h
edge.o: jobdefs.h
edgesub.o: jobdefs.h bottle.h
getprob.o: jobdefs.h
init.o: jobdefs.h
jobsort.o: jobdefs.h
lbound.o: jobdefs.h
edgemain.o: jobdefs.h bottle.h
preempt.o: jobdefs.h
printp.o: jobdefs.h
slug.o: bottle.h
boteval.o: bottle.h
onemain.o: bottle.h
onemach.o: bottle.h
botmain.o: bottle.h
bottle.o: bottle.h

clean:
	rm -f *.o

lint:
	lint *.c
