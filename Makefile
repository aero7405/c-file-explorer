CC			:= gcc

LIBS		:= -lws2_32
INCLUDES 	:= -Iinclude
CFLAGS 		:= -Wall -g # [warnings] [debug]

SRC_DIR 	:= src
INCLUDE_DIR := include
OBJ_DIR		:= obj

TARGET 		:= c-file-explorer
OBJ_FILES 	:= $(OBJ_DIR)/main.o $(OBJ_DIR)/server.o $(OBJ_DIR)/http_response.o $(OBJ_DIR)/file.o
INCLUDE_FILES := $(INCLUDE_DIR)/server.h $(INCLUDE_DIR)/http_response.h $(INCLUDE_DIR)/file.h

# Running target after compilation
all: $(TARGET)
	@echo Running "$(TARGET)"...
	@$(TARGET)

# Compiling obj/ into target
$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $(TARGET) $(LIBS)

# Compiling specific project files
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(INCLUDE_FILES)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES) $(LIBS) 

$(OBJ_DIR)/server.o: $(SRC_DIR)/server.c $(INCLUDE_FILES)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES) $(LIBS) 

$(OBJ_DIR)/http_response.o: $(SRC_DIR)/http_response.c $(INCLUDE_FILES)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES) $(LIBS)

$(OBJ_DIR)/file.o: $(SRC_DIR)/file.c $(INCLUDE_FILES)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES) $(LIBS)
