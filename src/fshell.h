
/*********************************************************************
 * PROMPT
 *********************************************************************/

void pantallaBienvenida()
{
        printf("\n-------------------------------------------------\n");
        printf("\tBienvenido a mi shell V. %s\n", SHELL_VERSION);
        printf("\tProcess ID: %d\n", (int) SHELL_PID);
        printf("-------------------------------------------------\n");
        printf("\n\n");
}

/**
 *	despliega el prompt de nuestro shell
 */
void shellPrompt()
{
        printf("mishell# ");
}

void init()
{
        SHELL_PID = getpid();    // retorna el pid del shell
        SHELL_TERMINAL = STDIN_FILENO;     // terminal = STDIN
        SHELL_ES_INTERACTIVO = isatty(SHELL_TERMINAL); // el shell es interactivo si STDIN es el terminal
        //isatty - testea si el descriptor de archivo refiere a el terminal
        /*STDIN_FILENO es una macro definida en <unistd.h>.
         * Al menos para sistemas compatibles con POSIX,  es requerido que sea 0
        /STDIN_FILENO es el descriptor de archivo de entrada estandar por defecto el cual es 0
         * es esencialmente una directiva definida como cero*/
        if (SHELL_ES_INTERACTIVO) {     // es el shell interactivo?
        } else {
                printf("No es posible hacer el shell interactivo. Saliendo..\n");
                exit(EXIT_FAILURE);
        }
}

/*********************************************************************
 * FUNCIONES DE ENTRADA DE USUARIO
 *********************************************************************/

/**
 *	obtiene una linea de texto del teclado
 */
void getLineaTexto()
{
		destruirComando();    // limpia el buffer y la linea de comandos
        while ((entradaUsuario != '\n') && (bufferChars < BUFFER_MAX_LONG)) {
                buffer[bufferChars++] = entradaUsuario;
                entradaUsuario = getchar();
        }
        buffer[bufferChars] = 0x00;    // null para terminar el buffer
        rellenarComando();
}

/**
 * rellena el array comandoArgv con todas las palabras
 * de la linea de comandos (un espacio  " " es usado como separador)
 */
void rellenarComando()
{
        char* pbuffer;            // un puntero al buffer
        pbuffer = strtok(buffer, " ");
        /*La funcion de la libreria de C char *strtok(char *str, const char *delim)
        * divide la cadena str en una serie de tokens usando el delimitador delim*/

        while (pbuffer != NULL) {   // mientras el puntero no sea NULL
                comandoArgv[comandoArgc] = pbuffer;   // rellenar el array comandoArgv
                pbuffer = strtok(NULL, " "); // Posteriores llamadas
                comandoArgc++;
        }
}

/**
 *	destruye el array de chars, dejando que el usuario introduzca otra linea de comandos
 */
void destruirComando()
{
        while (comandoArgc != 0) {
                comandoArgv[comandoArgc] = NULL;  // elimina el puntero a la cadena
                comandoArgc--;        // decrementa el numero de las palabras en la linea
        }
        bufferChars = 0;              // reinicia el conteo de chars en el buffer
}

/*********************************************************************
 * FUNCIONES DEL SHELL
 *********************************************************************/

/* Función para mostrar el uso correcto del programa */
void mostrar_uso(char *nombre_programa) {
    fprintf(stderr, "Uso: %s <archivo_origen> <archivo_destino>\n", nombre_programa);
    fprintf(stderr, "Copia el contenido de archivo_origen a archivo_destino\n");
}

