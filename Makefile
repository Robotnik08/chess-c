CC = gcc
CFLAGS = -Iinclude -Werror -Wno-format -fPIC
SRCDIR = src
INCDIR = include
BUILDDIR = build
TEMPDIR = temp
TARGET = $(BUILDDIR)/chess

ifeq ($(OS),Windows_NT)
    LIBEXT = dll
    LIB_TARGET = $(BUILDDIR)/chesslib.$(LIBEXT)
else
    LIBEXT = so
    LIB_TARGET = $(BUILDDIR)/libchess.$(LIBEXT)
endif

SOURCES := $(wildcard $(SRCDIR)/*.c) main.c
OBJECTS := $(patsubst $(SRCDIR)/%.c,$(TEMPDIR)/%.o,$(SOURCES:main.c=$(TEMPDIR)/main.o))

all: $(BUILDDIR) $(TEMPDIR) $(TARGET) $(LIB_TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

$(LIB_TARGET): $(OBJECTS)
	$(CC) -shared -o $@ $^ -lm

$(TEMPDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $< -lm

$(TEMPDIR)/main.o: main.c
	$(CC) $(CFLAGS) -c -o $@ $< -lm

$(BUILDDIR):
ifeq ($(OS),Windows_NT)
	if not exist $(BUILDDIR) mkdir $(BUILDDIR)
else
	mkdir -p $(BUILDDIR)
endif

$(TEMPDIR):
ifeq ($(OS),Windows_NT)
	if not exist $(TEMPDIR) mkdir $(TEMPDIR)
else
	mkdir -p $(TEMPDIR)
endif

clean:
ifeq ($(OS),Windows_NT)
	rmdir /s /q $(TEMPDIR)
	rmdir /s /q $(BUILDDIR)
else
	rm -rf $(TEMPDIR) $(BUILDDIR)
endif

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run lib
