/*------------------------------------------------------------------------------
Titulación: Ingeniería Informática
Asignatura: Programación II
Alumno: David Criado Díez
NIP:  931129
Archivo: mainGSenku.hpp
------------------------------------------------------------------------------*/
#include "GSenku.hpp"
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char* argv[]) {
	if (argc < 4 || argc > 5) {
		cerr << "Uso: " << argv[0] << " ficheroTablero ficheroMovimientos retardo [ficheroSalida]\n";
		return 1;
	}

	string ficheroTablero = argv[1];
	string ficheroMovimientos = argv[2];
	int retardo = stoi(argv[3]);
	string ficheroSalida = (argc == 5) ? argv[4] : "resultado.txt";

	tpTablero tablero;
	tpMovimientosValidos movimientos;
	tpListaMovimientos lista;

	if (!inicializarTablero(ficheroTablero, tablero)) return 1;
	if (!inicializarMovimientosValidos(ficheroMovimientos, movimientos)) return 1;

	int exito = buscaSolucion(tablero, movimientos, lista, retardo);
	if (exito == 1) {
		cout << VERDE << "¡Solucion encontrada!\n";
	} else {
		cout << ROJO << "No se encontro solucion.\n";
	}

	escribeListaMovimientos(ficheroSalida, lista);
	return 0;
}



