#ifndef LISTA_H_
#define LISTA_H_

#include <stdlib.h>
#include <stdbool.h>

typedef struct lista lista_t;
typedef struct lista_iterador lista_iterador_t;

/**
 * Crea un lista vacía.
* Devuelve NULL en caso de error.
 */
lista_t *lista_crear();

/**
 * Devuelve true si la lista está vacía. Una lista nula está vacía siempre.
*/
bool lista_vacia(lista_t *lista);

/**
 * Devuelve la cantidad de elementos en la lista o 0 en caso de error.
 */
size_t lista_cantidad(lista_t *lista);

/**
 * Agrega un elemento al final de la lista.
 *
 * Devuelve true si pudo agregar el elemento.
 */
bool lista_agregar(lista_t *lista, void *dato);

/**
 * Agrega un elemento en la posición dada de la lista.
 *
 * Si se intenta insertar en una posición que no existe no lo inserta y
 * devuelve false.
 */
bool lista_insertar(lista_t *lista, void *elemento, size_t posicion);

/**
 * Elimina un elemento de la lista. Devuelve el elemento.
 *
 * En caso de error devuelve NULL
 */
void *lista_eliminar_elemento(lista_t *lista, size_t posicion);

/**
 * Busca un elemento en la lista y devuelve la posición del mismo.
 *
 * Si no existe el elemento devuelve -1
 */
int lista_buscar_posicion(lista_t *lista, void *elemento,
			  int (*comparador)(const void *, const void *));

/**
 * Dada una posición de la lista, devuelve el elemento en esa posicion. En caso de error devuelve NULL.
 */
void *lista_buscar_elemento(lista_t *lista, size_t posicion);

/**
 * Recorre los elementos de la lista y aplica a cada uno la función f.
 *
 * Cuando la función f devuelve false se deja de recorrer la lista.
 *
 * La función retorna la cantidad de elementos a los cuales se le aplicó f
 */
size_t lista_con_cada_elemento(lista_t *lista, bool (*f)(void *, void *),
			       void *extra);

/**
 * Destruye la lista aplicando la función destructora a cada elemento.
 */
void lista_destruir_todo(lista_t *lista, void (*destructor)(void *));

/**
* Destruye la lista.
*/
void lista_destruir(lista_t *lista);

/**
* Crea un iterador externo para la lista.
 */
lista_iterador_t *lista_iterador_crear(lista_t *lista);

/**
 * Devuelve true si aún quedan elementos por iterar.
 */
bool lista_iterador_hay_mas_elementos(lista_iterador_t *it);

/**
* Avanza el iterador a la siguiente iteración.
 */
void lista_iterador_siguiente(lista_iterador_t *it);

/**
* Devuelve el elemento de la iteración actual.
 */
void *lista_iterador_obtener_actual(lista_iterador_t *it);

/**
* Destruye el iterador.
*/
void lista_iterador_destruir(lista_iterador_t *it);

#endif // LISTA_H_