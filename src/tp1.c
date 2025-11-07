#include <stdio.h>
#include <string.h>

#include "functions.h"
#include "archivo.h"

struct tp1 {
	struct pokemon **pokemones;
	size_t cantidad_pokemones;
	size_t capacidad;
};

/*
	Pre: El parametro no debe ser NULL
	Post: Devuelve el largo del nombre en la linea pasada por parametro
	      Devuelve cero en caso de error
*/
size_t buscar_largo_nombre(const char *linea)
{
	const char *inicio = strchr(linea, ',');
	if (!inicio)
		return 0;

	inicio++;

	const char *fin = strchr(inicio, ',');
	if (!fin)
		return 0;

	return (size_t)(fin - inicio);
}

/*
	Pre: "linea" y "estado_error" no pueden ser NULL
	Post: Devuelve un puntero a un string reservado con el largo del nombre
		  Devuelve NULL en caso de error
*/
char *reservar_memoria_nombre(const char *linea, int *estado_error)
{
	size_t largo_nombre = buscar_largo_nombre(linea);
	if (largo_nombre == 0) {
		*estado_error = ERROR_PARSEO;
		return NULL;
	}

	char *nombre = malloc(largo_nombre + 1);
	if (!nombre) {
		*estado_error = ERROR_MEMORIA;
	}
	return nombre;
}

/*
	Pre: "linea" no debe ser NULL
	Post Devuelve el largo del tipo de la linea pasada por parametro
		 Devuelve cero en caso de error.
*/
size_t contar_largo_tipo(const char *linea)
{
	const char *primero = strchr(linea, ',');
	if (!primero)
		return 0;

	const char *segundo = strchr(primero + 1, ',');
	if (!segundo)
		return 0;

	const char *tercero = strchr(segundo + 1, ',');
	if (!tercero)
		return 0;

	return (size_t)(tercero - (segundo + 1));
}

/*
	Pre: -
	Post: Destruye toda la memoria reservada para
		  el pokemon pasado por parametro
*/
void destruir_pokemon(struct pokemon *p)
{
	if (!p)
		return;
	free(p->nombre);
	free(p);
}

/*
	Pre: "linea" no debe ser NULL,
		 "estado_error" no debe ser NULL
	Post: Devuelve un pokemon con toda su memoria reservada en el heap,
		  Devuelve NULL en caso de error
*/
struct pokemon *reservar_memoria_parseo(const char *linea, int *estado_error)
{
	struct pokemon *pokemon = calloc(1, sizeof(struct pokemon));
	if (!pokemon) {
		*estado_error = ERROR_MEMORIA;
		return NULL;
	}

	pokemon->nombre = reservar_memoria_nombre(linea, estado_error);
	if (!pokemon->nombre) {
		free(pokemon);
		return NULL;
	}

	return pokemon;
}

/*
	Pre: "linea", "estado_error" no deben ser NULL,
		 "p" no debe ser NULL y tener sus campos inicializados
	Post: Devuelve la cantidad de columnas leidas luego de llenar el pokemon
	 	  Devuelve 0 en caso de error.
*/
int completar_pokemon(const char *linea, struct pokemon *p,
		      char tipo_aux[MAX_TIPO], int *estado_error)
{
	size_t largo_tipo = contar_largo_tipo(linea);
	if (largo_tipo > MAX_TIPO) {
		destruir_pokemon(p);
		*estado_error = ERROR_PARSEO;
		return 0;
	}

	int columnas_leidas = sscanf(linea, "%d,%[^,],%[^,],%d,%d,%d", &p->id,
				     p->nombre, tipo_aux, &p->ataque,
				     &p->defensa, &p->velocidad);

	return columnas_leidas;
}

