CXX = g++
CXXFLAGS = -Wall -Wextra -O3 -std=c++17 -fopenmp
LDFLAGS = -fopenmp
TARGET = yapt
ARGS = 512 40 2
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
DBJS = $(SRCS:.cpp=.d)

.PHONY: all
all: $(TARGET)

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM $< > $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

-include $(DBJS)

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) $(DBJS)

.PHONY: run
run:
	./$(TARGET) $(ARGS)
