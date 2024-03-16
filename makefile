OBJETOS = main.o
CXX = g++
CXXFLAGS = -std=c++20

build: ${OBJETOS}
	${CXX} -o main ${OBJETOS}

clean:
	rm -f ${OBJETOS} main .*.swp
