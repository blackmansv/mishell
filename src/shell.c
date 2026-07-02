#include "headers.h" // declaraciones y prototipos
#include "fshell.h"

int main(int argc, char **argv, char **envp)
{
	if (!validar_usuario()) {
	        printf("Acceso denegado. Cerrando sesion...\n");
	        exit(1);
	    }
        init();
        pantallaBienvenida();
        shellPrompt();    // muestra el prompt
        while (TRUE) {
                entradaUsuario = getchar();
                switch (entradaUsuario) {
                case '\n':   // si el usuario solo presiona enter
                        shellPrompt();   // no pasa nada
                        break;
                default:
                        getLineaTexto();     // almacena la entrada de usuario en el buffer
                        manejarComandoUsuario(); // ejecuta el comando de usuario
                        shellPrompt();  // muestra el prompt
                        break;
                }
        }
        printf("\n"); // salto de linea

        return 0;

}
