
COMPILER         := -c++
#COMPILER        := -clang++
OPTIMIZATION_OPT := -O2 -DNDEBUG
BASE_OPTIONS     := -pedantic-errors -Wall -Wextra -Werror -Wno-long-long
OPTIONS          := $(BASE_OPTIONS) $(OPTIMIZATION_OPT)
LINKER_OPT       := -L/usr/lib -lstdc++ -lm
ASAN_OPT         := -g -fsanitize=address   -fno-omit-frame-pointer
MSAN_OPT         := -g -fsanitize=memory    -fno-omit-frame-pointer
LSAN_OPT         := -g -fsanitize=leak      -fno-omit-frame-pointer
USAN_OPT         := -g -fsanitize=undefined -fno-omit-frame-pointer
BIN_DIR          := bin
vpath %.cpp example test
BUILD_SRC        := $(sort $(wildcard example/exprtk_*.cpp) $(wildcard test/exprtk_*.cpp))
BUILD_LIST       := $(sort $(foreach f,$(BUILD_SRC),$(notdir $(f:.cpp=))))
BIN_TARGETS      := $(addprefix $(BIN_DIR)/,$(BUILD_LIST))


all: $(BIN_TARGETS)

$(BIN_TARGETS): $(BIN_DIR)/%: %.cpp src/exprtk.hpp
	@mkdir -p $(BIN_DIR)
	$(COMPILER) $(OPTIONS) -Isrc -o $@ $< $(LINKER_OPT)

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
	rm -rf $(BIN_DIR)
	rm -f core.* *~ *.o *.bak *stackdump gmon.out *.gcda *.gcno *.gcnor *.gch
