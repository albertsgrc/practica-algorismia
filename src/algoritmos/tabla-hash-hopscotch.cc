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

class hash_hopscotch {
    
    private:
        struct Bucket {
            bool ocupado;
            T_HOPMAP mapa_saltos;
            int clave;

            Bucket() : ocupado(false), mapa_saltos((T_HOPMAP)(0)), clave(int()) {}
        };

        vector<Bucket> T;

        int mascara_modulo;

        static const int H = 8*sizeof(T_HOPMAP);

        inline void rehash() {

            #if _STATS_
                ++total_rehashes;
            #endif

            int nuevo_tamano = 2*T.size();
            vector<Bucket> aux(nuevo_tamano);
            mascara_modulo = nuevo_tamano - 1;

            for (const Bucket& b : T) insertar(aux, b.clave);

            T.swap(aux);
        }

        inline void insertar(vector<Bucket>& t, int k) {
            int pos = posicion(k);

            // Miramos las posiciones del vecindario de tamaño H de t[pos]
            // hasta que encontremos una libre. En este caso no podemos
            // dar saltos ya que hay que comprobar que el elemento no esté
            // ya en la tabla
            for (int i = 0; i < H; ++i) {

                #if _STATS_
                    ++total_saltos_creacion;
                #endif

                int indice_salto = (pos + i) & mascara_modulo;

                if (not t[indice_salto].ocupado) { // Si no esta ocupado
                    // Entonces lo ocupamos, actualizando el mapa de saltos
                    t[indice_salto].clave = k;
                    t[indice_salto].ocupado = true;
                    t[pos].mapa_saltos |= (T_HOPMAP)(1) << ((T_HOPMAP) i);
                    return;
                }
                else {

                    #if _STATS_
                        ++total_comparaciones_creacion;
                    #endif

                    // Si ya existe no hay que insertar
                    if (t[indice_salto].clave == k) return; 
                }
            } 

            rehash();
            insertar(t, k);
        }

        // builtin_ctz se traduce a una instruccion de lenguaje maquina
        // (si la maquina la soporta, lo cual es bastante comun)
        // que cuenta los bits a 0 a partir del bit a 1 de menos peso
        // Ej.: 101101000 daria 3 por los 3 ultimos ceros
        // Esto es útil para saltar bits que esten a 0 del mapa de hopscotch
        // que hay en cada posicion de la tabla
        inline int cerosFinales(T_HOPMAP x) {
            #if _HOPSCOTCH_64_
                return __builtin_ctzl(x);
            #else
                return __builtin_ctz(x);
            #endif
        }

        // Devuelve la siguiente potencia de 2 >= x
        inline int siguiente_potencia_2(int x) {
            int p = 1;
            while (p < x) p <<= 1;
            return p;
        }

        // En este caso la funcion de hash es el entero mismo, ya que esto produce
        // un hashing perfecto
        inline int posicion(int k) {
            return k & mascara_modulo;
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

        hash_hopscotch() {}
        hash_hopscotch(const VI& v) {
            #if _STATS_
                total_rehashes = total_saltos_creacion = 
                total_comparaciones_creacion = 
                total_comparaciones_busqueda_fracaso =
                total_comparaciones_busqueda_exito = 0;
            #endif

            int tamano = siguiente_potencia_2(2*v.size());
            T = vector<Bucket>(tamano);
            mascara_modulo = tamano - 1;

            for (int k : v) insertar(T, k);

            #if _STATS_
                tamano_tabla = tamano;
            #endif

        }

        inline bool contiene(int k) {
            int pos = posicion(k);
            T_HOPMAP ms = T[pos].mapa_saltos;

            #if _STATS_
                int comparaciones = 0;
            #endif

            // Miramos si está en el vecindario de tamaño H de T[pos]                
            while (ms > (T_HOPMAP)(0)) {
                // saltar al siguiente indice que indique el mapa de saltos
                // nunca nos saldremos del vecindario ya que ms > 0, 
                // entonces siempre hay algun uno en el mapa, y cerosFinales < H
                int indice_salto = (pos + cerosFinales(ms)) & mascara_modulo;

                #if _STATS_
                    ++comparaciones;
                #endif

                if (T[indice_salto].clave == k) {

                    #if _STATS_
                        total_comparaciones_busqueda_exito += comparaciones;
                    #endif

                    return true;
                }

                // Eliminamos el bit a 1 de menos peso
                ms &= ms - (T_HOPMAP)(1);
            }

            #if _STATS_
                total_comparaciones_busqueda_fracaso += comparaciones;
            #endif

            return false;
        }
};


hash_hopscotch diccionario_hash_hopscotch;
Cronometro<> insercion, busqueda;

void algoritmo(const VI& diccionario, const VI& texto, VB& resultado) {
    insercion.iniciar();
    diccionario_hash_hopscotch = hash_hopscotch(diccionario);
    insercion.finalizar();

    busqueda.iniciar();
    for (int i = 0; i < texto.size(); ++i)
        resultado[i] = diccionario_hash_hopscotch.contiene(texto[i]);
    busqueda.finalizar();
}


int main(int argc, char* argv[]) {
    if (argc < 3) usage(argv[0]);

    VI diccionario, texto;
    lee_entrada(argv[1], argv[2], diccionario, texto);

    VB resultado(texto.size());

    Cronometro<> total;
    total.iniciar();
    algoritmo(diccionario, texto, resultado);
    total.finalizar();

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
                    diccionario_hash_hopscotch.total_comparaciones_busqueda_fracaso
                },
                {
                    "total_comparaciones_busqueda_exito", 
                    diccionario_hash_hopscotch.total_comparaciones_busqueda_exito
                },
                {
                    "total_comparaciones_creacion", 
                    diccionario_hash_hopscotch.total_comparaciones_creacion
                },
                {
                    "total_saltos_creacion",
                    diccionario_hash_hopscotch.total_saltos_creacion
                },
                {
                    "total_rehashes",
                    diccionario_hash_hopscotch.total_rehashes
                },
                {
                    "tamano_tabla_hash",
                    diccionario_hash_hopscotch.tamano_tabla
                }
            }
        , estadisticas);
        estadisticas.close();
    #else
        ofstream tiempo;
        tiempo.open(argc > 3 ? argv[3] : "tiempo.json");
        escribe_json(
            {
                {"tiempo_total", total.transcurrido()},
                {"tiempo_insercion", insercion.transcurrido()},
                {"tiempo_busqueda", busqueda.transcurrido()}
            }
        ,
        tiempo);
        tiempo.close();
    #endif
}