.PHONY: test
test: out ../eeprom_circular_buffer.c ../eeprom_circular_buffer.h test.c
	gcc test.c ../eeprom_circular_buffer.c -I../ -g -o out/test -Wall -Wextra -std=c99
	./out/test

out:
	mkdir out

.PHONY: clean
clean:
	rm -rf out