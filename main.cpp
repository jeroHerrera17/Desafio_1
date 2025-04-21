#include <fstream>
#include <iostream>
#include <QCoreApplication>
#include <QImage>


using namespace std;

//modulo 1:
unsigned char* loadPixels(QString input, int &width, int &height);
bool exportImage(unsigned char* pixelData, int width,int height, QString archivoSalida);
unsigned int* loadSeedMasking(const char* nombreArchivo, int &seed, int &n_pixels);
bool validarEnmascaramiento(const unsigned char* imagen, const unsigned char* mascara, const unsigned int* resultado, int seed, int n_pixels, int total_bytes);

//modulo 2:
unsigned char* aplicarXOR(unsigned char* imagen, unsigned char* imagenMascara, int tamaño);
unsigned char rotacion(unsigned char elemento, unsigned int NumRotaciones, char direccion);
unsigned char* rotarBits(unsigned char* imagen, int tamaño, int NumRotaciones, char direccion);
unsigned char* desplazarBits(unsigned char* imagen, int tamaño, int NumDesplazamientos, char direccion);

//modulo  :
bool detectarTransformacion(unsigned char* imagenActual, unsigned char* IM, unsigned char* mascara, unsigned int* resultado, int seed, int n_pixels, int total_bytes, char* transformacionUsada);
void convertirEnteroEnTexto(int numero, char* buffer);
bool detectarTransformacion(unsigned char* imagenActual, unsigned char* IM, unsigned char* mascara, unsigned int* resultado, int seed, int n_pixels, int total_bytes, char* transformacionUsada);
void aplicarTransformacionInversa(unsigned char* entrada, unsigned char* salida, unsigned char* imagenRandom, const char* nombreTransformacion, int totalBytes);
void aplicarTransformacionInversa(unsigned char* entrada, unsigned char* salida, unsigned char* imagenRandom, const char* nombreTransformacion, int totalBytes);


int main()
{

}

unsigned char* loadPixels(QString input, int &width, int &height){
    /*
  * @brief Carga una imagen BMP desde un archivo y extrae los datos de píxeles en formato RGB.
  *
  * Esta función utiliza la clase QImage de Qt para abrir una imagen en formato BMP, convertirla al
  * formato RGB888 (24 bits: 8 bits por canal), y copiar sus datos de píxeles a un arreglo dinámico
  * de tipo unsigned char. El arreglo contendrá los valores de los canales Rojo, Verde y Azul (R, G, B)
  * de cada píxel de la imagen, sin rellenos (padding).
  *
  * @param input Ruta del archivo de imagen BMP a cargar (tipo QString).
  * @param width Parámetro de salida que contendrá el ancho de la imagen cargada (en píxeles).
  * @param height Parámetro de salida que contendrá la altura de la imagen cargada (en píxeles).
  * @return Puntero a un arreglo dinámico que contiene los datos de los píxeles en formato RGB.
  *         Devuelve nullptr si la imagen no pudo cargarse.
  *
  * @note Es responsabilidad del usuario liberar la memoria asignada al arreglo devuelto usando `delete[]`.
  */

    QImage imagen(input);

    if (imagen.isNull()) {
        cout << "Error: No se pudo cargar la imagen BMP." << std::endl;
        return nullptr;
    }

    imagen = imagen.convertToFormat(QImage::Format_RGB888);

    width = imagen.width();
    height = imagen.height();

    int dataSize = width * height * 3;

    unsigned char* pixelData = new unsigned char[dataSize];

    for (int y = 0; y < height; ++y) {
        const uchar* srcLine = imagen.scanLine(y);
        unsigned char* dstLine = pixelData + y * width * 3;
        memcpy(dstLine, srcLine, width * 3);
    }

    return pixelData;
}

