#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <forward_list>
#include "io.hpp"
#include "cronometro.hpp"
using namespace std;

#if _HOPSCOTCH_64_
#define T_HOPMAP unsigned long
#else
#define T_HOPMAP unsigned int
#endif

class hash_set {
    
    private:
        struct Bucket {
            bool ocupado;
            T_HOPMAP hopmap;
            int key;

            Bucket() : ocupado(false), hopmap((T_HOPMAP)(0)), key(int()) {}
        };

        vector<Bucket> T;

        int mascara;

        static const int H = 8*sizeof(T_HOPMAP);

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

        inline void insertar(vector<Bucket>& t, int k) {
            int pos = hash_mod(k);
            T_HOPMAP hm = t[pos].hopmap;

            for (int i = 0; i < H; ++i) {
                #if _STATS_
                ++total_saltos_creacion;
                #endif

                int index = (pos+i)&mascara;
                if (hm & (T_HOPMAP)(1)) {
                    #if _STATS_
                    ++total_comparaciones_creacion;
                    #endif

                    if (t[index].key == k) return;
                }
                else if (not t[index].ocupado) {
                    t[index].key = k;
                    t[index].ocupado = true;
                    t[pos].hopmap |= (T_HOPMAP)(1) << ((T_HOPMAP) i);
                    return;
                }

                hm >>= (T_HOPMAP)(1);
            } 

            rehash();
            insertar(t, k);
        }

        inline int cerosFinales(T_HOPMAP x) {
            #if _HOPSCOTCH_64_
            return __builtin_ctzl(x);
            #else
            return __builtin_ctz(x);
            #endif
        }

        inline int siguiente_potencia_2(int x) {
            int p = 1;
            while (p < x) p <<= 1;
            return p;
        }

        inline int hash_mod(int k) {
            return k & mascara;
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

        // avg comparaciones busqueda exito = total comparaciones busqueda exito / exitos
        // avg comparaciones busqueda fracaso = total comparaciones busqueda fracaso / fracasos
        // total comparaciones busqueda = total_comparaciones_busqueda_exito + total_comparaciones_busqueda_fracaso
        // avg comparaciones busqueda = total comparaciones busqueda / tamano texto

        // avg comparaciones creacion = total comparaciones creacion / tamano diccionario
        // total comparaciones = total_comparaciones busqueda + total comparaciones creacion
        // avg comparaciones = total comparaciones / (tamano texto + tamano diccionario)

        // total iteraciones = total_saltos_creacion + total_comparaciones_busqueda
        // total accesos memoria = total comparaciones (en este caso seguramente de la misma línea de cache)

        #endif

        hash_set() {}
        hash_set(const vector<int>& v) {
            #if _STATS_

            total_rehashes = total_saltos_creacion = 
            total_comparaciones_creacion = 
            total_comparaciones_busqueda_fracaso =
            total_comparaciones_busqueda_exito = 0;

            #endif

            int tamano = siguiente_potencia_2(2*v.size());
            T = vector<Bucket>(tamano);
            mascara = tamano - 1;

            for (int k : v) insertar(T, k);

            #if _STATS_
            tamano_tabla = tamano;
            #endif
        }

        inline bool contiene(int k) {
            int pos = hash_mod(k);
            T_HOPMAP hm = T[pos].hopmap;

            #if _STATS_
            int comparaciones = 0;
            #endif

            while (hm > (T_HOPMAP)(0)) {
                int index = (pos + cerosFinales(hm)) & mascara;

                #if _STATS_
                ++comparaciones;
                #endif

                if (T[index].key == k) {
                    #if _STATS_
                    total_comparaciones_busqueda_exito += comparaciones;
                    #endif
                    return true;
                }

                // Eliminamos el 1 final
                hm &= hm - (T_HOPMAP)(1);
            }

            #if _STATS_
            total_comparaciones_busqueda_fracaso += comparaciones;
            #endif

            return false;
        }
};


hash_set diccionario_hash;

void algoritmo(const VI& diccionario, const VI& texto, VB& resultado) {
    diccionario_hash = hash_set(diccionario);

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
                "total_rehashes",
                diccionario_hash.total_rehashes
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