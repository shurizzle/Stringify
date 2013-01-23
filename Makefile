all:

test: test.out
	@./test.out

test.out: test.cpp
	@g++ -o test.out -I. test.cpp

clean:
	-@rm -rf test.out
