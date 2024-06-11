#!/bin/bash

# Impostazione della directory
directory="${1:-.}"

# Controllo se la directory esiste
if [ ! -d "$directory" ]; then
    echo "Directory non valida: $directory"
    exit 1
fi

# Creazione del catalogo
catalog=$(find "$directory" -type f -exec file {} + | awk -F ': ' '{print $1 " " $2}')

# Stampa del catalogo ordinato
echo "$catalog" | sort -k2 | awk '
    BEGIN {
        category=""
    }
    {
        if ($2 != category) {
            print $2 ":"
            category=$2
        }
        print "\t" $1
    }
'
