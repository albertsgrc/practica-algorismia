#include <vector>
#include <forward_list>
using namespace std;

template<typename Key>
class hash_set {
	
	private:
		vector<forward_list<Key>> T;

		inline bool dentro(const Key &k, forward_list<Key> &l) {
			#if _STATS_
			int comparaciones = 0;
			#endif

			for (Key& kit : l) {
				#if _STATS_
				++comparaciones;
				#endif

				if (kit == k) {
					#if _STATS_
						if (en_creacion) total_comparaciones_creacion += comparaciones;
						else total_comparaciones_busqueda_exito += comparaciones;
					#endif

					return true;
				}
			}

			#if _STATS_
				if (en_creacion) total_comparaciones_creacion += comparaciones;
				else total_comparaciones_busqueda_fracaso += comparaciones;
			#endif

			return false; 
		}

		inline void insertar(const Key& k) {
			int pos = hash<Key>()(k)&(T.size() - 1);
			if (not dentro(k, T[pos])) T[pos].push_front(k);
		}

	public:
		#if _STATS_

		int total_comparaciones_busqueda_exito;
		int total_comparaciones_busqueda_fracaso;
		int total_comparaciones_creacion;

		bool en_creacion;

		// llamadas a insertar = tamaño diccionario
		// llamadas a contiene = tamaño texto 
		// llamadas a hash = llamadas a insertar + llamadas a contiene

		#endif

		hash_set() {}
		hash_set(const vector<Key>& v) {
			#if _STATS_
			en_creacion = true;
			total_comparaciones_busqueda_fracaso = 0;
			total_comparaciones_busqueda_exito = 0;
			total_comparaciones_creacion = 0;
			#endif

			int size = 1;
			int desiredsize = 2*v.size();
			while (size < desiredsize) size <<= 2;
			T = vector<forward_list<Key>>(size);

			for (const Key& k : v) insertar(k);

			#if _STATS_
			en_creacion = false;
			#endif
		}

		inline bool contiene(const Key &k) {
			int pos = hash<Key>()(k)&(T.size() - 1);
			return dentro(k, T[pos]); 
		}

		int colisiones() {
			int occupied_buckets = 0;
			int elements = 0;

			for (const forward_list<Key>& l : T) {
				occupied_buckets += l.empty() ? 0 : 1;
				for (auto it = l.begin(); it != l.end(); ++it) ++elements; 
			}

			return elements - occupied_buckets;
		}
};
