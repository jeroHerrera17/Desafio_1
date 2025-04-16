#ifndef MODULO2_H
#define MODULO2_H

#include <iostream>

using namespace std;

/* Funcion aplicarXOR():
 * Esta funcion aplica el operador bitwise de XOR, entre los arreglos dinamicos con los
 * canales RGB de cada pixel de 2 imagenes BMP.
 * Entradas:
 *      unsigned char* imagen = arreglo dinamico de la imagen inicial.
 *      unsigned char* imagenMascara = arreglo dinamico de la imagen mascara con la que aplicaremos el XOR.
 *
 * Salidas:
 *      unsigned char* imagenXOR = arreglo dinamico resultante de la aplicacion de XOR.
 *
  */
unsigned char* aplicarXOR(unsigned char* imagen, unsigned char* imagenMascara, int tamaño){

    //se crea un arreglo dinamico en el que guardaremos los resultados de la aplicacion del operador XOR
    unsigned char* imagenXOR = new unsigned char[tamaño];

    //inicializamos un ciclo con el cual pasaremos por cada uno de los elementos de imagen e imagenMascara
    //este ciclo tambien nos ayudara a asignar los valores a cada uno de los elementos de imagenXOR
    for(int i = 0; i < tamaño; i += 3){
        imagenXOR[i] = imagen[i] ^ imagenMascara[i];
        imagenXOR[i + 1] = imagen[i + 1] ^ imagenMascara[i + 1];
        imagenXOR[i + 2] = imagen[i + 2] ^ imagenMascara[i + 2];
    }

    return imagenXOR;

}

/* Funcion rotarBits():
 * esta funcion rota los bits de la imagen en una direccion especifica, el maximo de rotacion es de 8.
 * Entradas:
 *
 * Salidas:
 */
#endif // MODULO2_H
