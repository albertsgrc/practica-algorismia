FLAGS = -std=c++1y -Wall -Wextra -Werror -Wno-uninitialized -Wno-sign-compare -Wshadow -O3 -I./headers

ALGORITMOS = busqueda-binaria filtro-bloom tabla-hash-cuckoo tabla-hash-lista trie
OTROS      = generador-archivos algoritmo-seguro

all: $(OTROS) algoritmos/$(ALGORITMOS)/main algoritmos/$(ALGORITMOS)/main-stats

generador-archivos: generador-archivos.cc
	g++ $(FLAGS) $< -o $@

algoritmo-seguro: algoritmo-seguro.cc
	g++ $(FLAGS) $< -o $@

algoritmos/%/main: algoritmos/%/main.cc
	g++ $(FLAGS) $< -o $@

algoritmos/%/main-stats: algoritmos/%/main.cc
	g++ $(FLAGS) -D_STATS_ $< -o $@

clean: 
	rm -f algoritmos/$(ALGORITMOS)/main
	rm -f algoritmos/$(ALGORITMOS)/main-stats
	rm -f $(OTROS)

ultraclean:
	find algoritmos \( -name "main" -type f -o -name "main-stats" -type f \) -delete
	rm -f $(OTROS)

	find . \( -name "*.out" -type f -o -name "*.json" -type f \) -delete
	rm -f archivos/*