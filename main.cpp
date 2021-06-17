#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>
#include <math.h>
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

typedef struct{
	int nVertice;//Coordenada x
	double dist; //Coordenada y
} VECINO;


class Vertice { 
	private:
		int n;
		POS p;
		list<VECINO> vecinos; //Arreglo que almacenara nodos vecinos y las distancias ([n][dist])
	public:
		Vertice(int n, int x, int y){
			this -> n = n;
			this -> p.x = x;
			this -> p.y = y;
		}
		int getNumero(){return n;};
		//list<VECINO>* getVecinos(){return vecinos;};
		POS getCoordenadas() {return p;};
		void printVertice();
		void printVecinos();
		void addVecino(VECINO v);
		
};

void Vertice :: printVertice(){

   cout <<"Vertice "<< this -> n<<endl
   		<<"\tCoordenadas: ("<<this -> p.x << ", "<< this -> p.y <<")"<<endl
   		<<"\tVertices alcanzables:"<<endl;
		printVecinos();
}

void Vertice :: addVecino(VECINO v){
	this -> vecinos.push_back(v);
}

void Vertice :: printVecinos(){
	list <VECINO> :: iterator it;
	for(it = vecinos.begin(); it != vecinos.end(); ++it){
		cout<<"\t\t<Vertice "<< it -> nVertice << ", distancia "<< it -> dist<<">"<< endl;
	}
}

/*typedef struct{
	int n; //Numero de vertice
	POS p; //Coordenadas
	list<int[2]>* vecinos; //Arreglo que almacenara nodos vecinos y las distancias ([n][dist])
} VERTICE;*/

//DECLARACION DE FUNCIONES 
DATA cargarMatriz();
Matriz filtrarMatriz(Matriz m, int tamanoFiltro);
Matriz generarVertices(list<Vertice*>* vertices, Matriz m);
void printVertices(list<Vertice*>* l);
void enlazarVerticesAlcanzables(list<Vertice*>* vertices, Matriz b);
void corroborarCamino();
double calcularDistancia(Vertice* v1, Vertice* v2);
void definirCircuito();



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
	list<Vertice*> v;
	list<Vertice*>* vertices = &v;
	d.m = generarVertices(vertices, d.m);
	cout<< endl<<"Ubicacion de las manchas significativas: "<<endl;
	d.m.print();
	cout<<endl;
	cout<<endl<<"Vertices del grafo sin enlazar: "<<endl;
	printVertices(vertices);
	
	//4)Enlazado de vertices: para cada uno se agregan los vertices alcanzables y su distancia
	cout<<endl<<"Vertices del grafo enlazados: "<<endl;
	enlazarVerticesAlcanzables(vertices, d.b);
	
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
				d.b.setBit(i,line.size()-c);//Se agrega a matriz de barreras
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

Matriz generarVertices(list<Vertice*>* vertices, Matriz m){
	Matriz s = m;
	int nVertice = 0;
	bool flag = false;
	
    for(int i=0;i<F;i++){
        for(int j=C-1;j>=0;j--) {
			if(s.getBit(i, j) == 1 && flag==false){ 
				flag = true;
				nVertice++;
				Vertice* v = new Vertice (nVertice, i+1, C-j); //Se crea un nuevo vertice con las coordenadas segun su ubicacion en la matriz
				vertices -> push_back(v); //Se lo agrega a la lista de vertices
			}
			else if(s.getBit(i, j) == 1 && flag==true){
				s.clearBit(i,j);//Limpia el de la derecha
				s.clearBit(i+1,j);//Limpia el de la derecha abajo
				s.clearBit(i+1,j+1);//Limpia el de abajo
				flag = false;
			}
			else{//if(s.getBit(i, j) == 0)
				flag = false;
			}
		}
	}
	
	return s;
}

void printVertices(list <Vertice*>* l){
	list <Vertice*> :: iterator it;
    for(it = l -> begin(); it != l -> end(); ++it){
        (*it) -> printVertice();
    	cout << '\n';
    }
}

void enlazarVerticesAlcanzables(list <Vertice*>* vertices, Matriz b){
	POS coord;
	POS coordVecino;
	
	POS inicio;
	POS final;
	
	VECINO nuevoVecino;
	
	bool nodoEnlazable = true;
	double distancia;
	
	Vertice* v1 = vertices -> front();
	vertices -> pop_front();
	//cout<<"Vertice "<< v1 -> getNumero()<<endl;
	
	Vertice* v2;
	
	
	list <Vertice*> :: iterator it;
	while(!vertices -> empty()){
	
	    for(it = vertices -> begin(); it != vertices -> end(); ++it){
				v2 = (*it);
				coord = v1 -> getCoordenadas();
				coordVecino = v2 -> getCoordenadas();
				inicio = coord;
				final = coordVecino;
				
				if(coord.x > coordVecino.x){
					inicio.x = coordVecino.x;
					final.x = coord.x;	
				} 
				if(coord.y > coordVecino.y){
					inicio.y = coordVecino.y;
					final.y = coord.y;	
				} 
				
				
				for(int i = inicio.x -1; i<= final.x-1; i++){
					//cout<<endl;
					//if(v1 -> getNumero() == 5 && v2 -> getNumero() ==6) cout<<"i "<<i <<endl;
					for(int j = final.y-1; j>=inicio.y-1; j--){
					//	if(v1 -> getNumero() == 5 && v2 -> getNumero() ==6) cout<<"i "<<i <<" j "<<j<<endl;
						if(b.getBit(i, C-j) == 1) {
							nodoEnlazable = false;
							break;
						}
						
					}
				}
				
				if(nodoEnlazable){
					//cout<< "Vertice " << v1 -> getNumero() << " es enlazable con vertice "<< v2 -> getNumero()<<endl;
					
					distancia = calcularDistancia(v1, v2);
					//cout<<"Distancia: "<< distancia<<endl;
					
					VECINO nuevoVecino;
					nuevoVecino.nVertice = v2 -> getNumero();
					nuevoVecino.dist = distancia;
					
					//cout<<"Fffff "<<nuevoVecino.nVertice<<"     "<< nuevoVecino.dist<<endl;
					//list<VECINO>* f = 
					v1 -> addVecino(nuevoVecino);
					
					
					// f -> push_front(nuevoVecino);
					//cout<<"ddddd"<<endl;
					
					nuevoVecino.nVertice = v1 -> getNumero();
					//v2 -> getVecinos() -> push_front(nuevoVecino);
					v2 -> addVecino(nuevoVecino);
				}
				
				nodoEnlazable = true;
	    }
	    v1 -> printVertice();
	    v1 = vertices -> front();
		vertices -> pop_front();
		//cout<<"Vertice "<< v1 -> getNumero()<<endl;
	}
	v1 -> printVertice();
}

double calcularDistancia(Vertice* v1, Vertice* v2){
	double resultado;
	int x1 = v1 -> getCoordenadas().x;
	int x2 = v2 -> getCoordenadas().x;
	int y1 = v1 -> getCoordenadas().y;
	int y2 = v2 -> getCoordenadas().y;
	
	resultado = sqrt((x2-x1) * (x2-x1) + (y2-y1) * (y2-y1));
	
	return resultado;
}