/* Función para copiar archivos */
int copiar_archivo(const char *origen, const char *destino) {
    int fd_origen, fd_destino;
    ssize_t bytes_leidos, bytes_escritos;
    char buffer[BUFFER_SIZE];
    struct stat stat_origen;

    /* Abrir archivo origen en modo lectura */
    fd_origen = open(origen, O_RDONLY);
    if (fd_origen == -1) {
        fprintf(stderr, "Error: No se pudo abrir el archivo origen '%s': %s\n",
                origen, strerror(errno));
        return -1;
    }

    /* Obtener información del archivo origen */
    if (fstat(fd_origen, &stat_origen) == -1) {
        fprintf(stderr, "Error: No se pudo obtener información del archivo '%s': %s\n",
                origen, strerror(errno));
        close(fd_origen);
        return -1;
    }

    /* Verificar que el origen sea un archivo regular */
    if (!S_ISREG(stat_origen.st_mode)) {
        fprintf(stderr, "Error: '%s' no es un archivo regular\n", origen);
        close(fd_origen);
        return -1;
    }

    /* Crear archivo destino con los mismos permisos que el origen */
    fd_destino = open(destino, O_WRONLY | O_CREAT | O_TRUNC, stat_origen.st_mode);
    if (fd_destino == -1) {
        fprintf(stderr, "Error: No se pudo crear el archivo destino '%s': %s\n",
                destino, strerror(errno));
        close(fd_origen);
        return -1;
    }

    /* Copiar datos en bloques */
    while ((bytes_leidos = read(fd_origen, buffer, BUFFER_SIZE)) > 0) {
        char *ptr = buffer;
        ssize_t bytes_restantes = bytes_leidos;

        /* Escribir todos los bytes leídos */
        while (bytes_restantes > 0) {
            bytes_escritos = write(fd_destino, ptr, bytes_restantes);
            if (bytes_escritos == -1) {
                fprintf(stderr, "Error: Fallo al escribir en '%s': %s\n",
                        destino, strerror(errno));
                close(fd_origen);
                close(fd_destino);
                unlink(destino); /* Eliminar archivo parcialmente copiado */
                return -1;
            }
            bytes_restantes -= bytes_escritos;
            ptr += bytes_escritos;
        }
    }

    /* Verificar errores en la lectura */
    if (bytes_leidos == -1) {
        fprintf(stderr, "Error: Fallo al leer desde '%s': %s\n",
                origen, strerror(errno));
        close(fd_origen);
        close(fd_destino);
        unlink(destino);
        return -1;
    }

    /* Cerrar archivos */
    if (close(fd_origen) == -1) {
        fprintf(stderr, "Advertencia: Error al cerrar archivo origen: %s\n",
                strerror(errno));
    }

    if (close(fd_destino) == -1) {
        fprintf(stderr, "Error: Error al cerrar archivo destino: %s\n",
                strerror(errno));
        return -1;
    }

    return 0;
}

void limpiar_nueva_linea(char *cadena) {
    size_t len = strlen(cadena);
    if (len > 0 && (cadena[len - 1] == '\n' || cadena[len - 1] == '\r'))
        cadena[len - 1] = '\0';
}

int validar_usuario() {
    FILE *f = fopen("/home/mishell/login.txt", "r");
    if (f == NULL) {
        printf(" Error: no se puede abrir el archivo /home/mishell/login.txt\n");
        return 0;
    }

    char usuario_archivo[MAX_USER], pass_archivo[MAX_PASS];
    char usr_ingresado[MAX_USER], pass_ingresada[MAX_PASS];
    char linea[MAX_LINE];
    int validado = 0;

    printf("Usuario: ");
    fgets(usr_ingresado, MAX_USER, stdin);
    limpiar_nueva_linea(usr_ingresado);

    printf("Contrasena: ");
    fgets(pass_ingresada, MAX_PASS, stdin);
    limpiar_nueva_linea(pass_ingresada);

    while (fgets(linea, sizeof(linea), f)) {
        limpiar_nueva_linea(linea);

        // Buscar el separador ':'
        char *sep = strchr(linea, ':');
        if (sep == NULL) continue; // línea mal formada

        // Dividir en usuario y contraseña
        size_t len_user = sep - linea;
        strncpy(usuario_archivo, linea, len_user);
        usuario_archivo[len_user] = '\0';

        strcpy(pass_archivo, sep + 1);

        // Quitar espacios accidentales
        limpiar_nueva_linea(usuario_archivo);
        limpiar_nueva_linea(pass_archivo);

        // Comparar
        if (strcmp(usuario_archivo, usr_ingresado) == 0 &&
            strcmp(pass_archivo, pass_ingresada) == 0) {
            validado = 1;
            break;
        }
    }

    fclose(f);

    if (validado) {
        printf("\n Bienvenido %s\n", usr_ingresado);
        return 1;
    } else {
        printf("\n Usuario o contrasena incorrectos\n");
        return 0;
    }
}

