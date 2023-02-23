SOURCES = micro.parser.c micro.lexer.c micro.parser.h
EXEC_NAME = micro

OBJECTS = $(filter-out %.h, $(SOURCES))

all : $(EXEC_NAME)

$(EXEC_NAME) : $(OBJECTS)
	gcc -o $@ $^ -lm

%.parser.h : %.parser.c

%.parser.c : %.y
	bison -d -o $@ $<

%.lexer.c : %.l
	flex -o $@ $<

clean:
	rm -rf *.c *.o *.h $(EXEC_NAME)
