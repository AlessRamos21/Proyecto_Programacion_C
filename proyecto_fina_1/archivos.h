#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#define LIBRE 0
#define OCUPADA 1
#define LIMPIA 0
#define SUCIA 1
#define MANTENIMIENTO 2
#define DNI 1
#define PASAPORTE 2
#define MAX_PASAPORTE 16
#define MAX_HUESPEDES_POR_HAB 4
#define PRECIO_BASE 10000.0f
#define SIMPLE 1
#define DOBLE 2
#define SUITE 3

/* CAMPOS DE BITS: ocupa solo 3 bits en lugar de dos int completos */
struct estado_habitacion {
    unsigned int ocupada : 1;
    unsigned int limpieza : 2;
};

/* UNION: comparte memoria entre DNI y pasaporte */
union documento_identidad {
    long int dni;
    char pasaporte[MAX_PASAPORTE];
};

struct huesped {
    char *nombre;
    int tipo_documento;
    union documento_identidad documento;
    char fecha_checkin[20];
};

/* STRUCT: tipo de dato compuesto principal del sistema hotelero */
struct habitacion {
    int numero;
    int tipo;
    int capacidad;
    float precio;
    struct estado_habitacion estado;
    struct huesped huespedes[MAX_HUESPEDES_POR_HAB];
    int cantidad_huespedes;
};

void guardar_base_datos(const struct habitacion *hotel, int cantidad);
void cargar_base_datos(struct habitacion *hotel, int cantidad);
int contar_habitaciones_guardadas(void);
void generar_reporte(const struct habitacion *hotel, int cantidad, char *nombre_out, int tam_out);
void ver_ultimo_reporte(const char *nombre_archivo);
const char *texto_tipo(int tipo);
const char *texto_estado(unsigned int ocupada);
const char *texto_limpieza(unsigned int limpieza);

#endif
