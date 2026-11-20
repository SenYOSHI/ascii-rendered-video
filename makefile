CC = gcc
CCFLAGS = -IC:/ffmpeg/include
LDFLAGS = -LC:/ffmpeg/lib
LDLIBS = -lavcodec -lavutil

decoder : main.c
	$(CC) $(CCFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)
clean :
	del /Q *.exe 2>NUL