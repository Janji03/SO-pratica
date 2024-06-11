#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>



/*

Il programma run_name deve cercare nel sottoalbero della directory corrente tutti i file eseguibili con
un nome file specifico (primo parametro di run_name) e li deve mettere in esecuzione uno dopo l'altro
passando i successivi parametri.
Ad esempio il comando:
./run_name testprog a b c
deve cercare i file eseguibili chiamati testprog nell'albero della directory corrente. Poniamo
siano ./testprog, ./dir1/testprog, ./dir/dir3/testprog, run_name deve eseguire
testprog a b c
per 3 volte. Nella prima esecuzione la working directory deve essere la dir corrente '.', la seconda
deve avere come working directory './dir1' e la terza './dir2/dir3'.


*/

// Funzione per eseguire un file con i parametri specificati
void execute_file(const char* filename, char* const argv[]) {
    execvp(filename, argv);
    perror("execvp"); // Stampa un messaggio di errore se l'esecuzione fallisce
    exit(EXIT_FAILURE);
}

// Funzione ricorsiva per cercare i file eseguibili nel sottoalbero e eseguirli
void search_and_execute(const char* dirname, const char* target_name, char* const argv[]) {
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
                    // Esegui la ricerca e l'esecuzione nella sottodirectory
                    chdir(path);
                    search_and_execute(".", target_name, argv);
                    chdir("..");
                } else if (S_ISREG(statbuf.st_mode) && access(path, X_OK) == 0) {
                    // Verifica se il file è eseguibile e ha il nome target
                    if (strcmp(entry->d_name, target_name) == 0) {
                        execute_file(path, argv);
                    }
                }
            }
        }
        closedir(dir);
    }
}

int main(int argc, char* argv[]) {
    // Verifica che ci siano almeno due argomenti (il nome del file target e almeno un argomento da passare)
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <target_name> <arg1> [<arg2> ...]\n", argv[0]);
        return EXIT_FAILURE;
    }
    // Ottieni il nome del file target
    const char* target_name = argv[1];
    // Esegui la ricerca e l'esecuzione dei file eseguibili
    search_and_execute(".", target_name, argv + 1);
    return EXIT_SUCCESS;
}
