#include <stdio.h> //input output 
#include <stdlib.h> // memoria dinamica 
#include <string.h> // manipolazione delle stringhe 


/*

Scrivere un programma argsend che converta i parametri del programma (da argv[1] in poi) in una
unica sequenza di caratteri: vengono concatenati i parametri (compreso il terminatore della stringa).
Esempio di funzionamento:
$ ./argsend ls -l /tmp | od -c
0000000 l s \0 - l \0 / t m p \0
0000013


*/

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s arg1 [arg2 ...]\n", argv[0]);
        return 1;
    }

    // Calcola la lunghezza totale della stringa risultante
    size_t total_length = 0;
    for (int i = 1; i < argc; i++) {
        total_length += strlen(argv[i]) + 1;  // +1 per il terminatore di stringa '\0'
    }

    // Alloca spazio per la stringa concatenata
    char *result = malloc(total_length);
    if (result == NULL) {
        perror("malloc");
        return 1;
    }

    // Copia e concatena gli argomenti nella stringa risultante
    char *ptr = result;
    for (int i = 1; i < argc; i++) {
        size_t len = strlen(argv[i]);
        strcpy(ptr, argv[i]);
        ptr += len;
        *ptr = '\0';  // Aggiungi il terminatore di stringa
        ptr++;
    }

    // Stampa la stringa risultante
    fwrite(result, 1, total_length, stdout);

    // Libera la memoria allocata
    free(result);

    return 0;
}


