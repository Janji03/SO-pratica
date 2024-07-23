#!/bin/bash

# Verifica del numero di argomenti
if [ "$#" -ne 3 ]; then
  echo "Uso: $0 source_dir1 source_dir2 destination_dir"
  exit 1
fi

# Directory di origine e destinazione
src1="$1"
src2="$2"
dest="$3"

# Assicurarsi che le directory di origine esistano
if [ ! -d "$src1" ]; then
  echo "La directory $src1 non esiste."
  exit 1
fi

if [ ! -d "$src2" ]; then
  echo "La directory $src2 non esiste."
  exit 1
fi

# Creare la directory di destinazione se non esiste
mkdir -p "$dest"

# Funzione per copiare e concatenare file
copy_and_concatenate() {
  src_file="$1"
  dest_file="$2"
  if [ -f "$dest_file" ]; then
    # Concatenare i contenuti se il file esiste già
    cat "$src_file" >> "$dest_file"
  else
    # Copiare il file se non esiste
    cp "$src_file" "$dest_file"
  fi
}

# Funzione per processare l'albero del file system
process_tree() {
  src_dir="$1"
  dest_dir="$2"

  find "$src_dir" -type d | while read -r dir; do
    # Creare la directory corrispondente nella destinazione
    rel_path="${dir#$src_dir/}"
    mkdir -p "$dest_dir/$rel_path"
  done

  find "$src_dir" -type f | while read -r file; do
    # Copiare e concatenare i file
    rel_path="${file#$src_dir/}"
    copy_and_concatenate "$file" "$dest_dir/$rel_path"
  done
}

# Processare i due alberi del file system
process_tree "$src1" "$dest"
process_tree "$src2" "$dest"

echo "Merge completato. I file sono stati copiati in $dest."
