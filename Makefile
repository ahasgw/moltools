CC ?= gcc
CXX ?= g++
CXXFLAGS ?= -g -std=c++17 -Ofast -march=native -Wall -fopenmp
CPPFLAGS ?= -DNDEBUG -I/usr/include/rdkit
LDFLAGS ?= 
LDLIBS ?= 

RDKLIBS = -lRDKitRDGeneral -lRDKitGraphMol -lRDKitSmilesParse -lRDKitMolStandardize

SOURCES = \
	  smi2std.cc \
	  smi2sup.cc \
	  Makefile

SRCS = $(filter %.cc,$(SOURCES))
HDRS = $(filter %.h ,$(SOURCES))
OBJS = $(SRCS:.cc=.o)
EXES = $(SRCS:.cc=)

#PCHS = $(HDRS:.h,.h.gch)
#
#.SUFFIXES: .h.gch
#
#%.h.gch: %.h
#	$(COMPILE.cc) $(OUTPUT_OPTION) $<

.PHONY: all clean moreclean check

all: $(EXES)

clean:
	$(RM) $(EXES) $(OBJS)

moreclean: clean
	$(MAKE) -C test clean

check: all
	$(MAKE) -C test check


smi2std: LDLIBS += $(RDKLIBS)
smi2sup: LDLIBS += $(RDKLIBS)
