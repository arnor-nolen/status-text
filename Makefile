SRC = status_text.c
OBJ = ${SRC:.c=.o}

all: options status-text

options:
	@echo status-text build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.h

status-text: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

config.h:
	cp config.def.h $@

clean:
	rm -f status-text ${OBJ}
	
install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f status-text ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/status-text

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/status-text
