#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sys/time.h>
#include <sys/resource.h>
#include "io.hpp"
#include "cronometro.hpp"
using namespace std;

int llamadas_a_creadora_nodo = 0;

class trie {

private:

    // El numero que representara cada nodo estara en base BASE
    // Utilizamos siempre bases potencias de 2 para calcular el ultimo digito
    // (modulo la base) y el resto de digitos (dividir por la base)
    // rapidamente con una & bit a bit y un shift binario
    // Hemos escogido base 16 (hexadecimal) porque es la que daba mejores
    // resultados
    static const int BASE       = 16;
    static const int MASCARA    = BASE - 1;
    static const int BITS_BASE  = 4; 

    #if _STATS_
        int llamadas_aux;
    #endif

    struct nodo {
        bool existe;
        nodo* hijos[BASE];

        nodo (bool exis) : existe(exis) {

            #if _STATS_
                ++llamadas_a_creadora_nodo;
            #endif

            for (int i = 0; i < BASE; ++i) hijos[i] = NULL;
        }
    };

    nodo* raiz_p; // Trie para los positivos, el 0 se guarda en este nodo
    nodo* raiz_n; // Trie para los negativos, en este nodo solo se guardan hijos

    // PRE: x != 0
    void insertar_imm(int x, nodo* &n) {
        #if _STATS_
        ++llamadas_recursivas_creacion;
        #endif

        if (n == NULL) n = new nodo(false);

        int ult_dig = x & MASCARA;
        int resto   = x >> BITS_BASE;
        nodo* hijo  = n->hijos[ult_dig];

        if (resto == 0) {
            if (hijo == NULL) n->hijos[ult_dig] = new nodo(true);
            else hijo->existe = true;
        }
        else insertar_imm(resto, n->hijos[ult_dig]);
    }

    // PRE: x != 0, n != NULL
    bool contiene_imm(int x, nodo* n) {
        #if _STATS_
            ++llamadas_aux;
        #endif

        int ult_dig = x & MASCARA;
        int resto   = x >> BITS_BASE;
        nodo* hijo  = n->hijos[ult_dig]; 

        if (hijo == NULL) return false;
        else return resto == 0 ? hijo->existe : contiene_imm(resto, hijo);
    }

public:

    #if _STATS_
        int llamadas_recursivas_creacion;
        int llamadas_recursivas_busqueda_fracaso;
        int llamadas_recursivas_busqueda_exito;

    // total_iteraciones = 
    // llamadas_recursivas = 
    // llamadas_recursivas_creacion + llamadas_recursivas_busqueda_exito + llamadas_recursivas_busqueda_fracaso
    // numero comparaciones booleanos/punteros ~~ 2*total_iteraciones
    // numero & y >> ~~ total_iteraciones
    // numero accesos a memoria ~~ total_iteraciones
    
    #endif

    trie() : raiz_p(NULL), raiz_n(NULL) {
        #if _STATS_
        llamadas_recursivas_creacion =
        llamadas_recursivas_busqueda_fracaso =
        llamadas_recursivas_busqueda_exito = 0;
        #endif
    }

    inline void insertar(int x) {
        if (x == 0) {
            if (raiz_p != NULL) raiz_p->existe = true;
            else raiz_p = new nodo(true);
        }
        else if (x > 0) insertar_imm(x, raiz_p);
        else insertar_imm(-x, raiz_n); 
    }

    inline bool contiene(int x) {
        
        #if _STATS_

            if (x == 0) return raiz_p != NULL and raiz_p->existe;
            else {
                llamadas_aux = 0;
                bool encontrado;
                if (x > 0) encontrado = raiz_p != NULL and contiene_imm(x, raiz_p);
                else encontrado = raiz_n != NULL and contiene_imm(-x, raiz_n);

                if (encontrado) llamadas_recursivas_busqueda_exito += llamadas_aux;
                else llamadas_recursivas_busqueda_fracaso += llamadas_aux;

                return encontrado;
            }

        #else

            if (x == 0) return raiz_p != NULL and raiz_p->existe;
            else if (x > 0) return raiz_p != NULL and contiene_imm(x, raiz_p);
            else return raiz_n != NULL and contiene_imm(-x, raiz_n);
        
        #endif
    }
};

trie diccionario_trie;
Cronometro<> insercion, busqueda;

void algoritmo(const VI& diccionario, const VI& texto, VB& resultado) {
    insercion.iniciar();

    for (int x : diccionario) diccionario_trie.insertar(x);
    
    insercion.finalizar();

    busqueda.iniciar();
    
    for (int i = 0; i < texto.size(); ++i)
        resultado[i] = diccionario_trie.contiene(texto[i]);
    
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
                {"tamaño_diccionario", diccionario.size()},
                {"tamaño_texto", texto.size()},
                {
                    "llamadas_recursivas_creacion",
                    diccionario_trie.llamadas_recursivas_creacion
                },
                {
                    "llamadas_recursivas_busqueda_fracaso",
                    diccionario_trie.llamadas_recursivas_busqueda_fracaso
                },
                {
                    "llamadas_recursivas_busqueda_exito",
                    diccionario_trie.llamadas_recursivas_busqueda_exito
                },
                {
                    "llamadas_a_creadora_nodo",
                    llamadas_a_creadora_nodo
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