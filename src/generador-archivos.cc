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
         << " n proporcion [numero_textos]" 
         << endl;

    exit(1);
}

void error(const string& msg) {
    cerr << msg << endl;
    exit(1);
}

void parsea_parametros(char* i_n, char* i_proporcion, int& n, double& proporcion) {
    try { n = stoi(i_n); }
    catch (const exception& ia) {
        error("El primer argumento tiene que ser un natural");
    }

    try { proporcion = stod(i_proporcion); }
    catch (const exception& ia) {
        error("El segundo argumento tiene que ser un real entre 0 y 1");
    }

    if (n < 0)
        error("La n tiene que ser positiva");

    if (proporcion > 1.0 or proporcion < 0)
        error("La proporcion tiene que estar entre 0 y 1");
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
void crea_texto(int n, double proporcion, const vector<int>& diccionario, int num) {
    int repetidos = 2*n*proporcion;
    vector<int> texto(2*n);

    for (int i = 0; i < repetidos; ++i) {
        int indice_arxiu1 = rand()%n;
        texto[i] = diccionario[indice_arxiu1];
    }

    for (int i = repetidos; i < texto.size(); ++i) texto[i] = rand();

    random_shuffle(texto.begin(), texto.end());

    ofstream arxiu2;
    arxiu2.open("archivos/arxiu" + to_string(num));

    for (int x : texto) arxiu2 << x << endl;

    arxiu2.close();
}

int main(int argc, char* argv[]) {
    if (argc < 3) usage(argv[0]);

    int n; double proporcion;
    parsea_parametros(argv[1], argv[2], n, proporcion);

    srand(unsigned (time(0)));
    int r = system("mkdir -p archivos");

    if (r < 0) error("Se ha producido un error al crear la carpeta archivos");

    vector<int> diccionario;
    crea_diccionario(n, diccionario);

    int numero_textos = argc > 3 ? atoi(argv[3]) : 1;
    for (int i = 0; i < numero_textos; ++i)
        crea_texto(n, proporcion, diccionario, i + 2);
}
