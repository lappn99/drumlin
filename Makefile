.SUFFIXES:
.SUFFIXES: .c .o

CC=gcc
CCFLAGS=-c -Wall -Werror -fPIC -ggdb
LDFLAGS=-shared
MODULES=drumlin map tile tileservice app renderer

SRCDIR=./impl
OBJDIR=./obj
OBJS= ${addprefix ${OBJDIR}/, ${addsuffix .o, ${MODULES}}}
INCLUDE_DIR=./interface ./extern/stb_image

INCLUDE=${addprefix -I, ${INCLUDE_DIR}}
CURRENT_DIR = $(shell pwd)

TILESERVICE_IMPL=./impl/tileservice/sokol/tileservice_curl.c
APP_IMPL=./impl/app/sdl/app_sdl.c
RENDERER_IMPL=./impl/renderer/sdl/renderer_sdl.c
EXAMPLE_LIBS=-lasan -ldrumlin -lSDL2 -lm -lcurl -lpthread

build: libdrumlin.so

${OBJDIR}/app.o: ${APP_IMPL}
	@mkdir -p ${dir $@}
	@echo $@
	${CC} ${CCFLAGS} ${LIBS} ${INCLUDE} $< -D_GNU_SOURCE -o $@

${OBJDIR}/tileservice.o: ${TILESERVICE_IMPL}
	@mkdir -p ${dir $@}
	@echo $@
	${CC} ${CCFLAGS} ${LIBS} ${INCLUDE} $< -D_GNU_SOURCE -o $@

${OBJDIR}/renderer.o: ${RENDERER_IMPL}
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
	${CC} -fsanitize=address ${EXAMPLE_LIBS} ${INCLUDE} -o $@ example.c -ggdb



