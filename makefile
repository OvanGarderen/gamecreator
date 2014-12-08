CC :=gcc
DBG :=-g -DDEBUG
CFLAGS := -c -Wall -Wno-variadic-macros -Wno-unused-parameter -Werror=implicit -Werror=shadow -Wextra -std=gnu11 -Wl,-export-dynamic
INCLUDE := $(shell pkg-config --cflags glib-2.0) \
	$(shell pkg-config --cflags allegro-5.0) \
	$(shell pkg-config --cflags allegro_image-5.0) \
	$(shell pkg-config --cflags allegro_font-5.0) \
	$(shell pkg-config --cflags allegro_ttf-5.0) \
	$(shell pkg-config --cflags allegro_primitives-5.0) \
	$(shell sdl-config --cflags) \
	$(shell pkg-config --cflags lua)\
	$(shell pkg-config --cflags libconfig) -I./src/ 
LDFLAGS := $(shell sdl-config --libs) -pthread \
	$(shell pkg-config --libs allegro-5.0) \
	$(shell pkg-config --libs allegro_image-5.0) \
	$(shell pkg-config --libs allegro_font-5.0) \
	$(shell pkg-config --libs allegro_ttf-5.0) \
	$(shell pkg-config --libs allegro_primitives-5.0) \
	$(shell pkg-config --libs glib-2.0) \
	$(shell pkg-config --libs libconfig) \
	-lm -ldl -rdynamic

SOURCES := $(wildcard src/*.c)
EXTSOURCES := $(wildcard src/galdr_external/*.c)
_SOURCES := $(patsubst src/%, %, $(SOURCES))
_EXTSOURCES := $(patsubst src/%, %, $(EXTSOURCES)) 
OBJECTS := $(patsubst %.c, objects/%.o, $(_SOURCES)) $(patsubst %.c, objects/%.o, $(_EXTSOURCES))
HEADERS := $(wildcard src/*.h)
EXECUTABLE=engine

SCRIPTSOURCES := $(wildcard src/galdr/*.c) 
_SCRIPTSOURCES := $(patsubst src/galdr/%, %, $(SCRIPTSOURCES))
SCRIPTOBJS := $(patsubst %.c, objects/galdr/%.o, $(_SCRIPTSOURCES))

LIBSOURCES  := $(wildcard src/galdr/libsrc/*.c)
_LIBSOURCES := $(patsubst src/galdr/libsrc/%, %, $(LIBSOURCES))
LIBS := $(patsubst %.c, libs/lib%.so, $(_LIBSOURCES))

SCRIPTHEADERS := $(wildcard src/galdr/*.h)

SCRIPTER=volva

.DEPEND: $(SOURCES) $(wildcard src/*.h) $(LIBS)

all: $(SCRIPTER) $(LIBS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(HEADERS) $(SCRIPTOBJS)
	$(CC) $(OBJECTS) $(SCRIPTOBJS) -o $@ $(LDFLAGS)

$(SCRIPTER): $(SCRIPTHEADERS) $(SCRIPTOBJS) objects/galdr/volva/shell.o objects/shared.o
	$(CC) $(SCRIPTOBJS) objects/shared.o objects/galdr/volva/shell.o -o $@ -lm -ldl -rdynamic

objects/galdr/volva/%.o: src/galdr/shell/%.c 
	$(CC) $(CFLAGS) $(DBG) $(INCLUDE) -I./src/galdr/ $< -o $@ -lm -ldl -rdynamic
	@echo -e "\033[32m----------------------------------------------------------\033[0m"

objects/galdr/%.o: src/galdr/%.c 
	$(CC) $(CFLAGS) $(DBG) $(INCLUDE) $< -o $@ -lm -ldl -rdynamic
	@echo -e "\033[32m----------------------------------------------------------\033[0m"

objects/%.o: src/%.c 
	$(CC) $(CFLAGS) $(DBG) $(INCLUDE) $< -o $@ -lm -ldl -rdynamic
	@echo -e "\033[31m----------------------------------------------------------\033[0m"

objects/libobjs/%.o: src/galdr/libsrc/%.c
	$(CC) $(CFLAGS) $(DBG) -fPIC -I./src/galdr/ $< -o $@ -rdynamic

libs/lib%.so: objects/libobjs/%.o
	$(CC) -shared -Wl,-soname,$@ $< -o $@

run:	all
	./$(EXECUTABLE)

.dummy:
	@echo "wat"

clean:
	rm objects/galdr/*.o
	rm objects/*.o

restart: clean
	rm $(EXECUTABLE)
	rm $(SCRIPTER)
