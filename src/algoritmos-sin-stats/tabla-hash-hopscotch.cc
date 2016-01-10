#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <forward_list>
#include <cstdint>
#include <sys/time.h>
#include <sys/resource.h>
#include "io.hpp"
#include "cronometro.hpp"
using namespace std;

class hash_hopscotch {
    
    private:
        struct Bucket {
            bool ocupado;
            unsigned long mapa_saltos;
            int clave;

            Bucket() : ocupado(false), mapa_saltos(0L), clave(int()) {}
        };

        vector<Bucket> T;

        int mascara_modulo;

        static const int H = 8*sizeof(unsigned long);


        // builtin_ctz se traduce a una instruccion de lenguaje maquina
        // (si la maquina la soporta, lo cual es bastante comun)
        // que cuenta los bits a 0 a partir del bit a 1 de menos peso
        // Ej.: 101101000 daria 3 por los 3 ultimos ceros
        // Esto es útil para saltar bits que esten a 0 del mapa de hopscotch
        // que hay en cada posicion de la tabla
        inline int cerosFinales(unsigned long x) {
            return __builtin_ctzl(x);
        }

        // Devuelve la siguiente potencia de 2 >= x
        inline int siguiente_potencia_2(int x) {
            int p = 1;
            while (p < x) p <<= 1;
            return p;
        }

        // En este caso la funcion de hash es el entero mismo, ya que esto produce
        // un hashing perfecto
        inline int posicion(int k) {
            return k & mascara_modulo;
        }

        inline bool contiene_imm(int k, int pos, unsigned long ms) {
            // Miramos si está en el vecindario de tamaño H de T[pos]                
            while (ms > 0L) {
                // saltar al siguiente indice que indique el mapa de saltos
                // nunca nos saldremos del vecindario ya que ms > 0, 
                // entonces siempre hay algun uno en el mapa, y cerosFinales < H
                int indice_salto = (pos + cerosFinales(ms)) & mascara_modulo;

                if (T[indice_salto].clave == k) return true;
                
                // Eliminamos el bit a 1 de menos peso
                ms &= ms - 1L;
            }

            return false;
        }

        inline void rehash() {
            int nuevo_tamano = 2*T.size();
            vector<Bucket> aux(nuevo_tamano);
            mascara_modulo = nuevo_tamano - 1;

            for (int i = 0; i < T.size(); ++i) {
                if (T[i].ocupado) {
                    int pos = posicion(T[i].clave);
                    insertar_imm(aux, T[i].clave, pos, ~aux[pos].mapa_saltos);
                }
            }

            T.swap(aux);
        }

        inline void insertar_imm(vector<Bucket>& t, int k, int pos, unsigned long msi) {
            do {
                // Mientras exista un bit a 1
                while (msi > 0L) {
                    int salto = cerosFinales(msi);
                    int indice_salto = (pos + salto) & mascara_modulo;
                    if (not t[indice_salto].ocupado) {
                        t[indice_salto].clave = k;
                        t[indice_salto].ocupado = true;
                        t[pos].mapa_saltos |= 1L << ((unsigned long) salto);
                        return;
                    }
                    else msi &= msi - 1L;
                }
                
                // Si no hay ningun bit a 1 no ocupado, todo el vecindario
                // esta ocupado, rehasheamos
                rehash();

                pos = posicion(k);
                msi = ~t[pos].mapa_saltos;
            } while(true);
        }

        inline void insertar(int k) {
            int pos = posicion(k);
            unsigned long msb = T[pos].mapa_saltos;

            // Comprobamos que no esté ya
            // Será rápido porque solo mirará bits a 1
            if (contiene_imm(k, pos, msb)) return;

            // El mapa de insercion es como el de busqueda negado,
            // en este caso no buscamos un bit a 1 sino uno a 0
            insertar_imm(T, k, pos, ~msb);
        }


    public:
        hash_hopscotch() {}
        hash_hopscotch(const VI& v) {
            int tamano     = siguiente_potencia_2(1.75*v.size());
            T              = vector<Bucket>(tamano);
            mascara_modulo = tamano - 1;

            for (int k : v) insertar(k);
        }

        inline bool contiene(int k) {
            int pos = posicion(k);
            unsigned long ms = T[pos].mapa_saltos;

            return contiene_imm(k, pos, ms);
        }
};


hash_hopscotch diccionario_hash_hopscotch;
Cronometro<> insercion, busqueda;

void algoritmo(const VI& diccionario, const VI& texto, VB& resultado) {
    insercion.iniciar();

    diccionario_hash_hopscotch = hash_hopscotch(diccionario);
    
    insercion.finalizar();

    busqueda.iniciar();
    
    for (int i = 0; i < texto.size(); ++i)
        resultado[i] = diccionario_hash_hopscotch.contiene(texto[i]);
    
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