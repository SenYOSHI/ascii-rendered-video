CC = gcc
CCFLAGS = -IC:/ffmpeg/include
LDFLAGS = -LC:/ffmpeg/lib
LDLIBS = -lavcodec -lavutil -lavformat

decoder : main.c
	$(CC) $(CCFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)
demux : demux_decode.c
	$(CC) $(CCFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)
ascii : ascii.c bmpstruct.h
	$(CC) -o ascii.exe ascii.c
clean :
	del /Q *.exe 2>NUL  