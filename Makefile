GCC=g++
FILES=main.cpp
OUT=a.out

all:
	$(GCC) $(FILES) -o $(OUT)
clean:
	rm *.tmp
	rm OUTPUT
	rm $(OUT)
