TARGET := libixtli.so
SRC_DIRS:=$(shell find Ixtli -type d)
OBJ_PATH:=built

SRC := $(foreach DIR,$(SRC_DIRS),$(patsubst ./%,%,$(wildcard $(DIR)/*.cpp)))
OBJ := $(patsubst %.cpp,$(OBJ_PATH)/%.o,$(SRC)) #$(OBJ_PATH)/main.o
HEADERS := $(foreach DIR,$(SRC_DIRS),$(patsubst ./%,%,$(wildcard $(DIR)/*.h)))

INCLUDES := $(foreach DIR,$(SRC_DIRS),-I$(DIR)) -I./

CXXFLAGS := -Wall -Wextra -Wno-delete-incomplete -Wno-unused-parameter -std=c++11
CPPFLAGS := $(INCLUDES) -fPIC

CXX := g++

LIBS:= -lglfw -lGL -lglut -luuid


GRAPH_CALC_TARGET := grapher
GRAPH_CALC_OBJ_PATH := built.grapher
GRAPH_CALC_SRC_DIR := $(shell find ./GraphCalculator -type d)
GRAPH_CALC_SRC := $(foreach DIR,$(GRAPH_CALC_SRC_DIR),$(patsubst ./%,%,$(wildcard $(DIR)/*.cpp)))
GRAPH_CALC_OBJ := $(patsubst %.cpp,$(GRAPH_CALC_OBJ_PATH)/%.o,$(GRAPH_CALC_SRC))
GRAPH_CALC_INCLUDES := $(foreach DIR,$(GRAPH_CALC_SRC_DIR),-I$(DIR)) -Iinstall/include/

GRAPH_CALC_CXXFLAGS := -Wall -Wextra -Wno-unused-parameter
GRAPH_CALC_CPPFLAGS := $(GRAPH_CALC_INCLUDES)

GRAPH_CALC_LIBS := -lGL -lglut -luuid  -Linstall/lib -lixtli 

.PHONY: all clean install

all: $(TARGET)

install: $(TARGET)
	@mkdir -p install/include/Ixtli
	@mkdir -p install/lib
	@echo $(foreach DIR,$(SRC_DIRS),$(shell mkdir -p install/include/$(DIR))) > /dev/null
	@echo $(foreach header,$(HEADERS),$(shell cp $(header) install/include/$(header))) > /dev/null
	@cp -v $(TARGET) install/lib



$(OBJ):$(OBJ_PATH)/%.o:%.cpp
	@echo "CXX $<"
	@mkdir -p $(shell dirname $(OBJ_PATH)/$<)
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(TARGET):$(OBJ)
	@echo "BUILD $@"
	@$(CXX) -shared -std=c++11 $(LDFLAGS) -o $(TARGET) $(OBJ) $(LIBS) -lm


$(GRAPH_CALC_OBJ):$(GRAPH_CALC_OBJ_PATH)/%.o:%.cpp
	@echo "CXX $<"
	@mkdir -p $(shell dirname $(GRAPH_CALC_OBJ_PATH)/$<)
	@$(CXX) $(GRAPH_CALC_CXXFLAGS) $(GRAPH_CALC_CPPFLAGS) -c $< -o $@ 

$(GRAPH_CALC_TARGET):$(GRAPH_CALC_OBJ)
	@echo "BUILD $@"
	@$(CXX) -std=c++11 $(LDFLAGS) -o $(GRAPH_CALC_TARGET) $(GRAPH_CALC_OBJ) $(GRAPH_CALC_LIBS) -lm



clean:
	@rm -rf $(OBJ_PATH) $(TARGET) $(GRAPH_CALC_OBJ_PATH) $(GRAPH_CALC_TARGET) install/
