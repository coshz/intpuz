TARGET_EXE := comp24
SRC_DIR := src
BUILD_DIR := build

CXXFLAGS=-std=c++17 -O2 -DNDEBUG

src_files = $(shell find $(SRC_DIR) -name '*.cpp')
obj_files = $(src_files:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/$(TARGET_EXE): $(obj_files)
	$(CXX) $(obj_files) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.phony: clean, run

clean: 
	rm -r $(BUILD_DIR)

run:
	./$(BUILD_DIR)/$(TARGET_EXE)