#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <forward_list>
#include <sys/time.h>
#include <sys/resource.h>
#include "io.hpp"
#include "cronometro.hpp"
using namespace std;

class hash_lista {
    
private:
    vector<forward_list<int>> T;

    // Retorna si el entero k esta en la lista l
    inline bool dentro(int k, forward_list<int> &l) {

        #if _STATS_
            int comparaciones = 0;
        #endif

        for (int kit : l) {

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

    // En este caso la funcion de hash es el entero mismo, ya que esto produce
    // un hashing perfecto, y dado que los enteros son aleatorios estaran bien
    // distribuidos en todos los bits (los bits bajos tambien), con lo que cojer
    // modulo potencia de 2 no es una mala solucion. Si no pudiesemos asumir
    // esa distribucion en los bits bajos habria que aplicar una funcion de hash
    // al entero que distribuyese bien el espacio de valores en esos bits
    inline int posicion(int k) {
        return k%T.size();
    }

    inline void insertar(int k) {
        int pos = posicion(k);
        if (not dentro(k, T[pos])) T[pos].push_front(k);
    }

public:
    #if _STATS_

        int total_comparaciones_busqueda_exito;
        int total_comparaciones_busqueda_fracaso;
        int total_comparaciones_creacion;
        int tamano_tabla;

        bool en_creacion;

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

        // total iteraciones = total comparaciones
        // total accesos memoria = total comparaciones

    #endif

    hash_lista() {}
    hash_lista(const VI& v) {

        #if _STATS_
        en_creacion = true;
        total_comparaciones_busqueda_fracaso = 
        total_comparaciones_busqueda_exito =
        total_comparaciones_creacion = 0;
        #endif

        int tamano = 2*v.size();
        T = vector<forward_list<int>>(tamano);

        for (int k : v) insertar(k);

        #if _STATS_
        tamano_tabla = tamano;
        en_creacion = false;
        #endif

    }

    inline bool contiene(int k) {
        int pos = posicion(k);
        return dentro(k, T[pos]); 
    }

    int colisiones() {
        int listas_no_vacias = 0;
        int elementos = 0;

        for (const forward_list<int>& l : T) {
            listas_no_vacias += l.empty() ? 0 : 1;
            for (auto it = l.begin(); it != l.end(); ++it) ++elementos; 
        }

        return elementos - listas_no_vacias;
    }

    // Devuelve el tamano de la lista mas grande de la tabla de hash
    int tamano_maximo_lista() {
        int maximo = 0;

        for (const forward_list<int>& l : T) {
            int elementos = 0;
            for (auto it = l.begin(); it != l.end(); ++it) ++elementos; 
            maximo = max(maximo, elementos);
        }

        return maximo;
    }
};


hash_lista diccionario_hash_lista;
Cronometro<> insercion, busqueda;

void algoritmo(const VI& diccionario, const VI& texto, VB& resultado) {
    insercion.iniciar();
    
    diccionario_hash_lista = hash_lista(diccionario);

    insercion.finalizar();

    busqueda.iniciar();

    for (int i = 0; i < texto.size(); ++i)
        resultado[i] = diccionario_hash_lista.contiene(texto[i]);

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
        // Calculamos el uso maximo de memoria
        rusage resusage;
        getrusage(RUSAGE_SELF, &resusage);

        ofstream estadisticas;
        estadisticas.open(argc > 3 ? argv[3] : "estadisticas.json");
        escribe_json(
            {
                {"tamano_diccionario", diccionario.size()},
                {"tamano_texto", texto.size()},
                {
                    "total_comparaciones_busqueda_fracaso", 
                    diccionario_hash_lista.total_comparaciones_busqueda_fracaso
                },
                {
                    "total_comparaciones_busqueda_exito", 
                    diccionario_hash_lista.total_comparaciones_busqueda_exito
                },
                {
                    "total_comparaciones_creacion", 
                    diccionario_hash_lista.total_comparaciones_creacion
                },
                {
                    "tamano_tabla_hash",
                    diccionario_hash_lista.tamano_tabla
                },
                {
                    "colisiones",
                    diccionario_hash_lista.colisiones()
                },
                {
                    "tamano_maximo_lista",
                    diccionario_hash_lista.tamano_maximo_lista()
                },
                {
                    "memoria_maxima_kb",
                    resusage.ru_maxrss
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