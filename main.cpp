#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>
//#include <typeinfo>
#include <list>
#include <iterator>
#include <fstream>

#define C 28
#define F 12
#define FILTRO 3

using namespace std;




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

//ESTRUCTURAS
typedef struct{
	Matriz m;//Malezas
	Matriz b; //Barreras
} DATA;

typedef struct{
	int x;//Coordenada x
	int y; //Coordenada y
} POS;

class VERTICE { 
	private:
		int n;
		POS p;
		list<int[2]>* vecinos; //Arreglo que almacenara nodos vecinos y las distancias ([n][dist])
	public:
		VERTICE(int n, int x, int y){
			this -> n = n;
			this -> p.x = x;
			this -> p.y = y;
		}
		void printVertice();
		
};

void VERTICE :: printVertice(){

   cout <<"Vertice "<< this -> n
   		<<"\t Coordenadas: ("<<this -> p.x << ", "<< this -> p.y <<")"<<endl;
}

/*typedef struct{
	int n; //Numero de vertice
	POS p; //Coordenadas
	list<int[2]>* vecinos; //Arreglo que almacenara nodos vecinos y las distancias ([n][dist])
} VERTICE;*/

//DECLARACION DE FUNCIONES 
DATA cargarMatriz();
Matriz filtrarMatriz(Matriz m, int tamanoFiltro);
void generarVertices(list<VERTICE*>* vertices, Matriz m);
//VERTICE crearVertice(int n, int x, int y);
void printVertices(list<VERTICE*>* l);
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
	DATA d = cargarMatriz();
	cout<< "Matriz de malezas: "<<endl;
	d.m.print();
	cout<<endl<< "Matriz de barreras: "<<endl;
	d.b.print();
	
	//2)Filtrado de manchas significativas. Si la mancha es mayor al tamano del filtro quedan 1s consecutivos
	d.m = filtrarMatriz(d.m, 3);
	cout<< endl<<"Matriz de malezas filtrada: "<<endl;
	d.m.print();
	
	
	//3)Creacion de un vertice del grafo por cada mancha significativa. 
	//Si la mancha es mayor al filtro aplicado, se la reduce al punto correspondiente a su esquina superior izquierda
	//Se registran las coordenadas de cada vertice.
	list<VERTICE*> vv;
	list<VERTICE*>* vertices = &vv;
	generarVertices(vertices, d.m);
	printVertices(vertices);
	//cout<< endl<<"Coordenadas manchas significativas: "<<endl;
	//d.m.print();
	
	
	return 0;	
}

//CargarMatriz() -> genera las matrices de entrada correspondientes a las malezas y a las barreras
DATA cargarMatriz(){
	DATA d;

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
	
	//Se aplica filtro cuadrado del tamano definido por parametro
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
    
    return s;
	
}

void generarVertices(list<VERTICE*>* vertices, Matriz m){
	Matriz s = m;
	//list<VERTICE> vertices;
	//VERTICE v;
	int nVertice = 0;
	bool flag = false;
	
	//s.print();
	
    for(int i=0;i<F;i++){
        for(int j=C-1;j>=0;j--) {
			if(s.getBit(i, j) == 1 && flag==false){ 
				flag = true;
				nVertice++;
				//cout<<"v "<<nVertice<<endl;
				VERTICE* v = new VERTICE (nVertice, i+1, C-j);
				//v.printVertice();
				//VERTICE* vp = &v;
				vertices -> push_back(v);
				//printVertices(vertices);
				//cout<<"(" << i+1<<", "<< C - j <<")"<<endl; //Coordenadas del vertice
			}
			else if(s.getBit(i, j) == 1 && flag==true){
				s.clearBit(i,j);//Limpia el de la derecha
				s.clearBit(i+1,j);//Limpia el de la derecha abajo
				s.clearBit(i+1,j+1);//Limpia el de abajo
				flag = false;
			}
			else{//if(s.getBit(i, j) == 0){
				flag = false;
			}
		}
	}
	
	cout<< endl<<"Coordenadas manchas significativas: "<<endl;
	s.print();
	//printVertices(vertices);
    
    //return vertices;
}

void printVertices(list <VERTICE*>* l){
	list <VERTICE*> :: iterator it;
    for(it = l -> begin(); it != l -> end(); ++it){
   //for(it = l.begin(); it != l.end(); ++it){
   	//cout << '\t' << it -> VERTICE -> p.x;
        (*it) -> printVertice();
    	cout << '\n';
   }
}

/*VERTICE crearVertice(int n, int x, int y){
	VERTICE v;
	//VERTICE* ad = &v;
	//ad -> n = n;
	//ad -> p.x = x;
	//ad -> p.y= y;
	//ad -> vecinos;
	cout<<"dentro de "<< n<<endl;
	v.n = n;
	v.p.x = x;
	v.p.y= y;
	v.vecinos;
	return v;
}*/




