CXX := g++
CXXFLAGS := -Wall -Werror -std=c++20 -g -O

.default: all

all: firstfit bestfit

firstfit: firstfit.o frag_sim.o firstfit_sim.o allocation.o
	$(CXX) $(CXXFLAGS) -o $@ $^

bestfit: bestfit.o frag_sim.o bestfit_sim.o allocation.o
	$(CXX) $(CXXFLAGS) -o $@ $^

better_bestfit: better_bestfit.o frag_sim.o better_bestfit_sim.o allocation.o
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^

clean:
	rm -rf firstfit bestfit better_bestfit *.o *.dSYM