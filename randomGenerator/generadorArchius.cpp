#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

vector<int> dicc;

void makeArxiu1(int n){
    dicc = vector<int>();
    ofstream diccionari;
    diccionari.open ("arxiu1");
    for (int i = 0; i<n;++i){
        int value= rand();
        dicc.push_back(value);
        diccionari << value<<endl;
    }
}

void makeArxiu2(int n,double percentatge){
    int porpot= n*percentatge/100;
    vector<int> text = vector<int> (2*n);
    for (int i =0;i<2*n;++i) text[i] = rand();
    for (int i = 0;i<porpot;++i){
        int j = rand() % n; // j = position of a random value from arxiu1
        int k = rand() % 2*n; //k= position in arxiu2 of the random value
        int value = dicc[j];
        text[k] = value;
    }
    ofstream file;
    file.open("arxiu2");
    for(int i=0;i<2*n;++i) file<< text[i]<<endl;
    file.close();
}

int main(){
    int n;
    cout<< "Enter size of the dictionary: ";
    cin>>n;
    cout<<endl;
    makeArxiu1(n);
    double percentatge;
    cout<< "Enter porpotion: ";
    cin>>percentatge;
    cout<<endl;
    makeArxiu2(n,percentatge);
}
