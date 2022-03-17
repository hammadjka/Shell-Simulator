# .*. MakeFile .*.


all: SS

cshell: SS.c
	gcc -g -Wall SS.c -o SS
	
clean:
	rm -f *.o SS
