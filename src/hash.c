#include "hash.h"
#include "lista.h"

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
typedef struct h_insertar_aux{
    par_t* par;
    void** encontrado;
    bool insertado;
    enum modo modo;
}h_insertar_aux_t;

// Función interna de hash (no visible desde afuera)
static size_t funcion_hash(const char* str) {
    size_t hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + (size_t)c;
    return hash;
}

bool inicializar_tabla(hash_t* h){
    int indice=0;
    bool error=false;
    while (indice<h->capacidad || !error){
        h->tabla[indice]=lista_crear();
        if (!h->tabla[indice]){
            error=true;
        }
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

    if (!inicializar_tabla(h)){
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

bool insertar_no_vacio(void *_p, void *aux){
    par_t *par=_p;
    h_insertar_aux_t* h_aux=aux;

    if (strcmp(par->clave,h_aux->par->clave) == 0){
        if (h_aux->encontrado) *(h_aux->encontrado)=par->valor;
        par->valor=h_aux->par->valor;
        return false;
    }

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
    // falta ver: 
    // factores de carga
    // y redimensionar

    size_t indice_tabla= funcion_hash(clave) % hash->capacidad;

    par_t* par_nuevo=inicializar_par(clave,valor);
    if (!par_nuevo) return false;

    lista_t* lista_indicada= hash->tabla[indice_tabla];

    if (!lista_indicada){
        hash->tabla[indice_tabla]=lista_crear();
        if (!hash->tabla[indice_tabla]){
            return false;
        }
        lista_indicada=hash->tabla[indice_tabla];
    }
    //if (lista_cantidad(lista_indicada) == 0){
    //    return (lista_agregar(lista_indicada,par_nuevo));
    //}

    //se puede hacer tmb con lista_iterador (conviene para buscar elemento y saber si contiene x elemento)
    h_insertar_aux_t aux = {.par=par_nuevo, .encontrado = encontrado, .insertado=false};
    lista_con_cada_elemento(lista_indicada,insertar_no_vacio,&aux); 
    // puedo iterar la lista, y por cada uno ver si coincide la clave
    // si la clave coincide entonces actualizo el valor,
    // de esa forma recorro una sola vez y puedo actualizar el valor en caso de que ya este
    // para eso armo una estructura auxiliar para
    // pasarle como parametro el "**encontrado", en caso de tener que guardarlo.

    if (aux.insertado){
        par_destruir(par_nuevo);
        hash->insertados++;
        return true;
    }

    bool agregado = lista_agregar(lista_indicada,par_nuevo);
    if (!agregado){
        par_destruir(par_nuevo);
        return false;
    }

    hash->insertados++;
    return agregado;

}

/**
 * Busca el elemento asociado a la clave en la tabla.
 **/
void *hash_buscar(hash_t *hash, char *clave);

/**
 *Devuelve si la clave existe en la tabla o no
 */
bool hash_contiene(hash_t *hash, char *clave);

/**
 * Quita el elemento asociado a la clave de la tabla y lo devuelve.
 */
void *hash_quitar(hash_t *hash, char *clave);

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