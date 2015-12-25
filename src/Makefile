FLAGS = -std=c++1y -Wall -Wextra -Werror -Wno-uninitialized -Wno-sign-compare -Wshadow -O3 -I./headers

#ALGORITMOS = busqueda-binaria filtro-bloom tabla-hash-cuckoo tabla-hash-lista trie
ALGORITMOS       = algoritmos/tabla-hash-lista algoritmos/tabla-hash-hopscotch
ALGORITMOS_STATS = algoritmos/tabla-hash-lista-stats algoritmos/tabla-hash-hopscotch-stats
OTROS      = generador-archivos algoritmo-seguro

all: $(OTROS) $(ALGORITMOS) $(ALGORITMOS_STATS)

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