/*------------------------------------------------------------------------------
Titulación: Ingeniería Informática
Asignatura: Programación II
Alumno: David Criado Díez
NIP:  931129
Archivo: GSenku.cpp
------------------------------------------------------------------------------*/

#include "GSenku.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
using namespace std;

bool inicializarTablero(const string nombreFichero, tpTablero &tablero){
	ifstream archivo(nombreFichero);
	if (!archivo) {
    		cerr << "Error: No se pudo abrir el fichero " << nombreFichero << endl;
    		return false;
	}

	archivo >> tablero.nfils >> tablero.ncols;
	if (tablero.nfils > MAXDIM || tablero.ncols > MAXDIM) {
    		cerr << "Error: Dimensiones fuera de los límites permitidos." << endl;
    		archivo.close();
    		return false;
	}

	for (int i = 0; i < tablero.nfils; i++) {
    		for (int j = 0; j < tablero.ncols; j++) {
        		char celda;
        		archivo >> celda;
        		switch (celda) {
            		case '-': tablero.matriz[i][j] = NO_USADA; break;
            		case 'o': tablero.matriz[i][j] = OCUPADA; break;
            		case 'x': tablero.matriz[i][j] = VACIA; break;
            		default:
                		cerr << "Error: Caracter inesperado en el fichero." << endl;
                		archivo.close();
                		return false;
        		}
    		}
	}

	archivo.close();
	return true;
}

bool inicializarMovimientosValidos(const string nombreFichero, tpMovimientosValidos &movimientos) {
	ifstream archivo(nombreFichero);
	if (!archivo) {
    		cerr << "Error: No se pudo abrir el fichero " << nombreFichero << endl;
    		return false;
	}

	char matriz[3][3];
	for (int i = 0; i < 3; i++) {
    		for (int j = 0; j < 3; j++) {
        		archivo >> matriz[i][j];
        		if (i == 1 && j == 1) continue;
        		if (matriz[i][j] != '+' && matriz[i][j] != '-') {
            		cerr << "Error: Caracter inesperado en el fichero de movimientos." << endl;
            		archivo.close();
            		return false;
        		}
    		}
	}
	archivo.close();

	movimientos.validos[superiorIzquierda] = (matriz[0][0] == '+');
	movimientos.validos[superior] = (matriz[0][1] == '+');
	movimientos.validos[superiorDerecha] = (matriz[0][2] == '+');
	movimientos.validos[izquierda] = (matriz[1][0] == '+');
	movimientos.validos[derecha] = (matriz[1][2] == '+');
	movimientos.validos[inferiorIzquierda] = (matriz[2][0] == '+');
	movimientos.validos[inferior] = (matriz[2][1] == '+');
	movimientos.validos[inferiorDerecha] = (matriz[2][2] == '+');

	return true;
}

void mostrarTablero(const tpTablero &tablero) {
	cout << "\n=== TABLERO ===\n";
	for (int i = 0; i < tablero.nfils; i++) {
    		for (int j = 0; j < tablero.ncols; j++) {
        		switch (tablero.matriz[i][j]) {
            		case NO_USADA: cout << " - "; break;
            		case VACIA:    cout << " x "; break;
            		case OCUPADA:  cout << " o "; break;
            		default:       cout << " ? ";
        		}
    		}
    		cout << endl;
	}
	cout << "================\n";
}

bool tableroResuelto(const tpTablero &tablero) {
	int contadorFichas = 0;
	for (int i = 0; i < tablero.nfils; i++) {
    		for (int j = 0; j < tablero.ncols; j++) {
        		if (tablero.matriz[i][j] == OCUPADA) {
            		contadorFichas++;
        		}
    		}
	}
	return (contadorFichas == 1);
}

bool esMovimientoValido(const tpTablero &tablero, const tpMovimientosValidos &movValidos, int dir, int fila, int col) {
	if (!movValidos.validos[dir]) return false;
	static const int dx[] = {-1,-1,-1, 0,0,1,1,1};
	static const int dy[] = {-1, 0, 1,-1,1,-1,0,1};
	int f1 = fila + dy[dir], c1 = col + dx[dir];
	int f2 = fila + 2*dy[dir], c2 = col + 2*dx[dir];
	return (f2 >= 0 && f2 < tablero.nfils && c2 >= 0 && c2 < tablero.ncols &&
		tablero.matriz[f1][c1] == OCUPADA && tablero.matriz[f2][c2] == VACIA);
}

