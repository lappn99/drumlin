.SUFFIXES:
.SUFFIXES: .c .o

CC=gcc
CCFLAGS=-c -Wall -Werror -fpic -Bsymbolic -ggdb
LDFLAGS=-shared
MODULES=drumlin map tile tileservice

SRCDIR=./impl
OBJDIR=./obj
OBJS= ${addprefix ${OBJDIR}/, ${addsuffix .o, ${MODULES}}}
INCLUDE_DIR=./interface
INCLUDE=-I${INCLUDE_DIR}
CURRENT_DIR = $(shell pwd)

TILESERVICE_IMPL=./impl/tileservice/sokol/tileservice_sokol.c

build: libdrumlin.so

${OBJDIR}/tileservice.o: ${TILESERVICE_IMPL}
	@mkdir -p ${dir $@}
	@echo $@
	${CC} ${CCFLAGS} ${LIBS} ${INCLUDE} $< -D_GNU_SOURCE -o $@

${OBJDIR}/%.o : ${SRCDIR}/%.c
	@mkdir -p ${dir $@}
	@echo $@
	${CC} ${CCFLAGS} ${LIBS} ${INCLUDE} $< -D_GNU_SOURCE -o $@

libdrumlin.so : ${OBJS}	
	@echo ${OBJDIRS}
	${CC} ${LDFLAGS} -o $@ $^ 

install: build
	cp ./libdrumlin.so /usr/lib
	cp -r ${INCLUDE_DIR}/. /usr/include/
	ldconfig

clean:
	rm -rf ${OBJDIR}
	rm -f libdrumlin.so
	rm -f ./example

uninstall:
	rm -f /usr/lib/libdrumlin.so
	rm -rf /usr/include/drumlin


example: install example.c 
	${CC} -fsanitize=address -lasan -ldrumlin ${INCLUDE} -o  $@ example.c -ggdb



