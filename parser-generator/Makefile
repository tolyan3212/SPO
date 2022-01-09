generator_main : generator_main.o parser.o generator.o string_template.o lexer.o
	g++ -g -o generator_main generator_main.o parser.o generator.o string_template.o lexer.o

generator_main.o : generator_main.cpp generator.h parser.h lexer.h
	g++ -g -c generator_main.cpp

parser.o : parser.cpp parser.h lexer.h
	g++ -g -c parser.cpp

generator.o : generator.cpp generator.h string_template.h parser.h lexer.h
	g++ -g -c generator.cpp

string_template.o : string_template.cpp string_template.h
	g++ -g -c string_template.cpp

lexer.o : lexer.cpp lexer.h
	g++ -g -c lexer.cpp

