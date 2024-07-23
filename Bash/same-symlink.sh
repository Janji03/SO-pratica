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

declare -A link_map

# Trova tutti i link simbolici nella directory specificata
while IFS= read -r -d '' symlink; do
  target=$(readlink -f "$symlink")
  link_map["$target"]+="$symlink"$'\n'
done < <(find "$dir" -type l -print0)

# Stampa i risultati
for target in "${!link_map[@]}"; do
  # Se ci sono più link che puntano allo stesso target, stamparli
  if [ $(echo "${link_map[$target]}" | wc -l) -gt 1 ]; then
    echo "I seguenti link simbolici puntano a $target:"
    echo "${link_map[$target]}"
  fi
done
