#!/bin/bash

#*****************************************************************************
# FUNCTION
#   Restore original tree view before execution of tree_view_create.sh script.
#   It must be launched in the same directory where tree_view_create.sh has 
#   been executed.
#
#   Do the following:
#     - For each founded directory:
#         + Move directory content in the parent directory.
#         + Delete directory.
#
# PARAMETERS
#   None.
#
# USAGE
#   sh tree_view_restore.sh
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
# None


#*****************************************************************************
# START
#*****************************************************************************

# Browse each directory in current directory.
find ./ -maxdepth 1 -type d -exec basename {} \; | while read fichier
do

  # Keep directory name (without path).
  CURRENT_DIRNAME="$(basename $fichier .${fichier##*.})"

  # Skip the current directory where this script has been launched.
  if [ "$CURRENT_DIRNAME" != "." ]
  then
    # Process directory

    echo "Process directory \"$CURRENT_DIRNAME\""
 
    # Move all content of the directoy in the parent directory
    mv ./$CURRENT_DIRNAME/* ./

    # Remove directory
    rm -fr ./$CURRENT_DIRNAME
  fi

done 



