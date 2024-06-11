#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>



/*

Scrivere un programma search_name che deve cercare nel sottoalbero della directory corrente tutti i
file eseguibili con un nome file specifico passato come primo e unico parametro indicando per ogni
file il tipo di eseguibile (script o eseguibile binario).
Ad esempio il comando:
./search_name testprog
deve cercare i file eseguibili chiamati testprog nell'albero della directory corrente. Poniamo
siano ./testprog, ./dir1/testprog, ./dir/dir3/testprog, search_name deve stampare:
. /testprog: script
./dir1/testprog: ELF executable
./dir/dir3/testprog: ELF executable


*/

// Funzione per determinare se un file è uno script o un eseguibile binario
char* get_file_type(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        char line[100];
        // Leggi la prima riga del file per determinare il tipo
        fgets(line, sizeof(line), file);
        fclose(file);
        if (strstr(line, "#!") == line) {
            return "script";
        } else {
            return "ELF executable";
        }
    }
    return "unknown";
}

// Funzione ricorsiva per cercare i file eseguibili nel sottoalbero
void search_executables(const char* dirname, const char* target_name) {
    DIR* dir = opendir(dirname);
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            // Ignora le voci di directory speciali . e ..
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            // Costruisci il percorso completo del file
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);
            struct stat statbuf;
            if (stat(path, &statbuf) == 0) {
                if (S_ISDIR(statbuf.st_mode)) {
                    // Ricorsione nelle directory
                    search_executables(path, target_name);
                } else if (S_ISREG(statbuf.st_mode) && access(path, X_OK) == 0) {
                    // Verifica se il file è eseguibile e ha il nome target
                    if (strcmp(entry->d_name, target_name) == 0) {
                        printf("%s: %s\n", path, get_file_type(path));
                    }
                }
            }
        }
        closedir(dir);
    }
}

int main(int argc, char* argv[]) {
    // Verifica che il numero di argomenti sia corretto
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <target_name>\n", argv[0]);
        return 1;
    }
    // Ottieni il nome del file target
    const char* target_name = argv[1];
    // Avvia la ricerca dei file eseguibili
    search_executables(".", target_name);
    return 0;
}
