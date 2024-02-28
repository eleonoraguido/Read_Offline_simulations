#
# General Makefile for code based on the ADST package
#
#
# Source files (excluding main source file)
SRCS = $(filter-out extract.cc, $(wildcard *.cc))

# Object files (generated from source files)
OBJS = $(SRCS:.cc=.o)

# Main executable
MAIN_EXE = extract

#############################################################

## You should not need to change anything below this line ###

.PHONY: all clean

ifdef AUGEROFFLINEROOT
ADSTROOT = $(AUGEROFFLINEROOT)
endif

CXXFLAGS    = $(shell root-config --cflags)
CXXFLAGS   += -I$(ADSTROOT)/include/adst
CXXFLAGS   += -I$(ADSTROOT)/include
LDFLAGS     = $(shell root-config --ldflags)
LDFLAGS    += $(shell root-config --libs)
LDFLAGS	   += -lMinuit -lTMVA
LDFLAGS    += -L$(ADSTROOT)/lib -lRecEventKG -lAnalysisKG

all: $(MAIN_EXE)

$(MAIN_EXE): extract.cc $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

# Compilation rule to generate object files from source files
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	- rm -f *.o  *.so *.ps core $(MAIN_EXE)