#include "hash.h"
#include "lista.h"
#include <string.h>

#define FACTOR_CARGA_MAX 0.7
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
    par_t* par_encontrado;
    void** valor_anterior;
    bool accion_realizada;
    enum modo modo_accion;
}h_accionar_aux_t;

// Función interna de hash (no visible desde afuera)
static size_t funcion_hash(const char* str) {
    size_t hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + (size_t)c;
    return hash;
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
    hash_t* h=calloc(1,sizeof(hash_t));
    if (!h) return NULL;

    h->capacidad = (capacidad_inicial < 3) ? 3 : capacidad_inicial;
    
    h->tabla=calloc(capacidad_inicial,sizeof(lista_t*));
    if (!h->tabla) return NULL;

    if (!inicializar_tabla(h, h->capacidad)){
        hash_destruir(h);
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

par_t* inicializar_par(char *clave_a_insertar, void *valor_a_insertar){
    par_t* p=calloc(1,sizeof(par_t));
    if (!p) return NULL;
    
    char *clave_dup=strdup(clave_a_insertar);
    if (!clave_dup) return NULL;

    p->clave=clave_dup;
    p->valor=valor_a_insertar;

    return p;
}

size_t hash_iterar_indice(lista_t* l, bool (*f)(char *, void *, void *), void *ctx){
    lista_iterador_t *li=lista_iterador_crear(l);
    if (!li) return 0;

    size_t iterados=0;
    bool cortar=false;

    while (lista_iterador_hay_mas_elementos(li) && !cortar){
        par_t* par_actual=lista_iterador_obtener_actual(li);
        iterados++;
        if (!f(par_actual->clave,par_actual->valor,ctx)){
            cortar=true;
        }
        lista_iterador_siguiente(li);
    }


    lista_iterador_destruir(li);

    return iterados;
}

bool recorrer_hasta(char *clave, void *valor, void *aux){
    h_accionar_aux_t* h_aux=aux;

    int comparacion = strcmp(clave,h_aux->par_conocido->clave);
    if (comparacion == 0 && h_aux->modo_accion==INSERCION){
        if (h_aux->valor_anterior) *(h_aux->valor_anterior)=valor;
        valor=h_aux->par_conocido->valor;
        h_aux->accion_realizada=true;
        return false;
    }else if (comparacion == 0 && h_aux->modo_accion==BUSQUEDA){
        h_aux->par_encontrado->clave=clave;
        h_aux->par_encontrado->valor=valor;
        h_aux->accion_realizada=true;
        return false;
    }

    return true;
}
/*
    Pre: La tabla pasada por parametro no debe ser NULL.

    Post: Libera la memoria reservada para la tabla pasada por parametro.
*/
void destruir_tabla(lista_t **tabla, size_t capacidad){
    for (size_t i=0; i<capacidad;i++){
        if (tabla[i]){
            lista_destruir(tabla[i]);
        }
    }
    free(tabla);
}

/*
    Pre: La tabla vieja y la tabla nueva no deben ser NULL.

    Post: Copia los elementos de la tabla vieja en la tabla nueva.
          Devuelve false en caso de error, devuelve true en caso contrario.
*/
bool clonar_tabla(lista_t **tabla_vieja, lista_t **tabla_nueva, size_t cap_vieja, size_t cap_nueva){
    bool seguir = true;

    for (size_t i = 0; i < cap_vieja; i++) {
        if (seguir) {
            lista_iterador_t *li = lista_iterador_crear(tabla_vieja[i]);
            if (!li) seguir = false;

            while (seguir && lista_iterador_hay_mas_elementos(li)) {
                par_t *p = lista_iterador_obtener_actual(li);
                size_t j = funcion_hash(p->clave) % cap_nueva;

                if (!lista_agregar(tabla_nueva[j], p)) seguir = false;
                else lista_iterador_siguiente(li);
            }

            if (li) lista_iterador_destruir(li);
        }
    }

    return seguir;
}

/*
	Pre: El hash pasado por parametro debe ser valido y haber sido creado previamente
		 con "hash_crear" o con "hash_crear_con_funcion" 

	Post: Devuelve true si se logro rehashear la tabla correctamente
		  Devuelve false si no se logro rehashear, se vuelve a la tabla anterior.
*/
bool hash_rehash(hash_t *h, size_t nueva_cap)
{
    lista_t **nuevos = calloc(nueva_cap, sizeof(lista_t*));
    if (!nuevos) return false;

    if (!inicializar_tabla(nuevos, nueva_cap)) {
        destruir_tabla(nuevos, nueva_cap);
        return false;
    }

    bool seguir = clonar_tabla(h->tabla,nuevos,h->capacidad,nueva_cap);

    if (!seguir) {
        destruir_tabla(nuevos, nueva_cap);
        return false;
    }

    for (size_t i = 0; i < h->capacidad; i++)
        lista_destruir(h->tabla[i]);
    free(h->tabla);

    h->tabla     = nuevos;
    h->capacidad = nueva_cap;

    return true;
}

/*
	Pre: El hash pasado por parametro debe haber sido creado previamente con
		 "hash_crear" o "hash_crear_con_funcion", la clave debe ser un string valido
		 y la posicion debe estar dentro de los rangos de la tabla.

	Post: Devuelve true si se debe rehashear la tabla, devuelve falso en caso contrario.
*/
bool debe_rehashear(hash_t *h)
{
	return (double)(h->insertados + 1) / (double)h->capacidad > FACTOR_CARGA_MAX;
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
    
	if (debe_rehashear(hash)) {
        if (!hash_rehash(hash,hash->capacidad * 2))
        return false;
	}
    
    size_t indice_tabla= funcion_hash(clave) % hash->capacidad;

    par_t* par_nuevo=inicializar_par(clave,valor);
    if (!par_nuevo) return false;

    lista_t* lista_indicada= hash->tabla[indice_tabla];

    if (!lista_indicada){
        hash->tabla[indice_tabla]=lista_crear();
        if (!hash->tabla[indice_tabla]){
            par_destruir(par_nuevo);
            return false;
        }
        lista_indicada=hash->tabla[indice_tabla];
    }

    h_accionar_aux_t aux = {.par_conocido=par_nuevo, .valor_anterior = encontrado, .accion_realizada=false, .modo_accion=INSERCION};
    hash_iterar_indice(lista_indicada,recorrer_hasta,&aux);

    if (aux.accion_realizada){
        //si se actualizo
        //no sumo 1 a insertados
        par_destruir(par_nuevo);
        return true;
    }

    bool agregado = lista_agregar(lista_indicada,par_nuevo);
    if (!agregado){
        par_destruir(par_nuevo);
        return false;
    }

    //si no se actualizo
    //sumo 1 a insertados
    hash->insertados++;
    
    return agregado;
}

/**
 * Busca el elemento asociado a la clave en la tabla.
 **/
void *hash_buscar(hash_t *hash, char *clave){
    if (!hash || !clave) return NULL;

    size_t indice_tabla= funcion_hash(clave) % hash->capacidad;
    lista_t* lista_indicada= hash->tabla[indice_tabla];

    par_t par_buscado={.clave=clave};
    h_accionar_aux_t aux={.par_conocido=&par_buscado, .accion_realizada=false, .modo_accion=BUSQUEDA};
    hash_iterar_indice(lista_indicada,recorrer_hasta,&aux);

    return aux.par_encontrado->valor;
}



int comparar_par(const void *_p1, const void *_p2){
    par_t *p1=_p1;
    par_t *p2=_p2;
    
    return (strcmp(p1->clave,p2->clave));
}


/**
 *Devuelve si la clave existe en la tabla o no
 */
bool hash_contiene(hash_t *hash, char *clave){
    if (!hash || !clave) return false;

    lista_t* l=obtener_lista(hash,clave);

    par_t aux={.clave=clave, .valor=NULL};
    size_t indice=lista_buscar_posicion(l,&aux,comparar_par);

    if (indice == -1)
        return false;

    return true;
}

/**
 * Quita el elemento asociado a la clave de la tabla y lo devuelve.
 */
void *hash_quitar(hash_t *hash, char *clave){
    if (!hash || !clave) return NULL;

    lista_t *l= obtener_lista(hash,clave);

    par_t par_aux={.clave=clave, .valor=NULL};
    size_t indice=lista_buscar_posicion(l,&par_aux,comparar_par);

    par_t* par_eliminado=lista_eliminar_elemento(l,indice);

    return par_eliminado->valor;
}

/**
 * Itera cada elemento del hash y aplica la función f.
 *
 * La iteración se corta al completar el total de elementos o cuando la función devuelve false.
 *
 * Devuelve la cantidad de veces que se aplica la función.
 */
size_t hash_iterar(hash_t *hash, bool (*f)(char *, void *, void *), void *ctx);

/**
 * Destruye la tabla
 */
void hash_destruir(hash_t *hash);
/**
 * Destruye la tabla y aplica el destructor a los elementos
 */
void hash_destruir_todo(hash_t *hash, void (*destructor)(void *));