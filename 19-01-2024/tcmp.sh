#!/bin/bash

# Funzione per stampare l'uso corretto dello script
usage() {
    echo "Usage: $0 [-1|-2] dir1 dir2"
    exit 1
}

# Controlla se il numero di argomenti è corretto
if [ $# -lt 2 ] || [ $# -gt 3 ]; then
    usage
fi

# Analizza i parametri
if [ $# -eq 2 ]; then
    dir1=$1
    dir2=$2
    mode=0
elif [ $# -eq 3 ]; then
    case $1 in
        -1)
            mode=1
            ;;
        -2)
            mode=2
            ;;
        *)
            usage
            ;;
    esac
    dir1=$2
    dir2=$3
else
    usage
fi

# Controlla se le directory esistono
if [ ! -d "$dir1" ]; then
    echo "Directory $dir1 does not exist."
    exit 1
fi

if [ ! -d "$dir2" ]; then
    echo "Directory $dir2 does not exist."
    exit 1
fi

# Funzione per elencare i file e le directory in una struttura di directory
list_files() {
    find "$1" -print | sed "s|^$1/||"
}

# Elenca i file e le directory
files_dir1=$(list_files "$dir1")
files_dir2=$(list_files "$dir2")

# Trova e stampa i risultati in base al parametro
if [ $mode -eq 0 ]; then
    # Comuni ad entrambi i sottoalberi
    comm -12 <(echo "$files_dir1" | sort) <(echo "$files_dir2" | sort)
elif [ $mode -eq 1 ]; then
    # Esistono solo nel primo sottoalbero
    comm -23 <(echo "$files_dir1" | sort) <(echo "$files_dir2" | sort)
elif [ $mode -eq 2 ]; then
    # Esistono solo nel secondo sottoalbero
    comm -13 <(echo "$files_dir1" | sort) <(echo "$files_dir2" | sort)
fi

exit 0