/**
 * ejecucion generica void
 */
void manejarComandoUsuario()
{
	if ((verificarComandosBuiltIn(comandoArgv[0])) == 0) {
        crearProceso(comandoArgv);
	}
}

/**
 * comandos incorporados:
 * retorna 1 si un comando built-in es ejecutado de lo contrario 0
 */
int verificarComandosBuiltIn()
{

        if (strcmp("cd", comandoArgv[0]) == 0) {
                cambiarDirectorio();
                return 1;
        }
        if (strcmp("salir", comandoArgv[0]) == 0) {
        		exit(EXIT_SUCCESS);
                return 1;
                }
        if (strcmp("nombre", comandoArgv[0]) == 0 ) {
        	printf("\tBienvenido %s este es su nuevo shell\n", comandoArgv[1]);
                return 1;
                }
        if (strcmp("suma", comandoArgv[0]) == 0 ) {
        	if (comandoArgv[1] != NULL && comandoArgv[2] != NULL) {

        		//Convertir las cadenas a enteros usando atoi()
        		int num1 = atoi(comandoArgv[1]);
        		int num2 = atoi(comandoArgv[2]);

        		int resultado = num1 + num2;
        		// Imprimir el resultado
        		 printf("Resultado de la suma es : %d\n", resultado);
        	}
                return 1;
                }
        if (strcmp("mycopy", comandoArgv[0]) == 0) {
            /* Verificar número de argumentos */
            if (comandoArgc != 3) {
                mostrar_uso(comandoArgv[0]);
                return 1;
            }

            /* Verificar que origen y destino no sean el mismo archivo */
            if (strcmp(comandoArgv[1], comandoArgv[2]) == 0) {
                fprintf(stderr, "Error: Los archivos origen y destino son el mismo\n");
                return 1;
            }

            /* Realizar la copia */
            if (copiar_archivo(comandoArgv[1], comandoArgv[2]) == 0) {
                printf("Archivo copiado exitosamente: %s -> %s\n", comandoArgv[1], comandoArgv[2]);
                return 0;
            } else {
                return 1;
            }
                        return 1;
                }
        return 0;
}

/**
 * cambia el directorio actual
 */
void cambiarDirectorio()
{
	//chdir -> Cambia el directorio actual de trabajo
	//getenv -> Retorna una cadena conteniendo el valor de la variable de entorno cuyo nombre es
	//especificado como argumento
        if (comandoArgv[1] == NULL) {
                chdir(getenv("HOME"));            // simula un comando "cd"
        } else {
                if (chdir(comandoArgv[1]) == -1) {     // ir al nuevo directorio
                        printf(" %s: no such directory\n", comandoArgv[1]);
                }
        }
}
/**
 * ejecuta un programa
 */
void ejecutarComando(char *comando[])
{
        if (execvp(*comando, comando) == -1)
                perror("error en execvp");
}

/**
 * crear un proceso con fork y ejecuta un programa hijo
 */
void crearProceso(char *comando[])
{
        pid_t pid;
        pid = fork();
        switch (pid) {
        case -1:
                perror("error en fork");
                exit(EXIT_FAILURE);
                break;
        case 0:
                ejecutarComando(comando);
                exit(EXIT_SUCCESS);
                break;
        default:
                break;
        }
}


