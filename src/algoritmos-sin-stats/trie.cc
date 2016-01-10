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

    struct nodo {
        bool existe;
        nodo* hijos[BASE];

        nodo (bool exis) : existe(exis) {
            for (int i = 0; i < BASE; ++i) hijos[i] = NULL;
        }
    };

    nodo* raiz_p; // Trie para los positivos, el 0 se guarda en este nodo
    nodo* raiz_n; // Trie para los negativos, en este nodo solo se guardan hijos

    // PRE: x != 0
    void insertar_imm(int x, nodo* &n) {
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
        int ult_dig = x & MASCARA;
        int resto   = x >> BITS_BASE;
        nodo* hijo  = n->hijos[ult_dig]; 

        if (hijo == NULL) return false;
        else return resto == 0 ? hijo->existe : contiene_imm(resto, hijo);
    }

public:

    trie() : raiz_p(NULL), raiz_n(NULL) {}

    inline void insertar(int x) {
        if (x == 0) {
            if (raiz_p != NULL) raiz_p->existe = true;
            else raiz_p = new nodo(true);
        }
        else if (x > 0) insertar_imm(x, raiz_p);
        else            insertar_imm(-x, raiz_n); 
    }

    inline bool contiene(int x) {
        if (x == 0)     return raiz_p != NULL and raiz_p->existe;
        else if (x > 0) return raiz_p != NULL and contiene_imm(x, raiz_p);
        else            return raiz_n != NULL and contiene_imm(-x, raiz_n);
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
}