#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

void usage(string prog) {
    cerr << "Usage: "
         << prog 
         << " n proporcion factor_texto_diccionario" 
         << "proporcion entre 0 y 1" << endl
         << "factor_texto_diccionario es el tamaño del texto en funcion del tamaño diccionario, como minimo 2"
         << endl;

    exit(1);
}

void error(const string& msg) {
    cerr << msg << endl;
    exit(1);
}

void parsea_parametros(char* i_n, char* i_proporcion, char* i_factor_texto_diccionario, int& n, double& proporcion, double& factor_texto_diccionario) {
    try { n = stoi(i_n); }
    catch (const exception& ia) {
        error("El primer argumento tiene que ser un natural");
    }

    try { proporcion = stod(i_proporcion); }
    catch (const exception& ia) {
        error("El segundo argumento tiene que ser un real entre 0 y 1");
    }

    try { factor_texto_diccionario = stod(i_factor_texto_diccionario); }
    catch (const exception& ia) {
        error("El tercero argumento tiene que ser un real >= 2");
    }

    if (n < 0)
        error("La n tiene que ser positiva");

    if (proporcion > 1.0 or proporcion < 0)
        error("La proporcion tiene que estar entre 0 y 1");

    if (factor_texto_diccionario < 2)
        error("El tamaño del texto en funcion del tamaño del diccionari tiene que ser como minimo 2");
}

void crea_diccionario(int n, vector<int>& diccionario) {
    diccionario = vector<int>(n);

    ofstream arxiu1;
    arxiu1.open("archivos/arxiu1");

    for (int& x : diccionario) {
        x = rand();
        arxiu1 << x << endl;
    }

    arxiu1.close();
}

// TODO: Això s'ha d'arreglar
void crea_texto(int n, double proporcion, double factor_texto_diccionario, const vector<int>& diccionario) {
    int repetidos = factor_texto_diccionario*n*proporcion;
    vector<int> texto(factor_texto_diccionario*n);

    for (int i = 0; i < repetidos; ++i) {
        int indice_arxiu1 = rand()%n;
        texto[i] = diccionario[indice_arxiu1];
    }

    for (int i = repetidos; i < texto.size(); ++i) texto[i] = rand();

    random_shuffle(texto.begin(), texto.end());

    ofstream arxiu2;
    arxiu2.open("archivos/arxiu2");

    for (int x : texto) arxiu2 << x << endl;

    arxiu2.close();
}

int main(int argc, char* argv[]) {
    if (argc < 3) usage(argv[0]);

    int n; double proporcion, factor_texto_diccionario;
    parsea_parametros(argv[1], argv[2], argv[3], n, proporcion, factor_texto_diccionario);

    srand(unsigned (time(0)));
    int r = system("mkdir -p archivos");

    if (r < 0) error("Se ha producido un error al crear la carpeta archivos");

    vector<int> diccionario;
    crea_diccionario(n, diccionario);
    crea_texto(n, proporcion, factor_texto_diccionario, diccionario);
}
