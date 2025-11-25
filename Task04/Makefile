all: clean gen build
	@echo "==OD==";
	@od file.hole;
	@echo "=STAT=";
	@stat file.hole;
	@echo "======"

clean:
	@rm -rf a.out gen.out file.hole

build:
	@g++ -Wall -Wextra main.cpp

build-gen:
	@gcc -Wall -Wextra hole.c -o gen.out

run-gen:
	./gen.out

gen: build-gen run-gen
