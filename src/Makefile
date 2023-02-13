CC = g++
AR = ar
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib

all: dirs $(LIB_DIR)/libCPetBasic.a

dirs:
	@if [ ! -e ../obj ]; then mkdir ../obj; fi
	@if [ ! -e ../lib ]; then mkdir ../lib; fi
	@if [ ! -e ../bin ]; then mkdir ../bin; fi

SRC = \
CPetBasic.cpp \
CPetBasicRawTerm.cpp \
CPetBasicTerm.cpp \
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
\
CEscape.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-DPET_EXPR \
-std=c++17 \
-I./Expr \
-I../include/Expr \
-I../include \
-I../../CReadLine/include \
-I../../CEscape/include \
-I../../CFile/include \
-I../../CMath/include \
-I../../CStrUtil/include \
-I../../CUtil/include \
-I../../COS/include \
-I.

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCPetBasic.a

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

.SUFFIXES: .cpp

$(LIB_DIR)/libCPetBasic.a: $(OBJS)
	$(AR) crv $(LIB_DIR)/libCPetBasic.a $(OBJS)
