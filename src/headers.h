#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <string.h>

#define TRUE 1
#define FALSE !TRUE
#define MAX_LINE 100
#define MAX_USER 50
#define MAX_PASS 50
#define BUFFER_SIZE 4096

static const char* SHELL_VERSION = "2.0";

/* definicion de las estructuras de datos de entrada de usuario */
#define BUFFER_MAX_LONG 50              // tama�o maximo del buffer (50 chars)
static char entradaUsuario = '\0';
static char buffer[BUFFER_MAX_LONG];    // buffer para la entrada del usuario
static int bufferChars = 0;

static char *comandoArgv[5];       // vector de cadenas que componen el comando dado
static int comandoArgc = 0;        // contador de las palabras del comando dado

/* shell pid, default terminal */
static pid_t SHELL_PID;
static int SHELL_TERMINAL, SHELL_ES_INTERACTIVO;

/*********************************************************************
 * PROMPT
 *********************************************************************/

/* despliega la pantalla de bienvenida */
void pantallaBienvenida();
/* despliega el prompt de nuestra terminal */
void shellPrompt();

/*********************************************************************
 * FUNCIONES DE ENTRADA DE USUARIO
 *********************************************************************/

/* obtiene una linea de texto digitada por el usuario */
void getLineaTexto();
/* rellena el arreglo comandoArgv[] con cada palabra que compone la linea de comandos */
void rellenarComando();
/* libera el buffer y comandoArgv[] */
void destruirComando();

/*********************************************************************
 * FUNCIONES DEL SHELL
 *********************************************************************/

/* maneja el comando dado: es un comando incorporado o un programa externo? */
void manejarComandoUsuario();
/* ejecuta un comando de sistema */
void ejecutarComando(char *comando[]);
/* crear un procesos hijo */
void crearProceso(char *comando[]);
/* inicializa el shell */
void init();
int verificarComandosBuiltIn();
void cambiarDirectorio();
int validar_usuario();
void limpiar_nueva_linea(char *cadena);
int copiar_archivo(const char *origen, const char *destino);
void mostrar_uso(char *nombre_programa);
