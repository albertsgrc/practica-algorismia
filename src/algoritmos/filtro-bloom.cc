#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include "io.hpp"
#include "cronometro.hpp"
using namespace std;

class filtro_bloom {

private:

    int mascara_modulo;
    VB bits;
    VI funciones_hash; // i.e. semillas de la funcion

    constexpr static const double LN_2 = 0.69314718055994530941;
    constexpr static const double OPT  = 9.0/13.0;

    constexpr static const double PROB_FALSO_POSITIVO = 0.000001;

    // Calcula la posicion de la llave k con una funcion de hash basada en 
    // murmurhash2, y usando la h como semilla 
    inline int posicion(unsigned int h, unsigned int k) {
        const unsigned int m = 0x5bd1e995;
        const int r = 24;

        h ^= 4;
        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;

        return h & mascara_modulo;
    }

    void insertar(int x) {
        for (int funcion : funciones_hash) {
            bits[posicion(funcion, x)] = true;
        }
    }

    // Devuelve la siguiente potencia de 2 >= x
    inline int siguiente_potencia_2(int x) {
        int p = 1;
        while (p < x) p <<= 1;
        return p;
    }

public:

    #if _STATS_

    int numero_funciones;
    int tamano_vector_bits;
    int hashes_busqueda_fracaso;
    int hashes_busqueda_exito;

    // hashes_creacion = numero_funciones*tamaño_diccionario
    // hashes_busqueda = hashes_busqueda_exito + hashes_busqueda_fracaso
    // total iteraciones = hashes_busqueda + hashes_creacion

    #endif

    filtro_bloom() {}
    filtro_bloom(const VI& v) {
        int n = v.size();
        // Calculamos el tamaño del vector de bits en funcion de la probabilidad
        int m = log(PROB_FALSO_POSITIVO)*n/(-OPT*LN_2);
        m = siguiente_potencia_2(m);

        mascara_modulo = m - 1;

        bits = VB(m);

        int n_funciones = LN_2*double(m)/n;

        funciones_hash = VI(n_funciones);

        srand(time(NULL));

        // Creamos las semillas de la familia de funciones de hash
        // con un numero aleatorio impar
        for (int& funcion : funciones_hash) funcion = 2*rand() - 1;
        
        #if _STATS_
            tamano_vector_bits = m;
            numero_funciones = funciones_hash.size();
            hashes_busqueda_exito = hashes_busqueda_fracaso = 0;
        #endif

        for (int x : v) insertar(x);
    }

    bool contiene(int x) {
        #if _STATS_
            int hashes = 0;
        #endif

        for (int funcion : funciones_hash) {

            #if _STATS_
                ++hashes;
            #endif

            if (not bits[posicion(funcion, x)]) {

                #if _STATS_
                    hashes_busqueda_fracaso += hashes;
                #endif

                return false;
            }
        }

        #if _STATS_
            hashes_busqueda_exito += hashes;
        #endif
        
        return true;
    }
};

filtro_bloom diccionario_bloom;
Cronometro<> insercion, busqueda;

void algoritmo(const VI& diccionario, const VI& texto, VB& resultado) {
    insercion.iniciar();
    diccionario_bloom = filtro_bloom(diccionario);
    insercion.finalizar();

    busqueda.iniciar();
    for (int i = 0; i < texto.size(); ++i)
        resultado[i] = diccionario_bloom.contiene(texto[i]);
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
                {"tamaño_diccionario", diccionario.size()},
                {"tamaño_texto", texto.size()},
                {
                    "numero_funciones",
                    diccionario_bloom.numero_funciones
                },
                {
                    "tamano_vector_bits",
                    diccionario_bloom.tamano_vector_bits
                },
                {
                    "hashes_busqueda_exito",
                    diccionario_bloom.hashes_busqueda_exito
                },
                {
                    "hashes_busqueda_fracaso",
                    diccionario_bloom.hashes_busqueda_fracaso
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
