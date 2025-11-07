#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "archivo.h"

const int TAM_LINEA_INICIAL = 0;
const int CANT_LINEAS_INICIAL = 0;

struct archivo {
	FILE *apertura;
	char *linea_leida;
	int tam_linea;
	int cant_lineas;
	int final_archivo;
};

archivo_t *archivo_abrir(const char *nombre, const char *modo)
{
	if (nombre == NULL)
		return NULL;
	archivo_t *archivo_abierto = malloc(sizeof(archivo_t));
	if (archivo_abierto == NULL)
		return NULL;
	archivo_abierto->apertura = fopen(nombre, modo);
	if (archivo_abierto->apertura == NULL) {
		free(archivo_abierto);
		return NULL;
	}
	archivo_abierto->linea_leida = NULL;
	archivo_abierto->tam_linea = TAM_LINEA_INICIAL;
	archivo_abierto->final_archivo = 0;
	return archivo_abierto;
}

char *guardar_memoria_linea(archivo_t *archivo, int tamanio)
{
	char *linea_temporal;
	if (archivo->linea_leida == NULL) {
		linea_temporal = malloc((size_t)tamanio);
		if (linea_temporal == NULL)
			return NULL;
	} else {
		linea_temporal = realloc(archivo->linea_leida, (size_t)tamanio);
		if (linea_temporal == NULL)
			return NULL;
	}
	return linea_temporal;
}

void quitar_salto_de_linea(archivo_t *archivo, int tamanio_linea)
{
	if (tamanio_linea > 0 &&
	    archivo->linea_leida[tamanio_linea - 1] == '\n') {
		archivo->linea_leida[tamanio_linea - 1] = '\0';
		tamanio_linea--;
	}
}

int leer_linea_hasta_final_de_linea(archivo_t *archivo, int *fin_de_linea,
				    int *tam_linea)
{
	if (*tam_linea > 0 && archivo->linea_leida[*(tam_linea)-1] == '\n')
		(*fin_de_linea) = 1;
	else {
		archivo->tam_linea *= 2;
		char *temp = guardar_memoria_linea(archivo, archivo->tam_linea);
		if (temp == NULL) {
			free(archivo->linea_leida);
			archivo->linea_leida = NULL;
			return 1;
		}
		archivo->linea_leida = temp;
		if (fgets(archivo->linea_leida + *(tam_linea),
			  archivo->tam_linea - *(tam_linea),
			  archivo->apertura) == NULL) {
			(*fin_de_linea) = 1;
		} else
			*tam_linea = (int)strlen(archivo->linea_leida);
	}
	return 0;
}

const char *archivo_leer_linea(archivo_t *archivo)
{
	if (archivo == NULL || archivo->final_archivo)
		return NULL;
	archivo->tam_linea = 10;
	archivo->linea_leida =
		guardar_memoria_linea(archivo, archivo->tam_linea);
	if (archivo->linea_leida == NULL)
		return NULL;

	if (fgets(archivo->linea_leida, archivo->tam_linea,
		  archivo->apertura) == NULL) {
		archivo->linea_leida[0] = '\0';
		archivo->final_archivo = 1;
	}

	int tam_linea_leida = (int)strlen(archivo->linea_leida);
	int fin_de_linea = 0;

	while (!fin_de_linea) {
		int lectura_correcta = leer_linea_hasta_final_de_linea(
			archivo, &fin_de_linea, &tam_linea_leida);
		if (lectura_correcta == 1)
			return NULL;
	}

	if (archivo->linea_leida != NULL) {
		quitar_salto_de_linea(archivo, tam_linea_leida);
	}
	return archivo->linea_leida;
}

bool archivo_escribir_linea(archivo_t *archivo, const char *linea)
{
	if (!archivo || !linea)
		return NULL;

	int estado = fputs(linea, archivo->apertura);
	if (estado == EOF)
		return false;

	return true;
}

int archivo_hay_mas_lineas(archivo_t *archivo)
{
	return (archivo == NULL || feof(archivo->apertura)) ? 0 : 1;
}

void archivo_cerrar(archivo_t *archivo)
{
	if (archivo != NULL) {
		if (archivo->linea_leida != NULL) {
			free(archivo->linea_leida);
		}
		if (archivo->apertura != NULL) {
			fclose(archivo->apertura);
		}
		free(archivo);
	}
}