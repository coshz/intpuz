
src_dir   := src
build_dir := build

CXXFLAGS = -std=c++17 -Wall

ifdef RELEASE
	CXXFLAGS += -DNDEBUG -O2
else 
	CXXFLAGS += -g -DVERBOSE
endif

target_exe := solcube

src_files = $(shell find $(src_dir) -name '*.cpp')
obj_files = $(src_files:%.cpp=$(build_dir)/%.o)

$(build_dir)/$(target_exe): $(obj_files)
	$(CXX) $(obj_files) -o $@ $(LDFLAGS)

$(build_dir)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: 
	$(build_dir)/$(target_exe)

clean:
	rm -rf build/
