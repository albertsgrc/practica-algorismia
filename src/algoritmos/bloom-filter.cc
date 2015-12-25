#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include "io.hpp"
#include "cronometro.hpp"
using namespace std;

class Bloom{
private:
    int bits;
    vector<bool> bit_vector;
    int hash_functions;
    VI values_hash;
    int hash_it(int k, int key){
        return (hash<int>()(key) ^ values_hash[k]) % bits;
    }
public:
    Bloom(){}
    Bloom(int n,int k){
        bit_vector = vector<bool> (n,false);
        hash_functions = k;
        bits = n;
    }
    Bloom(const VI& diccionario){
        int entrada = diccionario.size();
        bits = 10*entrada; // m/n = 10 => k = 6 => fp = 0.0084
        double aux = log(2)*2;
        hash_functions = (int) aux;
        cout << "asdahsda "<<hash_functions<<endl;
        bit_vector = vector<bool> (bits,false);
        for (int i : diccionario) insertar(i);
        values_hash = VI (hash_functions);
        for (int i=0;i<hash_functions;++i) values_hash[i] = rand();
    }

    void insertar(int a){
        for (int i = 0;i<hash_functions;++i){
            int value = hash_it(i,a);
            bit_vector[value] = true;
        }
    }

    bool contiene(int a){
        for (int i =0;i<hash_functions;++i){
            int value = hash_it(i,a);
            if (not bit_vector[value]) return false;
        }
        return true;
    }
};

Bloom diccionario_hash;

void algoritmo(const VI& diccionario, const VI& texto, VB& resultado) {
// El algoritmo deberá calcular las estadísticas de ejecución sólo si se
// ha compilado el programa en versión estadísticas. Así se evita
// que el tiempo de ejecución se vea afectado por el cálculo extra.
// Esto se puede hacer por ejemplo como vemos en el main
// (#if _STATS_ ... #endif)

    diccionario_hash = Bloom(diccionario);

    for (int i = 0; i < texto.size(); ++i)
        resultado[i] = diccionario_hash.contiene(texto[i]);
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
