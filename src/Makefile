FLAGS = -std=c++1y -Wall -Wextra -Werror -Wno-uninitialized -Wno-sign-compare -Wshadow -O2 -I./headers

all: generador busqueda-binaria filtro-bloom tabla-hash-cuckoo tabla-hash-lista trie

generador: generador_archivos.cpp
	g++ $(FLAGS) $< -o generador_archivos.exe

busqueda-binaria:  algoritmos/busqueda-binaria/main.cpp
	g++ $(FLAGS) $< -o algoritmos/$@/main.exe

filtro-bloom: algoritmos/filtro-bloom/main.cpp
	g++ $(FLAGS) $< -o algoritmos/$@/main.exe

tabla-hash-cuckoo: algoritmos/tabla-hash-cuckoo/main.cpp
	g++ $(FLAGS) $< -o algoritmos/$@/main.exe

tabla-hash-lista: algoritmos/tabla-hash-lista/main.cpp
	g++ $(FLAGS) $< -o algoritmos/$@/main.exe

trie: algoritmos/trie/main.cpp
	g++ $(FLAGS) $< -o algoritmos/$@/main.exe

clean:
	find . -name "*.exe" -type f -delete

ultraclean:
	find . \( -name "*.exe" -type f -o  -name "*.out" -type f -o -name "*.json" -type f \) -delete
	rm -f archivos/*