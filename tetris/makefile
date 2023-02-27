CXXFALGS = -std=c++17 `wx-config --cxxflags`
LDFLAGS = `wx-config --libs`

src = $(shell find . -d 1 -type f -name "*.cpp")
tgt = tetris
obj = $(src:%.cpp=%.o)

%.o: %.cpp
	$(CXX) $(CXXFALGS) -c $^ -o $@
	
$(tgt): $(obj)
	$(CXX) $(LDFLAGS) $(obj) -o $@

.PHONY: all, clean, run

all: $(tgt)

clean: 
	rm *.o $(tgt)
	
run: 
	./$(tgt)


