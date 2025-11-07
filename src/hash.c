#include "hash.h"
#include "lista.h"
#include "functions.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define FACTOR_CARGA_MAX 0.7
#define MAX_LISTA 8

typedef struct par {
	char *clave;
	void *valor;
} par_t;

struct hash {
	lista_t **tabla;
	size_t capacidad;
	size_t insertados;
};

typedef struct h_accionar_aux {
	par_t *par_conocido;
	void **valor_anterior;
	bool realizado;
} h_accionar_aux_t;

typedef struct rehash_aux {
	lista_t **tabla;
	size_t cap_tabla;
	bool error;
} rehash_aux_t;

// Función interna de hash
static size_t funcion_hash(const char *str)
{
	size_t hash = 5381;
	int c;
	while ((c = *str++))
		hash = ((hash << 5) + hash) + (size_t)c;
	return hash;
}

lista_t *obtener_lista(lista_t **tabla, size_t cap, char *c)
{
	size_t indice_tabla = funcion_hash(c) % cap;
	lista_t *lista_indicada = tabla[indice_tabla];

	return lista_indicada;
}

void par_destruir_todo(par_t *par, void (*destructor)(void *))
{
	if (!par)
		return;
	if (destructor)
		destructor(par->valor);
	free(par->clave);
	free(par);
}

void par_destruir(void *_p)
{
	par_t *par = _p;
	par_destruir_todo(par, NULL);
}

void vaciar_tabla_indice(lista_t *l, void (*destructor)(void *))
{
	if (!l)
		return;

	while (!lista_vacia(l)) {
		par_t *p = lista_eliminar_elemento(l, 0);
		par_destruir_todo(p, destructor);
	}
}

/*
    Pre: La tabla pasada por parametro no debe ser NULL.

    Post: Libera la memoria reservada para la tabla pasada por parametro.
*/
void tabla_destruir_todo(lista_t **tabla, size_t capacidad,
			 void (*destructor)(void *))
{
	for (size_t i = 0; i < capacidad; i++) {
		if (tabla[i]) {
			vaciar_tabla_indice(tabla[i], destructor);
			lista_destruir(tabla[i]);
		}
	}
	free(tabla);
}

void tabla_destruir(lista_t **tabla, size_t capacidad)
{
	tabla_destruir_todo(tabla, capacidad, NULL);
}

bool inicializar_tabla(lista_t **tabla, size_t capacidad)
{
	int indice = 0;
	bool error = false;
	while (indice < capacidad && !error) {
		tabla[indice] = lista_crear();
		if (!tabla[indice]) {
			error = true;
		}
		indice++;
	}

	return !error;
}

/**
 * Crea una tabla de hash con la capacidad especificada (o 3 si es menor a 3).
 *
 * Devuelve el hash creado o NULL en caso de error.
 *
 */
hash_t *hash_crear(size_t capacidad_inicial)
{
	hash_t *h = calloc(1, sizeof *h);
	if (!h)
		return NULL;

	h->capacidad = (capacidad_inicial < 3) ? 3 : capacidad_inicial;

	h->tabla = calloc(h->capacidad, sizeof h->tabla[0]);
	if (!h->tabla) {
		free(h);
		return NULL;
	}

	if (!inicializar_tabla(h->tabla, h->capacidad)) {
		tabla_destruir(h->tabla, h->capacidad);
		free(h);
		return NULL;
	}

	return h;
}

/**
 * Devuelve la cantidad de elementos en la tabla.
 *
 */
size_t hash_cantidad(hash_t *hash)
{
	return (hash) ? hash->insertados : 0;
}

par_t *inicializar_par(char *clave_a_insertar, void *valor_a_insertar)
{
	par_t *p = calloc(1, sizeof(par_t));
	if (!p)
		return NULL;

	char *clave_dup = mi_strdup(clave_a_insertar);
	if (!clave_dup)
		return NULL;

	p->clave = clave_dup;
	p->valor = valor_a_insertar;

	return p;
}

size_t hash_iterar_indice(lista_t *l, bool (*f)(char *, void *, void *),
			  void *ctx, bool *corto)
{
	lista_iterador_t *li = lista_iterador_crear(l);
	if (!li) {
		if (corto)
			*corto = false;
		return 0;
	}

	size_t iterados = 0;
	bool cortar = false;

	while (lista_iterador_hay_mas_elementos(li) && !cortar) {
		par_t *par = lista_iterador_obtener_actual(li);
		iterados++;
		if (!f(par->clave, par->valor, ctx))
			cortar = true;
		lista_iterador_siguiente(li);
	}

	lista_iterador_destruir(li);
	if (corto)
		*corto = cortar;
	return iterados;
}

