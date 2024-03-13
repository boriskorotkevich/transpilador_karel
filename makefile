OBJETOS = main.o
CXX = g++
CXXFLAGS = -std=c++20

build: ${OBJETOS}
	${CXX} -o main ${OBJETOS}

main.o: lexer_pascal.h lexer_java.h error.h debug.h

clean:
	rm -f ${OBJETOS} main .*.swp
