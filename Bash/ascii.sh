#!/bin/bash

# Verifica del numero di argomenti
if [ "$#" -ne 1 ]; then
  echo "Uso: $0 directory"
  exit 1
fi

# Directory da esaminare
dir="$1"

# Assicurarsi che la directory esista
if [ ! -d "$dir" ]; then
  echo "La directory $dir non esiste."
  exit 1
fi

# Funzione per verificare se una stringa contiene solo caratteri ASCII
contains_non_ascii() {
  LC_ALL=C
  [[ "$1" =~ [^[:ascii:]] ]]
}

# Trova tutti i file e le directory nella directory specificata
while IFS= read -r -d '' entry; do
  name=$(basename "$entry")
  if contains_non_ascii "$name"; then
    echo "$entry"
  fi
done < <(find "$dir" -print0)

