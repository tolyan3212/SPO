main : main.o executor.o pn.o parser.o lexer.o triad_optimizer.o pn_token.o tree_optimizer.o triad.o
	g++ -g -o main main.o executor.o pn.o parser.o lexer.o triad_optimizer.o pn_token.o tree_optimizer.o triad.o

main.o : main.cpp executor.h parser.h triad_optimizer.h pn.h triad.h tree_optimizer.h lexer.h pn_token.h
	g++ -g -c main.cpp

executor.o : executor.cpp executor.h parser.h pn.h tree_optimizer.h lexer.h pn_token.h
	g++ -g -c executor.cpp

pn.o : pn.cpp parser.h pn.h tree_optimizer.h lexer.h pn_token.h
	g++ -g -c pn.cpp

parser.o : parser.cpp lexer.h parser.h
	g++ -g -c parser.cpp

lexer.o : lexer.cpp lexer.h
	g++ -g -c lexer.cpp

triad_optimizer.o : triad_optimizer.cpp parser.h triad_optimizer.h pn.h triad.h tree_optimizer.h lexer.h pn_token.h
	g++ -g -c triad_optimizer.cpp

pn_token.o : pn_token.cpp lexer.h pn_token.h
	g++ -g -c pn_token.cpp

tree_optimizer.o : tree_optimizer.cpp lexer.h tree_optimizer.h parser.h
	g++ -g -c tree_optimizer.cpp

triad.o : triad.cpp triad.h pn.h
	g++ -g -c triad.cpp

