#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>
#include <typeinfo>
#include <fstream>

#define C 28
#define F 12
#define FILTRO 3

using namespace std;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

//CLASE MATRIZ DE BITSETS
class Matriz { 
	private:
		bitset<C> M[F];
	public:
		//Matriz(){bitset<C> M[F];};
		//Matriz(){bitset<c>M[f]};
    	void print();
    	void setBit(int f, int c);
    	void clearBit(int f, int c);
    	bool getBit(int f, int c);
    	void setRow(int f, bitset<C> r);
    	void subMatriz(int finicio, int cinicio, int ffin, int cfin);
};

void Matriz::print(){
     for(int i=0;i<F;i++){
        cout<<endl;
        for(int j=C-1;j>=0;j--)                 
            cout<<this->M[i][j]<<"  ";
     }
     cout<<endl;
}
void Matriz::setBit(int f, int c){
	M[f].set(c);
}
void Matriz::clearBit(int f, int c){
	M[f].reset(c);
}
bool Matriz::getBit(int f, int c){
	return this->M[f][c];
}
void Matriz::setRow(int f, bitset<C> r){
	M[f] = r;
}


typedef struct{
	Matriz m;//Malezas
	Matriz b; //Barreras
} data;

//DECLARACION DE FUNCIONES 
data cargarMatriz();
Matriz filtrarMatriz(Matriz m, int tamanoFiltro);
void crearVertices();
//void enlazarVerticesAlcanzables();
void corroborarCamino();
void calcularDistanciaEuclidiana();
void definirCircuito();

/*Matriz subMatriz(int finicio, int cinicio, int ffin, int cfin){
	//Tamano de la submatriz
	int c = cfin-cinicio;
	int f = ffin - finicio;
	
	bitset<c> S[f];
		
}*/

//CLASE PRINCIPAL
int main(int argc, char** argv) {
	
	//1) Carga de datos desde el archivo
	data entrada = cargarMatriz();
	cout<< "Matriz de malezas: "<<endl;
	entrada.m.print();
	cout<<endl<< "Matriz de barreras: "<<endl;
	entrada.b.print();
	
	//2)Filtrado de manchas significativas
	filtrarMatriz(entrada.m, 3);
	return 0;
}

//CargarMatriz() -> genera las matrices de entrada correspondientes a las malezas y a las barreras
data cargarMatriz(){
	data d;

	ifstream f("img.txt");
	string line;
	
	int i = 0;
	while (getline(f, line)) {
	
		for(int c = 0; c<line.size(); c++){
			if(line[c]== 'x'){
				d.b.setBit(i,c);//Se agrega a matriz de barreras
				line[c] = '0';//Se elimina de matriz de malezas	
			} 
			
		}
		bitset<C> fila(line);
		d.m.setRow(i, fila);
		//d.m.print();
   		i++;
	}
	
    f.close();
    return d;
}

Matriz filtrarMatriz(Matriz m, int tamanoFiltro){
	Matriz s; //Matriz de salida
	bool submatriz[tamanoFiltro][tamanoFiltro];
	
	int cant = 0;
	int cantVertices = 0;
	
	for(int i=0;i<F;i++){
        for(int j=C-1;j>=0;j--) {
        	
        	for(int f = 0; f<tamanoFiltro; f++){ 
        		//cout<<endl;
        		for(int c = 0; c<tamanoFiltro; c++){
        			submatriz[f][c] = m.getBit((i+f), (j-c)) * 1;
        			if(submatriz[f][c] == 1) cant++;
				}
			}
			if(cant == (tamanoFiltro * tamanoFiltro)) {
				s.setBit(i+1, j-1);	
			}
			cant = 0;
			//cout<<endl<<endl;
		}
			
    }
    
    s.print();
    bool flag = false;
    for(int i=0;i<F;i++){
        for(int j=C-1;j>=0;j--) {
			if(s.getBit(i, j) == 1 && flag==false){
				flag = true;
				cantVertices++;
			}
			else if(s.getBit(i, j) == 1 && flag==true){
				s.clearBit(i,j);//Limpia el de la derecha
				s.clearBit(i+1,j);//Limpia el de la derecha abajo
				s.clearBit(i+1,j+1);//Limpia el de abajo
				//s.clearBit(i-1,j);
				//s.clearBit(i,j);
				flag = false;
			}
			else{//if(s.getBit(i, j) == 0){
				flag = false;
			}
		}
	}
    
    s.print();
    cout<<"Cantidad de vertices: "<<cantVertices<<endl;
    
    return s;
	
}


