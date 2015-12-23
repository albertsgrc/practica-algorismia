#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

typedef vector<int>  VI;
typedef vector<bool> VB;

void usage() {
    cerr << "programa path_fichero_diccionario path_fichero_texto" << endl;
    exit(1);
}

void error(string msg) {
    cerr << msg << endl;
    exit(1);
}

void lee_entrada(char* path_diccionario, char* path_texto,
                 VI& diccionario, VI&texto) {
    ifstream fich_diccionario, fich_texto;

    fich_diccionario.open(path_diccionario);

    if (not fich_diccionario.is_open()) 
        error("Path del fichero del diccionario inválido");

    fich_texto.open(path_texto);

    if (not fich_texto.is_open())
        error("Path del fichero del texto inválido");

    int x;
    while (fich_diccionario >> x) diccionario.push_back(x);
    while (fich_texto >> x)       texto.push_back(x);
}

void algoritmo(const VI& diccionario, const VI& texto, VB& resultado) {

}

int main(int argc, char* argv[]) {
    if (argc < 3) usage();

    VI diccionario, texto;
    lee_entrada(argv[1], argv[2], diccionario, texto);

    vector<bool> resultado(diccionario.size());
    algoritmo(diccionario, texto, resultado);

    for (bool b : resultado) cout << b << endl;
}