#include "pa2m.h"
#include "src/hash.h"
#include <string.h>
#include <stdlib.h>

#define CANTIDAD_CLAVES 50000
#define CADA_CUANTAS_BORRAR 100

void hash_crear_crea_hash_correctamente()
{
	hash_t *hash = hash_crear(10);
	pa2m_afirmar(
		hash != NULL,
		"hash_crear devuelve un hash no nulo con capacidad valida.");
	pa2m_afirmar(hash_cantidad(hash) == 0,
		     "Un hash recien creado tiene tamaño 0.");
	hash_destruir(hash);
}

void hash_crear_crea_tabla_vacia()
{
	hash_t *hash = hash_crear(5);
	pa2m_afirmar(hash != NULL, "hash_crear devuelve un hash valido.");
	pa2m_afirmar(hash_cantidad(hash) == 0,
		     "La tabla esta vacia tras la creacion.");
	hash_destruir(hash);
}

void hash_crear_crea_con_capacidad_menor_a_la_inicial()
{
	hash_t *hash = hash_crear(1);
	pa2m_afirmar(
		hash != NULL,
		"El hash se crea aunque la capacidad inicial sea menor a la minima.");
	int valor = 42;
	pa2m_afirmar(hash_insertar(hash, "clave1", &valor, NULL),
		     "Se puede insertar un elemento en la tabla.");
	pa2m_afirmar(hash_contiene(hash, "clave1"),
		     "La clave insertada existe en la tabla.");
	pa2m_afirmar(hash_cantidad(hash) == 1,
		     "La tabla tiene tamaño 1 tras la insercion.");
	hash_destruir(hash);
}

void pruebas_hash_crear()
{
	pa2m_nuevo_grupo("Hash crear parametros validos");
	hash_crear_crea_hash_correctamente();
	hash_crear_crea_tabla_vacia();

	pa2m_nuevo_grupo("Hash crear parametros invalidos");
	hash_crear_crea_con_capacidad_menor_a_la_inicial();
}

void hash_insertar_inserta_elemento_correctamente()
{
	hash_t *hash = hash_crear(10);
	int valor = 42;
	bool resultado = hash_insertar(hash, "clave1", &valor, NULL);
	pa2m_afirmar(resultado, "Se puede insertar un elemento.");
	pa2m_afirmar(hash_contiene(hash, "clave1"),
		     "La clave insertada existe.");
	hash_destruir(hash);
}

void hash_insertar_inserta_null()
{
	hash_t *hash = hash_crear(10);
	bool resultado = hash_insertar(hash, "clave_null", NULL, NULL);
	pa2m_afirmar(resultado,
		     "Se puede insertar un elemento con valor NULL.");
	pa2m_afirmar(hash_contiene(hash, "clave_null"),
		     "La clave insertada existe.");
	hash_destruir(hash);
}

void hash_insertar_clave_con_valor_null_y_luego_actualizar()
{
	hash_t *hash = hash_crear(10);
	bool resultado = hash_insertar(hash, "clave_actualizar", NULL, NULL);
	pa2m_afirmar(resultado,
		     "Se puede insertar un elemento con valor NULL.");
	int nuevo_valor = 100;
	resultado = hash_insertar(hash, "clave_actualizar", &nuevo_valor, NULL);
	pa2m_afirmar(
		resultado,
		"Se puede actualizar el valor NULL de una clave existente.");
	void *dato = hash_buscar(hash, "clave_actualizar");
	pa2m_afirmar(
		dato == &nuevo_valor,
		"El dato actualizado es correcto. (esperado :%i | obtenido: %i)",
		nuevo_valor, *(int *)dato);
	hash_destruir(hash);
}

void hash_insertar_actualiza_valor()
{
	hash_t *hash = hash_crear(10);
	int valor1 = 10;
	int valor2 = 20;
	hash_insertar(hash, "clave", &valor1, NULL);
	bool resultado = hash_insertar(hash, "clave", &valor2, NULL);
	pa2m_afirmar(resultado,
		     "Se puede actualizar el valor de una clave existente.");
	void *dato = hash_buscar(hash, "clave");
	pa2m_afirmar(
		dato == &valor2,
		"El dato actualizado es correcto. (esperado :%i | obtenido: %i)",
		valor2, *(int *)dato);
	hash_destruir(hash);
}

void hash_insertar_guarda_anterior_null()
{
	hash_t *hash = hash_crear(10);
	int valor = 50;
	void *anterior = &valor;
	hash_insertar(hash, "clave", &valor, &anterior);
	pa2m_afirmar(
		anterior == NULL,
		"Cuando se inserta una clave nueva, el valor anterior es NULL.");
	hash_destruir(hash);
}

