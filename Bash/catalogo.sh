#!/bin/bash

# Funzione per creare il catalogo
create_catalog() {
  local dir=$1
  declare -A file_types

  # Scorrere tutti i file nella directory specificata
  while IFS= read -r -d $'\0' file; do
    # Ottenere il tipo di file
    file_type=$(file -b "$file")
    # Aggiungere il file alla categoria appropriata
    file_types["$file_type"]+="$file"$'\n'
  done < <(find "$dir" -maxdepth 1 -type f -print0)

  # Stampa del catalogo ordinato per tipo di file
  for type in "${!file_types[@]}"; do
    echo "$type:"
    echo "${file_types[$type]}"
  done
}

# Verificare se è stato passato un parametro, altrimenti usare la directory corrente
if [ -n "$1" ]; then
  target_dir="$1"
else
  target_dir="."
fi

# Creare il catalogo
create_catalog "$target_dir"
