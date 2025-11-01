#include "lista.h"
#include <stdio.h>

#define POSICION_NO_ENCONTRADA -1

typedef struct nodo {
	void *elemento;
	struct nodo *siguiente;
} nodo_t;

typedef struct comparador_ctx {
	int (*comparador)(const void *, const void *);
	void *elemento_a_comparar;
	bool encontrado;
} comparador_ctx_t;

struct lista {
	nodo_t *principio;
	nodo_t *final;
	size_t cantidad;
};

struct lista_iterador {
	nodo_t *nodo_actual;
};

/*
* Pre: La lista no debe ser NULL y la posicion debe ser valida
*      (mayor que cero y menor que la cantidad)
* Post: Devuelve el nodo en la posicion pasada por parametro
* 	  Devuelve NULL si la posicion excede el tamanio de la lista
* 	  Si la posicion es menor al tamanio de la lista devuelve el principio.
*/
nodo_t *iterar_hasta_posicion(lista_t *lista, size_t posicion)
{
	int iterando = 0;

	nodo_t *actual = lista->principio;

	while (iterando < posicion) {
		actual = actual->siguiente;
		iterando += 1;
	}

	return actual;
}

lista_t *lista_crear()
{
	lista_t *l = calloc(1, sizeof(lista_t));
	if (!l)
		return NULL;
	return l;
}

bool lista_vacia(lista_t *lista)
{
	return !lista || lista_cantidad(lista) == 0;
}

size_t lista_cantidad(lista_t *lista)
{
	return (!lista) ? 0 : lista->cantidad;
}

bool lista_agregar(lista_t *lista, void *dato)
{
	if (!lista)
		return false;

	nodo_t *nodo_a_agregar = calloc(1, sizeof(nodo_t));
	if (nodo_a_agregar == NULL)
		return false;

	nodo_a_agregar->elemento = dato;
	if (lista->final == NULL)
		lista->principio = nodo_a_agregar;
	else
		lista->final->siguiente = nodo_a_agregar;

	lista->final = nodo_a_agregar;
	lista->cantidad += 1;
	return true;
}

/*
* Pre: La lista y el nodo no debe ser NULL y la posicion debe ser valida
*      (mayor que cero y menor que la cantidad)
* Post: Inserta el nodo en la posicion correcta del medio.
*/
void lista_insertar_medio(lista_t *l, nodo_t *nodo, size_t pos)
{
	nodo_t *anterior = iterar_hasta_posicion(l, pos - 1);

	nodo->siguiente = anterior->siguiente;
	anterior->siguiente = nodo;
}

bool lista_insertar(lista_t *lista, void *elemento, size_t posicion)
{
	if (!lista || posicion > lista->cantidad) {
		return false;
	}
	if (posicion == 0 && lista->cantidad == 0)
		return false;

	if (posicion == lista->cantidad)
		return lista_agregar(lista, elemento);

	nodo_t *nodo_nuevo = calloc(1, sizeof(nodo_t));
	if (!nodo_nuevo)
		return false;

	nodo_nuevo->elemento = elemento;

	if (posicion == 0) {
		nodo_nuevo->siguiente = lista->principio;
		lista->principio = nodo_nuevo;
	} else
		lista_insertar_medio(lista, nodo_nuevo, posicion);

	lista->cantidad += 1;
	return true;
}

/*
* Pre: la lista y el nodo pasados por parametro
*      no deben ser NULL.
*
* Post: Devuelve el nodo a sacar de la lista del inicio.
*/
nodo_t *lista_sacar_inicio(lista_t *l)
{
	nodo_t *buscado = l->principio;

	l->principio = buscado->siguiente;

	if (lista_cantidad(l) == 1) {
		l->final = NULL;
	}

	return buscado;
}

/*
* Pre: la lista y el nodo pasados por parametro
*      no deben ser NULL.
*
* Post: Devuelve el nodo a sacar de la lista.
*/
nodo_t *lista_sacar_medio(lista_t *l, nodo_t *ant)
{
	nodo_t *buscado = ant->siguiente;

	ant->siguiente = buscado->siguiente;

	if (!ant->siguiente)
		l->final = ant;

	return buscado;
}

