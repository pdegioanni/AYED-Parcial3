#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>
#include <math.h>
//#include <typeinfo>
#include <list>
#include <queue>
#include <iterator>
#include <fstream>

#define C 28
#define F 12
//#define FILTRO 3
#define MAXNODOS 10

using namespace std;


double aristas[MAXNODOS][MAXNODOS];

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
} DATA;

typedef struct{
	int x;//Coordenada x
	int y; //Coordenada y
} POS;


typedef struct{
	int nVertice;
	double dist; 
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
		list<VECINO> getVecinos(){return vecinos;};
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

//ESTRUCTURAS


typedef struct{
	list<int> camino;
	double costo; 
} CAMINO;


//DECLARACION DE FUNCIONES 
DATA cargarMatriz();
Matriz filtrarMatriz(Matriz m, int tamanoFiltro);
Matriz generarVertices(list<Vertice*>* vertices, Matriz m);
void printVertices(list<Vertice*>* l);
queue<Vertice*> enlazarVerticesAlcanzables(list<Vertice*>* vertices, Matriz b);
void corroborarCamino();
double calcularDistancia(Vertice* v1, Vertice* v2);
CAMINO encontrarCircuito(queue<Vertice*> vertices);
CAMINO busquedaEnAmplitud(Vertice* v, int cantVertices);
void printAristas();
void printLista(list<int> l);
bool contiene(list<int> l, int n);


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
	queue<Vertice*> verticesEnlazados;
	cout<<endl<<"Vertices del grafo enlazados: "<<endl;
	verticesEnlazados = enlazarVerticesAlcanzables(vertices, d.b);
	cout<<"Cantidad total de vertices: "<<verticesEnlazados.size()<<endl;
	cout<<"Aristas: "<<endl;
	printAristas();
	
	//5) Explorar caminos hamiltonianos posibles y elegir el de menor costo 
	CAMINO respuesta;
	respuesta = encontrarCircuito(verticesEnlazados);
	
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

void printAristas(){
	for(int i= 0; i<MAXNODOS; i++){
		cout<<endl;
		for(int j = 0; j<MAXNODOS; j++){
			cout<<aristas[i][j] << "\t";
		}
	}
	cout<<endl;
}


queue<Vertice*> enlazarVerticesAlcanzables(list <Vertice*>* vertices, Matriz b){
	queue<Vertice*> verticesEnlazados;
	bool verticeEnlazable = true; //Se setea en false cuando hay una barrera de por medio
	VECINO nuevoVecino; //Vecino a evaluar

	Vertice* v1 = vertices -> front(); //Primer vertice en la lista
	vertices -> pop_front();
	Vertice* v2; //Vecino a evaluar

	POS coord; //Coordenadas de v1
	POS coordVecino; //Coordenadas de v2
	
	POS inicio; //Coordenadas de inicio y final en la matriz de barreras
	POS final;
	
	double distancia; //Distancia euclideana entre v1 y v2
	
	list <Vertice*> :: iterator it; //Iterador para recorrer la lista
	
	while(!vertices -> empty()){
	    for(it = vertices -> begin(); it != vertices -> end(); ++it){
			v2 = (*it); //Se evalua siguiente vecino en la lista
			
			coord = v1 -> getCoordenadas();
			coordVecino = v2 -> getCoordenadas();
			
			//Se va a evaluar una submatriz de la matriz de barreras, 
			//Comprende todos los elementos entre ambos vertices
			//Si en algun lugar hay una barrera (1 en la matriz de barrera), los vertices no son alcanzables,
			//Se toman las coordenadas xy de menor valor como inicio y las de mayor valor como final de la submatriz
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
				for(int j = final.y-1; j>=inicio.y-1; j--){
					if(b.getBit(i, C-j) == 1) { //Encontro una barrera en el medio
						verticeEnlazable = false; //Se descarta el vertice
						break;
					}
				}
			}
			
			if(verticeEnlazable){ //No hay barrera, se enlazan los nodos
				VECINO nuevoVecino;
				
				//Calcula distancia euclediana entre ambos vertices
				distancia = calcularDistancia(v1, v2);
				nuevoVecino.dist = distancia;
				
				aristas[v1 -> getNumero() -1 ][v2 ->getNumero()-1] = distancia;
				aristas[v2 -> getNumero()-1][v1 ->getNumero()-1] = distancia;
				
				//Enlaza a v2 con v1
				nuevoVecino.nVertice = v2 -> getNumero();
				v1 -> addVecino(nuevoVecino);
				
				//Enlaza a v1 con v2
				nuevoVecino.nVertice = v1 -> getNumero();
				v2 -> addVecino(nuevoVecino);
				
//				verticesEnlazados.push(v1);
				//verticesEnlazados.push(v2);
			}
			
			verticeEnlazable = true; //Resetea booleano para evaluar proximo vertice
	    }
	    
	    v1 -> printVertice();//Imprime el vertice en consola
	    verticesEnlazados.push(v1);
	    
	    v1 = vertices -> front(); //Toma el siguiente vertice de la lista y lo elimina
		vertices -> pop_front(); 
	}
	
	v1 -> printVertice(); //Imprime el ultimo nodo
	verticesEnlazados.push(v1);
	return verticesEnlazados;
}

