CXX = g++
CXXFLAGS = -O4 -Wall
LDFLAGS = -lm -lboost_program_options-mt

SRCS=main.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: core clean-middle

core: $(OBJS) 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o core $(OBJS) $(LDLIBS)

clean-middle:
	$(RM) $(OBJS)

clean:
	$(RM) $(OBJS)
	$(RM) core
