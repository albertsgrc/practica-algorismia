FLAGS = -std=c++11 -Wall -Wextra -Werror -Wno-uninitialized -Wno-sign-compare -Wshadow -O3 -I./headers

#ALGORITMOS = busqueda-binaria filtro-bloom tabla-hash-cuckoo tabla-hash-lista trie 
ALGORITMOS       = algoritmos/tabla-hash-lista algoritmos/tabla-hash-hopscotch algoritmos/trie algoritmos/filtro-bloom algoritmos/busqueda-binaria
ALGORITMOS_STATS = algoritmos/tabla-hash-lista-stats algoritmos/tabla-hash-hopscotch-stats algoritmos/trie-stats algoritmos/filtro-bloom-stats algoritmos/busqueda-binaria-stats
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
	rm -f $(OTROS) $(ALGORITMOS) $(ALGORITMOS_STATS)

ultraclean:
	rm -f $(OTROS) $(ALGORITMOS) $(ALGORITMOS_STATS)

	rm -rf salidas/*
	rm -rf archivos/*
	rm -rf datos/*