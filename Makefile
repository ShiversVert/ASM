TARGET=as-mips
TARGET_ELF=build/libpelf.so
DIRNAME=`basename $(PWD)`
CC=`which gcc`
LD=`which gcc`
RM=`which rm` -f
DOXYGEN=`which doxygen`

SRCDIR=src
SRCDIR_ELF=src/pelf
INCDIR=include
INCDIR_ELF=include/pelf
TESTDIR=testing
DOCDIR=doc

GARBAGE=*~ $(SRCDIR)/*~ $(INCDIR)/*~ $(TESTDIR)/*~

INCLUDE=-I$(INCDIR)
INCLUDE_ELF=-I$(INCDIR_ELF)

# Pour activer les sorties INFO_MSG, ajouter -DVERBOSE aux CFLAGS 
CFLAGS=-std=c99 $(INCLUDE)
LFLAGS=-lm
LFLAGS_ELF = -fPIC

CFLAGS_ELF=$(CFLAGS) -w
CFLAGS_DBG=$(CFLAGS) -g -DDEBUG -Wall
CFLAGS_RLS=$(CFLAGS)

SRC=$(wildcard $(SRCDIR)/*.c)
SRC_ELF=$(wildcard $(SRCDIR_ELF)/*.c)

OBJ_ELF=$(SRC_ELF:.c=.o)
OBJ_DBG=$(SRC:.c=.dbg)
OBJ_RLS=$(SRC:.c=.rls)

all : 
	@echo "in " $(DIRNAME)
	@echo ""
	@echo "Usage:"
	@echo ""
	@echo "make debug   => build DEBUG   version"
	@echo "make release => build RELEASE version"
	@echo "make clean   => clean everything"
	@echo "make archive => produce an archive for the deliverable"
	@echo ""

elf : $(TARGET_ELF)

$(TARGET_ELF) : $(OBJ_ELF)
	$(LD) $(LFLAGS_ELF) $^ -o $@  

.c.o : 
	$(CC) $(CFLAGS_ELF) $< -c -o $(basename $<).o

debug_no_warning   : $(OBJ_DBG)
	$(LD) $^ $(LFLAGS) -o $(TARGET)

debug   : $(OBJ_DBG)
	$(LD) $^ $(LFLAGS) -o $(TARGET)

release : $(OBJ_RLS)
	$(LD) $^ $(LFLAGS) -o $(TARGET)

%.dbg : %.c
	$(CC) $< $(CFLAGS_DBG) -c -o $(basename $<).dbg

%.rls : %.c
	$(CC) $< $(CFLAGS_RLS) -c -o $(basename $<).rls

docu : 
	$(DOXYGEN)

clean : 
	$(RM) $(TARGET) $(SRCDIR)/*.orig $(SRCDIR)/*.dbg $(SRCDIR)/*.rls $(GARBAGE)
	$(RM) -r $(DOCDIR)/*

clean_elf : 
	$(RM) $(TARGET_ELF) $(SRCDIR_ELF)/*.o

archive : 
	make clean 
	cd .. && tar -czvf as-mips-`whoami`-`date +%d-%m-%H-%M`.tgz $(DIRNAME) && cd $(DIRNAME) 

