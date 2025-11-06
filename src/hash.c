#include "hash.h"
#include "lista.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define FACTOR_CARGA_MAX 0.7
#define MAX_LISTA 8

#define DEBUG_PAR(par_ptr) \
    do { \
        if ((par_ptr) != NULL) { \
            printf("🔎 clave=\"%s\" | ", (par_ptr)->clave ? (par_ptr)->clave : "(null)"); \
            if ((par_ptr)->valor) \
                printf("valor=%d | ptr=%p\n", *((int*)((par_ptr)->valor)), (par_ptr)->valor); \
            else \
                printf("valor=NULL | ptr=%p\n", (par_ptr)->valor); \
        } else { \
            printf("⚠️ par_ptr es NULL\n"); \
        } \
    } while (0)
typedef struct par{
    char* clave;
    void* valor;
}par_t;

struct hash{
    lista_t** tabla;
    size_t capacidad;
    size_t insertados;
};

enum modo { BUSQUEDA, INSERCION};

//estructura aux para insertar
typedef struct h_accionar_aux{
    par_t* par_conocido;
    void** valor_anterior;
    bool realizado;
}h_accionar_aux_t;

// Función interna de hash (no visible desde afuera)
static size_t funcion_hash(const char* str) {
    size_t hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + (size_t)c;
    return hash;
}

void par_destruir_todo(par_t *par, void (*destructor)(void *)){
    if (!par) return;
    if (destructor) destructor(par->valor);
    free(par->clave);
    free(par);
}

void par_destruir(void *_p){
    par_t* par=_p;
    par_destruir_todo(par,NULL);
}

