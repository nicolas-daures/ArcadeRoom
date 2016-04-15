#!/bin/bash

#*****************************************************************************
# FUNCTION
#   Create tree view used by ArcadeRoom to check available roms and covers.
#   It must be launched in the directory containing covers and roms.
#
#   Do the following:
#     - Create a directory for each founded rom.
#     - Move rom in this directory.
#     - Move matching covers, if exists.
#
# PARAMETERS
#   None.
#
# USAGE
#   sh tree_view_create.sh
#
#*****************************************************************************


#*****************************************************************************
# CONSTANTS
#*****************************************************************************
# None


#*****************************************************************************
# GLOBAL VARIABLES
#*****************************************************************************
# None


#*****************************************************************************
# INTERNAL PROCEDURES
#*****************************************************************************

##
# create_tree_view
# 
# FUNCTION 
#  Create tree view of all files with specific extension.
# 
# PARAMETERS
#  [IN] Extension of file to process
#
# RETURN
#  None
#
create_tree_view()
{
  # Browse each rom in current directory with extension given in parameter
  find ./ -maxdepth 1 -type f -name "*.$1" -exec basename {} \; | while read ROM_FULLNAME
  do

    # Keep rom name (without extension and without path)
    ROM_NAME="$(basename $ROM_FULLNAME .${ROM_FULLNAME##*.})"

    # Check if directory do not exists
    if [ ! -d $ROM_NAME ]
    then
      # Process rom

     echo "Process rom \"$ROM_NAME\""

      # Create directory
      mkdir $ROM_NAME   

      # Move rom in the same name directory
      mv $ROM_FULLNAME $ROM_NAME

      # Move matching cover with other extension given in parameter
      # TODO traiter les divers cas d'extenstion case sensitive avec une liste en CTE début script
      mv $ROM_NAME.$2 $ROM_NAME

    else
      # Nothing to do if exists
      echo "Directory \"$ROM_NAME\" already exists, nothing to do."      
    fi

  done 
}


#*****************************************************************************
# START
#*****************************************************************************

# Process rom zip files
# TODO supprimer le 2ième param et le traiter direct dans la fonction avec une liste
create_tree_view zip jpg
create_tree_view ZIP jpg