bool ejecutarMovimiento(tpTablero &tablero, int dir, int fila, int col, tpMovimientoPieza &mov) {
	static const int dx[] = {-1,-1,-1, 0,0,1,1,1};
	static const int dy[] = {-1, 0, 1,-1,1,-1,0,1};
	int f1 = fila + dy[dir], c1 = col + dx[dir];
	int f2 = fila + 2*dy[dir], c2 = col + 2*dx[dir];
	tablero.matriz[fila][col] = VACIA;
	tablero.matriz[f1][c1] = VACIA;
	tablero.matriz[f2][c2] = OCUPADA;
	mov.origen = {fila, col};
	mov.destino = {f2, c2};
	return true;
}

void deshacerMovimiento(tpTablero &tablero, const tpMovimientoPieza &mov) {
	int dx = (mov.destino.y - mov.origen.y) / 2;
	int dy = (mov.destino.x - mov.origen.x) / 2;
	int interF = mov.origen.x + dy;
	int interC = mov.origen.y + dx;
	tablero.matriz[mov.origen.x][mov.origen.y] = OCUPADA;
	tablero.matriz[interF][interC] = OCUPADA;
	tablero.matriz[mov.destino.x][mov.destino.y] = VACIA;
}

bool intentarFicha(tpTablero &tablero, const tpMovimientosValidos &movValidos, tpListaMovimientos &solucionParcial, int fila, int col, const int retardo) {
	if (tablero.matriz[fila][col] != OCUPADA) return false;
	for (int dir = 0; dir < 8; dir++) {
		if (esMovimientoValido(tablero, movValidos, dir, fila, col)) {
			tpMovimientoPieza mov;
			ejecutarMovimiento(tablero, dir, fila, col, mov);
			solucionParcial.movs[solucionParcial.numMovs++] = mov;
			if (buscaSolucion(tablero, movValidos, solucionParcial, retardo) == 1)
				return true;
			solucionParcial.numMovs--;
			deshacerMovimiento(tablero, mov);
		}
	}
	return false;
}

int buscaSolucionDesde(tpTablero &tablero, const tpMovimientosValidos &movValidos,
                       tpListaMovimientos &solucionParcial, const int retardo,
                       int fila, int col, int dir) {
    if (tableroResuelto(tablero)) {
        return 1;
    }

    if (fila >= tablero.nfils) {
        return -1; // No hay más celdas
    }

    if (col >= tablero.ncols) {
        return buscaSolucionDesde(tablero, movValidos, solucionParcial, retardo, fila + 1, 0, 0);
    }

    if (dir >= 8) {
        return buscaSolucionDesde(tablero, movValidos, solucionParcial, retardo, fila, col + 1, 0);
    }

    if (tablero.matriz[fila][col] == OCUPADA && esMovimientoValido(tablero, movValidos, dir, fila, col)) {
        tpMovimientoPieza mov;
        ejecutarMovimiento(tablero, dir, fila, col, mov);
        solucionParcial.movs[solucionParcial.numMovs++] = mov;

        if (retardo > 0) {
            mostrarTablero(tablero);
            this_thread::sleep_for(chrono::milliseconds(retardo));
        }

        if (buscaSolucion(tablero, movValidos, solucionParcial, retardo) == 1) {
            return 1;
        }

        solucionParcial.numMovs--;
        deshacerMovimiento(tablero, mov);
    }

    return buscaSolucionDesde(tablero, movValidos, solucionParcial, retardo, fila, col, dir + 1);
}

int buscaSolucion(tpTablero &tablero, const tpMovimientosValidos &movValidos,
                  tpListaMovimientos &solucionParcial, const int retardo) {
    return buscaSolucionDesde(tablero, movValidos, solucionParcial, retardo, 0, 0, 0);
}




void escribeListaMovimientos(string nombreFichero, const tpListaMovimientos &solucion) {
	ofstream archivo(nombreFichero);
	if (!archivo) {
		cerr << "Error: No se pudo abrir el fichero para escribir: " << nombreFichero << endl;
		return;
	}

	if (solucion.numMovs == 0) {
		archivo << -1 << endl;
	} else {
		for (int i = 0; i < solucion.numMovs; i++) {
			archivo << solucion.movs[i].origen.x << "," << solucion.movs[i].origen.y << ":"
			        << solucion.movs[i].destino.x << "," << solucion.movs[i].destino.y << endl;
		}
	}
	archivo.close();
}



