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
        for (int kit : l)
            if (kit == k) return true;
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
    hash_lista() {}
    hash_lista(const VI& v) {
        int tamano = 2*v.size();
        T = vector<forward_list<int>>(tamano);

        for (int k : v) insertar(k);
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