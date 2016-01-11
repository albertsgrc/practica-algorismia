#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <sys/time.h>
#include <sys/resource.h>
#include "io.hpp"
using namespace std;

void algoritmo(const VI& diccionario, const VI& texto, VB& resultado) {
    unordered_set<int> diccionario_hash(diccionario.begin(), diccionario.end());

    for (int i = 0; i < texto.size(); ++i) 
        resultado[i] = diccionario_hash.find(texto[i]) != diccionario_hash.end();

    int cont = 0;
    for (int i = 0; i < resultado.size(); ++i) if (resultado[i]) ++cont;
    cerr << "proporcion " << double(cont)/resultado.size() << endl; 
}

int main(int argc, char* argv[]) {
    if (argc < 3) usage(argv[0]);

    VI diccionario, texto;
    lee_entrada(argv[1], argv[2], diccionario, texto);

    vector<bool> resultado(texto.size());

    algoritmo(diccionario, texto, resultado);

    for (bool b : resultado) cout << b << endl;
}