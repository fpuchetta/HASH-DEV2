#ifndef TP1_H_
#define TP1_H_
#include <stdlib.h>
#include <stdbool.h>

enum tipo_pokemon {
	TIPO_ELEC,
	TIPO_FUEG,
	TIPO_PLAN,
	TIPO_AGUA,
	TIPO_NORM,
	TIPO_FANT,
	TIPO_PSI,
	TIPO_LUCH
};

struct pokemon {
	int id;
	char *nombre;
	enum tipo_pokemon tipo;
	int ataque;
	int defensa;
	int velocidad;
};

typedef struct tp1 tp1_t;

/**
 * Lee el archivo indicado y devuelve la estructura tp1 con los pokemones.
 * En caso de error devuelve NULL.
*/
tp1_t *tp1_leer_archivo(const char *nombre);

/**
 * Devuevle la cantidad de pokemones leidos correctamente.
 * En caso de error devuelve 0.
*/
size_t tp1_cantidad(tp1_t *tp1);

/**
 * Guarda en el archivo indicado los pokemones contenidos en la estructura tp1 
 * de manera tal que tp1_leer_archivo pueda volver a leerlo correctamente.
 *
 * Devuelve NULL en caso de error o el tp1 pasado por parámetro en caso de exito.
*/
tp1_t *tp1_guardar_archivo(tp1_t *tp1, const char *nombre);

/**
 * Dados 2 tp1, devuelve un nuevo tp1 que es la unión de ambos.
 * No se deben incluír pokemones repetidos. En caso de repeticiones se debe
 * preferir el pokemon del primer tp1.
 *
 * En caso de error devuelve NULL.
*/
tp1_t *tp1_union(tp1_t *un_tp, tp1_t *otro_tp);

/**
 * Dados 2 tp1, devuelve un nuevo tp1 que es la intersección de ambos.
 * 
 * En caso de error devuelve NULL.
*/
tp1_t *tp1_interseccion(tp1_t *un_tp, tp1_t *otro_tp);

/**
* Dados 2 tp1, devuelve un nuevo tp1 que es la diferencia de ambos.
* La diferencia se define como los pokemones que están en el primer tp1
* pero no en el segundo.
*
* En caso de error devuelve NULL.
*/
tp1_t *tp1_diferencia(tp1_t *un_tp, tp1_t *otro_tp);

/**
* Busca un pokemon por nombre.
* En caso de no encontrarlo devuelve NULL.
*/
struct pokemon *tp1_buscar_nombre(tp1_t *tp, const char *nombre);

/**
* Busca un pokemon por id.
* En caso de no encontrarlo devuelve NULL.
*/
struct pokemon *tp1_buscar_id(tp1_t *tp, int id);

/**
 * Aplica la función f a cada pokemon por orden de id (de menor a mayor).
 * La función deja de aplicarse si f devuelve false o se terminaron los pokemones.
 * 
 * Devuelve la cantidad de pokemones sobre los cuales se aplicó la función f.
 */
size_t tp1_con_cada_pokemon(tp1_t *un_tp, bool (*f)(struct pokemon *, void *),
			    void *extra);

/**
 * Libera toda la memoria asociada al tp1
 */
void tp1_destruir(tp1_t *tp1);

#endif // TP1_H_