bool exportImage(unsigned char* pixelData, int width,int height, QString archivoSalida){
    /*
  * @brief Exporta una imagen en formato BMP a partir de un arreglo de píxeles en formato RGB.
  *
  * Esta función crea una imagen de tipo QImage utilizando los datos contenidos en el arreglo dinámico
  * `pixelData`, que debe representar una imagen en formato RGB888 (3 bytes por píxel, sin padding).
  * A continuación, copia los datos línea por línea a la imagen de salida y guarda el archivo resultante
  * en formato BMP en la ruta especificada.
  *
  * @param pixelData Puntero a un arreglo de bytes que contiene los datos RGB de la imagen a exportar.
  *                  El tamaño debe ser igual a width * height * 3 bytes.
  * @param width Ancho de la imagen en píxeles.
  * @param height Alto de la imagen en píxeles.
  * @param archivoSalida Ruta y nombre del archivo de salida en el que se guardará la imagen BMP (QString).
  *
  * @return true si la imagen se guardó exitosamente; false si ocurrió un error durante el proceso.
  *
  * @note La función no libera la memoria del arreglo pixelData; esta responsabilidad recae en el usuario.
  */

    QImage outputImage(width, height, QImage::Format_RGB888);

    for (int y = 0; y < height; ++y) {
        memcpy(outputImage.scanLine(y), pixelData + y * width * 3, width * 3);
    }

    if (!outputImage.save(archivoSalida, "BMP")) {
        cout << "Error: No se pudo guardar la imagen BMP modificada.";
        return false;
    } else {
        cout << "Imagen BMP modificada guardada como " << archivoSalida.toStdString() << endl;
        return true;
    }

}