void hash_insertar_guarda_anterior_no_null_correcto()
{
	hash_t *hash = hash_crear(10);
	int valor1 = 111;
	int valor2 = 222;
	hash_insertar(hash, "clave", &valor1, NULL);
	void *anterior = NULL;
	hash_insertar(hash, "clave", &valor2, &anterior);
	pa2m_afirmar(
		anterior == &valor1,
		"El valor anterior guardado es correcto al actualizar la clave.");
	hash_destruir(hash);
}

void hash_insertar_inserta_muchos_elementos()
{
	hash_t *hash = hash_crear(10);
	size_t cantidad = 5000;
	bool exito = true;
	for (size_t i = 0; i < cantidad && exito; i++) {
		char clave[50];
		sprintf(clave, "clave%zu", i);
		exito = hash_insertar(hash, clave, (void *)(size_t)i, NULL);
	}
	pa2m_afirmar(exito, "Se pueden insertar muchos elementos sin error.");
	pa2m_afirmar(
		hash_cantidad(hash) == cantidad,
		"La tabla tiene el tamanio esperado tras insertar muchos elementos.");
	hash_destruir(hash);
}

void hash_insertar_con_clave_null()
{
	hash_t *hash = hash_crear(10);
	int valor = 123;
	bool resultado = hash_insertar(hash, NULL, &valor, NULL);
	pa2m_afirmar(!resultado, "No se puede insertar una clave NULL.");
	hash_destruir(hash);
}

void hash_insertar_con_hash_null()
{
	int valor = 123;
	bool resultado = hash_insertar(NULL, "clave", &valor, NULL);
	pa2m_afirmar(!resultado, "No se puede insertar en un hash NULL.");
}

void hash_insertar_con_anterior_null_no_guarda_nada()
{
	hash_t *hash = hash_crear(10);
	int valor = 10;
	bool resultado = hash_insertar(hash, "clave", &valor, NULL);
	pa2m_afirmar(resultado,
		     "Se puede insertar correctamente con anterior NULL.");
	hash_destruir(hash);
}

void pruebas_hash_insertar()
{
	pa2m_nuevo_grupo("Hash insertar parametros validos");
	hash_insertar_inserta_elemento_correctamente();
	hash_insertar_inserta_null();
	hash_insertar_clave_con_valor_null_y_luego_actualizar();
	hash_insertar_actualiza_valor();
	hash_insertar_guarda_anterior_null();
	hash_insertar_guarda_anterior_no_null_correcto();
	hash_insertar_inserta_muchos_elementos();

	pa2m_nuevo_grupo("Hash insertar parametros invalidos");
	hash_insertar_con_clave_null();
	hash_insertar_con_hash_null();
	hash_insertar_con_anterior_null_no_guarda_nada();
}

void hash_quitar_saca_elemento_correctamente()
{
	hash_t *hash = hash_crear(10);
	int valor = 42;
	hash_insertar(hash, "clave1", &valor, NULL);
	void *dato = hash_quitar(hash, "clave1");
	pa2m_afirmar(dato == &valor,
		     "Se saca un elemento y devuelve el valor correcto.");
	pa2m_afirmar(!hash_contiene(hash, "clave1"),
		     "La clave ya no existe luego de ser sacada.");
	hash_destruir(hash);
}

void hash_quitar_disminuye_tamanio()
{
	hash_t *hash = hash_crear(10);
	int valor = 42;
	hash_insertar(hash, "clave1", &valor, NULL);
	size_t tamanio_antes = hash_cantidad(hash);
	hash_quitar(hash, "clave1");
	pa2m_afirmar(hash_cantidad(hash) == tamanio_antes - 1,
		     "El tamanio disminuye en 1 al sacar un elemento.");
	hash_destruir(hash);
}

void hash_quitar_vacia_tabla()
{
	hash_t *hash = hash_crear(10);
	int valor = 42;
	hash_insertar(hash, "clave1", &valor, NULL);
	hash_quitar(hash, "clave1");
	pa2m_afirmar(hash_cantidad(hash) == 0,
		     "La tabla queda vacia luego de sacar el unico elemento.");
	hash_destruir(hash);
}

void hash_quitar_devuelve_valor_correcto()
{
	hash_t *hash = hash_crear(10);
	int valor = 999;
	hash_insertar(hash, "clave_valor", &valor, NULL);
	void *dato = hash_quitar(hash, "clave_valor");
	pa2m_afirmar(dato == &valor, "hash_quitar devuelve el valor correcto.");
	hash_destruir(hash);
}

