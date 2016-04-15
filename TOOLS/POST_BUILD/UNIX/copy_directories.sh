#!/bin/bash

#*****************************************************************************
# FUNCTION
#   Copy each requiered directory in build directory.
#
#   Do the following:
#     - Delete from build directory each required directory to copy (if exists).
#     - Copy each requiered directory in build directory.
#
# PARAMETERS
#  [IN] Name of directory which contains required directories to copy.
#  [IN] Name of build directory where requiered directories should be copied.
#
# USAGE
#   sh copy_directories.sh ../../.. ../../../../build-RomStation-Desktop_Qt_5_5_1_GCC_64bit-Debug
#
#*****************************************************************************


#*****************************************************************************
# CONSTANTS
#*****************************************************************************

# Get current directory from which this script is executed
CUR_DIR=`pwd`

# List of directories to copy in build directory
CTE_LIST_OF_DIR_TO_COPY="\
    package/icons \
    package/images \
    locales \
    package/platforms \
    styles"


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
  echo ""
  echo "#*****************************************************************************"
  echo "# FUNCTION"
  echo "#   Copy each requiered directory in build directory."
  echo "#"
  echo "#   Do the following:"
  echo "#     - Delete from build directory each required directory to copy (if exists)."
  echo "#     - Copy each directory in build directory."
  echo "#"
  echo "# PARAMETERS"
  echo "#  [IN] Name of directory which contains required directories to copy."
  echo "#  [IN] Name of build directory where requiered directories should be copied."
  echo "#"
  echo "# USAGE"
  echo "#   sh copy_directories.sh ../../.. ../../../../build-RomStation-Desktop_Qt_5_5_1_GCC_64bit-Debug"
  echo "#"
  echo "#*****************************************************************************"
  echo""
}


#*****************************************************************************
# START
#*****************************************************************************

# Check parameters
if [ "$1" == "" ]
then
  proc_print_usage
  exit
fi

if [ "$2" == "" ]
then
  proc_print_usage
  exit
fi

# Get directory which contains required directories to copy and check if exists
SRC_DIR_PATH="$1"
if [ ! -d "$SRC_DIR_PATH" ]
then
  echo "[copy_directory] Directory which contains requiered directories to copy do not exists: '$SRC_DIR_PATH' from $CUR_DIR"

  # Return non zero value for error
  exit 1
fi

# Get build directory where requiered directories should be copied and check if exists
DST_DIR_PATH="$2"
if [ ! -d "$DST_DIR_PATH" ]
then
  CUR_DIR=`pwd`
  echo "[copy_directory] Build directory do not exists: '$DST_DIR_PATH' from $CUR_DIR"

  # Return non zero value for error
  exit 2
fi

# Copy each requiered directory to build directory 
for dirToCopy in $CTE_LIST_OF_DIR_TO_COPY
do
  # Get full path of requiered directory to copy
  FULL_SRC_DIR_PATH="$SRC_DIR_PATH/$dirToCopy"

  # Get only name of directory to copy (not a full or partial path)
  dirToCopyBasename=$(basename $FULL_SRC_DIR_PATH)

  # Get full path of directory to create in build directory
  FULL_DST_DIR_PATH="$DST_DIR_PATH/$dirToCopyBasename"

  # Delete from build directory requiered directory to copy if exists
  if [ -d "$FULL_DST_DIR_PATH" ]
  then
    rm -fr "$FULL_DST_DIR_PATH"
  fi  

  # Copy requiered directory (if exists) to build directory
  if [ -d "$FULL_SRC_DIR_PATH" ] 
  then
    cp -fr "$FULL_SRC_DIR_PATH" "$FULL_DST_DIR_PATH"
    echo "[copy_directory] Directory '$FULL_SRC_DIR_PATH' copied to '$FULL_DST_DIR_PATH' "
  else
    echo "[copy_directory] Requiered direcory to copy do not exists: '$FULL_SRC_DIR_PATH'"    
  fi
done

exit
