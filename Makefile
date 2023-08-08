CC			:= gcc

LIBS		:= -Lws2_32
INCLUDES 	:= -Iinclude
CFLAGS 		:= #-Wall

TARGET 		:= c-file-explorer
OBJFILES 	:= server.o http_response.o file.o

SRC_DIR 	:= src
INCLUDE_DIR := include
OBJ_DIR		:= build/obj
BIN_DIR		:= build/bin

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(EXEC_NAME) $(OBJFILES) $(LIBS)

server.o: $(SRC_DIR)/server.c $(INCLUDE_DIR)/http_response.h $(INCLUDE_DIR)/file.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/server.c $(INCLUDES) $(LIBS)

http_response.o: $(SRC_DIR)/http_response.c $(INCLUDE_DIR)/http_response.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/http_response.c $(INCLUDES) $(LIBS)

file.o: $(SRC_DIR)/file.c $(INCLUDE_DIR)/file.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/file.c $(INCLUDES) $(LIBS)