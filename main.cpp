#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>
#include <math.h>
#include <list>
#include <queue>
#include <iterator>
#include <fstream>

#define C 100
#define F 100
#define MAXNODOS 15

using namespace std;

//VARIABLE GLOBAL
//Matriz de adyacencia del grafo
double aristas[MAXNODOS][MAXNODOS];

//CLASE MATRIZ DE BITSETS
//Se usa para cargar los datos del archivo de entrada
class Matriz { 
	private:
		bitset<C> M[F];
	public:
    	void print();
    	void setBit(int f, int c){M[f].set(c);};
    	void clearBit(int f, int c){M[f].reset(c);};
    	bool getBit(int f, int c){return this->M[f][c];};
    	void setRow(int f, bitset<C> r){M[f] = r;};
};

void Matriz::print(){
     for(int i=0;i<F;i++){
        cout<<endl;
        for(int j=C-1;j>=0;j--)                 
            cout<<this->M[i][j]<<"  ";
     }
     cout<<endl;
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
	int nVertice;//numero del vertice vecino
	double dist; 
} VECINO;

typedef struct{
	list<int> camino;
	double costo; 
} CAMINO;


//CLASE VERTICE
//Modela un vertice del grafo
class Vertice { 
	private:
		int n;
		POS p;
		list<VECINO> vecinos; //Para almacenar nodos vecinos (solo para mostrar por consola, podria eliminarse)
	public:
		//Constructor
		Vertice(int n, int x, int y){
			this -> n = n;
			this -> p.x = x;
			this -> p.y = y;
		}
		//Getters
		list<VECINO> getVecinos(){return vecinos;};
		int getNumero(){return n;};
		POS getCoordenadas() {return p;};
		//Metodos
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


//DECLARACION DE FUNCIONES

//Flujo principal 
DATA cargarMatriz();//Crea matrices de entrada a partir de archivo de texto
Matriz filtrarMatriz(Matriz m, int tamanoFiltro); //Filtra manchas significativas de malezas
Matriz generarVertices(list<Vertice*>* vertices, Matriz m); //Asigna coordenadas y crea los vertices por cada mancha significativa
queue<Vertice*> enlazarVerticesAlcanzables(list<Vertice*>* vertices, Matriz b); //Asocia a cada vertice con todos sus vecinos (vertices alcanzables)
CAMINO encontrarCircuito(queue<Vertice*> vertices); //Busca posibles caminos Hamiltonianos a partir de algoritmo BFS

//Auxiliares
CAMINO busquedaEnAmplitud(Vertice* v, int cantVertices);//Hace BFS a partir del vertice indicado
double calcularDistancia(Vertice* v1, Vertice* v2); //Calcula distancia euclidiana entre dos vertices
void printAristas();//Imprime matriz de adyacencia en consola
void printVertices(list<Vertice*>* l);//Imprime la lista de vertices con sus respectivos vecinos
void printLista(list<int> l);//Imprime una lista de enteros (camino)
bool contiene(list<int> l, int n); //Devuelve true si el entero indicado est[a incluido en la lista

//MAIN
int main(int argc, char** argv) {
	
	//1) Carga de datos desde el archivo
	DATA d = cargarMatriz();
	//cout<< "Matriz de malezas: "<<endl;
	//d.m.print();
	//cout<<endl<< "Matriz de barreras: "<<endl;
	//d.b.print();
	
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
	cout<<endl<<endl;
	cout<<endl<<"Vertices del grafo sin enlazar: "<<endl;
	printVertices(vertices);
	
	//4)Enlazado de vertices: para cada uno se agregan los vertices alcanzables y su distancia
	queue<Vertice*> verticesEnlazados;
	cout<<endl<<endl<<"Vertices del grafo enlazados: "<<endl;
	verticesEnlazados = enlazarVerticesAlcanzables(vertices, d.b);
	cout<<endl<<"Cantidad total de vertices: "<<verticesEnlazados.size()<<endl;
	cout<<endl<<"Aristas: "<<endl;
	printAristas();
	
	//5) Explorar caminos hamiltonianos posibles y elegir el de menor costo 
	CAMINO respuesta;
	respuesta = encontrarCircuito(verticesEnlazados);
	
	cout<< endl<<" -------------------------FIN----------------------- "<<endl;
	cout<<"Camino recomendado: ";
	printLista(respuesta.camino);
	cout<<"Costo: "<< respuesta.costo<<endl;
	
	return 0;	
}

//FUNCIONES DEL FLUJO PRINCIPAL--------------------------------------------------

//Genera las matrices de entrada correspondientes a las malezas y a las barreras
DATA cargarMatriz(){
	DATA d;

	ifstream f("img.txt");
	//ifstream f("imgSinBarreras.txt");
	//ifstream f("grande.txt");
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
//Filtra manchas significativas de malezas
Matriz filtrarMatriz(Matriz m, int tamanoFiltro){
	Matriz s; //Matriz de salida
	bool submatriz[tamanoFiltro][tamanoFiltro];
	
	int cant = 0;
	
	//Se aplica filtro cuadrado del tamano definido por parametro
	for(int i=0;i<F;i++){
        for(int j=C-1;j>=0;j--) {
        	
        	for(int f = 0; f<tamanoFiltro; f++){ 
        		for(int c = 0; c<tamanoFiltro; c++){
        			submatriz[f][c] = m.getBit((i+f), (j-c)) * 1;
        			if(submatriz[f][c] == 1) cant++;
				}
			}
			if(cant == (tamanoFiltro * tamanoFiltro)) {
				s.setBit(i+1, j-1);	
			}
			cant = 0;
		}
    }
    return s;
}

//Asigna coordenadas y crea los vertices por cada mancha significativa
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

//Asocia a cada vertice con todos sus vecinos (vertices alcanzables)
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

//Busca posibles caminos Hamiltonianos a partir de algoritmo BFS
CAMINO encontrarCircuito(queue<Vertice*> vertices){
	CAMINO resultado;
	CAMINO aux;
	int cantVertices = vertices.size();
	double costo =0;
	//resultado.camino = list<int>;
	resultado.costo = costo;
	
	while(!vertices.empty()){
		//cout<<"Cantidad total de vertices: "<<vertices.size()<<endl;
		Vertice* v = vertices.front();
		vertices.pop();
		aux = busquedaEnAmplitud(v, cantVertices);
		
		if(aux.costo != 0){
			cout<<"Posible camino encontrado: ";
			printLista(aux.camino);
			cout<<"Costo: "<<  aux.costo<<endl;
	
			if(resultado.costo == 0 ||aux.costo < resultado.costo)	resultado = aux;
				
		}
	}
	
	return resultado;
	
}

//FUNCIONES AUXILIARES--------------------------------------------------
CAMINO busquedaEnAmplitud(Vertice* v, int cantVertices){
	int nV = v -> getNumero();
	cout<<endl<<"Buscando camino a partir de vertice "<< nV<<endl;
	int ultimo;
	bool salir = false;
	
	list<int> camino;
	queue<int> aux;
	double costo = 0;
	
	CAMINO resultado;
	resultado.costo = costo;
	resultado.camino = camino;
	
	camino.push_back(nV);
	
	for(int j = 0; j<MAXNODOS; j++){
		if(aristas[nV-1][j] != 0){ //Si hay camino
			aux.push(j+1);
		}
	}
	
	while((camino.size() <= cantVertices) || !salir){ //Mientras no se cumpla el ciclo
		ultimo = camino.back();
		nV = aux.front();
		aux.pop();
		//cout<<"ultimo en camino "<<ultimo<<" nv "<< nV<<endl;
		while((aristas[nV-1][ultimo -1] == 0 || contiene(camino, nV)) ){ //Mientras no haya camino, desencola
			if(aux.empty()) {
				salir = true;
				break;	
			}
			nV = aux.front();
			aux.pop();
			//cout<<"Se saca de la cola "<< nV<<endl;
		}
		
		if(!salir){
			camino.push_back(nV);
			costo = costo + aristas[nV-1][ultimo -1];
			for(int j = 0; j<MAXNODOS; j++){
				if(aristas[nV-1][j] != 0){ //Si hay camino
					aux.push(j+1); //encola vecinos
					//cout<<"nv "<< nV<<" agrega vecino "<< j+1<<endl;
				}
			}
		}
		else break;
	}
		int primerV = camino.front();
		int ultimoV = camino.back();
		
		printLista(camino);
		//cout<<"primer "<<primerV<<endl;
		//cout<<"iltimo "<<ultimoV<<endl;
		
	if(camino.size()== cantVertices && aristas[primerV-1][ultimoV-1] != 0){ //Se cierra el ciclo
		
		camino.push_back(primerV);
		costo = costo + aristas[primerV-1][ultimoV-1];
		resultado.camino = camino;
		resultado.costo = costo;
	}
	else cout<<"Camino no encontrado"<<endl;		
	return resultado;
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

double calcularDistancia(Vertice* v1, Vertice* v2){
	double resultado;
	int x1 = v1 -> getCoordenadas().x;
	int x2 = v2 -> getCoordenadas().x;
	int y1 = v1 -> getCoordenadas().y;
	int y2 = v2 -> getCoordenadas().y;
	
	resultado = sqrt((x2-x1) * (x2-x1) + (y2-y1) * (y2-y1));
	
	return resultado;
}

bool contiene(list<int> l, int n){
	list <int> :: iterator it; //Iterador para recorrer la lista
	for(it = l.begin(); it != l.end(); ++it){
		if((*it) == n) return true;
	}
	return false;
}

void printLista(list<int> l){
	list <int> :: iterator it; //Iterador para recorrer la lista
	for(it = l.begin(); it != l.end(); ++it){
		cout<<(*it)<<" ";
	}
	cout<<endl;
}

