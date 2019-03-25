#Makefile
CC=gcc
INCLUDE=-I.
LIB=
CFLAGS=-g -D_REENTRANT -D_GNU_SOURCE ${LIB} ${INCLUDE}
MainFile=main.c
OutPut=json
src=json.c
target=$(patsubst %.c, %.o, ${MainFile})
target+=$(patsubst %.c, %.o, ${src})
springcleaning=$(patsubst %.c, %, $(wildcard ./*.c))
springcleaning+=$(patsubst %.c, %.o, $(wildcard ./*.c))
springcleaning+=$(patsubst %.c, %.o, ${src})
springcleaning+=$(OutPut)

.PHONY: all clean

all: $(OutPut) formParser jsonParser
$(OutPut):${target}
	$(CC) ${target}  -o $@ ${CFLAGS} ${INCLUDE} 
formParser: formParser.o
	$(CC) formParser.c -o formParser ${CFLAGS}
jsonParser: jsonParser.o
	$(CC) json.c jsonParser.c -o jsonParser ${CFLAGS} ${INCLUDE}
	
clean:
	-@rm  ${springcleaning} ${OutPut}
