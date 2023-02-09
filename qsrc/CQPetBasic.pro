TEMPLATE = app

TARGET = CQPetBasic

QT += widgets

DEPENDPATH += .

QMAKE_CXXFLAGS += \
-std=c++17 \

MOC_DIR = .moc

CONFIG += c++17

SOURCES += \
main.cpp \
CQPetBasic.cpp \
CQPetBasicApp.cpp \
CQPetBasicCommand.cpp \
CQPetBasicDbg.cpp \
CQPetBasicKeyboard.cpp \
CQPetBasicStatus.cpp \
CQPetBasicTerm.cpp \
CQPetBasicVariables.cpp \
CQCommand.cpp \

HEADERS += \
CQPetBasic.h \
CQPetBasicApp.h \
CQPetBasicCommand.h \
CQPetBasicDbg.h \
CQPetBasicKeyboard.h \
CQPetBasicStatus.h \
CQPetBasicTerm.h \
CQPetBasicVariables.h \
CQCommand.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
. \
../include \
../include/Expr \
../../CQUtil/include \
../../CUtil/include \

PRE_TARGETDEPS = \
../lib/libCPetBasic.a \

unix:LIBS += \
-L../lib \
-L../../CQUtil/lib \
-L../../CReadLine/lib \
-L../../CFont/lib \
-L../../CImageLib/lib \
-L../../CConfig/lib \
-L../../CUtil/lib \
-L../../CFileUtil/lib \
-L../../CFile/lib \
-L../../CMath/lib \
-L../../CStrUtil/lib \
-L../../CRegExp/lib \
-L../../COS/lib \
-lCPetBasic -lCQUtil -lCReadLine \
-lCFont -lCImageLib -lCConfig -lCUtil \
-lCFileUtil -lCFile -lCMath -lCStrUtil -lCRegExp -lCOS \
-lpng -ljpeg -ltre -lreadline