bool inicializar_tabla(lista_t** tabla, size_t capacidad){
    int indice=0;
    bool error=false;
    while (indice<capacidad && !error){
        tabla[indice]=lista_crear();
        if (!tabla[indice]){
            error=true;
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
hash_t *hash_crear(size_t capacidad_inicial){
    hash_t *h = calloc(1, sizeof *h);
    if (!h) return NULL;

    h->capacidad = (capacidad_inicial < 3) ? 3 : capacidad_inicial;

    h->tabla = calloc(h->capacidad, sizeof h->tabla[0]);
    if (!h->tabla) {
        free(h);
        return NULL;
    }

    if (!inicializar_tabla(h->tabla, h->capacidad)){
        for (size_t i = 0; i < h->capacidad; i++){
            if (h->tabla[i]) lista_destruir(h->tabla[i]);
        }
        free(h->tabla);
        free(h);
        return NULL;
    }

    return h;
}

/**
 * Devuelve la cantidad de elementos en la tabla.
 *
 */
size_t hash_cantidad(hash_t *hash){
    return (hash) ? hash->insertados : 0;
}

char *mi_strdup(const char *s) {
    if (s == NULL)
        return NULL;

    size_t len = strlen(s) + 1;     // +1 para el '\0'
    char *dup = malloc(len);
    if (dup == NULL)
        return NULL;                // error de memoria

    memcpy(dup, s, len);            // copia todo, incluyendo el '\0'
    return dup;
}

par_t* inicializar_par(char *clave_a_insertar, void *valor_a_insertar){
    par_t* p=calloc(1,sizeof(par_t));
    if (!p) return NULL;
    
    char *clave_dup=mi_strdup(clave_a_insertar);
    if (!clave_dup) return NULL;

    p->clave=clave_dup;
    p->valor=valor_a_insertar;

    return p;
}

size_t hash_iterar_indice(lista_t* l, bool (*f)(char *, void *, void *), void *ctx, bool *corto){
    lista_iterador_t *li = lista_iterador_crear(l);
    if (!li){
        if (corto)
            *corto = false;
        return 0;
    }

    size_t iterados = 0;
    bool cortar = false;

    while (lista_iterador_hay_mas_elementos(li) && !cortar) {
        par_t* par = lista_iterador_obtener_actual(li);
        iterados++;
        if (!f(par->clave, par->valor, ctx)) cortar = true;
        lista_iterador_siguiente(li);
    }

    lista_iterador_destruir(li);
    if (corto) *corto = cortar;
    return iterados;
}

/*
    Pre: La tabla pasada por parametro no debe ser NULL.

    Post: Libera la memoria reservada para la tabla pasada por parametro.
*/
void destruir_tabla(lista_t **tabla, size_t capacidad, void (*destructor)(void *)){
    for (size_t i=0; i<capacidad;i++){
        if (tabla[i]){
            lista_destruir_todo(tabla[i],destructor);
        }
    }
    free(tabla);
}

/*
    Pre: La tabla vieja y la tabla nueva no deben ser NULL.

    Post: Copia los elementos de la tabla vieja en la tabla nueva.
          Devuelve false en caso de error, devuelve true en caso contrario.
*/
bool clonar_tabla(lista_t **vieja, lista_t **nueva, size_t cap_vieja, size_t cap_nueva){
    for (size_t i = 0; i < cap_vieja; i++) {
        lista_iterador_t *it = lista_iterador_crear(vieja[i]);
        if (!it) return false;

        bool ok = true;
        while (ok && lista_iterador_hay_mas_elementos(it)) {
            par_t *p = lista_iterador_obtener_actual(it);   // MISMO puntero
            size_t j = funcion_hash(p->clave) % cap_nueva;
            ok = lista_agregar(nueva[j], p);
            lista_iterador_siguiente(it);
        }
        lista_iterador_destruir(it);
        if (!ok) return false;
    }
    return true;
}

typedef struct rehash_aux{
    lista_t **tabla;
    size_t cap_tabla;
    bool error;
}rehash_aux_t;

bool llenar_lista(void *_p, void *_aux){
    par_t* par_lista_vieja=_p;
    rehash_aux_t *aux=_aux;

    par_t *p_nuevo=inicializar_par(par_lista_vieja->clave,par_lista_vieja->valor);
    if (!p_nuevo){
        aux->error=true;
        return false;
    }

    size_t indice= funcion_hash(par_lista_vieja->clave) % aux->cap_tabla;
    lista_t* lista_indicada=aux->tabla[indice];

    if(!lista_agregar(lista_indicada,p_nuevo)){
        par_destruir(p_nuevo);
        aux->error=true;
        return false;
    }

    return true;
}

bool lista_insertar_pares(lista_t* l_vieja_actual,lista_t **tabla_nueva, size_t cap_nueva){
    rehash_aux_t aux={.tabla=tabla_nueva, .cap_tabla=cap_nueva, .error=false};

    lista_con_cada_elemento(l_vieja_actual,llenar_lista,&aux);
    if (aux.error){
        return false;
    }

    return true;
}

bool hash_rehash(hash_t *h){
    lista_t **tabla_anterior=h->tabla;
    size_t capacidad_anterior=h->capacidad;

    size_t capacidad_nueva = capacidad_anterior * 2;

    lista_t **tabla_nueva=calloc(capacidad_nueva,sizeof(lista_t*));
    if (!tabla_nueva) return false;

    if (!inicializar_tabla(tabla_nueva,capacidad_nueva)){
        destruir_tabla(tabla_nueva,capacidad_nueva,NULL);
        return false;
    }
    
    h->tabla=tabla_nueva;
    h->capacidad=capacidad_nueva;
    //h->insertados=0;

    size_t indice_tabla=0;
    bool error_insercion=false;
    while(indice_tabla<capacidad_anterior && !error_insercion){
        lista_t* l=tabla_anterior[indice_tabla];

        if (!lista_insertar_pares(l,tabla_nueva,capacidad_nueva)){
            error_insercion=true;
        }
        indice_tabla++;
    }

    if (error_insercion){
        destruir_tabla(tabla_nueva,h->capacidad,par_destruir);
        h->tabla=tabla_anterior;
        h->capacidad=capacidad_anterior;
    }else
        destruir_tabla(tabla_anterior,capacidad_anterior,par_destruir);

    return !error_insercion;
}

int comparar_par(const void *_p1, const void *_p2){
    const par_t *p1=_p1;
    const par_t *p2=_p2;
    
    return (strcmp(p1->clave,p2->clave));
}


/*
	Pre: El hash pasado por parametro debe haber sido creado previamente con
		 "hash_crear" o "hash_crear_con_funcion", la clave debe ser un string valido
		 y la posicion debe estar dentro de los rangos de la tabla.

	Post: Devuelve true si se debe rehashear la tabla, devuelve falso en caso contrario.
*/
bool debe_rehashear(const hash_t *h, char *clave) {
    size_t cap = h->capacidad;

    size_t indice   = funcion_hash(clave) % cap;
    lista_t *l = h->tabla[indice];

    par_t p={.clave=clave, .valor=NULL};
    bool existe = lista_buscar_posicion(l,&p,comparar_par) != -1;
    size_t cantidad_nueva   = h->insertados + (existe ? 0 : 1);
    size_t largo_nuevo = lista_cantidad(l) + (existe ? 0 : 1);


    bool factor_max_superado = ((double)cantidad_nueva / (double)cap) > FACTOR_CARGA_MAX;
    bool max_lista_superado = (largo_nuevo > MAX_LISTA);

    return factor_max_superado || max_lista_superado;
}



bool actualizar_elemento(void *_p, void *_aux){
    par_t* p=_p;
    h_accionar_aux_t * aux=_aux;

    if (strcmp(p->clave,aux->par_conocido->clave)==0){
        if (aux->valor_anterior)
            *(aux->valor_anterior) = p->valor;

        p->valor=aux->par_conocido->valor;
        
        aux->realizado=true;
        return false;
    }

    return true;
}

bool insertar_par_lista(lista_t *l, char *clave, void *valor, void **encontrado, size_t *insertados){
    par_t* par_nuevo=inicializar_par(clave,valor);
    if (!par_nuevo) return false;
    
    h_accionar_aux_t aux={.realizado=false, .par_conocido=par_nuevo, .valor_anterior=encontrado};
    lista_con_cada_elemento(l,actualizar_elemento,&aux);

    if (aux.realizado) {
        // No inserté par_nuevo, lo uso solo como contenedor de clave/valor nuevos
        par_destruir(par_nuevo); // destruye la copia de clave del candidato
        return true;
    }

    // No existía → insertar nuevo
    if (!lista_agregar(l, par_nuevo)) {
        par_destruir(par_nuevo);
        return false;
    }

    if (insertados) (*insertados)++;
    return true;
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
bool hash_insertar(hash_t *hash, char *clave, void *valor, void **encontrado){
    if (!hash || !clave) return false;
    
	if (debe_rehashear(hash,clave)) {
        if (!hash_rehash(hash))
        return false;
	}
    
    if (encontrado) *encontrado = NULL;

    size_t indice_tabla= funcion_hash(clave) % hash->capacidad;

    lista_t* lista_indicada= hash->tabla[indice_tabla];

    bool insertado = insertar_par_lista(lista_indicada,clave,valor,encontrado,&hash->insertados);
    
    return insertado;
}

bool buscar_par(void *_p, void *_aux){
    par_t *p = _p;
    h_accionar_aux_t *aux = _aux;

    // Si garantizás claves no nulas, strcmp es seguro
    if (strcmp(p->clave, aux->par_conocido->clave) == 0){
        if (aux->valor_anterior) {
            *(aux->valor_anterior) = p->valor;
        }
        aux->realizado = true;
        return false;
    }
    return true; // seguir
}

/**
 * Busca el elemento asociado a la clave en la tabla.
 **/
void *hash_buscar(hash_t *hash, char *clave){
    if (!hash || !clave) return NULL;

    size_t indice_tabla= funcion_hash(clave) % hash->capacidad;
    lista_t* lista_indicada= hash->tabla[indice_tabla];

    void *resultado = NULL;                 // acá dejaremos el valor
    par_t buscado = {.clave = (char*)clave};  // sólo usamos la clave

    h_accionar_aux_t aux = {
        .par_conocido  = &buscado,
        .valor_anterior = &resultado,
        .realizado     = false
    };

    lista_con_cada_elemento(lista_indicada, buscar_par, &aux);

    return aux.realizado ? resultado : NULL;
}

/**
 *Devuelve si la clave existe en la tabla o no
 */
bool hash_contiene(hash_t *hash, char *clave){
    if (!hash || !clave) return false;

    size_t indice_tabla= funcion_hash(clave) % hash->capacidad;
    lista_t* l= hash->tabla[indice_tabla];

    par_t aux={.clave=clave, .valor=NULL};
    int indice=lista_buscar_posicion(l,&aux,comparar_par);

    if (indice == -1)
        return false;

    return true;
}

/**
 * Quita el elemento asociado a la clave de la tabla y lo devuelve.
 */
void *hash_quitar(hash_t *hash, char *clave){
    if (!hash || !clave) return NULL;

    size_t indice_tabla= funcion_hash(clave) % hash->capacidad;
    lista_t* l= hash->tabla[indice_tabla];

    par_t par_aux={.clave=clave, .valor=NULL};
    int indice=lista_buscar_posicion(l,&par_aux,comparar_par);
    if (indice == -1) return NULL;

    void *elemento_borrado = NULL;

    par_t* par_eliminado=lista_eliminar_elemento(l,(size_t)indice);
    if (par_eliminado){
        elemento_borrado=par_eliminado->valor;
        par_destruir(par_eliminado);
        hash->insertados--;
    }

    return elemento_borrado;
}

/**
 * Itera cada elemento del hash y aplica la función f.
 *
 * La iteración se corta al completar el total de elementos o cuando la función devuelve false.
 *
 * Devuelve la cantidad de veces que se aplica la función.
 */
size_t hash_iterar(hash_t *hash, bool (*f)(char *, void *, void *), void *ctx) {
    if (!hash || !f) return 0;

    size_t total = 0;
    size_t i = 0;
    bool corto = false;

    while (i < hash->capacidad && !corto) {
        lista_t *l = hash->tabla[i];
        if (l) total += hash_iterar_indice(l, f, ctx, &corto);
        i++;
    }
    return total;
}

void vaciar_tabla_indice(lista_t* l, void (*destructor)(void *)){
    if (!l) return;

    while (!lista_vacia(l)){
        par_t* p=lista_eliminar_elemento(l,0);
        par_destruir_todo(p,destructor);
    }
}

/**
 * Destruye la tabla
 */
void hash_destruir(hash_t *hash){
    hash_destruir_todo(hash,NULL);
}
/**
 * Destruye la tabla y aplica el destructor a los elementos
 */
void hash_destruir_todo(hash_t *hash, void (*destructor)(void *)){
    if (!hash) return;

    for (size_t i=0;i<hash->capacidad;i++){
        lista_t* l=hash->tabla[i];
        if (l){
            vaciar_tabla_indice(l,destructor);
            lista_destruir(l);
        }
    }
    free(hash->tabla);
    free(hash);
}