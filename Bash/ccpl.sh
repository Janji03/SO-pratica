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

# Dichiarare un array per memorizzare i conteggi di caratteri per ciascuna riga
declare -A line_counts

# Trova tutti i file di testo nella directory specificata
for file in "$dir"/*; do
  if [ -f "$file" ]; then
    # Leggi il file riga per riga
    while IFS= read -r line || [ -n "$line" ]; do
      # Ottieni il numero della riga
      line_number=$((line_number + 1))
      # Conta i caratteri nella riga
      char_count=${#line}
      # Aggiungi il conteggio dei caratteri all'array
      line_counts[$line_number]=$((line_counts[$line_number] + char_count))
    done < "$file"
    # Resetta il numero della riga per il prossimo file
    line_number=0
  fi
done

# Stampa i risultati
for line_number in "${!line_counts[@]}"; do
  echo "$line_number ${line_counts[$line_number]}"
done | sort -n
