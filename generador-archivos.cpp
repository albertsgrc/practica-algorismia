#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void usage() {
    cerr << "Usage: generador-archivos n proporcion" << endl;
    exit(1);
}

void error(const string& msg) {
    cerr << msg << endl;
    exit(1);
}

int randint() {
    return rand()&1 ? rand() : -rand();
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

void crear_arxiu1(int n, vector<int>& diccionario) {
    diccionario = vector<int>(n);

    ofstream arxiu1;
    arxiu1.open("arxiu1");

    for (int& x : diccionario) {
        x = randint();
        arxiu1 << x << endl;
    }

    arxiu1.close();
}

void crear_arxiu2(int n, double proporcion, const vector<int>& diccionario) {
    int repetidos = n*proporcion/100;
    vector<int> texto(2*n);

    for (int&x : texto) x = randint();

    for (int i = 0; i < repetidos; ++i) {
        int indice_arxiu1 = rand()%n;
        int indice_arxiu2 = rand()%(2*n);
        texto[indice_arxiu2] =  diccionario[indice_arxiu1];
    }

    ofstream arxiu2;
    arxiu2.open("arxiu2");
    for (int x : texto) arxiu2 << x << endl;

    arxiu2.close();
}

int main(int argc, char* argv[]) {
    if (argc < 3) usage();

    int n; double proporcion;
    parsea_parametros(argv[1], argv[2], n, proporcion);

    srand(time(NULL));

    vector<int> diccionario;
    crear_arxiu1(n, diccionario);
    crear_arxiu2(n, proporcion, diccionario);
}
