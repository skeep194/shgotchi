CC=gcc
LDLIBS=-lcurses
SRCS=$(wildcard ./src/*.c)
OBJS=save.o shgotchi-list.o shgotchi-manager.o shgotchi-process.o user.o shgotchi.o
INC=-I./include
TARGET=shgotchi
release: $(TARGET)
	$(CC) -o $(TARGET) $(OBJS) $(INC) $(LDLIBS)
$(TARGET):
	$(CC) -c $(SRCS) $(INC) $(LDLIBS)
debug: debug_compile
	$(CC) -g -o $(TARGET) $(OBJS) $(INC) $(LDLIBS)
debug_compile:
	$(CC) -g -D DEBUG -c $(SRCS) $(INC) $(LDLIBS)
clean:
	rm -f *.o
	rm -f $(TARGET)