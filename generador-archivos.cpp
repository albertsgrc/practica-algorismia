#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void usage() {
    cout << "Usage: generador-archivos n proporcion" << endl;
}

void error(const string& msg) {
    cerr << msg << endl;
    exit(1);
}

void crear_arxiu1(int n, vector<int>& diccionario) {
    diccionario = vector<int>(n);

    ofstream arxiu1;
    arxiu1.open("arxiu1");

    for (int& x : diccionario) {
        x = rand();
        arxiu1 << x << endl;
    }

    arxiu1.close();
}

void crear_arxiu2(int n, double proporcion, const vector<int>& diccionario) {
    int repetidos = n*proporcion/100;
    vector<int> texto(2*n);

    for (int&x : texto) x = rand();

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

    int n;
    try { n = stoi(argv[1]); }
    catch (const exception& ia) {
        error("El primer argumento tiene que ser un natural");
    }

    double proporcion;
    try { proporcion = stod(argv[2]); }
    catch (const exception& ia) {
        error("El segundo argumento tiene que ser un real entre 0 y 1");
    }

    if (n < 0)
        error("La n tiene que ser positiva");

    if (proporcion > 1.0 or proporcion < 0)
        error("La proporcion tiene que estar entre 0 y 1");

    srand(time(NULL));

    vector<int> diccionario;
    crear_arxiu1(n, diccionario);
    crear_arxiu2(n, proporcion, diccionario);
}
