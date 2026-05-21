IXTLI_TOP ?= $(realpath ../../)
IXTLI_EXAMPES ?= $(realpath ../)

TARGET=$(IXTLI_EXAMPES)/Buttons

OBJ_PATH := .built
SRC_DIR := $(shell find . -type d)
SRC := $(foreach DIR,$(SRC_DIR),$(patsubst ./%,%,$(wildcard $(DIR)/*.cpp)))
OBJ := $(patsubst %.cpp,$(OBJ_PATH)/%.o,$(SRC))
INCLUDES := $(foreach DIR,$(SRC_DIR),-I$(DIR)) -I$(IXTLI_TOP)

CXXFLAGS := -Wall -Wextra -Wno-unused-parameter
CPPFLAGS := $(INCLUDES)

LD_FLAGS := -Wl,-rpath=$(IXTLI_TOP)
LIBS := -lGL -lglut -luuid  -L$(IXTLI_TOP) -lixtli 

CXX ?= g++

.PHONY: all clean
all: $(TARGET)

$(OBJ):$(OBJ_PATH)/%.o:%.cpp
	@echo "CXX $<"
	@mkdir -p $(shell dirname $(OBJ_PATH)/$<)
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ 

$(TARGET):$(OBJ)
	@echo "BUILD $@"
	@$(CXX) -std=c++11 $(LDFLAGS) -o $(TARGET) $(OBJ) $(LD_FLAGS) $(LIBS) -lm

clean:
	@rm -rf $(OBJ_PATH) $(TARGET)