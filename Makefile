
COMPILER         := -c++
#COMPILER        := -clang++
OPTIMIZATION_OPT := -O2 -DNDEBUG
BASE_OPTIONS     := -pedantic-errors -Wall -Wextra -Werror -Wno-long-long
OPTIONS          := $(BASE_OPTIONS) $(OPTIMIZATION_OPT)
LINKER_OPT       := -L/usr/lib -lstdc++ -lm
# Suppress GCC var-tracking notes on very large template functions (e.g. run_test02).
ASAN_OPT         := -g -fsanitize=address   -fno-omit-frame-pointer -fno-var-tracking-assignments
ASAN_BUILD_OPTIONS := $(BASE_OPTIONS) $(ASAN_OPT) -O1
MSAN_OPT         := -g -fsanitize=memory    -fno-omit-frame-pointer
LSAN_OPT         := -g -fsanitize=leak      -fno-omit-frame-pointer
USAN_OPT         := -g -fsanitize=undefined -fno-omit-frame-pointer
BIN_DIR          := bin
vpath %.cpp example test
TEST_SRC        := $(sort $(wildcard test/exprtk_*.cpp))
EXAMPLE_SRC     := $(sort $(wildcard example/exprtk_*.cpp))
BUILD_LIST       := $(sort $(foreach f,$(TEST_SRC) $(EXAMPLE_SRC),$(notdir $(f:.cpp=))))
BIN_TARGETS         := $(addprefix $(BIN_DIR)/,$(BUILD_LIST))

# Regenerate single-header exprtk_single.hpp from src/*.hpp (run after src changes)
build:
	@python3 scripts/merge_hpp.py

all: $(BUILD_LIST)

test: $(TEST_SRC)

example: $(EXAMPLE_SRC)

exprtk_%: exprtk_%.cpp src/exprtk.hpp
	@mkdir -p $(BIN_DIR)
	@echo "Building $(BIN_DIR)/exprtk_$* from $<"
	$(COMPILER) $(OPTIONS) -Isrc -o $(BIN_DIR)/exprtk_$* $< $(LINKER_OPT)

exprtk_%_asan: exprtk_%.cpp src/exprtk.hpp
	@mkdir -p $(BIN_DIR)
	@echo "Building $(BIN_DIR)/exprtk_$*_asan from $<"
	$(COMPILER) $(ASAN_BUILD_OPTIONS) -Isrc -o $(BIN_DIR)/exprtk_$*_asan $< $(LINKER_OPT)

strip_bin :
	@for f in $(BIN_TARGETS); do if [ -f $$f ]; then strip -s $$f; echo $$f; fi done;

valgrind :
	@for f in $(BIN_TARGETS); do \
		if [ -f $$f ]; then \
			cmd="valgrind --leak-check=full --show-reachable=yes --track-origins=yes --log-file=$$f.log -v ./$$f"; \
			echo $$cmd; \
			$$cmd; \
		fi done;

pgo: test/exprtk_benchmark.cpp src/exprtk.hpp
	@mkdir -p $(BIN_DIR)
	$(COMPILER) $(BASE_OPTIONS) -O3 -DNDEBUG -march=native -Isrc -fprofile-generate -o $(BIN_DIR)/exprtk_benchmark test/exprtk_benchmark.cpp $(LINKER_OPT)
	./$(BIN_DIR)/exprtk_benchmark
	$(COMPILER) $(BASE_OPTIONS) -O3 -DNDEBUG -march=native -Isrc -fprofile-use -o $(BIN_DIR)/exprtk_benchmark test/exprtk_benchmark.cpp $(LINKER_OPT)

clean:
	rm -f core.* *~ *.o *.bak *stackdump gmon.out *.gcda *.gcno *.gcnor *.gch bin/*
