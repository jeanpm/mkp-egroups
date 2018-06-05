CXX      = g++
CXXFLAGS = -std=c++11 -O3

# Put the object files in a hidden directory
OBJDIR   = .objects

SRCS     = main.cc cbga.cc mkp.cc model.cc egroups.cc pop.cc param.cc global.cc runinfo.cc
OBJS    = $(patsubst %.cc,$(OBJDIR)/%.o,$(SRCS))

all: debug

# Prints additional info
debug: CXXFLAGS += -DDEBUG
debug: cbga

# Runs without printing info
cbga: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o cbga

$(OBJDIR)/%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm $(OBJDIR)/*.o cbga