/*
	Pre: "estado_error" no debe ser NULL,
		 "p" no debe ser NULL y tener sus campos inicializados
	Post: Cambia estado_error en caso de error de memoria o parseo segun corresponda
		  Devuelve void si sale bien.
*/
void verificar_errores(struct pokemon *p, char tipo_aux[MAX_TIPO],
		       int col_leidas, int *estado_error)
{
	if (col_leidas != COLUMNAS_CSV) {
		destruir_pokemon(p);
		if (estado_error)
			*estado_error = ERROR_PARSEO;
		return;
	}

	if (p->id < 0 || p->ataque < 0 || p->defensa < 0 || p->velocidad < 0) {
		destruir_pokemon(p);
		if (estado_error)
			*estado_error = ERROR_PARSEO;
		return;
	}

	enum tipo_pokemon t = parsear_tipo(tipo_aux);
	if (t < TIPO_ELEC || t > TIPO_LUCH) {
		destruir_pokemon(p);
		if (estado_error)
			*estado_error = ERROR_PARSEO;
		return;
	}

	p->tipo = t;
}

/*
	Pre: "estado_error" no debe ser NULL
	Post: Devuelve el pokemon parseado
		  Devuelve NULL en caso de error.
*/
struct pokemon *parsear_pokemon(const char *linea, int *estado_error)
{
	if (!linea || strcmp(linea, "") == 0) {
		*estado_error = ERROR_PARSEO;
		return NULL;
	}
	struct pokemon *pokemon = reservar_memoria_parseo(linea, estado_error);
	if (!pokemon)
		return NULL;

	char tipo_aux[MAX_TIPO] = { 0 };
	int columnas_leidas =
		completar_pokemon(linea, pokemon, tipo_aux, estado_error);
	if (*estado_error == ERROR_PARSEO)
		return NULL;

	verificar_errores(pokemon, tipo_aux, columnas_leidas, estado_error);
	if (*estado_error == ERROR_PARSEO)
		return NULL;

	return pokemon;
}

/*
	Pre: "p" y "tp1" no deben ser NULL
	Post: Devuelve true si se pudo cargar el pokemon en la estructura
	      Devuelve false en caso contrario
*/
bool cargar_pokemon(struct pokemon *p, tp1_t *tp1)
{
	if (tp1->cantidad_pokemones == tp1->capacidad) {
		size_t nueva_cap =
			tp1->capacidad ?
				tp1->capacidad * FACTOR_AUMENTO_TAMANIO :
				CAPACIDAD_MINIMA;

		struct pokemon **temp = realloc(
			tp1->pokemones, nueva_cap * sizeof(struct pokemon *));
		if (!temp)
			return false;

		tp1->pokemones = temp;
		tp1->capacidad = nueva_cap;
	}

	tp1->pokemones[tp1->cantidad_pokemones] = p;
	tp1->cantidad_pokemones++;
	return true;
}

/*
	Pre: "tp1" no debe ser NULL
	Post: Devuelve true si se encuentra el pokemon en tp1
	      Devuelve false en caso contrario
*/
bool esta_cargado(tp1_t *tp1, int id_buscada)
{
	int i = 0;
	bool encontrado = false;
	while (i < tp1->cantidad_pokemones && !encontrado) {
		if (tp1->pokemones[i]->id == id_buscada) {
			encontrado = true;
		}
		i++;
	}
	return encontrado;
}

/*
	Pre: "tp1" y "estado_actual" no deben ser NULL
	Post: Devuelve false si se incoporo correctamente el pokemon
		  Devuelve false en caso contrario.
*/
bool incorporacion_correcta(struct pokemon *p, tp1_t *tp1, int *estado_actual)
{
	bool agregado = false;
	if (p) {
		if (!esta_cargado(tp1, p->id)) {
			if (cargar_pokemon(p, tp1)) {
				agregado = true;
				*estado_actual = 0;
			} else
				*estado_actual = ERROR_MEMORIA;
		}
		if (!agregado)
			destruir_pokemon(p);
	}
	if (*estado_actual == ERROR_MEMORIA)
		return false;

	*estado_actual = 0;
	return true;
}

/*
	Pre: "archivo" y "tp1" no deben ser NULL
	Post: Devuelve true si se pudieron cargar todos los pokemones
		  Devuelve false en caso contrario
*/
bool cargar_pokemones(archivo_t *archivo, tp1_t *tp1)
{
	const char *linea_proxima;
	int estado_actual = 0;
	while (archivo_hay_mas_lineas(archivo)) {
		linea_proxima = archivo_leer_linea(archivo);
		struct pokemon *p =
			parsear_pokemon(linea_proxima, &estado_actual);

		if (!incorporacion_correcta(p, tp1, &estado_actual))
			return false;
	}

	return true;
}

