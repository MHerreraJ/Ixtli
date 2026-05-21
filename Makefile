TARGET := libixtli.so
INSTALL_PREFIX ?= out
SRC_DIRS:=$(shell find Ixtli -type d)
OBJ_PATH:=.built

SRC := $(foreach DIR,$(SRC_DIRS),$(patsubst ./%,%,$(wildcard $(DIR)/*.cpp)))
OBJ := $(patsubst %.cpp,$(OBJ_PATH)/%.o,$(SRC)) 
HEADERS := $(foreach DIR,$(SRC_DIRS),$(patsubst ./%,%,$(wildcard $(DIR)/*.h)))

INCLUDES := $(foreach DIR,$(SRC_DIRS),-I$(DIR)) -I./
FREETYPE_CFLAGS := $(shell pkg-config --cflags freetype2 2>/dev/null)
FREETYPE_LIBS := $(shell pkg-config --libs freetype2 2>/dev/null)

CXXFLAGS := -Wall -Wextra -Wno-delete-incomplete -Wno-unused-parameter -std=c++11 -pthread
CPPFLAGS := $(INCLUDES) $(FREETYPE_CFLAGS) -fPIC

ifneq ($(strip $(FREETYPE_LIBS)),)
CPPFLAGS += -DIXTLI_ENABLE_FREETYPE
endif

CXX := g++

LIBS:= -lGL -lglut -luuid $(FREETYPE_LIBS)

.PHONY: all clean install examples

all: $(TARGET) examples

examples: $(TARGET)
	$(MAKE) -C examples

install: $(TARGET)
	@mkdir -p $(INSTALL_PREFIX)/include/Ixtli
	@mkdir -p $(INSTALL_PREFIX)/lib
	@echo $(foreach DIR,$(SRC_DIRS),$(shell mkdir -p $(INSTALL_PREFIX)/include/$(DIR))) > /dev/null
	@echo $(foreach header,$(HEADERS),$(shell cp $(header) $(INSTALL_PREFIX)/include/$(header))) > /dev/null
	@cp $(TARGET) $(INSTALL_PREFIX)/lib
	@echo "Installed Ixtli to \033[1;36m$(INSTALL_PREFIX)/include/Ixtli\033[0m"
	@echo "Installed $(TARGET) to \033[1;36m$(INSTALL_PREFIX)/lib/$(TARGET)\033[0m" 

$(OBJ):$(OBJ_PATH)/%.o:%.cpp
	@echo "CXX $<"
	@mkdir -p $(shell dirname $(OBJ_PATH)/$<)
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(TARGET):$(OBJ)
	@echo "BUILD $@"
	@$(CXX) -shared -std=c++11 $(LDFLAGS) -o $(TARGET) $(OBJ) $(LIBS) -lm
	@echo "\033[1;36m$(TARGET) built successfully\033[0m"

clean:
	@rm -rf $(OBJ_PATH) $(TARGET)
	$(MAKE) -C examples clean