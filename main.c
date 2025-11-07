#include "src/hash.h"
#include "src/tp1.h"
#include "src/functions.h"

#include <stdio.h>
#include "string.h"

struct ctx_busqueda {
	int id;
	const char *nombre;
	struct pokemon *resultado;
};

void imprimir_pokemon(const struct pokemon *pokemon)
{
	printf("%s(%i) - Tipo:%s A:%u D:%u V:%u\n", pokemon->nombre,
	       pokemon->id, calcular_tipo((struct pokemon *)pokemon),
	       pokemon->ataque, pokemon->defensa, pokemon->velocidad);
}

bool buscar_por_nombre_cb(void *dato, void *extra)
{
	struct pokemon *p = dato;
	struct ctx_busqueda *ctx = extra;

	if (strcmp(p->nombre, ctx->nombre) == 0) {
		ctx->resultado = p;
		return false;
	}
	return true;
}

bool hash_buscar_por_id(char *clave, void *_p, void *_aux)
{
	struct pokemon *p = _p;
	struct ctx_busqueda *ctx = _aux;

	if (ctx->id == p->id) {
		ctx->resultado = p;
		return false;
	}

	return true;
}

struct pokemon *buscar_por_id(hash_t *hash, int id)
{
	struct ctx_busqueda ctx = { .id = id, .resultado = NULL };
	hash_iterar(hash, hash_buscar_por_id, &ctx);

	return ctx.resultado;
}

struct pokemon *buscar_por_nombre(hash_t *hash, char *nombre)
{
	return hash_buscar(hash, nombre);
}

bool agregar_a_hash(struct pokemon *p, void *ctx)
{
	hash_t *hash = ctx;
	return hash_insertar(hash, p->nombre, p, NULL);
}

bool validar_argumentos(int argc, char *argv[])
{
	const char *op = argv[2];
	int esperado = 0;

	if (strcmp(op, "buscar") == 0)
		esperado = 5;
	else {
		printf("Error: Operacion '%s' no reconocida.\n", op);
		return false;
	}

	if (argc != esperado) {
		printf("Error: cantidad de argumentos invalida para '%s'.\n",
		       op);
		return false;
	}

	return true;
}

void hash_buscar_pokemon(int argc, char *argv[], hash_t *hash)
{
	const struct pokemon *p = NULL;
	if (strcmp(argv[3], "id") == 0)
		p = buscar_por_id(hash, atoi(argv[4]));
	else if (strcmp(argv[3], "nombre") == 0)
		p = buscar_por_nombre(hash, argv[4]);

	if (!p) {
		printf("Pokemon con %s %s no encontrado.\n", argv[3], argv[4]);
		return;
	}

	imprimir_pokemon(p);
	return;
}

int cmp_pokemon_id(const void *a, const void *b)
{
	const struct pokemon *pa = a;
	const struct pokemon *pb = b;

	return pa->id - pb->id;
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("Uso:\n  %s <archivo.csv> buscar <id|nombre> <valor>\n",
		       argv[0]);
		return -1;
	}

	if (!validar_argumentos(argc, argv))
		return -1;

	tp1_t *tp = tp1_leer_archivo(argv[1]);
	if (!tp) {
		fprintf(stderr, "Error al leer archivo %s\n", argv[1]);
		return 1;
	}

	hash_t *hash = hash_crear(10);
	if (!hash) {
		tp1_destruir(tp);
		return 1;
	}

	tp1_con_cada_pokemon(tp, agregar_a_hash, hash);

	hash_buscar_pokemon(argc, argv, hash);

	hash_destruir(hash);
	tp1_destruir(tp);

	return 0;
}