bool actualizar_elemento(void *_p, void *_aux)
{
	par_t *p = _p;
	h_accionar_aux_t *aux = _aux;

	if (strcmp(p->clave, aux->par_conocido->clave) == 0) {
		if (aux->valor_anterior)
			*(aux->valor_anterior) = p->valor;

		p->valor = aux->par_conocido->valor;

		aux->realizado = true;
		return false;
	}

	return true;
}

bool insertar_par_lista(lista_t *l, char *clave, void *valor, void **encontrado,
			size_t *insertados, bool rehash)
{
	par_t *par_nuevo = inicializar_par(clave, valor);
	if (!par_nuevo)
		return false;

	if (!rehash) {
		h_accionar_aux_t aux = { .realizado = false,
					 .par_conocido = par_nuevo,
					 .valor_anterior = encontrado };
		lista_con_cada_elemento(l, actualizar_elemento, &aux);

		if (aux.realizado) {
			par_destruir(par_nuevo);
			return true;
		}
	}

	if (!lista_agregar(l, par_nuevo)) {
		par_destruir(par_nuevo);
		return false;
	}

	if (insertados)
		(*insertados)++;
	return true;
}

bool llenar_lista(void *_p, void *_aux)
{
	par_t *par_lista_vieja = _p;
	rehash_aux_t *aux = _aux;

	lista_t *lista_indicada = obtener_lista(aux->tabla, aux->cap_tabla,
						par_lista_vieja->clave);

	if (!insertar_par_lista(lista_indicada, par_lista_vieja->clave,
				par_lista_vieja->valor, NULL, NULL, true)) {
		aux->error = true;
		return false;
	}

	return true;
}

bool lista_insertar_pares(lista_t *l_vieja_actual, lista_t **tabla_nueva,
			  size_t cap_nueva)
{
	rehash_aux_t aux = { .tabla = tabla_nueva,
			     .cap_tabla = cap_nueva,
			     .error = false };

	lista_con_cada_elemento(l_vieja_actual, llenar_lista, &aux);
	if (aux.error) {
		return false;
	}

	return true;
}

bool inicializar_rehash(hash_t *h, size_t cap_nueva)
{
	lista_t **tabla_nueva = calloc(cap_nueva, sizeof(lista_t *));
	if (!tabla_nueva)
		return false;

	if (!inicializar_tabla(tabla_nueva, cap_nueva)) {
		tabla_destruir(tabla_nueva, cap_nueva);
		return false;
	}
	h->tabla = tabla_nueva;
	h->capacidad = cap_nueva;

	return true;
}

bool reinsertar_rehash(hash_t *h, lista_t **tabla_anterior, size_t cap_vieja)
{
	size_t indice_tabla = 0;
	bool error_insercion = false;
	while (indice_tabla < cap_vieja && !error_insercion) {
		lista_t *l = tabla_anterior[indice_tabla];

		if (!lista_insertar_pares(l, h->tabla, h->capacidad)) {
			error_insercion = true;
		}
		indice_tabla++;
	}
	return error_insercion;
}

bool hash_rehash(hash_t *h)
{
	lista_t **tabla_anterior = h->tabla;
	size_t capacidad_anterior = h->capacidad;
	size_t capacidad_nueva = capacidad_anterior * 2;

	if (!inicializar_rehash(h, capacidad_nueva))
		return false;

	bool error = reinsertar_rehash(h, tabla_anterior, capacidad_anterior);

	if (error) {
		tabla_destruir(h->tabla, h->capacidad);
		h->tabla = tabla_anterior;
		h->capacidad = capacidad_anterior;
	} else
		tabla_destruir(tabla_anterior, capacidad_anterior);

	return !error;
}

int comparar_par(const void *_p1, const void *_p2)
{
	const par_t *p1 = _p1;
	const par_t *p2 = _p2;

	return (strcmp(p1->clave, p2->clave));
}

/*
	Pre: El hash pasado por parametro debe haber sido creado previamente con
		 "hash_crear" o "hash_crear_con_funcion", la clave debe ser un string valido
		 y la posicion debe estar dentro de los rangos de la tabla.

	Post: Devuelve true si se debe rehashear la tabla, devuelve falso en caso contrario.
*/
bool debe_rehashear(const hash_t *h, char *clave)
{
	size_t cap = h->capacidad;
	lista_t *l = obtener_lista(h->tabla, cap, clave);

	par_t p = { .clave = clave, .valor = NULL };
	bool existe = lista_buscar_posicion(l, &p, comparar_par) != -1;
	size_t cantidad_nueva = h->insertados + (existe ? 0 : 1);
	size_t largo_nuevo = lista_cantidad(l) + (existe ? 0 : 1);

	bool factor_max_superado = ((double)cantidad_nueva / (double)cap) >
				   FACTOR_CARGA_MAX;
	bool max_lista_superado = (largo_nuevo > MAX_LISTA);

	return factor_max_superado || max_lista_superado;
}