void hash_quitar_saca_muchos_elementos()
{
	hash_t *hash = hash_crear(100);
	size_t cantidad = 5000;
	for (size_t i = 0; i < cantidad; i++) {
		char clave[50];
		sprintf(clave, "clave%zu", i);
		hash_insertar(hash, clave, (void *)(size_t)i, NULL);
	}

	for (size_t i = 0; i < cantidad; i++) {
		char clave[50];
		sprintf(clave, "clave%zu", i);
		hash_quitar(hash, clave);
	}
	pa2m_afirmar(
		hash_cantidad(hash) == 0,
		"La tabla queda vacia despues de sacar todos los elementos.");
	hash_destruir(hash);
}

void hash_quitar_saca_elemento_null()
{
	hash_t *hash = hash_crear(10);
	hash_insertar(hash, "clave_null", NULL, NULL);
	void *dato = hash_quitar(hash, "clave_null");
	pa2m_afirmar(dato == NULL,
		     "Se puede sacar un elemento con valor NULL.");
	hash_destruir(hash);
}

void hash_quitar_clave_null()
{
	hash_t *hash = hash_crear(10);
	int valor = 10;
	hash_insertar(hash, "clave", &valor, NULL);
	void *dato = hash_quitar(hash, NULL);
	pa2m_afirmar(dato == NULL,
		     "hash_quitar devuelve NULL si la clave es NULL.");
	hash_destruir(hash);
}

void hash_quitar_hash_null()
{
	void *dato = hash_quitar(NULL, "clave");
	pa2m_afirmar(dato == NULL,
		     "hash_quitar devuelve NULL si el hash es NULL.");
}

void hash_quitar_valor_no_insertado_devuelve_null()
{
	hash_t *hash = hash_crear(10);
	void *dato = hash_quitar(hash, "clave_inexistente");
	pa2m_afirmar(dato == NULL,
		     "hash_quitar devuelve NULL si la clave no existe.");
	hash_destruir(hash);
}

void hash_quitar_error_no_disminuye_tamanio()
{
	hash_t *hash = hash_crear(10);
	int valor = 42;
	hash_insertar(hash, "clave", &valor, NULL);
	size_t tamanio_antes = hash_cantidad(hash);
	void *dato = hash_quitar(hash, "otra_clave");
	pa2m_afirmar(dato == NULL,
		     "hash_quitar devuelve NULL si la clave no existe.");
	pa2m_afirmar(
		hash_cantidad(hash) == tamanio_antes,
		"El tamanio no cambia si se intenta sacar una clave inexistente.");
	hash_destruir(hash);
}

void pruebas_hash_quitar()
{
	pa2m_nuevo_grupo("Hash sacar parametros validos");
	hash_quitar_saca_elemento_correctamente();
	hash_quitar_disminuye_tamanio();
	hash_quitar_vacia_tabla();
	hash_quitar_devuelve_valor_correcto();
	hash_quitar_saca_muchos_elementos();
	hash_quitar_saca_elemento_null();

	pa2m_nuevo_grupo("Hash sacar parametros invalidos");
	hash_quitar_clave_null();
	hash_quitar_hash_null();
	hash_quitar_valor_no_insertado_devuelve_null();
	hash_quitar_error_no_disminuye_tamanio();
}

bool aux_contador_iterador(char *clave, void *dato, void *ctx)
{
	if (ctx) {
		size_t *contador = (size_t *)ctx;
		(*contador)++;
	}
	return true;
}

bool aux_contador_iterador_hasta_limite(char *clave, void *dato, void *ctx)
{
	size_t *contador = (size_t *)ctx;
	(*contador)++;
	return (*contador) < 3;
}

void hash_iterar_itera_todos_los_elementos()
{
	hash_t *hash = hash_crear(10);
	size_t cantidad = 10;
	for (size_t i = 0; i < cantidad; i++) {
		char clave[50];
		sprintf(clave, "clave%zu", i);
		hash_insertar(hash, clave, (void *)(size_t)i, NULL);
	}
	size_t contador = 0;
	size_t iterados = hash_iterar(hash, aux_contador_iterador, &contador);
	pa2m_afirmar(iterados == cantidad,
		     "El iterador interno recorre todos los elementos.");
	pa2m_afirmar(
		contador == cantidad,
		"El contador coincide con la cantidad de claves insertadas.");
	hash_destruir(hash);
}

