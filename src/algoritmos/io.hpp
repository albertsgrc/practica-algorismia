#include <iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

typedef vector<int>  VI;
typedef vector<bool> VB;

void usage(string prog) {
    cout << "Usage: " 
         << prog
         << " path_fichero_diccionario"
         << " path_fichero_texto"
         << " [path_salida_datos]" << endl << endl
         << "El programa escribe por la salida estándar una lista de 0 y 1 "
         << "que indica si el entero que está en la misma línea en el "
         << "fichero del texto pertenece al diccionario." << endl;

    exit(0);
}

void writeJson(vector<pair<string, int>> data, ostream& os) {
    os << "{";
    for (int i = 0; i < data.size(); ++i) {
        if (i > 0) os << ",";
        os << endl 
           << "    \"" << data[i].first << "\": " << data[i].second; 
    }
    os << endl << "}";
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
    while (fich_texto       >> x) texto.push_back(x);

    fich_diccionario.close();
    fich_texto.close();
}

