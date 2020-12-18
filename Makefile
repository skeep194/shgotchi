CC=gcc
LDLIBS=-lcurses
SRCS=$(wildcard ./src/*.c)
OBJS=save.o shgotchi-list.o shgotchi-manager.o shgotchi-process.o user.o
INC=-I./include
TARGET=shgotchi
all: $(TARGET)
	$(CC) -o $(TARGET) $(OBJS) $(INC) $(LDLIBS)
$(TARGET):
	$(CC) -c $(SRCS) $(INC) $(LDLIBS)
clean:
	rm -f *.o