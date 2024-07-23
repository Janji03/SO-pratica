#!/bin/bash

# Verifica se la directory è fornita come argomento
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <directory>"
    exit 1
fi

directory=$1

# Verifica se la directory esiste
if [ ! -d "$directory" ]; then
    echo "Directory not found!"
    exit 1
fi

# Usa il comando 'file' per identificare i tipi di file
# e raccoglie le informazioni in un array associativo
declare -A file_types

while IFS= read -r -d '' file; do
    type=$(file -b "$file")
    file_types["$type"]+=$(basename "$file")" "
done < <(find "$directory" -type f -print0)

# Stampa i risultati
for type in "${!file_types[@]}"; do
    # Rimuove l'ultimo spazio alla fine della stringa dei nomi dei file
    files=$(echo "${file_types[$type]}" | sed 's/ *$//')
    echo "$type: $files"
done