void *lista_eliminar_elemento(lista_t *lista, size_t posicion)
{
	if (!lista)
		return NULL;
	if (posicion >= lista_cantidad(lista))
		return NULL;

	nodo_t *aux = NULL;

	if (posicion == 0)
		aux = lista_sacar_inicio(lista);
	else {
		nodo_t *anterior = iterar_hasta_posicion(lista, posicion - 1);
		if (!anterior)
			return NULL;

		aux = lista_sacar_medio(lista, anterior);
	}

	void *elem = aux->elemento;
	free(aux);

	lista->cantidad--;

	return elem;
}

/*
* Pre: El elemento ni el CTX deben ser NULL.
*
* Post: Devuelve true si se encontro el elemento,
*       Devuelve false en caso contrario.
*/
bool recorrer_entero_cmp_ctx(void *elemento, void *ctx)
{
	comparador_ctx_t *contexto = (comparador_ctx_t *)ctx;

	if (contexto->comparador(elemento, contexto->elemento_a_comparar) ==
	    0) {
		contexto->encontrado = true;
		return false;
	}

	return true;
}

int lista_buscar_posicion(lista_t *lista, void *elemento,
			  int (*comparador)(const void *, const void *))
{
	if (!lista || !comparador)
		return POSICION_NO_ENCONTRADA;

	comparador_ctx_t ctx = { .comparador = comparador,
				 .elemento_a_comparar = elemento,
				 .encontrado = false };

	int iterados = (int)lista_con_cada_elemento(
		lista, recorrer_entero_cmp_ctx, &ctx);

	if (!ctx.encontrado) {
		return POSICION_NO_ENCONTRADA;
	}

	return iterados - 1;
}

void *lista_buscar_elemento(lista_t *lista, size_t posicion)
{
	if (!lista)
		return NULL;

	if (posicion >= lista_cantidad(lista))
		return NULL;

	return iterar_hasta_posicion(lista, posicion)->elemento;
}

size_t lista_con_cada_elemento(lista_t *lista, bool (*f)(void *, void *),
			       void *extra)
{
	if (!lista || !f)
		return 0;

	size_t iterados = 0;
	bool continuar = true;

	nodo_t *actual = lista->principio;

	while (actual != NULL && continuar) {
		continuar = f(actual->elemento, extra);
		actual = actual->siguiente;
		iterados++;
	}

	return iterados;
}

/*
* Pre: La lista pasada por parametro no debe ser NULL
*
* Post: Libera toda la memoria correspondiente a la lista,
        Si el destructor no es NULL tambien libera los elementos
*/
void destruir_completo(lista_t *l, void (*destructor)(void *))
{
	nodo_t *a_destruir = l->principio;
	nodo_t *siguiente_a_destruir;

	while (a_destruir != NULL) {
		siguiente_a_destruir = a_destruir->siguiente;

		if (destructor)
			destructor(a_destruir->elemento);

		free(a_destruir);
		a_destruir = siguiente_a_destruir;
	}

	free(l);
}

void lista_destruir_todo(lista_t *lista, void (*destructor)(void *))
{
	if (!lista)
		return;

	destruir_completo(lista, destructor);
}

void lista_destruir(lista_t *lista)
{
	if (!lista)
		return;

	destruir_completo(lista, NULL);
}

lista_iterador_t *lista_iterador_crear(lista_t *lista)
{
	if (!lista || lista_vacia(lista))
		return NULL;

	lista_iterador_t *it = malloc(sizeof(lista_iterador_t));
	if (!it)
		return NULL;

	it->nodo_actual = lista->principio;
	return it;
}

bool lista_iterador_hay_mas_elementos(lista_iterador_t *it)
{
	return it && it->nodo_actual != NULL;
}

void lista_iterador_siguiente(lista_iterador_t *it)
{
	if (!it || !lista_iterador_hay_mas_elementos(it))
		return;

	it->nodo_actual = it->nodo_actual->siguiente;
}

void *lista_iterador_obtener_actual(lista_iterador_t *it)
{
	if (!it || !lista_iterador_hay_mas_elementos(it))
		return NULL;

	return it->nodo_actual->elemento;
}

void lista_iterador_destruir(lista_iterador_t *it)
{
	if (!it)
		return;

	free(it);
}