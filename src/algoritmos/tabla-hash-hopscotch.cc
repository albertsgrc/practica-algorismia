#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <forward_list>
#include "io.hpp"
#include "cronometro.hpp"
using namespace std;

template<typename Key>
class hash_set {
    
    private:
        struct Bucket {
            bool ocupado;
            long hopmap;
            Key key;

            Bucket() : ocupado(false), hopmap(0), key(Key()) {}
        };

        vector<Bucket> T;

        int mascara;

        static const int H = 8*sizeof(long);

        inline void rehash() {
            #if _STATS_
            ++total_rehashes;
            #endif

            int nuevo_tamano = 2*T.size();
            vector<Bucket> aux(nuevo_tamano);
            mascara = nuevo_tamano - 1;

            for (const Bucket& b : T) insertar(aux, b.key);

            T.swap(aux);
        }

        inline void insertar(vector<Bucket>& t, const Key& k) {
            int pos = hash<Key>()(k)&mascara;
            long hm = t[pos].hopmap;

            for (int i = 0; i < H; ++i) {
                #if _STATS_
                ++total_saltos_creacion;
                #endif

                int index = (pos+i)&mascara;
                if (hm & 1) {
                    #if _STATS_
                    ++total_comparaciones_creacion;
                    #endif

                    if (t[index].key == k) return;
                }
                else if (not t[index].ocupado) {
                    t[index].key = k;
                    t[index].ocupado = true;
                    t[pos].hopmap |= 1 << i;
                    return;
                }
                hm >>= 1;
            } 

            rehash();
            insertar(t, k);
        }

        inline int cerosFinales(int i) {
            return __builtin_ctz(i);
        }

    public:
        #if _STATS_

        int total_rehashes;

        int total_saltos_creacion;
        int total_comparaciones_creacion;

        int total_comparaciones_busqueda_fracaso;
        int total_comparaciones_busqueda_exito;

        int tamano_tabla;

        // llamadas a insertar = tamaño diccionario
        // llamadas a contiene = tamaño texto 
        // llamadas a hash = llamadas a insertar + llamadas a contiene

        #endif

        hash_set() {}
        hash_set(const vector<Key>& v) {
            #if _STATS_

            total_rehashes = 0;

            total_saltos_creacion = 0;
            total_comparaciones_creacion = 0;

            total_comparaciones_busqueda_fracaso = 0;
            total_comparaciones_busqueda_exito = 0;

            #endif

            int desiredsize = 2*v.size();
            int size = 1;
            while (size < desiredsize) size <<= 1;
            T = vector<Bucket>(size);
            mascara = size - 1;

            for (const Key& k : v) insertar(T, k);

            #if _STATS_
            tamano_tabla = size;
            #endif
        }

        inline bool contiene(const Key &k) {
            int pos = hash<Key>()(k)&mascara;
            long hm = T[pos].hopmap;

            #if _STATS_
            int comparaciones = 0;
            #endif

            while (hm > 0) {
                int index = (pos + cerosFinales(hm))&mascara;

                #if _STATS_
                ++comparaciones;
                #endif

                if (T[index].key == k) {
                    #if _STATS_
                    total_comparaciones_busqueda_exito += comparaciones;
                    #endif
                    return true;
                }

                hm &= hm - 1;
            }

            #if _STATS_
            total_comparaciones_busqueda_fracaso += comparaciones;
            #endif

            return false;
        }
};


hash_set<int> diccionario_hash;

void algoritmo(const VI& diccionario, const VI& texto, VB& resultado) {
    diccionario_hash = hash_set<int>(diccionario);

    for (int i = 0; i < texto.size(); ++i) 
        resultado[i] = diccionario_hash.contiene(texto[i]);
}

int main(int argc, char* argv[]) {
    if (argc < 3) usage(argv[0]);

    VI diccionario, texto;
    lee_entrada(argv[1], argv[2], diccionario, texto);

    vector<bool> resultado(texto.size());

    Cronometro<> c;
    c.iniciar();
    algoritmo(diccionario, texto, resultado);
    c.finalizar();

    for (bool b : resultado) cout << b << endl;

    #if _STATS_
    ofstream estadisticas;
    estadisticas.open(argc > 3 ? argv[3] : "estadisticas.json");
    escribe_json(
        {
            {"tamano_diccionario", diccionario.size()},
            {"tamano_texto", texto.size()},
            {
                "total_comparaciones_busqueda_fracaso", 
                diccionario_hash.total_comparaciones_busqueda_fracaso
            },
            {
                "total_comparaciones_busqueda_exito", 
                diccionario_hash.total_comparaciones_busqueda_exito
            },
            {
                "total_comparaciones_creacion", 
                diccionario_hash.total_comparaciones_creacion
            },
            {
                "total_saltos_creacion",
                diccionario_hash.total_saltos_creacion
            },
            {
                "tamano_tabla_hash",
                diccionario_hash.tamano_tabla
            }
        }
        ,
        estadisticas);
    #else
    ofstream tiempo;
    tiempo.open(argc > 3 ? argv[3] : "tiempo.out");
    tiempo << c.transcurrido();
    tiempo.close();
    #endif
}