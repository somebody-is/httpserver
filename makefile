# 定义编译器
CXX = g++

# 定义编译参数
CXXFLAGS = -std=c++11 -Wall -Wextra -lpthread

OBJ_DIR = http

# 获取所有的.cpp和.h文件
#SOURCES := $(wildcard src/*.cpp)
#HEADERS := $(wildcard src/*.h)


rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
SOURCES := $(call rwildcard,src/,*.cpp)
HEADERS := $(call rwildcard,src/,*.h)

# 生成目标文件列表
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))

# 目标可执行文件
TARGET = hello

# 默认目标，生成可执行文件
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 源文件编译为目标文件
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理中间文件和可执行文件
clean:
	rm -f $(OBJECTS) $(TARGET)


