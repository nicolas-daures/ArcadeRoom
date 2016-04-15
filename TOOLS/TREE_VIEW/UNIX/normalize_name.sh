#!/bin/bash

#*****************************************************************************
# FUNCTION
#   Deleting extra characters or patterns in rom and cover file names.
#   It must be launched in the directory containing covers and roms.
#
#   Do the following:
#     - Rename each rom with an extra character or pattern to delete.
#     - Rename matching cover, if exists.
#     - TODO gérer les priorités (d'abord prendre le rom FR, ignorer les autres)
#
# PARAMETERS
#   None.
#   TODO accepter un parametre (nom de l'unique fichier à traiter). Cela 
#   permettra un appel de ce script par tree_view_create.sh. Par défaut traiter
#   l'ensemble des fichiers présents dans le répertoire.
#
# USAGE
#   sh normalize_name.sh
#
#*****************************************************************************


#*****************************************************************************
# CONSTANTS
#*****************************************************************************

# List of authorized file extension in cover name
CTE_LIST_OF_FILE_EXT_FOR_COVER="\
    jpg \
    JPG \
    png \
    PNG"


#*****************************************************************************
# GLOBAL VARIABLES
#*****************************************************************************
# None


#*****************************************************************************
# INTERNAL PROCEDURES
#*****************************************************************************

##
# proc_print_usage
# 
# FUNCTION 
#  Print how to use this script
# 
# PARAMETERS
#  None
#
# RETURN
#  None
#
proc_print_usage()
{
# TODO
echo "USAGE TO DO"

}

##
# proc_normalize_a_file
# 
# FUNCTION 
#  Normalize name of a file
# 
# PARAMETERS
#  [IN] File name to normalize
#
# RETURN
#  None
#
proc_normalize_a_file()
{
  NORMALIZED_NAME=`echo "$1"`

  # Delete '(xxx)' pattern
  NORMALIZED_NAME=`echo "$NORMALIZED_NAME" | sed "s/(.*)//g"`
 
  # Delete '[xxx]' pattern
  NORMALIZED_NAME=`echo "$NORMALIZED_NAME" | sed "s/\[.*\]//g"`

  # Move 'The' pattern from the end of the name to the begin
  NORMALIZED_NAME=`echo "$NORMALIZED_NAME" | sed "s/\(.*\)[ _-]\([Tt][Hh][Ee]\)\./\2_\1\./"` 

  # Replace ' ' (space) by '_' (underscore)
  NORMALIZED_NAME=`echo "$NORMALIZED_NAME" | sed "s/ /_/g"`

  # Rename file if there is at least one character to delete
  if [ "$1" != "$NORMALIZED_NAME" ]
  then
    mv "$1" "$NORMALIZED_NAME"
  fi
}

##
# proc_normalize_all_files
# 
# FUNCTION 
#  Normalize name of all files with specific extension.
# 
# PARAMETERS
#  [IN] Extension of file to normalize
#
# RETURN
#  None
#
proc_normalize_all_files()
{
  # Browse each rom in current directory with extension given in parameter
  find ./ -maxdepth 1 -type f -name "*.$1" -exec basename {} \; | while read ROM_FULLNAME
  do
    # Keep rom name (without extension and without path)
    ROM_NAME="$(basename "$ROM_FULLNAME" .${ROM_FULLNAME##*.})"

    # Normalize rom name
    proc_normalize_a_file "$ROM_FULLNAME"

    # For each authorized cover file extension 
    for file_ext in $CTE_LIST_OF_FILE_EXT_FOR_COVER
    do

      # Build matching cover name
      COVER_NAME=$ROM_NAME.$file_ext

      # Normalize matching cover, if exists
      if [ -f "$COVER_NAME" ]
      then
        proc_normalize_a_file "$COVER_NAME"  
      fi
    done

  done
}


#*****************************************************************************
# START
#*****************************************************************************

# Check parameters
# TODO
if [ "$1" == "" ]
then
  echo "CHECK PARAMETER TO DO"
  proc_print_usage
fi


# Process rom zip files
proc_normalize_all_files zip
proc_normalize_all_files ZIP

