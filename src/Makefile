FLAGS = -std=c++1y -Wall -Wextra -Werror -Wno-uninitialized -Wno-sign-compare -Wshadow -O3 -I./headers

#ALGORITMOS = busqueda-binaria filtro-bloom tabla-hash-cuckoo tabla-hash-lista trie
ALGORITMOS = tabla-hash-lista
OTROS      = generador-archivos algoritmo-seguro

all: $(OTROS) algoritmos/$(ALGORITMOS) algoritmos/$(ALGORITMOS)-stats

generador-archivos: generador-archivos.cc
	g++ $(FLAGS) $< -o $@

algoritmo-seguro: algoritmo-seguro.cc
	g++ $(FLAGS) $< -o $@

algoritmos/%: algoritmos/%.cc
	g++ $(FLAGS) $< -o $@

algoritmos/%-stats: algoritmos/%.cc
	g++ $(FLAGS) -D_STATS_ $< -o $@

clean: 
	for algo in $(ALGORITMOS); do \
		rm -f algoritmos/$$algo; \
		rm -f algoritmos/$$algo-stats; \
	done
	rm -f $(OTROS)

ultraclean:
	for algo in $(ALGORITMOS); do \
		rm -f algoritmos/$$algo; \
		rm -f algoritmos/$$algo-stats; \
	done
	rm -f $(OTROS)

	rm -rf salidas/*
	rm -f archivos/*