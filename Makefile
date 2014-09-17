TARGET=$(PACKAGE).$(LIB_EXTENSION)
SRC=process.c
OBJ=process.o


all: preprocess $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(WARNINGS) $(CPPFLAGS) -o $(OBJ) -c $(SRC)
	$(CC) -o $(TARGET) $(LDFLAGS) $(OBJ)

preprocess:
	lua ./errnogen.lua

install:
	mkdir -p $(LIBDIR)
	cp $(TARGET) $(LIBDIR)
	rm -f $(OBJ) $(TARGET)

