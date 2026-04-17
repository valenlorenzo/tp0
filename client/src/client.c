#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	// Usando el logger creado previamente
	log_info(logger, "Hola! Soy un log");


	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	valor = config_get_string_value(config, "CLAVE");

	// Loggeamos el valor de config
	log_info(logger, "Config cargada: IP=%s, Puerto=%s, Clave=%s", ip, puerto, valor);

	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);
	log_info(logger, "Intentando conectarme al servidor...");

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);

	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje(valor, conexion);
	
	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;

	// Creamos el logger con:
	// 1. Nombre del archivo: "cliente.log"
	// 2. Nombre del proceso: "CLIENTE"
	// 3. ¿Mostrar en consola?: 1 (es el true de C)
	// 4. Nivel de detalle: LOG_LEVEL_INFO

	nuevo_logger = log_create("cliente.log", "CLIENTE", 1, LOG_LEVEL_INFO);

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config;

	nuevo_config = config_create("cliente.config");

	if(nuevo_config == NULL){
		perror("¡No se pudo crear el config!");
		exit(2);
	}
	return nuevo_config;
}

void leer_consola(t_log* logger) {
	char* leido;

	// La primera te la dejo de yapa
	leido = readline("> ");

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
	while(strcmp(leido, "") != 0){
		// logueamos lo que el usuario escribió
		log_info(logger, "Consola: %s", leido);

		// Liberamos la memoria de la línea anterior
		free(leido);

		// Volvemos a pedir otra linea para que el bucle siga o termine
		leido = readline("> ");
	}

	// Liberamos la última línea (la que estaba vacía) antes de salir
	free(leido);

	// ¡No te olvides de liberar las lineas antes de regresar!

}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete = crear_paquete();

	// Leemos y esta vez agregamos las lineas al paquete
	leido = readline("> ");

	while (strcmp(leido, "") != 0) {
		// Agregamos el mensaje al paquete. 
		// El tamaño es el largo del texto + 1 (por el caracter de fin \0) [cite: 295]
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		
		// Liberamos la memoria de la línea leída para evitar Memory Leaks [cite: 227, 276]
		free(leido); 
		leido = readline("> ");
	}
	free(leido); // Liberamos la última línea vacía [cite: 234]

	// Enviamos todo el chorizo de bytes por la conexión [cite: 159, 295]
	enviar_paquete(paquete, conexion);

	// ¡Fundamental! Liberamos la memoria de la estructura del paquete [cite: 188, 296]
	eliminar_paquete(paquete);

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	
}

void terminar_programa(int conexion, t_log* logger, t_config* config) {
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	if (logger != NULL) log_destroy(logger);
	if (logger != NULL) config_destroy(config);

	liberar_conexion(conexion);
}
