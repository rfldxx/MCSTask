all:
	gcc main.c -o main -DSHOW_WITH_POWER
	./main


memory:
	gcc main.c -o main -DSHOW_WITH_POWER -fsanitize=address -static-libasan -g
	./main


test:
	gcc tests/binom_test.c -o binom_test -fsanitize=address -static-libasan -g
	gcc             main.c -o       main -fsanitize=address -static-libasan -g

	echo "\nTEST:"
	./binom_test
	
	echo "\nTEST:"
	bash tests/scroll.sh

	echo 
	rm ./binom_test
	rm ./main


clean:
	rm -rf tests/tests_obtain
	rm -f ./binom_test
	rm -f ./main


.PHONY: test
