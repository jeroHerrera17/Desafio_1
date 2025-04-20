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
 *      int tamaño = tamaño del arreglo dinamico de la imagen inicial.
 *
 * Salidas:
 *      unsigned char* imagenXOR = arreglo dinamico resultante de la aplicacion de XOR.
 *
  */
unsigned char* aplicarXOR(unsigned char* imagen, unsigned char* imagenMascara, int tamaño){

    //se crea un arreglo dinamico en el que guardaremos los resultados de la aplicacion del operador XOR
    //*ADVERTENCIA*, no olvidar eliminar espacio de memoria de imagenXOR cuando se dee de utilizar
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

/*funcion rotarBits():
* esta funcion aplica el operador bitwise de rotacion.
* nos servira como apoyo para rotar cada uno de los elementos de la funcion, rotarBitsImagen.
* Entradas:
*       unsigned char elemento = este es el elemento que deseamos rotar.
*       unsigned int NumRotaciones = cantidad de bits a rotar.
*       char direccion = esta tendra uno de los siguientes valores, "i" para izquierda, "d" para derecha
* Salidas:

*/
unsigned char rotarBits(unsigned char elemento, unsigned int NumRotaciones, char direccion){

    unsigned char elementoRotado;
    if(direccion == 'i'){
        elementoRotado = ((elemento << NumRotaciones) | (elemento >> (8 - NumRotaciones))); 
    }
    else if(direccion == 'd'){
        elementoRotado = ((elemento >> NumRotaciones) | (elemento << (8 - NumRotaciones)));
    }
    else{
        elementoRotado = elemento;
    }

    return elementoRotado;
}

/* Funcion rotarBitsImagen():
 * esta funcion rota los bits de la imagen en una direccion especifica.
 * Entradas:
 *      unsigned char* imagen = arreglo dinamico con los bits de la imagen inicial.
 *      int tamaño = tamaño del arreglo dinamico inicial.
 *      int NumRotaciones = cantidad de bits a rotar
 *      char direccion = esta tendra uno de los siguientes valores, "i" para izquierda, "d" para derecha
 * Salidas:
 */

unsigned char* rotarBitsImagen(unsigned char* imagen, int tamaño, int NumRotaciones, char direccion){

    //definimos una variable donde guardaremos la imagen rotada;
    //*ADVERTENCIA*, no olvide eliminar el espacio de memoria de imagenRotada cuando se deje de utilizar;
    unsigned char* imagenRotada = new unsigned char[tamaño];

    //creamos un ciclo el cual nos ayudara a pasar y rotar cada uno de los elementos de la imagen;
    //a cada elemento se le asignara el resultado de la funcion rotarBits con los diferentes parametros (Numero de rotaciones y direccion)
    for(int i = 0; i < tamaño; i += 3){
        imagenRotada[i] = rotarBits(imagen[i], NumRotaciones, direccion);
        imagenRotada[i + 1] = rotarBits(imagen[i + 1], NumRotaciones, direccion);
        imagenRotada[i + 2] = rotarBits(imagen[i + 2], NumRotaciones, direccion);
    }

    return imagenRotada;
}
#endif // MODULO2_H
