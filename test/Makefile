CC = g++
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(BIN_DIR)/CPetBasicTest

SRC = \
CPetBasicTest.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-DPET_EXPR \
-std=c++17 \
-I../include/Expr \
-I../include \

LFLAGS = \
$(LDEBUG) \
-L$(LIB_DIR) \
-L../../CFile/lib \
-L../../CReadLine/lib \
-L../../CMath/lib \
-L../../CStrUtil/lib \
-L../../CGlob/lib \
-L../../CUtil/lib \
-L../../COS/lib \
-lCPetBasic \
-lCFile \
-lCReadLine \
-lCMath \
-lCStrUtil \
-lCUtil \
-lCOS \
-lreadline \
-lcurses

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/CPetBasicTest

.SUFFIXES: .cpp

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(BIN_DIR)/CPetBasicTest: $(OBJS) $(LIB_DIR)/libCPetBasic.a
	$(CC) $(LDEBUG) -o $(BIN_DIR)/CPetBasicTest $(OBJS) $(LFLAGS) $(LIBS)