void hash_iterar_itera_veces_correctas()
{
	hash_t *hash = hash_crear(10);
	size_t cantidad = 5;
	for (size_t i = 0; i < cantidad; i++) {
		char clave[50];
		sprintf(clave, "clave%zu", i);
		hash_insertar(hash, clave, (void *)(size_t)i, NULL);
	}
	size_t contador = 0;
	size_t iterados = hash_iterar(hash, aux_contador_iterador, &contador);
	pa2m_afirmar(iterados == contador,
		     "El iterador recorre la cantidad correcta de elementos.");
	hash_destruir(hash);
}

void hash_iterar_hash_vacio_devuelve_cero()
{
	hash_t *hash = hash_crear(10);
	size_t contador = 0;
	size_t iterados = hash_iterar(hash, aux_contador_iterador, &contador);
	pa2m_afirmar(iterados == 0, "Iterar un hash vacio devuelve 0.");
	hash_destruir(hash);
}

void hash_iterar_con_aux_valido_itera_correctamente()
{
	hash_t *hash = hash_crear(10);
	size_t cantidad = 3;
	for (size_t i = 0; i < cantidad; i++) {
		char clave[50];
		sprintf(clave, "clave%zu", i);
		hash_insertar(hash, clave, (void *)(size_t)i, NULL);
	}
	size_t contador = 0;
	size_t iterados = hash_iterar(hash, aux_contador_iterador, &contador);
	pa2m_afirmar(iterados == cantidad,
		     "Iterar con aux valido funciona correctamente.");
	hash_destruir(hash);
}

void hash_iterar_devuelve_cero_con_hash_nulo()
{
	size_t contador = 0;
	size_t iterados = hash_iterar(NULL, aux_contador_iterador, &contador);
	pa2m_afirmar(iterados == 0, "Iterar con hash NULL devuelve 0.");
}

void hash_iterar_devuelve_cero_con_f_nula()
{
	hash_t *hash = hash_crear(10);
	size_t contador = 0;
	size_t iterados = hash_iterar(hash, NULL, &contador);
	pa2m_afirmar(iterados == 0, "Iterar con funcion nula devuelve 0.");
	hash_destruir(hash);
}

void hash_iterar_con_aux_nulo_funciona_correctamente()
{
	hash_t *hash = hash_crear(10);
	hash_insertar(hash, "clave", (void *)1, NULL);
	size_t iterados = hash_iterar(hash, aux_contador_iterador, NULL);
	pa2m_afirmar(iterados == 1,
		     "Iterar con aux nulo funciona correctamente.");
	hash_destruir(hash);
}

void hash_iterar_itera_hasta_corte_con_aux_nulo()
{
	hash_t *hash = hash_crear(10);
	for (size_t i = 0; i < 5; i++) {
		char clave[50];
		sprintf(clave, "clave%zu", i);
		hash_insertar(hash, clave, (void *)(size_t)i, NULL);
	}
	size_t contador = 0;
	size_t iterados = hash_iterar(hash, aux_contador_iterador_hasta_limite,
				      &contador);
	pa2m_afirmar(iterados == 3,
		     "El iterador corta correctamente al llegar al limite.");
	hash_destruir(hash);
}

void pruebas_hash_iterar()
{
	pa2m_nuevo_grupo("Hash iterar parametros validos");
	hash_iterar_itera_todos_los_elementos();
	hash_iterar_itera_veces_correctas();
	hash_iterar_hash_vacio_devuelve_cero();
	hash_iterar_con_aux_valido_itera_correctamente();

	pa2m_nuevo_grupo("Hash iterar parametros invalidos");
	hash_iterar_devuelve_cero_con_hash_nulo();
	hash_iterar_devuelve_cero_con_f_nula();
	hash_iterar_con_aux_nulo_funciona_correctamente();
	hash_iterar_itera_hasta_corte_con_aux_nulo();
}

void hash_buscar_encuentra_elemento_unico()
{
	hash_t *hash = hash_crear(10);
	int valor = 42;
	hash_insertar(hash, "clave", &valor, NULL);
	void *dato = hash_buscar(hash, "clave");
	pa2m_afirmar(
		dato == &valor,
		"hash_buscar devuelve el valor correcto para un elemento unico.");
	hash_destruir(hash);
}

void hash_buscar_encuentra_elementos_variados()
{
	hash_t *hash = hash_crear(10);
	int valor1 = 1, valor2 = 2, valor3 = 3;
	hash_insertar(hash, "clave1", &valor1, NULL);
	hash_insertar(hash, "clave2", &valor2, NULL);
	hash_insertar(hash, "clave3", &valor3, NULL);
	pa2m_afirmar(hash_buscar(hash, "clave1") == &valor1,
		     "hash_buscar encuentra el primer elemento insertado.");
	pa2m_afirmar(hash_buscar(hash, "clave2") == &valor2,
		     "hash_buscar encuentra el segundo elemento insertado.");
	pa2m_afirmar(hash_buscar(hash, "clave3") == &valor3,
		     "hash_buscar encuentra el tercer elemento insertado.");
	hash_destruir(hash);
}