unsigned int* loadSeedMasking(const char* nombreArchivo, int &seed, int &n_pixels){
    /*
  * @brief Carga la semilla y los resultados del enmascaramiento desde un archivo de texto.
  *
  * Esta función abre un archivo de texto que contiene una semilla en la primera línea y,
  * a continuación, una lista de valores RGB resultantes del proceso de enmascaramiento.
  * Primero cuenta cuántos tripletes de píxeles hay, luego reserva memoria dinámica
  * y finalmente carga los valores en un arreglo de enteros.
  *
  * @param nombreArchivo Ruta del archivo de texto que contiene la semilla y los valores RGB.
  * @param seed Variable de referencia donde se almacenará el valor entero de la semilla.
  * @param n_pixels Variable de referencia donde se almacenará la cantidad de píxeles leídos
  *                 (equivalente al número de líneas después de la semilla).
  *
  * @return Puntero a un arreglo dinámico de enteros que contiene los valores RGB
  *         en orden secuencial (R, G, B, R, G, B, ...). Devuelve nullptr si ocurre un error al abrir el archivo.
  *
  * @note Es responsabilidad del usuario liberar la memoria reservada con delete[].
  */

    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo." << endl;
        return nullptr;
    }

    archivo >> seed;

    int r, g, b;

    while (archivo >> r >> g >> b) {
        n_pixels++;
    }

    archivo.close();
    archivo.open(nombreArchivo);

    if (!archivo.is_open()) {
        cout << "Error al reabrir el archivo." << endl;
        return nullptr;
    }

    unsigned int* RGB = new unsigned int[n_pixels * 3];

    archivo >> seed;

    for (int i = 0; i < n_pixels * 3; i += 3) {
        archivo >> r >> g >> b;
        RGB[i] = r;
        RGB[i + 1] = g;
        RGB[i + 2] = b;
    }

    archivo.close();

    cout << "Semilla: " << seed << endl;
    cout << "Cantidad de píxeles leídos: " << n_pixels << endl;

    return RGB;
}

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
unsigned char rotacion(unsigned char elemento, unsigned int NumRotaciones, char direccion){

    unsigned char elementoRotado;
    if(direccion == 'I'){
        elementoRotado = ((elemento << NumRotaciones) | (elemento >> (8 - NumRotaciones)));
    }
    else if(direccion == 'D'){
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

unsigned char* rotarBits(unsigned char* imagen, int tamaño, int NumRotaciones, char direccion){

    //definimos una variable donde guardaremos la imagen rotada;
    //*ADVERTENCIA*, no olvide eliminar el espacio de memoria de imagenRotada cuando se deje de utilizar;
    unsigned char* imagenRotada = new unsigned char[tamaño];

    //creamos un ciclo el cual nos ayudara a pasar y rotar cada uno de los elementos de la imagen;
    //a cada elemento se le asignara el resultado de la funcion rotarBits con los diferentes parametros (Numero de rotaciones y direccion)
    for(int i = 0; i < tamaño; i += 3){
        imagenRotada[i] = rotacion(imagen[i], NumRotaciones, direccion);
        imagenRotada[i + 1] = rotacion(imagen[i + 1], NumRotaciones, direccion);
        imagenRotada[i + 2] = rotacion(imagen[i + 2], NumRotaciones, direccion);
    }

    return imagenRotada;
}

/* Funcion desplazarBits():
  * esta funcion desplaza los bits de la imagen una cantidad y en una direccion especifica.
  * Entradas:
  *      unsigned char* imagen = arreglo dinamico con los bits de la imagen inicial.
  *      int tamaño = tamaño del arreglo dinamico inicial.
  *      int NumDesplazamientos = cantidad de bits a desplazar.
  *      char direccion = direccion en la cual se desplazaran los bits.
  */

unsigned char* desplazarBits(unsigned char* imagen, int tamaño, int NumDesplazamientos, char direccion){

    //definimos una variable donde guardaremos la imagen desplazada.
    //*ADVERTENCIA*, no olvide eliminar el espacio de memoria de imagenDesplazada cuando se deje de utilizar.
    unsigned char* imagenDesplazada = new unsigned char[tamaño];

    for(int i = 0; i < tamaño; i += 3){
        if (direccion == 'D'){
            imagenDesplazada[i] = imagen[i] >> NumDesplazamientos;
            imagenDesplazada[i + 1] = imagen[i + 1] >> NumDesplazamientos;
            imagenDesplazada[i + 2] = imagen[i + 2] >> NumDesplazamientos;
        }

        else if (direccion == 'I'){
            imagenDesplazada[i] = imagen[i] << NumDesplazamientos;
            imagenDesplazada[i + 1] = imagen[i + 1] << NumDesplazamientos;
            imagenDesplazada[i + 2] = imagen[i + 2] << NumDesplazamientos;
        }
    }

    return imagenDesplazada;
}


bool validarEnmascaramiento(const unsigned char* imagen, const unsigned char* mascara, const unsigned int* resultado, int seed, int n_pixels, int total_bytes) {
    /* Esta función verifica si al validar el enmascaramiento aplicado sobre el arreglo de la imagen candidata coincide con el .txt
  */
    if (imagen == 0 || mascara == 0 || resultado == 0) return false;
    if (seed < 0 || n_pixels <= 0) return false;

    unsigned long inicio = (unsigned long)seed * 3UL;
    unsigned long requerido = (unsigned long)n_pixels * 3UL;
    unsigned long total_bytes_ul = (unsigned long)total_bytes;

    if (inicio >= total_bytes_ul) return false;
    if (requerido > total_bytes_ul - inicio) return false;

    const unsigned char* pImagen = imagen + inicio;
    const unsigned char* pMascara = mascara;

    for (unsigned long i = 0; i < requerido; ++i) {
        unsigned int suma = (unsigned int)pImagen[i] + (unsigned int)pMascara[i];

        if (suma != resultado[i]) {
            return false;
        }
    }

    return true;
}

void convertirEnteroEnTexto(int numero, char* buffer) {
    if (numero < 0 || numero > 99) {
        buffer[0] = '\0';
        return;
    }

    if (numero < 10) {
        buffer[0] = '0' + numero;
        buffer[1] = '\0';
    } else {
        buffer[0] = '0' + (numero / 10);
        buffer[1] = '0' + (numero % 10);
        buffer[2] = '\0';
    }
}

bool detectarTransformacion(unsigned char* imagenActual, unsigned char* IM, unsigned char* mascara, unsigned int* resultado, int seed, int n_pixels, int total_bytes, char* transformacionUsada) {
    unsigned char* bufferPrueba = new unsigned char[total_bytes];

    for (int i = 0; i < total_bytes; ++i) {
        bufferPrueba[i] = imagenActual[i] ^ IM[i];
    }
    if (validarEnmascaramiento(bufferPrueba, mascara, resultado, seed, n_pixels, total_bytes)) {
        transformacionUsada[0] = 'X';
        transformacionUsada[1] = 'O';
        transformacionUsada[2] = 'R';
        transformacionUsada[3] = '\0';
        delete[] bufferPrueba;
        bufferPrueba = nullptr;
        return true;
    }

    for (int b = 1; b <= 7; ++b) {
        unsigned char* rotadaDerecha = rotarBits(imagenActual, total_bytes, b, 'D');
        if (validarEnmascaramiento(rotadaDerecha, mascara, resultado, seed, n_pixels, total_bytes)) {
            transformacionUsada[0] = 'R';
            transformacionUsada[1] = 'O';
            transformacionUsada[2] = 'T';
            transformacionUsada[3] = '_';
            transformacionUsada[4] = '0' + b;
            transformacionUsada[5] = '_';
            transformacionUsada[6] = 'D';
            transformacionUsada[7] = '\0';

            delete[] rotadaDerecha;
            rotadaDerecha = nullptr;
            delete[] bufferPrueba;
            bufferPrueba = nullptr;
            return true;
        }
        delete[] rotadaDerecha;
        rotadaDerecha = nullptr;

        unsigned char* rotadaIzquierda = rotarBits(imagenActual, total_bytes, b, 'I');
        if (validarEnmascaramiento(rotadaIzquierda, mascara, resultado, seed, n_pixels, total_bytes)) {
            transformacionUsada[0] = 'R';
            transformacionUsada[1] = 'O';
            transformacionUsada[2] = 'T';
            transformacionUsada[3] = '_';
            transformacionUsada[4] = '0' + b;
            transformacionUsada[5] = '_';
            transformacionUsada[6] = 'I';
            transformacionUsada[7] = '\0';

            delete[] rotadaIzquierda;
            rotadaIzquierda = nullptr;
            delete[] bufferPrueba;
            bufferPrueba = nullptr;
            return true;
        }
        delete[] rotadaIzquierda;
        rotadaIzquierda = nullptr;
    }

    for (int b = 1; b <= 7; ++b) {
        unsigned char* desplazadaDerecha = desplazarBits(imagenActual, total_bytes, b, 'D');
        if (validarEnmascaramiento(desplazadaDerecha, mascara, resultado, seed, n_pixels, total_bytes)) {
            transformacionUsada[0] = 'D';
            transformacionUsada[1] = 'E';
            transformacionUsada[2] = 'S';
            transformacionUsada[3] = '_';
            transformacionUsada[4] = '0' + b;
            transformacionUsada[5] = '_';
            transformacionUsada[6] = 'D';
            transformacionUsada[7] = '\0';

            delete[] desplazadaDerecha;
            desplazadaDerecha = nullptr;
            delete[] bufferPrueba;
            bufferPrueba = nullptr;
            return true;
        }
        delete[] desplazadaDerecha;
        desplazadaDerecha = nullptr;

        unsigned char* desplazadaIzquierda = desplazarBits(imagenActual, total_bytes, b, 'I');
        if (validarEnmascaramiento(desplazadaIzquierda, mascara, resultado, seed, n_pixels, total_bytes)) {
            transformacionUsada[0] = 'D';
            transformacionUsada[1] = 'E';
            transformacionUsada[2] = 'S';
            transformacionUsada[3] = '_';
            transformacionUsada[4] = '0' + b;
            transformacionUsada[5] = '_';
            transformacionUsada[6] = 'I';
            transformacionUsada[7] = '\0';

            delete[] desplazadaIzquierda;
            desplazadaIzquierda = nullptr;
            delete[] bufferPrueba;
            bufferPrueba = nullptr;
            return true;
        }
        delete[] desplazadaIzquierda;
        desplazadaIzquierda = nullptr;
    }

    delete[] bufferPrueba;
    bufferPrueba = nullptr;
    return false;
}

void aplicarTransformacionInversa(unsigned char* entrada, unsigned char* salida, unsigned char* imagenRandom, const char* nombreTransformacion, int totalBytes) {
    auto compararCadenas = [](const char* a, const char* b, int n) -> bool {
        for (int i = 0; i < n; ++i) {
            if (a[i] != b[i]) return false;
            if (a[i] == '\0') break;
        }
        return true;
    };

    if (compararCadenas(nombreTransformacion, "XOR", 3)) {
        for (int i = 0; i < totalBytes; ++i) {
            salida[i] = entrada[i] ^ imagenRandom[i];
        }
    }
    else if (compararCadenas(nombreTransformacion, "ROT_", 4)) {
        int bits = nombreTransformacion[4] - '0';
        char direccionOriginal = nombreTransformacion[6];

        char direccionInversa = (direccionOriginal == 'D') ? 'I' : 'D';

        unsigned char* temp = rotarBits(entrada, totalBytes, bits, direccionInversa);
        for (int i = 0; i < totalBytes; ++i) {
            salida[i] = temp[i];
        }
        delete[] temp;
        temp = nullptr;
    }
    else if (compararCadenas(nombreTransformacion, "DES_", 4)) {
        int bits = nombreTransformacion[4] - '0';
        char direccionOriginal = nombreTransformacion[6];

        char direccionInversa = (direccionOriginal == 'D') ? 'I' : 'D';

        unsigned char* temp = desplazarBits(entrada, totalBytes, bits, direccionInversa);
        for (int i = 0; i < totalBytes; ++i) {
            salida[i] = temp[i];
        }
        delete[] temp;
        temp = nullptr;
    }
}

bool reconstruirSecuencial(const char* archivoImagenFinal, const char* archivoImagenRandom, const char* archivoMascara, const char** archivosTxT, int numArchivos, const char* archivoSalida) {

    int ancho, alto;
    unsigned char* imagenFinal = loadPixels(QString(archivoImagenFinal), ancho, alto);
    unsigned char* imagenRandom = loadPixels(QString(archivoImagenRandom), ancho, alto);
    unsigned char* mascara = loadPixels(QString(archivoMascara), ancho, alto);

    if (!imagenFinal || !imagenRandom || !mascara) {
        if (imagenFinal) delete[] imagenFinal;
        if (imagenRandom) delete[] imagenRandom;
        if (mascara) delete[] mascara;
        return false;
    }

    const int totalBytes = ancho * alto * 3;
    unsigned char* buffer1 = new unsigned char[totalBytes];
    unsigned char* buffer2 = new unsigned char[totalBytes];

    for (int i = 0; i < totalBytes; i++) buffer1[i] = imagenFinal[i];

    for (int i = numArchivos - 1; i >= 0; i--) {
        int seed, n_pixels;
        unsigned int* resultado = loadSeedMasking(archivosTxT[i], seed, n_pixels);
        if (!resultado) {
            delete[] buffer1;
            delete[] buffer2;
            delete[] imagenFinal;
            delete[] imagenRandom;
            delete[] mascara;
            return false;
        }

        char transformacionUsada[32];
        if (!detectarTransformacion(buffer1, imagenRandom, mascara, resultado,
                                    seed, n_pixels, totalBytes, transformacionUsada)) {
            delete[] buffer1;
            delete[] buffer2;
            delete[] imagenFinal;
            delete[] imagenRandom;
            delete[] mascara;
            delete[] resultado;
            return false;
        }

        aplicarTransformacionInversa(buffer1, buffer2, imagenRandom, transformacionUsada, totalBytes);

        unsigned char* tempSwap = buffer1;
        buffer1 = buffer2;
        buffer2 = tempSwap;

        delete[] resultado;
    }

    bool exito = exportImage(buffer1, ancho, alto, QString(archivoSalida));

    delete[] imagenFinal;
    delete[] imagenRandom;
    delete[] mascara;
    delete[] buffer1;
    delete[] buffer2;

    return exito;
}





