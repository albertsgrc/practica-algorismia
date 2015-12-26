FLAGS = -std=c++1y -Wall -Wextra -Werror -Wno-uninitialized -Wno-sign-compare -Wshadow -O3 -I./headers

#ALGORITMOS = busqueda-binaria filtro-bloom tabla-hash-cuckoo tabla-hash-lista trie 
ALGORITMOS       = algoritmos/tabla-hash-lista algoritmos/tabla-hash-hopscotch-32 algoritmos/tabla-hash-hopscotch-64 algoritmos/trie algoritmos/filtro-bloom algoritmos/busqueda-binaria
ALGORITMOS_STATS = algoritmos/tabla-hash-lista-stats algoritmos/tabla-hash-hopscotch-32-stats algoritmos/tabla-hash-hopscotch-64-stats algoritmos/trie-stats algoritmos/filtro-bloom-stats algoritmos/busqueda-binaria-stats
OTROS      = generador-archivos algoritmo-seguro

all: $(OTROS) $(ALGORITMOS) $(ALGORITMOS_STATS)

generador-archivos: generador-archivos.cc
	g++ $(FLAGS) $< -o $@

algoritmo-seguro: algoritmo-seguro.cc
	g++ $(FLAGS) $< -o $@

algoritmos/tabla-hash-hopscotch-64: algoritmos/tabla-hash-hopscotch.cc
	g++ $(FLAGS) $< -D_HOPSCOTCH_64_ -o $@

algoritmos/tabla-hash-hopscotch-64-stats: algoritmos/tabla-hash-hopscotch.cc
	g++ $(FLAGS) $< -D_HOPSCOTCH_64_ -D_STATS_ -o $@

algoritmos/tabla-hash-hopscotch-32: algoritmos/tabla-hash-hopscotch.cc
	g++ $(FLAGS) $< -o $@

algoritmos/tabla-hash-hopscotch-32-stats: algoritmos/tabla-hash-hopscotch.cc
	g++ $(FLAGS) -D_STATS_ $< -o $@

algoritmos/%: algoritmos/%.cc
	g++ $(FLAGS) $< -o $@

algoritmos/%-stats: algoritmos/%.cc
	g++ $(FLAGS) -D_STATS_ $< -o $@

clean: 
	rm -f $(OTROS) $(ALGORITMOS) $(ALGORITMOS_STATS)

ultraclean:
	rm -f $(OTROS) $(ALGORITMOS) $(ALGORITMOS_STATS)

	rm -rf salidas/*
	rm -f archivos/*