/**
 *
 * Inserta un elemento asociado a la clave en la tabla de hash.
 *
 * Si la clave ya existe en la tabla, modificamos el valor y ponemos el nuevo.
 * Si encontrado no es NULL, se almacena el elemento reemplazado en el mismo.
 *
 * Esta funcion almacena una copia de la clave.
 *
 * No se admiten claves nulas.
 *
 * Devuelve true si se pudo almacenar el valor.
 **/
bool hash_insertar(hash_t *hash, char *clave, void *valor, void **encontrado)
{
	if (!hash || !clave)
		return false;
	if (encontrado)
		*encontrado = NULL;

	if (debe_rehashear(hash, clave)) {
		if (!hash_rehash(hash))
			return false;
	}

	lista_t *lista_indicada =
		obtener_lista(hash->tabla, hash->capacidad, clave);
	bool insertado = insertar_par_lista(lista_indicada, clave, valor,
					    encontrado, &hash->insertados,
					    false);

	return insertado;
}

bool buscar_par(void *_p, void *_aux)
{
	par_t *p = _p;
	h_accionar_aux_t *aux = _aux;

	if (strcmp(p->clave, aux->par_conocido->clave) == 0) {
		if (aux->valor_anterior) {
			*(aux->valor_anterior) = p->valor;
		}
		aux->realizado = true;
		return false;
	}
	return true;
}

/**
 * Busca el elemento asociado a la clave en la tabla.
 **/
void *hash_buscar(hash_t *hash, char *clave)
{
	if (!hash || !clave)
		return NULL;

	lista_t *lista_indicada =
		obtener_lista(hash->tabla, hash->capacidad, clave);

	void *resultado = NULL;
	par_t buscado = { .clave = (char *)clave };

	h_accionar_aux_t aux = { .par_conocido = &buscado,
				 .valor_anterior = &resultado,
				 .realizado = false };

	lista_con_cada_elemento(lista_indicada, buscar_par, &aux);

	return aux.realizado ? resultado : NULL;
}

/**
 *Devuelve si la clave existe en la tabla o no
 */
bool hash_contiene(hash_t *hash, char *clave)
{
	if (!hash || !clave)
		return false;

	lista_t *l = obtener_lista(hash->tabla, hash->capacidad, clave);

	par_t aux = { .clave = clave, .valor = NULL };
	int indice = lista_buscar_posicion(l, &aux, comparar_par);

	if (indice == -1)
		return false;

	return true;
}

/**
 * Quita el elemento asociado a la clave de la tabla y lo devuelve.
 */
void *hash_quitar(hash_t *hash, char *clave)
{
	if (!hash || !clave)
		return NULL;

	lista_t *l = obtener_lista(hash->tabla, hash->capacidad, clave);

	par_t par_aux = { .clave = clave, .valor = NULL };
	int indice = lista_buscar_posicion(l, &par_aux, comparar_par);
	if (indice == -1)
		return NULL;

	void *elemento_borrado = NULL;

	par_t *par_eliminado = lista_eliminar_elemento(l, (size_t)indice);
	if (par_eliminado) {
		elemento_borrado = par_eliminado->valor;
		par_destruir(par_eliminado);
		hash->insertados--;
	}

	return elemento_borrado;
}

/**
 * Itera cada elemento del hash y aplica la funcion f.
 *
 * La iteración se corta al completar el total de elementos o cuando la función devuelve false.
 *
 * Devuelve la cantidad de veces que se aplica la función.
 */
size_t hash_iterar(hash_t *hash, bool (*f)(char *, void *, void *), void *ctx)
{
	if (!hash || !f)
		return 0;

	size_t total = 0;
	size_t i = 0;
	bool corto = false;

	while (i < hash->capacidad && !corto) {
		lista_t *l = hash->tabla[i];
		if (l)
			total += hash_iterar_indice(l, f, ctx, &corto);
		i++;
	}
	return total;
}

/**
 * Destruye la tabla
 */
void hash_destruir(hash_t *hash)
{
	hash_destruir_todo(hash, NULL);
}
/**
 * Destruye la tabla y aplica el destructor a los elementos
 */
void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if (!hash)
		return;

	tabla_destruir_todo(hash->tabla, hash->capacidad, destructor);

	free(hash);
}