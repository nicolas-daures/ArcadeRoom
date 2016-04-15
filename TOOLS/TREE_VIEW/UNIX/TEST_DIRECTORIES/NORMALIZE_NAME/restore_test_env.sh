#!/bin/bash

#*****************************************************************************
# FUNCTION
#   Retore environment to test normalize_name.sh script by creating files 
#   with various characters and patterns to delete or replace in their name.
#   It must be launched in the test directory where normalize_name.sh script 
#   is executed. 
#
#   Do the following:
#     - Delete all previous created test files.
#     - Create test files.
#
#   Note that each file created by this script contains a specific pattern 
#   in this name (to easily delete it).
#
# PARAMETERS
#   None.
#
# USAGE
#   sh restore_test_env.sh
#
#*****************************************************************************


#*****************************************************************************
# CONSTANTS
#*****************************************************************************

# Define specific pattern of each file created by thi script 
CTE_SPECIFIC_FILE_PATTERN="testfile"

# Define name of zip files to create (without zip extension)
CTE_LIST_OF_ZIP_FILES_TO_CREATE="\
    oneEmptyParenthesis() \
    oneFullParenthesis(d4!) \
    two(del)Full(ete)Parenthesis \
    oneEmptyBrackets[] \
    oneFullBrackets[8aà@] \
    two[rem]Full[ove]Brackets \
    oneFullParenthesis(del)And[ete]Brackets \
    move-thePattern-the \
    move_THEPattern_THE \
    noMoveThePatternThe"


#*****************************************************************************
# GLOBAL VARIABLES
#*****************************************************************************

# Number of created rom test files
VAR_CREATED_ROM_FILE_NUMBER=0


#*****************************************************************************
# INTERNAL PROCEDURES
#*****************************************************************************

# proc_create_a_rom_file
# 
# FUNCTION 
#  Create a rom file with a specific pattern in its name.
#  Create matching cover file with extension given in parameter.
# 
# PARAMETERS
#  [IN] Name of the rom file to create
#  [IN] Extension of the rom file 
#  [IN] Extension of the matching cover file 
#
# RETURN
#  None
#
proc_create_a_rom_file()
{
  # Update number of created rom file
  VAR_CREATED_ROM_FILE_NUMBER=`expr ${VAR_CREATED_ROM_FILE_NUMBER} + 1`

  # Define filename to create without extension
  ROM_ROM_FILENAME_TO_CREATE_NO_EXT="$CTE_SPECIFIC_FILE_PATTERN${VAR_CREATED_ROM_FILE_NUMBER}_$1"

  # Define filename to create (with extension)
  ROM_FILENAME_TO_CREATE="$ROM_ROM_FILENAME_TO_CREATE_NO_EXT.$2"

  # Create rom file if do not already exists
  if [ ! -f "$ROM_FILENAME_TO_CREATE" ]
  then
    touch "$ROM_FILENAME_TO_CREATE"
  fi

  # Define matching cover filename to create (with extension)
  COVER_FILENAME_TO_CREATE="$ROM_ROM_FILENAME_TO_CREATE_NO_EXT.$3"

  # Create matching cover file if do not already exists
  if [ ! -f "$COVER_FILENAME_TO_CREATE" ]
  then
    touch "$COVER_FILENAME_TO_CREATE"
  fi

}


#*****************************************************************************
# START
#*****************************************************************************

# Purge previous created rom test files
rm -f *$CTE_SPECIFIC_FILE_PATTERN*

# Create rom test files
for file_to_create in $CTE_LIST_OF_ZIP_FILES_TO_CREATE
do
  proc_create_a_rom_file $file_to_create zip jpg
done

# Create specific rom test files 
#   + With space character in the name
proc_create_a_rom_file "two Spaces " zip jpg
proc_create_a_rom_file "moveSpaceThePattern the" zip jpg
proc_create_a_rom_file "noMoveSpace ThePattern" zip jpg

#   + With various extension for rom
proc_create_a_rom_file "rom[deleteThisPattern]zip" zip jpg
proc_create_a_rom_file "rom[deleteThisPattern]ZIP" ZIP jpg
proc_create_a_rom_file "rom[noDeleteThisPattern]zIp" zIp jpg

#   + With various extensions for cover
proc_create_a_rom_file "cover[deleteThisPattern]jpg" zip jpg
proc_create_a_rom_file "cover[deleteThisPattern]JPG" zip JPG
proc_create_a_rom_file "cover[deleteThisPattern]png" zip png
proc_create_a_rom_file "cover[deleteThisPattern]PNG" zip PNG
proc_create_a_rom_file "noNormalize[noDeleteThisPattern]UnauthorizedCoverExtensionjpG" zip jpG

