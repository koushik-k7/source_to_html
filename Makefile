SRC := $(wildcard *.c)
OBJ := $(patsubst %.c, %.o, $(SRC))

c_to_html_converter : $(OBJ)
	gcc -o $@ $^

clean_obj : 
	rm *.o
clean_all :
	rm *.o *.out