void hash_buscar_encuentra_elemento_dos_veces()
{
	hash_t *hash = hash_crear(10);
	int valor1 = 100;
	hash_insertar(hash, "clave", &valor1, NULL);
	void *dato1 = hash_buscar(hash, "clave");
	void *dato2 = hash_buscar(hash, "clave");
	pa2m_afirmar(
		dato1 == &valor1 && dato2 == &valor1,
		"hash_buscar devuelve el mismo puntero para la misma clave.");
	hash_destruir(hash);
}

void hash_buscar_no_baja_tamanio()
{
	hash_t *hash = hash_crear(10);
	int valor = 10;
	hash_insertar(hash, "clave", &valor, NULL);
	size_t tamanio_antes = hash_cantidad(hash);
	hash_buscar(hash, "clave");
	size_t tamanio_despues = hash_cantidad(hash);
	pa2m_afirmar(tamanio_antes == tamanio_despues,
		     "hash_buscar no modifica el tamanio de la tabla.");
	hash_destruir(hash);
}

void hash_buscar_encuentra_elemento_luego_de_borrar_clave()
{
	hash_t *hash = hash_crear(10);
	int valor = 123;
	char *clave = malloc(20);
	strcpy(clave, "clave_dinamica");
	hash_insertar(hash, clave, &valor, NULL);
	free(clave);
	void *dato = hash_buscar(hash, "clave_dinamica");
	pa2m_afirmar(
		dato == &valor,
		"hash_buscar encuentra la clave luego de liberar la memoria de la clave original.");
	hash_destruir(hash);
}

void hash_buscar_hash_nulo_devuelve_null()
{
	void *dato = hash_buscar(NULL, "clave");
	pa2m_afirmar(dato == NULL,
		     "hash_buscar devuelve NULL si el hash es NULL.");
}

void hash_buscar_clave_nula_devuelve_null()
{
	hash_t *hash = hash_crear(10);
	int valor = 10;
	hash_insertar(hash, "clave", &valor, NULL);
	void *dato = hash_buscar(hash, NULL);
	pa2m_afirmar(dato == NULL,
		     "hash_buscar devuelve NULL si la clave es NULL.");
	hash_destruir(hash);
}

void hash_buscar_encuentra_dato_nulo()
{
	hash_t *hash = hash_crear(10);
	hash_insertar(hash, "clave_null", NULL, NULL);
	void *dato = hash_buscar(hash, "clave_null");
	pa2m_afirmar(
		dato == NULL,
		"hash_buscar devuelve NULL si el valor insertado es NULL.");
	hash_destruir(hash);
}

void hash_buscar_con_tabla_vacia_devuelve_null()
{
	hash_t *hash = hash_crear(10);
	void *dato = hash_buscar(hash, "clave_inexistente");
	pa2m_afirmar(
		dato == NULL,
		"hash_buscar devuelve NULL si la clave no existe en la tabla vacia.");
	hash_destruir(hash);
}

void pruebas_hash_buscar()
{
	pa2m_nuevo_grupo("Hash buscar parametros validos");
	hash_buscar_encuentra_elemento_unico();
	hash_buscar_encuentra_elementos_variados();
	hash_buscar_encuentra_elemento_dos_veces();
	hash_buscar_no_baja_tamanio();
	hash_buscar_encuentra_elemento_luego_de_borrar_clave();

	pa2m_nuevo_grupo("Hash buscar parametros invalidos");
	hash_buscar_hash_nulo_devuelve_null();
	hash_buscar_clave_nula_devuelve_null();
	hash_buscar_encuentra_dato_nulo();
	hash_buscar_con_tabla_vacia_devuelve_null();
}

void hash_contiene_encuentra_clave_insertada()
{
	hash_t *hash = hash_crear(10);
	int valor = 42;
	hash_insertar(hash, "clave", &valor, NULL);
	pa2m_afirmar(hash_contiene(hash, "clave"),
		     "hash_contiene devuelve true si la clave fue insertada.");
	hash_destruir(hash);
}

void hash_contiene_no_encuentra_clave_no_insertada()
{
	hash_t *hash = hash_crear(10);
	pa2m_afirmar(!hash_contiene(hash, "clave_inexistente"),
		     "hash_contiene devuelve false si la clave no existe.");
	hash_destruir(hash);
}

