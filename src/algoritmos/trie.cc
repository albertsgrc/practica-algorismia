#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "io.hpp"
#include "cronometro.hpp"
using namespace std;

class trie {
private:
    // Siempre base potencia de 2
    static const int BASE    = 16;
    static const int MASCARA = BASE - 1;
    static const int POT_2   = 4; 

    struct nodo {
        bool existe;
        nodo* hijos[BASE];

        nodo (bool exis) : existe(exis) {
            for (int i = 0; i < BASE; ++i) hijos[i] = NULL;
        }
    };

    nodo* raiz_p; // Trie para los positivos
    nodo* raiz_n; // Trie para los negativos

    void insertar_imm(int x, nodo* &n) {
        if (n == NULL) n = new nodo(false);

        int ult_dig = x & MASCARA;
        int resto   = x >> POT_2;
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
        int resto   = x >> POT_2;
        nodo* hijo  = n->hijos[ult_dig]; 

        return hijo != NULL and 
              (resto == 0 ? hijo->existe : contiene_imm(resto, hijo));
    }

public:
    trie() : raiz_p(NULL), raiz_n(NULL) {}

    inline void insertar(int x) {
        if (x == 0) {
            if (raiz_p != NULL) raiz_p->existe = true;
            else raiz_p = new nodo(true);
        }
        else if (x > 0) insertar_imm(x, raiz_p);
        else insertar_imm(-x, raiz_n); 
    }

    inline bool contiene(int x) {
        if (x == 0) return raiz_p != NULL and raiz_p->existe;
        else if (x > 0) return raiz_p != NULL and contiene_imm(x, raiz_p);
        else return raiz_n != NULL and contiene_imm(-x, raiz_n);
    }
};

void algoritmo(const VI& diccionario, const VI& texto, VB& resultado) {
    trie diccionario_trie;

    for (int x : diccionario) diccionario_trie.insertar(x);

    for (int i = 0; i < texto.size(); ++i) 
        resultado[i] = diccionario_trie.contiene(texto[i]);
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
            {"tamaño_diccionario", diccionario.size()},
            {"tamaño_texto", texto.size()}
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