CAMINO encontrarCircuito(queue<Vertice*> vertices){
	CAMINO resultado;
	CAMINO aux;
	int cantVertices = vertices.size();
	double costo;
	
	while(!vertices.empty()){
		//cout<<"Cantidad total de vertices: "<<vertices.size()<<endl;
		Vertice* v = vertices.front();
		vertices.pop();
		aux = busquedaEnAmplitud(v, cantVertices);
		
		if(aux.costo != 0){
			cout<<"Camino encontrado: ";
			printLista(aux.camino);
			cout<<endl<<"Costo: "<<  aux.costo<<endl;
		
			if(aux.costo < resultado.costo)	resultado = aux;	
		}
	}
	
	return resultado;
	
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

CAMINO busquedaEnAmplitud(Vertice* v, int cantVertices){
	int nV = v -> getNumero();
	int ultimo;
	
	list<int> camino;
	queue<int> aux;
	double costo = 0;
	
	CAMINO resultado;
	resultado.costo = costo;
	resultado.camino = camino;
	
	camino.push_front(nV);
	
	
	//list <VECINO> :: iterator it; //Iterador para recorrer la lista
	/*for(it = vecinos.begin(); it != vecinos.end(); ++it){
		int n = (it) -> nVertice;
		aux.push(n);
		//cout<<"n "<< n<<endl;
	}*/
	
	//aux.push(v);
	for(int j = 0; j<MAXNODOS; j++){
		if(aristas[nV-1][j] != 0){ //Si hay camino
			aux.push(j+1);
		}
	}
	
	while(!(camino.size() == cantVertices)){ //Mientras no se cumpla el ciclo
		ultimo = camino.front();
		nV = aux.front();
		aux.pop();
		
		//cout<<"Camino ";
		//printLista(camino);
		
		while(aristas[nV-1][ultimo -1] == 0 || contiene(camino, nV)){ //Mientras no haya camino, desencola
			//cout<< aristas[nV-1][ultimo -1]<<endl;
			aux.pop();
			nV = aux.front();
		}
		
		camino.push_front(nV);
		costo = costo + aristas[nV-1][ultimo -1];
		
		for(int j = 0; j<MAXNODOS; j++){
			if(aristas[nV-1][j] != 0){ //Si hay camino
				aux.push(j+1); //encola vecinos
			}
		}
	}
	
	if(aristas[nV-1][v -> getNumero()-1] != 0){ //Se cierra el ciclo
		
		camino.push_front(v -> getNumero());
		costo = costo + aristas[nV-1][v -> getNumero()-1];
		cout<<"Costo en bea "<< costo<<endl;
		cout<<" Camino encontrado en bea: ";
		printLista(camino);
		
		resultado.camino = camino;
	resultado.costo = costo;
	}
		
	return resultado;
}

bool contiene(list<int> l, int n){
	//bool esta = false;
	list <int> :: iterator it; //Iterador para recorrer la lista
	
	for(it = l.begin(); it != l.end(); ++it){
		if((*it) == n) return true;
	}
	
	return false;
}

void printLista(list<int> l){
	//bool esta = false;
	list <int> :: iterator it; //Iterador para recorrer la lista
	
	for(it = l.begin(); it != l.end(); ++it){
		cout<<(*it)<<" ";
	}
	
	cout<<endl;
}