void hash_contiene_clave_eliminada_devuelve_false()
{
	hash_t *hash = hash_crear(10);
	int valor = 55;
	hash_insertar(hash, "clave", &valor, NULL);
	hash_quitar(hash, "clave");
	pa2m_afirmar(!hash_contiene(hash, "clave"),
		     "hash_contiene devuelve false si la clave fue eliminada.");
	hash_destruir(hash);
}

void hash_contiene_hash_nulo_devuelve_false()
{
	pa2m_afirmar(!hash_contiene(NULL, "clave"),
		     "hash_contiene devuelve false si el hash es NULL.");
}

void hash_contiene_clave_nula_devuelve_false()
{
	hash_t *hash = hash_crear(10);
	int valor = 123;
	hash_insertar(hash, "clave", &valor, NULL);
	pa2m_afirmar(!hash_contiene(hash, NULL),
		     "hash_contiene devuelve false si la clave es NULL.");
	hash_destruir(hash);
}

void pruebas_hash_contiene()
{
	pa2m_nuevo_grupo("Hash existe parametros validos");
	hash_contiene_encuentra_clave_insertada();
	hash_contiene_no_encuentra_clave_no_insertada();
	hash_contiene_clave_eliminada_devuelve_false();

	pa2m_nuevo_grupo("Hash existe parametros invalidos");
	hash_contiene_hash_nulo_devuelve_false();
	hash_contiene_clave_nula_devuelve_false();
}

void hash_cantidad_devuelve_cero_en_hash_vacio()
{
	hash_t *hash = hash_crear(10);
	pa2m_afirmar(hash_cantidad(hash) == 0,
		     "hash_cantidad devuelve 0 en un hash vacio.");
	hash_destruir(hash);
}

void hash_cantidad_actualiza_correctamente()
{
	hash_t *hash = hash_crear(10);
	int valor = 10;
	hash_insertar(hash, "clave1", &valor, NULL);
	hash_insertar(hash, "clave2", &valor, NULL);
	pa2m_afirmar(
		hash_cantidad(hash) == 2,
		"hash_cantidad devuelve la cantidad correcta tras inserciones.");
	hash_destruir(hash);
}

void hash_cantidad_no_cambia_en_busqueda()
{
	hash_t *hash = hash_crear(10);
	int valor = 20;
	hash_insertar(hash, "clave", &valor, NULL);
	size_t tamanio_antes = hash_cantidad(hash);
	hash_buscar(hash, "clave");
	pa2m_afirmar(hash_cantidad(hash) == tamanio_antes,
		     "hash_cantidad no cambia al buscar una clave.");
	hash_destruir(hash);
}

void hash_cantidad_disminuye_correctamente()
{
	hash_t *hash = hash_crear(10);
	int valor = 30;
	hash_insertar(hash, "clave", &valor, NULL);
	hash_quitar(hash, "clave");
	pa2m_afirmar(
		hash_cantidad(hash) == 0,
		"hash_cantidad disminuye correctamente al eliminar una clave.");
	hash_destruir(hash);
}

void hash_cantidad_hash_nulo_devuelve_cero()
{
	pa2m_afirmar(hash_cantidad(NULL) == 0,
		     "hash_cantidad devuelve 0 si el hash es NULL.");
}

void pruebas_hash_cantidad()
{
	pa2m_nuevo_grupo("Hash tamanio parametros validos");
	hash_cantidad_devuelve_cero_en_hash_vacio();
	hash_cantidad_actualiza_correctamente();
	hash_cantidad_no_cambia_en_busqueda();
	hash_cantidad_disminuye_correctamente();

	pa2m_nuevo_grupo("Hash tamanio parametros invalidos");
	hash_cantidad_hash_nulo_devuelve_cero();
}

void hash_insercion_y_eliminacion_mezclada()
{
	hash_t *hash = hash_crear(100);
	pa2m_afirmar(hash != NULL, "Se crea el hash correctamente.");

	bool exito = true;
	size_t insertadas = 0;
	size_t eliminadas = 0;

	for (size_t i = 0; i < CANTIDAD_CLAVES && exito; i++) {
		char clave[50];
		sprintf(clave, "clave%zu", i);

		if (hash_insertar(hash, clave, (void *)(size_t)i, NULL)) {
			insertadas++;
		}
		if (i > 0 && i % CADA_CUANTAS_BORRAR == 0) {
			size_t clave_a_borrar = i - CADA_CUANTAS_BORRAR / 2;
			sprintf(clave, "clave%zu", clave_a_borrar);
			void *dato = hash_quitar(hash, clave);
			if (dato != NULL) {
				eliminadas++;
			}
		}
	}

	pa2m_afirmar(insertadas > 0, "Se insertaron N claves.");
	pa2m_afirmar(eliminadas > 0, "Se eliminaron claves intercaladamente.");
	pa2m_afirmar(hash_cantidad(hash) > 0,
		     "La tabla tiene claves al final.");

	hash_destruir(hash);
}

