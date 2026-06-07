#ifndef HOTEL_H
#define HOTEL_H

#include "archivos.h"

//prototipos de las funciones que llevan la logica
void inicializar_hotel(struct habitacion *hotel, int cantidad);
void liberar_hotel(struct habitacion *hotel, int cantidad);
void listar_habitaciones(const struct habitacion *hotel, int cantidad);
int pedir_cantidad_habitaciones(void);
int leer_entero(const char *mensaje, int *valor);
int leer_long(const char *mensaje, long int *valor);
char *leer_texto_dinamico(const char *mensaje);
int buscar_habitacion_por_numero(const struct habitacion *hotel, int cantidad, int numero);
int buscar_huesped_por_nombre(const struct habitacion *hotel, int cantidad, const char *nombre);
int cargar_documento(struct huesped *huesped, const char *mensaje_dni, const char *mensaje_pasaporte);
void hacer_checkin(struct habitacion *hotel, int cantidad);
int generar_ticket_checkout(const struct habitacion *habitacion, const struct huesped *huesped);
void hacer_checkout(struct habitacion *hotel, int cantidad);
void mostrar_documento(const struct huesped *huesped);
void mostrar_datos_huesped(const struct habitacion *habitacion);
void buscar_huesped_en_menu(const struct habitacion *hotel, int cantidad);
void buscar_huesped_por_documento(const struct habitacion *hotel, int cantidad);
void modificar_huesped(struct habitacion *hotel, int cantidad);
void cambiar_limpieza_menu(struct habitacion *hotel, int cantidad);
void mostrar_menu(void);
void pausa(void);

const char *texto_tipo(int tipo);
const char *texto_estado(unsigned int ocupada);
const char *texto_limpieza(unsigned int limpieza);

#endif
