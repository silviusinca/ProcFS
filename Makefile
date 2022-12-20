all:
	gcc main.c -o  main -lprocps
	./main
clear:
	rm -rf procFS main