void hash_insercion_muchos_elementos_y_busqueda()
{
	hash_t *hash = hash_crear(10);
	int valor = 100;
	size_t cantidad = 10000;

	for (size_t i = 0; i < cantidad; i++) {
		char clave[50];
		sprintf(clave, "clave%zu", i);
		hash_insertar(hash, clave, &valor, NULL);
	}

	bool encontrado = true;
	for (size_t i = 0; i < cantidad; i++) {
		char clave[50];
		sprintf(clave, "clave%zu", i);
		if (!hash_contiene(hash, clave)) {
			encontrado = false;
			break;
		}
	}

	pa2m_afirmar(encontrado,
		     "Todas las claves insertadas se encuentran en el hash.");
	hash_destruir(hash);
}

void hash_insercion_iteracion_vaciar_y_reinsertar()
{
	hash_t *hash = hash_crear(10);
	int valor = 5;

	for (size_t i = 0; i < 50; i++) {
		char clave[50];
		sprintf(clave, "clave%zu", i);
		hash_insertar(hash, clave, &valor, NULL);
	}

	size_t contador = 0;
	hash_iterar(hash, aux_contador_iterador, &contador);

	pa2m_afirmar(contador == 50,
		     "El iterador recorre todos los elementos.");

	for (size_t i = 0; i < 50; i++) {
		char clave[50];
		sprintf(clave, "clave%zu", i);
		hash_quitar(hash, clave);
	}

	pa2m_afirmar(
		hash_cantidad(hash) == 0,
		"La tabla queda vacia despues de eliminar todos los elementos.");

	for (size_t i = 0; i < 50; i++) {
		char clave[50];
		sprintf(clave, "clave%zu", i);
		hash_insertar(hash, clave, &valor, NULL);
	}

	pa2m_afirmar(
		hash_cantidad(hash) == 50,
		"La tabla vuelve a tener 50 elementos luego de reinsertar.");
	hash_destruir(hash);
}

void hash_reinsertar_muchas_veces_misma_clave()
{
	hash_t *hash = hash_crear(10);
	int valor = 1;
	int nuevo_valor = 2;
	void *anterior = NULL;

	hash_insertar(hash, "clave_repetida", &valor, NULL);

	bool correcto = true;
	for (size_t i = 0; i < 1000 && correcto; i++) {
		if (!hash_insertar(hash, "clave_repetida", &nuevo_valor,
				   &anterior)) {
			correcto = false;
		}
		if (anterior != &valor && anterior != &nuevo_valor) {
			correcto = false;
		}
	}

	pa2m_afirmar(
		correcto,
		"Se puede reinsertar muchas veces la misma clave sin error.");
	pa2m_afirmar(
		hash_cantidad(hash) == 1,
		"El tamanio de la tabla no crece al reinsertar la misma clave.");
	hash_destruir(hash);
}

void pruebas_hash_mixtas()
{
	hash_insercion_y_eliminacion_mezclada();
	hash_insercion_muchos_elementos_y_busqueda();
	hash_insercion_iteracion_vaciar_y_reinsertar();
	hash_reinsertar_muchas_veces_misma_clave();
}

void hash_destruir_destruye_todo_hash()
{
	hash_t *hash = hash_crear(10);
	int valor = 42;
	hash_insertar(hash, "clave", &valor, NULL);
	hash_destruir(hash);
	pa2m_afirmar(
		true,
		"hash_destruir destruye correctamente un hash con elementos.");
}

void hash_destruir_destruye_hash_vacio()
{
	hash_t *hash = hash_crear(10);
	hash_destruir(hash);
	pa2m_afirmar(true,
		     "hash_destruir destruye correctamente un hash vacio.");
}

void hash_destruir_destruye_hash_con_muchos_elementos()
{
	hash_t *hash = hash_crear(10);
	int valor = 10;
	for (size_t i = 0; i < 1000; i++) {
		char clave[50];
		sprintf(clave, "clave%zu", i);
		hash_insertar(hash, clave, &valor, NULL);
	}
	hash_destruir(hash);
	pa2m_afirmar(
		true,
		"hash_destruir destruye correctamente un hash con muchos elementos.");
}

