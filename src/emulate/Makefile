CC	=	gcc
CFLAGS	=	-g -Wall
BUILD	=	emulate
OBJS	=	emulate.o filewriter.o processor.o dataprocess.o datatransfer.o branch.o
PATH_TO_TEST = "PATH TO TESTSUITE"

all:	$(BUILD)

clean:
	/bin/rm -f $(BUILD) $(OBJS) core a.out .nfs*

emulate:	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o emulate $(LDLIBS)
	cp emulate $(PATH_TO_TEST)solution
	cd $(PATH_TO_TEST) && make

emulate.o:	emulate.c emulate.h filewriter.h processor.h dataprocess.h datatransfer.h branch.h
filewriter.o:	filewriter.c filewriter.h
processor.o:	processor.c processor.h
dataprocess.o:	dataprocess.c dataprocess.h
datatransfer.o:	datatransfer.c dataprocess.h
branch.o:	branch.c branch.h
