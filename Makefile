CC = gcc
CFLAGS = -Iinclude -Werror -Wno-format -fPIC
SRCDIR = src
INCDIR = include
BUILDDIR = build
TEMPDIR = temp
TARGET = $(BUILDDIR)/chess


SOURCES := $(wildcard $(SRCDIR)/*.c) main.c
OBJECTS := $(patsubst $(SRCDIR)/%.c,$(TEMPDIR)/%.o,$(SOURCES:main.c=$(TEMPDIR)/main.o))

all: $(BUILDDIR) $(TEMPDIR) $(TARGET) lib

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

lib: $(LIB_TARGET)

ifeq ($(OS),Windows_NT)
$(LIB_TARGET): $(OBJECTS)
	$(CC) -shared -o $(LIB_TARGET).dll $^ -lm
else
$(LIB_TARGET): $(OBJECTS)
	$(CC) -shared -o $(LIB_TARGET).so $^ -lm
endif

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
else
	rm -rf $(TEMPDIR)
endif

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run lib
