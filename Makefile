#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = 
CFLAGS = -std=c++11 -Wall -fexceptions
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = 

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = bin/Debug/pq

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O2
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS)
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = bin/Release/pq

OBJ_DEBUG = $(OBJDIR_DEBUG)/pqencoder.o $(OBJDIR_DEBUG)/pqmath.o $(OBJDIR_DEBUG)/pqnnsearch.o $(OBJDIR_DEBUG)/pqtrainer.o $(OBJDIR_DEBUG)/scriptparser.o $(OBJDIR_DEBUG)/timer.o $(OBJDIR_DEBUG)/topkheap.o $(OBJDIR_DEBUG)/vstring.o $(OBJDIR_DEBUG)/abstractpquantizer.o $(OBJDIR_DEBUG)/binaryheap.o $(OBJDIR_DEBUG)/cleaner.o $(OBJDIR_DEBUG)/hardwaresetup.o $(OBJDIR_DEBUG)/abstractkmeans.o $(OBJDIR_DEBUG)/invtflloader.o $(OBJDIR_DEBUG)/iodelegator.o $(OBJDIR_DEBUG)/kppmeans.o $(OBJDIR_DEBUG)/main.o $(OBJDIR_DEBUG)/missionagent.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/pqencoder.o $(OBJDIR_RELEASE)/pqmath.o $(OBJDIR_RELEASE)/pqnnsearch.o $(OBJDIR_RELEASE)/pqtrainer.o $(OBJDIR_RELEASE)/scriptparser.o $(OBJDIR_RELEASE)/timer.o $(OBJDIR_RELEASE)/topkheap.o $(OBJDIR_RELEASE)/vstring.o $(OBJDIR_RELEASE)/abstractpquantizer.o $(OBJDIR_RELEASE)/binaryheap.o $(OBJDIR_RELEASE)/cleaner.o $(OBJDIR_RELEASE)/hardwaresetup.o $(OBJDIR_RELEASE)/abstractkmeans.o $(OBJDIR_RELEASE)/invtflloader.o $(OBJDIR_RELEASE)/iodelegator.o $(OBJDIR_RELEASE)/kppmeans.o $(OBJDIR_RELEASE)/main.o $(OBJDIR_RELEASE)/missionagent.o

all: debug release

clean: clean_debug clean_release

before_debug: 
	test -d bin/Debug || mkdir -p bin/Debug
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/pqencoder.o: pqencoder.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c pqencoder.cpp -o $(OBJDIR_DEBUG)/pqencoder.o

$(OBJDIR_DEBUG)/pqmath.o: pqmath.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c pqmath.cpp -o $(OBJDIR_DEBUG)/pqmath.o

$(OBJDIR_DEBUG)/pqnnsearch.o: pqnnsearch.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c pqnnsearch.cpp -o $(OBJDIR_DEBUG)/pqnnsearch.o

$(OBJDIR_DEBUG)/pqtrainer.o: pqtrainer.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c pqtrainer.cpp -o $(OBJDIR_DEBUG)/pqtrainer.o

$(OBJDIR_DEBUG)/scriptparser.o: scriptparser.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c scriptparser.cpp -o $(OBJDIR_DEBUG)/scriptparser.o

$(OBJDIR_DEBUG)/timer.o: timer.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c timer.cpp -o $(OBJDIR_DEBUG)/timer.o

$(OBJDIR_DEBUG)/topkheap.o: topkheap.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c topkheap.cpp -o $(OBJDIR_DEBUG)/topkheap.o

$(OBJDIR_DEBUG)/vstring.o: vstring.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c vstring.cpp -o $(OBJDIR_DEBUG)/vstring.o

$(OBJDIR_DEBUG)/abstractpquantizer.o: abstractpquantizer.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c abstractpquantizer.cpp -o $(OBJDIR_DEBUG)/abstractpquantizer.o

$(OBJDIR_DEBUG)/binaryheap.o: binaryheap.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c binaryheap.cpp -o $(OBJDIR_DEBUG)/binaryheap.o

$(OBJDIR_DEBUG)/cleaner.o: cleaner.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c cleaner.cpp -o $(OBJDIR_DEBUG)/cleaner.o

