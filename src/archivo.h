#ifndef _ARCHIVO_H_
#define _ARCHIVO_H_

typedef struct archivo archivo_t;

/**
 * Abre un archivo DE TEXTO para su posterior lectura.
 * 
 * Devuelve el archivo si se pudo abrir, NULL en caso contrario.
 */
archivo_t *archivo_abrir(const char *nombre, const char *modo);

/**
 * Lee una línea del archivo.
 * 
 * Devuelve un puntero a la línea leída, o NULL si no hay más líneas.
 */
const char *archivo_leer_linea(archivo_t *archivo);

/**
 * Escribe una linea en el archivo.
 * 
 * Devuelve true si se pudo escribir, false en caso contrario.
 */
bool archivo_escribir_linea(archivo_t *archivo, const char *linea);

/**
 * Indica si hay más líneas por leer en el archivo.
 * 
 * Devuelve 1 si hay más líneas, 0 en caso contrario.
 */
int archivo_hay_mas_lineas(archivo_t *archivo);

/**
 * Devuelve la cantidad de líneas leídas hasta el momento.
 * 
 * Devuelve 0 si el archivo es NULL.
 */
int archivo_lineas_leidas(archivo_t *archivo);

/**
 * Cierra el archivo y libera toda la memoria reservada.
 */
void archivo_cerrar(archivo_t *archivo);

#endif // _ARCHIVO_H_