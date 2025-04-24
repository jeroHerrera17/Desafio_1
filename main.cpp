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

//modulo 3:
bool detectarTransformacion(unsigned char* imagenActual, unsigned char* IM, unsigned char* mascara, unsigned int* resultado, int seed, int n_pixels, int total_bytes, char* transformacionUsada);
void convertirEnteroEnTexto(int numero, char* buffer);
bool detectarTransformacion(unsigned char* imagenActual, unsigned char* IM, unsigned char* mascara, unsigned int* resultado, int seed, int n_pixels, int total_bytes, char* transformacionUsada);
void aplicarTransformacionInversa(unsigned char* entrada, unsigned char* salida, unsigned char* imagenRandom, const char* nombreTransformacion, int totalBytes);
bool reconstruirSecuencial(const char* archivoImagenFinal, const char* archivoImagenRandom, const char* archivoMascara, const char** archivosTxT, int numArchivos, const char* archivoSalida);


int main() {
    const char* imagenFinal = "D:/Pruebas/Caso 1/I_D.bmp";
    const char* imagenRandom = "D:/Pruebas/Caso 1/I_M.bmp";
    const char* mascara = "D:/Pruebas/Caso 1/M.bmp";

    const char* archivosTxt[] = {
        "D:/Pruebas/Caso 1/M0.txt",
        "D:/Pruebas/Caso 1/M1.txt",
        "D:/Pruebas/Caso 1/M2.txt",
    };
    int numArchivos = sizeof(archivosTxt) / sizeof(archivosTxt[0]);

    const char* salida = "D:/Pruebas/Caso 1/resultado.bmp";

    if (reconstruirSecuencial(imagenFinal, imagenRandom, mascara, archivosTxt, numArchivos, salida)) {
        cout << "Reconstrucción completada con éxito." << endl;
    } else {
        cerr << "Error durante la reconstrucción." << endl;
    }

    return 0;
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

    // Cargar la imagen BMP desde el archivo especificado (usando Qt)
    QImage imagen(input);

    // Verifica si la imagen fue cargada correctamente
    if (imagen.isNull()) {
        cout << "Error: No se pudo cargar la imagen BMP." << std::endl;
        return nullptr; // Retorna un puntero nulo si la carga falló
    }

    // Convierte la imagen al formato RGB888 (3 canales de 8 bits sin transparencia)
    imagen = imagen.convertToFormat(QImage::Format_RGB888);

    // Obtiene el ancho y el alto de la imagen cargada
    width = imagen.width();
    height = imagen.height();

    // Calcula el tamaño total de datos (3 bytes por píxel: R, G, B)
    int dataSize = width * height * 3;

    // Reserva memoria dinámica para almacenar los valores RGB de cada píxel
    unsigned char* pixelData = new unsigned char[dataSize];

    // Copia cada línea de píxeles de la imagen Qt a nuestro arreglo lineal
    for (int y = 0; y < height; ++y) {
        const uchar* srcLine = imagen.scanLine(y);              // Línea original de la imagen con posible padding
        unsigned char* dstLine = pixelData + y * width * 3;     // Línea destino en el arreglo lineal sin padding
        memcpy(dstLine, srcLine, width * 3);                    // Copia los píxeles RGB de esa línea (sin padding)
    }

    // Retorna el puntero al arreglo de datos de píxeles cargado en memoria
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

    // Crear una nueva imagen de salida con el mismo tamaño que la original
    // usando el formato RGB888 (3 bytes por píxel, sin canal alfa)
    QImage outputImage(width, height, QImage::Format_RGB888);

    // Copiar los datos de píxeles desde el buffer al objeto QImage
    for (int y = 0; y < height; ++y) {
        // outputImage.scanLine(y) devuelve un puntero a la línea y-ésima de píxeles en la imagen
        // pixelData + y * width * 3 apunta al inicio de la línea y-ésima en el buffer (sin padding)
        // width * 3 son los bytes a copiar (3 por píxel)
        memcpy(outputImage.scanLine(y), pixelData + y * width * 3, width * 3);
    }

    // Guardar la imagen en disco como archivo BMP
    if (!outputImage.save(archivoSalida, "BMP")) {
        // Si hubo un error al guardar, mostrar mensaje de error
        cout << "Error: No se pudo guardar la imagen BMP modificada.";
        return false; // Indica que la operación falló
    } else {
        // Si la imagen fue guardada correctamente, mostrar mensaje de éxito
        cout << "Imagen BMP modificada guardada como " << archivoSalida.toStdString() << endl;
        return true; // Indica éxito
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

    // Abrir el archivo que contiene la semilla y los valores RGB
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        // Verificar si el archivo pudo abrirse correctamente
        cout << "No se pudo abrir el archivo." << endl;
        return nullptr;
    }

    // Leer la semilla desde la primera línea del archivo
    archivo >> seed;

    int r, g, b;

    // Contar cuántos grupos de valores RGB hay en el archivo
    // Se asume que cada línea después de la semilla tiene tres valores (r, g, b)
    while (archivo >> r >> g >> b) {
        n_pixels++;  // Contamos la cantidad de píxeles
    }

    // Cerrar el archivo para volver a abrirlo desde el inicio
    archivo.close();
    archivo.open(nombreArchivo);

    // Verificar que se pudo reabrir el archivo correctamente
    if (!archivo.is_open()) {
        cout << "Error al reabrir el archivo." << endl;
        return nullptr;
    }

    // Reservar memoria dinámica para guardar todos los valores RGB
    // Cada píxel tiene 3 componentes: R, G y B
    unsigned int* RGB = new unsigned int[n_pixels * 3];

    // Leer nuevamente la semilla desde el archivo (se descarta su valor porque ya se cargó antes)
    archivo >> seed;

    // Leer y almacenar los valores RGB uno por uno en el arreglo dinámico
    for (int i = 0; i < n_pixels * 3; i += 3) {
        archivo >> r >> g >> b;
        RGB[i] = r;
        RGB[i + 1] = g;
        RGB[i + 2] = b;
    }

    // Cerrar el archivo después de terminar la lectura
    archivo.close();

    // Mostrar información de control en consola
    cout << "Semilla: " << seed << endl;
    cout << "Cantidad de pixeles leidos: " << n_pixels << endl;

    // Retornar el puntero al arreglo con los datos RGB
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

        else{
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

    int width, height;
    unsigned char* I_D = loadPixels(QString(archivoImagenFinal), width, height);
    unsigned char* I_M = loadPixels(QString(archivoImagenRandom), width, height);
    unsigned char* M = loadPixels(QString(archivoMascara), width, height);

    if (!I_D || !I_M || !M) {
        if (I_D) delete[] I_D;
        if (I_M) delete[] I_M;
        if (M) delete[] M;
        return false;
    }

    const int totalBytes = width * height * 3;
    unsigned char* bufferActual = new unsigned char[totalBytes];
    unsigned char* bufferAnterior = new unsigned char[totalBytes];
    memcpy(bufferActual, I_D, totalBytes);

    bool exito = true;
    for (int i = 0; i < numArchivos; ++i) {  // Cambiado a orden directo
        int seed, n_pixels;
        unsigned int* enmascarado = loadSeedMasking(archivosTxT[i], seed, n_pixels);
        if (!enmascarado) {
            exito = false;
            break;
        }

        for (int k = 0; k < n_pixels * 3; ++k) {
            int pos = (seed * 3 + k) % totalBytes;
            bufferActual[pos] = enmascarado[k] - M[k % (width * height * 3)];
        }

        char transformacion[32];
        if (!detectarTransformacion(bufferActual, I_M, M, enmascarado,
                                    seed, n_pixels, totalBytes, transformacion)) {
            delete[] enmascarado;
            exito = false;
            break;
        }

        cout << "Paso " << (i + 1) << ": Se detectó la transformación '" << transformacion << "'." << endl;

        aplicarTransformacionInversa(bufferActual, bufferAnterior, I_M,
                                     transformacion, totalBytes);
        swap(bufferActual, bufferAnterior);

        delete[] enmascarado;
    }

    if (exito) {
        exportImage(bufferActual, width, height, QString(archivoSalida));
    }

    delete[] I_D;
    delete[] I_M;
    delete[] M;
    delete[] bufferActual;
    delete[] bufferAnterior;

    return exito;
}