$(OBJDIR_DEBUG)/hardwaresetup.o: hardwaresetup.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c hardwaresetup.cpp -o $(OBJDIR_DEBUG)/hardwaresetup.o

$(OBJDIR_DEBUG)/abstractkmeans.o: abstractkmeans.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c abstractkmeans.cpp -o $(OBJDIR_DEBUG)/abstractkmeans.o

$(OBJDIR_DEBUG)/invtflloader.o: invtflloader.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c invtflloader.cpp -o $(OBJDIR_DEBUG)/invtflloader.o

$(OBJDIR_DEBUG)/iodelegator.o: iodelegator.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c iodelegator.cpp -o $(OBJDIR_DEBUG)/iodelegator.o

$(OBJDIR_DEBUG)/kppmeans.o: kppmeans.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c kppmeans.cpp -o $(OBJDIR_DEBUG)/kppmeans.o

$(OBJDIR_DEBUG)/main.o: main.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c main.cpp -o $(OBJDIR_DEBUG)/main.o

$(OBJDIR_DEBUG)/missionagent.o: missionagent.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c missionagent.cpp -o $(OBJDIR_DEBUG)/missionagent.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf bin/Debug
	rm -rf $(OBJDIR_DEBUG)

before_release: 
	test -d bin/Release || mkdir -p bin/Release
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/pqencoder.o: pqencoder.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c pqencoder.cpp -o $(OBJDIR_RELEASE)/pqencoder.o

$(OBJDIR_RELEASE)/pqmath.o: pqmath.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c pqmath.cpp -o $(OBJDIR_RELEASE)/pqmath.o

$(OBJDIR_RELEASE)/pqnnsearch.o: pqnnsearch.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c pqnnsearch.cpp -o $(OBJDIR_RELEASE)/pqnnsearch.o

$(OBJDIR_RELEASE)/pqtrainer.o: pqtrainer.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c pqtrainer.cpp -o $(OBJDIR_RELEASE)/pqtrainer.o

$(OBJDIR_RELEASE)/scriptparser.o: scriptparser.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c scriptparser.cpp -o $(OBJDIR_RELEASE)/scriptparser.o

$(OBJDIR_RELEASE)/timer.o: timer.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c timer.cpp -o $(OBJDIR_RELEASE)/timer.o

$(OBJDIR_RELEASE)/topkheap.o: topkheap.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c topkheap.cpp -o $(OBJDIR_RELEASE)/topkheap.o

$(OBJDIR_RELEASE)/vstring.o: vstring.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c vstring.cpp -o $(OBJDIR_RELEASE)/vstring.o

$(OBJDIR_RELEASE)/abstractpquantizer.o: abstractpquantizer.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c abstractpquantizer.cpp -o $(OBJDIR_RELEASE)/abstractpquantizer.o

$(OBJDIR_RELEASE)/binaryheap.o: binaryheap.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c binaryheap.cpp -o $(OBJDIR_RELEASE)/binaryheap.o

$(OBJDIR_RELEASE)/cleaner.o: cleaner.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c cleaner.cpp -o $(OBJDIR_RELEASE)/cleaner.o

$(OBJDIR_RELEASE)/hardwaresetup.o: hardwaresetup.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c hardwaresetup.cpp -o $(OBJDIR_RELEASE)/hardwaresetup.o

$(OBJDIR_RELEASE)/abstractkmeans.o: abstractkmeans.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c abstractkmeans.cpp -o $(OBJDIR_RELEASE)/abstractkmeans.o

$(OBJDIR_RELEASE)/invtflloader.o: invtflloader.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c invtflloader.cpp -o $(OBJDIR_RELEASE)/invtflloader.o

$(OBJDIR_RELEASE)/iodelegator.o: iodelegator.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c iodelegator.cpp -o $(OBJDIR_RELEASE)/iodelegator.o

$(OBJDIR_RELEASE)/kppmeans.o: kppmeans.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c kppmeans.cpp -o $(OBJDIR_RELEASE)/kppmeans.o

$(OBJDIR_RELEASE)/main.o: main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c main.cpp -o $(OBJDIR_RELEASE)/main.o

$(OBJDIR_RELEASE)/missionagent.o: missionagent.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c missionagent.cpp -o $(OBJDIR_RELEASE)/missionagent.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf bin/Release
	rm -rf $(OBJDIR_RELEASE)

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

