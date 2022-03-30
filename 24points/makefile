CXX=g++
CXXFLAG=-std=c++11 -g
OUTPUT=output

$(OUTPUT): 24points.o
	$(CXX) -o  $(OUTPUT) 24points.o

%.o: %.cpp %.h
	$(CXX) $(CXXFLAG) $< -c

clean: 
	rm -rf *.o output