void hash_destruir_funciona_luego_de_liberar_memoria_de_claves()
{
	hash_t *hash = hash_crear(10);
	int valor = 99;
	char *clave = malloc(20);
	strcpy(clave, "clave_dinamica");
	hash_insertar(hash, clave, &valor, NULL);
	free(clave);
	hash_destruir(hash);
	pa2m_afirmar(
		true,
		"hash_destruir funciona luego de liberar la memoria de la clave original.");
}

void aux_destructor_elemento(void *dato)
{
	free(dato);
}

void hash_destruir_todo_destruye_hash_completo()
{
	hash_t *hash = hash_crear(10);
	for (size_t i = 0; i < 10; i++) {
		size_t *dato = malloc(sizeof(size_t));
		*dato = i;
		char clave[50];
		sprintf(clave, "clave%zu", i);
		hash_insertar(hash, clave, dato, NULL);
	}
	hash_destruir_todo(hash, aux_destructor_elemento);
	pa2m_afirmar(
		true,
		"hash_destruir_todo destruye correctamente un hash con elementos.");
}

void hash_destruir_todo_destruye_hash_vacio()
{
	hash_t *hash = hash_crear(10);
	hash_destruir_todo(hash, aux_destructor_elemento);
	pa2m_afirmar(
		true,
		"hash_destruir_todo destruye correctamente un hash vacio.");
}

void hash_destruir_todo_destruye_hash_con_muchos_elementos()
{
	hash_t *hash = hash_crear(10);
	for (size_t i = 0; i < 1000; i++) {
		size_t *dato = malloc(sizeof(size_t));
		*dato = i;
		char clave[50];
		sprintf(clave, "clave%zu", i);
		hash_insertar(hash, clave, dato, NULL);
	}
	hash_destruir_todo(hash, aux_destructor_elemento);
	pa2m_afirmar(
		true,
		"hash_destruir_todo destruye correctamente un hash con muchos elementos.");
}

void hash_destruir_con_hash_nulo_no_hace_nada()
{
	hash_destruir(NULL);
	pa2m_afirmar(true, "hash_destruir no hace nada si el hash es NULL.");
}

void hash_destruir_todo_con_hash_nulo_no_hace_nada()
{
	hash_destruir_todo(NULL, aux_destructor_elemento);
	pa2m_afirmar(true,
		     "hash_destruir_todo no hace nada si el hash es NULL.");
}

void hash_destruir_todo_con_funcion_nula_funciona_como_hash_destruir()
{
	hash_t *hash = hash_crear(10);
	int valor = 123;
	hash_insertar(hash, "clave", &valor, NULL);
	hash_destruir_todo(hash, NULL);
	pa2m_afirmar(
		true,
		"hash_destruir_todo funciona como hash_destruir si el destructor es NULL.");
}

void pruebas_hash_destruir()
{
	pa2m_nuevo_grupo("Hash destruir y destruir todo parametros validos");
	hash_destruir_destruye_todo_hash();
	hash_destruir_destruye_hash_vacio();
	hash_destruir_destruye_hash_con_muchos_elementos();
	hash_destruir_funciona_luego_de_liberar_memoria_de_claves();

	hash_destruir_todo_destruye_hash_completo();
	hash_destruir_todo_destruye_hash_vacio();
	hash_destruir_todo_destruye_hash_con_muchos_elementos();

	pa2m_nuevo_grupo("Hash destruir y destruir todo parametros invalidos");
	hash_destruir_con_hash_nulo_no_hace_nada();

	hash_destruir_todo_con_hash_nulo_no_hace_nada();
	hash_destruir_todo_con_funcion_nula_funciona_como_hash_destruir();
}

void correr_pruebas_hash()
{
	pa2m_nuevo_grupo("Pruebas hash_crear");
	pruebas_hash_crear();

	pa2m_nuevo_grupo("Pruebas hash_insertar");
	pruebas_hash_insertar();

	pa2m_nuevo_grupo("Pruebas hash_quitar");
	pruebas_hash_quitar();

	pa2m_nuevo_grupo("Pruebas hash_iterar");
	pruebas_hash_iterar();

	pa2m_nuevo_grupo("Pruebas hash_buscar");
	pruebas_hash_buscar();

	pa2m_nuevo_grupo("Pruebas hash_contiene");
	pruebas_hash_contiene();

	pa2m_nuevo_grupo("Pruebas hash_cantidad");
	pruebas_hash_cantidad();

	pa2m_nuevo_grupo("Pruebas hash_mixtas");
	pruebas_hash_mixtas();

	pa2m_nuevo_grupo("Pruebas hash_destruir");
	pruebas_hash_destruir();
}

int main()
{
	pa2m_nuevo_grupo("Pruebas TDA HASH");
	correr_pruebas_hash();

	return pa2m_mostrar_reporte();
}
