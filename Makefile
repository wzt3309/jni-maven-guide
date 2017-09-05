CC = gcc
CCFLAGS = -Wall -g
LDFLAGS = -shared -fPIC
SRC_PATH = ./src/main/c
JNI_INC_ROOT = $(JAVA_HOME)/include
JNI_INC_LINUX = ${JNI_INC_ROOT}/linux
TARGET_PATH = ./clib

all: libSample01.so
	cp -rf libSample01.so ${TARGET_PATH}
	rm -rf libSample01.so

libSample01.so:
	$(CC) ${SRC_PATH}/Sample01.c\
		-I${JNI_INC_ROOT} -I${JNI_INC_LINUX} ${CCFLAGS} $(LDFLAGS)\
		-o libSample01.so

clean:
	rm -rf ${TARGET_PATH}/libSample01.so
