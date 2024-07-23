#!/bin/bash

# Funzione per elencare i file comuni
list_common() {
  comm -12 <(find "$1" -printf "%P\n" | sort) <(find "$2" -printf "%P\n" | sort)
}

# Funzione per elencare i file che esistono solo nel primo sottoalbero
list_only_in_first() {
  comm -23 <(find "$1" -printf "%P\n" | sort) <(find "$2" -printf "%P\n" | sort)
}

# Funzione per elencare i file che esistono solo nel secondo sottoalbero
list_only_in_second() {
  comm -13 <(find "$1" -printf "%P\n" | sort) <(find "$2" -printf "%P\n" | sort)
}

# Verifica del numero di argomenti
if [ "$#" -lt 2 ] || [ "$#" -gt 3 ]; then
  echo "Uso: $0 [-1 | -2] dir1 dir2"
  exit 1
fi

# Gestione dei casi con o senza parametro
if [ "$#" -eq 2 ]; then
  dir1="$1"
  dir2="$2"
  list_common "$dir1" "$dir2"
else
  case "$1" in
    -1)
      dir1="$2"
      dir2="$3"
      list_only_in_first "$dir1" "$dir2"
      ;;
    -2)
      dir1="$2"
      dir2="$3"
      list_only_in_second "$dir1" "$dir2"
      ;;
    *)
      echo "Parametro non valido. Uso: $0 [-1 | -2] dir1 dir2"
      exit 1
      ;;
  esac
fi
