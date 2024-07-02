test:
	g++ test.cpp -O2 -std=c++17 -o test -fsanitize=address -static-libasan -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment -g
	
	./test
	rm ./test


.PHONY: test