tp1_t *tp1_leer_archivo(const char *nombre)
{
	if (!nombre)
		return NULL;

	tp1_t *pokedex = calloc(1, sizeof(tp1_t));
	if (!pokedex)
		return NULL;

	archivo_t *archivo = archivo_abrir(nombre, MODO_LECTURA);
	if (!archivo) {
		tp1_destruir(pokedex);
		return NULL;
	}
	if (!cargar_pokemones(archivo, pokedex)) {
		tp1_destruir(pokedex);
		archivo_cerrar(archivo);
		return NULL;
	}

	if (pokedex->cantidad_pokemones > 1)
		ordenar_vec_pokemones(pokedex->pokemones,
				      pokedex->cantidad_pokemones,
				      sizeof(struct pokemon *),
				      cmp_pokemon_id_ptr);

	archivo_cerrar(archivo);
	return pokedex;
}

size_t tp1_cantidad(tp1_t *tp1)
{
	if (!tp1)
		return 0;
	return tp1->cantidad_pokemones;
}

/*
	Pre: "p" no debe ser NULL
	Post: Devuelve la linea parseada para cargar en el archivo
		  Devuelve NULL en caso de error
*/
char *parsear_linea(struct pokemon *p)
{
	size_t tam = calcular_largo_linea(p);

	const char *tipo = calcular_tipo(p);
	tam += strlen(tipo);

	char *linea = malloc(tam);
	if (!linea)
		return NULL;

	int k = snprintf(linea, tam, "%d,%s,%s,%d,%d,%d\n", p->id, p->nombre,
			 tipo, p->ataque, p->defensa, p->velocidad);

	if (k < 0 || (size_t)k >= tam) {
		free(linea);
		return NULL;
	}
	return linea;
}

/*
	Pre: "archivo" y "tp1" no deben ser NULL
	Post: Llena el archivo con las lineas parseadas
*/
void llenar_archivo(archivo_t *archivo, tp1_t *tp1)
{
	char *linea;
	for (int i = 0; i < tp1->cantidad_pokemones; i++) {
		linea = parsear_linea(tp1->pokemones[i]);
		archivo_escribir_linea(archivo, (const char *)linea);
		free(linea);
	}
}

tp1_t *tp1_guardar_archivo(tp1_t *tp1, const char *nombre)
{
	if (!tp1 || !nombre)
		return NULL;

	archivo_t *archivo = archivo_abrir(nombre, MODO_ESCRITURA);
	if (!archivo)
		return NULL;

	llenar_archivo(archivo, tp1);

	archivo_cerrar(archivo);
	return tp1;
}

/*
	Pre: "destino" y "p" n odeben ser NULL
	Post: Devuelve True si se pudo clonar el pokemon correctamente
		  Devuelve false en caso contrario
*/
bool cargar_clon(tp1_t *destino, struct pokemon *p)
{
	struct pokemon *c = pokemon_clonar(p);
	if (!c)
		return false;
	if (!cargar_pokemon(c, destino)) {
		destruir_pokemon(c);
		return false;
	}
	return true;
}

