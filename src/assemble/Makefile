CC	=	gcc
CFLAGS	=	-g -Wall
BUILD	=	assemble
OBJS	=	assemble.o filewriter.o filereader.o symtable.o parser.o tokenizer.o funtable.o binbuilder.o util.o
PATH_TO_TEST = "PATH TO TESTSUITE"

all:	$(BUILD)

clean:
	/bin/rm -f $(BUILD) $(OBJS) core a.out .nfs*

assemble:	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o assemble $(LDLIBS)
	cp assemble $(PATH_TO_TEST)/solution
	cd $(PATH_TO_TEST) && make

assemble.o:	assemble.c assemble.h filewriter.h filereader.h symtable.h
filewriter.o:	filewriter.c filewriter.h
filereader.o:	filereader.c filereader.h parser.h
symtable.o:	symtable.c symtable.h
parser.o:	parser.c parser.h tokenizer.h funtable.h
tokenizer.o:	tokenizer.c tokenizer.h
funtable.o:	funtable.c funtable.h binbuilder.h
binbuilder.o:	binbuilder.c binbuilder.h util.h
util.o:	util.c util.h
