SRCS_DIR := ./src
BUILD_DIR := ./build

SRCS := $(wildcard $(SRCS_DIR)/*.cc)
OBJS := $(subst $(SRCS_DIR),$(BUILD_DIR),$(SRCS))
OBJS := $(OBJS:.cc=.o)

override CXXFLAGS += -Wall -Wextra -g -Wshadow -std=c++14

all: main

main: main.cc $(OBJS)
	$(CXX) $(CXXFLAGS) -o main main.cc $(OBJS)

$(BUILD_DIR)/%.o: $(SRCS_DIR)/%.cc
	mkdir -p build && $(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR) main

.PHONY: all clean