/*
*   Pre: La operacion pasada por parametro debe ser un tipo de operacion valida por el enum.
         Los tps A y B, no pueden tener duplicados internos.
*   Post: Devuelve un tp resultado con los pokemones de la operacion correspondiente
*/
tp1_t *operacion_conjuntos(tp1_t *a, tp1_t *b, tp1_t *res,
			   operacion_t operacion)
{
	if (!a || !b || !res)
		return NULL;

	int i = 0, j = 0;
	bool error = false;

	while (i < a->cantidad_pokemones && j < b->cantidad_pokemones &&
	       !error) {
		int ida = a->pokemones[i]->id;
		int idb = b->pokemones[j]->id;

		if (ida == idb) {
			if (operacion == UNION || operacion == INTERSECCION) {
				error = !cargar_clon(res, a->pokemones[i]);
			}
			i++;
			j++;
		} else if (ida < idb) {
			if (operacion == UNION || operacion == DIFERENCIA) {
				error = !cargar_clon(res, a->pokemones[i]);
			}
			i++;
		} else {
			if (operacion == UNION) {
				error = !cargar_clon(res, b->pokemones[j]);
			}
			j++;
		}
	}
	if (operacion == UNION || operacion == DIFERENCIA) {
		while (i < a->cantidad_pokemones && !error)
			error = !cargar_clon(res, a->pokemones[i++]);
	}

	if (operacion == UNION) {
		while (j < b->cantidad_pokemones && !error)
			error = !cargar_clon(res, b->pokemones[j++]);
	}

	if (error) {
		return NULL;
	}

	return res;
}

tp1_t *tp1_union(tp1_t *un_tp, tp1_t *otro_tp)
{
	if (!un_tp || !otro_tp)
		return NULL;

	tp1_t *tp_union = calloc(1, sizeof(tp1_t));
	if (!tp_union)
		return NULL;

	if (!operacion_conjuntos(un_tp, otro_tp, tp_union, UNION)) {
		tp1_destruir(tp_union);
		return NULL;
	}
	return tp_union;
}

tp1_t *tp1_interseccion(tp1_t *un_tp, tp1_t *otro_tp)
{
	if (!un_tp || !otro_tp)
		return NULL;

	tp1_t *tp_interseccion = calloc(1, sizeof(tp1_t));
	if (!tp_interseccion)
		return NULL;

	if (!operacion_conjuntos(un_tp, otro_tp, tp_interseccion,
				 INTERSECCION)) {
		tp1_destruir(tp_interseccion);
		return NULL;
	}
	return tp_interseccion;
}

tp1_t *tp1_diferencia(tp1_t *un_tp, tp1_t *otro_tp)
{
	if (!un_tp || !otro_tp)
		return NULL;

	tp1_t *tp_diferencia = calloc(1, sizeof(tp1_t));
	if (!tp_diferencia)
		return NULL;

	if (!operacion_conjuntos(un_tp, otro_tp, tp_diferencia, DIFERENCIA)) {
		tp1_destruir(tp_diferencia);
		return NULL;
	}
	return tp_diferencia;
}

struct pokemon *tp1_buscar_nombre(tp1_t *tp, const char *nombre)
{
	if (!tp || !nombre)
		return NULL;

	int i = 0;
	struct pokemon *pokemon = NULL;
	while (i < tp->cantidad_pokemones && !pokemon) {
		if (strcmp(nombre, tp->pokemones[i]->nombre) == 0) {
			pokemon = tp->pokemones[i];
		}
		i++;
	}
	return pokemon;
}

struct pokemon *tp1_buscar_id(tp1_t *tp, int id)
{
	if (!tp || id < 0)
		return NULL;
	size_t inicio = 0;
	size_t fin = tp->cantidad_pokemones;

	while (inicio < fin) {
		size_t medio = inicio + (fin - inicio) / 2;

		if (tp->pokemones[medio]->id == id) {
			return tp->pokemones[medio];
		} else if (tp->pokemones[medio]->id < id) {
			inicio = medio + 1;
		} else {
			fin = medio;
		}
	}
	return NULL;
}

size_t tp1_con_cada_pokemon(tp1_t *un_tp, bool (*f)(struct pokemon *, void *),
			    void *extra)
{
	if (!un_tp || !f)
		return 0;

	size_t indice = 0;
	bool iteracion_terminada = false;
	while (indice < un_tp->cantidad_pokemones && !iteracion_terminada) {
		if (!f(un_tp->pokemones[indice], extra)) {
			iteracion_terminada = true;
		}
		indice++;
	}
	return indice;
}

void tp1_destruir(tp1_t *tp1)
{
	if (!tp1)
		return;

	for (int i = 0; i < tp1->cantidad_pokemones; i++) {
		destruir_pokemon(tp1->pokemones[i]);
	}
	free(tp1->pokemones);
	free(tp1);
}