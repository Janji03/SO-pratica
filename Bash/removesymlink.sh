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

# Trova tutti i link simbolici nella directory specificata
find "$dir" -type l | while read -r symlink; do
  # Verifica se il link è relativo
  target=$(readlink "$symlink")
  if [[ "$target" != /* ]]; then
    # Cancella il link simbolico relativo
    echo "Cancellando link simbolico relativo: $symlink -> $target"
    rm "$symlink"
  fi
done
