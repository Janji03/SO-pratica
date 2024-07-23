#!/bin/bash

# Directory da esaminare (default alla directory corrente se non specificata)
dir="${1:-.}"

# Assicurarsi che la directory esista
if [ ! -d "$dir" ]; then
  echo "La directory $dir non esiste."
  exit 1
fi

# Funzione per determinare se un file è uno script
is_script() {
  file "$1" | grep -qE 'shell script|text executable'
}

# Trova tutti i file nella directory specificata
for file in "$dir"/*; do
  # Controlla se il file è uno script e non un eseguibile binario ELF
  if [ -f "$file" ] && is_script "$file"; then
    echo "Eseguendo $file..."
    bash "$file"
  fi
done
