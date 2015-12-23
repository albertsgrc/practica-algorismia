#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void usage() {
    cout << "Usage: generador-arxius n proporcio" << endl;
}

void error(const string& msg) {
    cerr << msg << endl;
    exit(1);
}

void crear_arxiu1(int n, vector<int>& diccionari) {
    diccionari = vector<int>(n);

    ofstream arxiu1;
    arxiu1.open("arxiu1");

    for (int& x : diccionari) {
        x = rand();
        arxiu1 << x << endl;
    }

    arxiu1.close();
}

void crear_arxiu2(int n, double proporcio, const vector<int>& diccionari) {
    int repetits = n*proporcio/100;
    vector<int> text(2*n);

    for (int&x : text) x = rand();

    for (int i = 0; i < repetits; ++i) {
        int index_arxiu1 = rand()%n;
        int index_arxiu2 = rand()%(2*n);
        text[index_arxiu2] =  diccionari[index_arxiu1];
    }

    ofstream arxiu2;
    arxiu2.open("arxiu2");
    for (int x : text) arxiu2 << x << endl;

    arxiu2.close();
}

int main(int argc, char* argv[]) {
    if (argc < 3) usage();

    int n;
    try { n = stoi(argv[1]); }
    catch (const exception& ia) {
        error("El primer argument ha de ser un natural");
    }

    double proporcio;
    try { proporcio = stod(argv[2]); }
    catch (const exception& ia) {
        error("El segon argument ha de ser un real entre 0 i 1");
    }

    if (n < 0)
        error("La n ha de ser positiva");

    if (proporcio > 1.0 or proporcio < 0)
        error("La proporcio ha d'estar entre 0 i 1");

    srand(time(NULL));

    vector<int> diccionari;
    crear_arxiu1(n, diccionari);
    crear_arxiu2(n, proporcio, diccionari);
}
