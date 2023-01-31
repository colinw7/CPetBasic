CC = g++
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(BIN_DIR)/CPetBasic

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/CPetBasic

SRC = \
CPetBasic.cpp \
\
Expr/CExprBValue.cpp \
Expr/CExprCompile.cpp \
Expr/CExpr.cpp \
Expr/CExprExecute.cpp \
Expr/CExprFunction.cpp \
Expr/CExprInterp.cpp \
Expr/CExprIValue.cpp \
Expr/CExprOperator.cpp \
Expr/CExprParse.cpp \
Expr/CExprRValue.cpp \
Expr/CExprStrgen.cpp \
Expr/CExprSValue.cpp \
Expr/CExprToken.cpp \
Expr/CExprTokenStack.cpp \
Expr/CExprValue.cpp \
Expr/CExprVariable.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-DPET_EXPR \
-std=c++17 \
-I./Expr \
-I../../CReadLine/include \
-I../../CFile/include \
-I../../CMath/include \
-I../../CStrUtil/include \
-I../../CUtil/include \
-I../../COS/include \
-I.

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
-lCFile \
-lCReadLine \
-lCMath \
-lCStrUtil \
-lCUtil \
-lCOS \
-lreadline \
-lcurses

.SUFFIXES: .cpp

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(BIN_DIR)/CPetBasic: $(OBJS)
	$(CC) -o $(BIN_DIR)/CPetBasic $(OBJS) $(LFLAGS)
