#!/bin/bash

#*****************************************************************************
# FUNCTION
#   Set an icon for the application in Gnome environment.
#
#   Do the following:
#     - Create desktop file.
#     - Copy destop file an icon file in relevant directories.
#
# PARAMETERS
#  [IN] Name of application for which an icon must be created
#  [IN] Name of the icon file
#  [IN] Type of paths in desktop file to create:
#         - a for absolute paths
#         - r for relative paths
#  [IN] Directory where application has been installed (executable and icon)
#
# USAGE
#   sh set_icon_gnome.sh my_appli my_icon.jpg a /home/my_dir/my_apply
#
#*****************************************************************************


#*****************************************************************************
# CONSTANTS
#*****************************************************************************
# Desktop specification used
CTE_DESKTOP_VERSION="1.0"

# Return error codes
CTE_ERR_DIR_DESKTOP_NOT_FOUND=1
CTE_ERR_DIR_ICON_NOT_FOUND=2
CTE_ERR_WRONG_USAGE=3
CTE_ERR_COPY_DESKTOP=4
CTE_ERR_COPY_ICON=5
CTE_ERR_ICON_NOT_FOUND=6

# Pattern to print when an error occurs
CTE_PATTERN_ERR="[set_icon_gnome][ERROR]"

# Pattern to print when an information must be indicated
CTE_PATTERN_INF="[set_icon_gnome][INFO]"


#*****************************************************************************
# GLOBAL VARIABLES
#*****************************************************************************
# Script parameters
PARAM_APP_NAME=""
PARAM_ICON_NAME=""
PARAM_PATH_TYPE=""
PARAM_APP_PATH=""

# Application properties
APP_CATEGORIE=""
APP_COMMENT=""
APP_EXEC=""
APP_GENERIC_NAME=""
APP_ICON=""
APP_NAME=""
APP_PATH=""
APP_TERMINAL=""
APP_TYPE=""
APP_DESKTOP_FILENAME=""

# Directories where flies must be copied to set icon
DIR_DESKTOP_FILE=""
DIR_ICON_FILE=""


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
  echo "Usage is the following:"
  echo "#*****************************************************************************"
  echo "# FUNCTION"
  echo "#   Set an icon for the application in Gnome environment."
  echo "#"
  echo "#   Do the following:"
  echo "#     - Create desktop file."
  echo "#     - Copy destop file an icon file in relevant directories."
  echo "#"
  echo "# PARAMETERS"
  echo "#  [IN] Name of application for which an icon must be created"
  echo "#  [IN] Name of the icon file"
  echo "#  [IN] Type of paths in desktop file to create:"
  echo "#         - a for absolute paths"
  echo "#         - r for relative paths"
  echo "#  [IN] Directory where application has been installed (executable and icon)"
  echo "#"
  echo "# USAGE"
  echo "#   sh set_icon_gnome.sh my_appli my_icon.jpg a /home/my_dir/my_apply"
  echo "#"
  echo "#*****************************************************************************"
  echo""
}

##
# proc_check_parameters
# 
# FUNCTION 
#  Check if parameters required for this script has been correctly entered.
# 
# PARAMETERS
#  None
#
# RETURN
#  None
#
proc_check_parameters()
{
  # Check that application name has been entered
  if [ "$PARAM_APP_NAME" == "" ]
  then
    # No entered, print usage and return non zero value for error
    echo "$CTE_PATTERN_ERR Parameter 'application name' is missing."
    proc_print_usage
    proc_exit $CTE_ERR_WRONG_USAGE
  fi

  # Check that icon name has been entered
  if [ "$PARAM_ICON_NAME" == "" ]
  then
    # No entered, print usage and return non zero value for error
    echo "$CTE_PATTERN_ERR Parameter 'icon name' is missing."
    proc_print_usage
    proc_exit $CTE_ERR_WRONG_USAGE
  fi

  # Check that type of paths has been entered
  if [ "$PARAM_PATH_TYPE" == "" ]
  then
    # No entered, print usage and return non zero value for error
    echo "$CTE_PATTERN_ERR Parameter 'path type' is missing."
    proc_print_usage
    proc_exit $CTE_ERR_WRONG_USAGE
  fi

  # Check that type of paths has an authorized value
  if [ "$PARAM_PATH_TYPE" != "a" ] &&  [ "$PARAM_PATH_TYPE" != "r" ] 
  then
    # Invalid value entered, print usage and return non zero value for error
    echo "$CTE_PATTERN_ERR Parameter 'path type' with value '$PARAM_PATH_TYPE' is not valid, it must be 'a' or 'r'."
    proc_print_usage
    proc_exit $CTE_ERR_WRONG_USAGE
  fi

  # Check that application path has been entered
  if [ "$PARAM_APP_PATH" == "" ]
  then
    # No entered, print usage and return non zero value for error
    echo "$CTE_PATTERN_ERR Parameter 'application path' is missing."
    proc_print_usage
    proc_exit $CTE_ERR_WRONG_USAGE
  fi

  # Check if application path exists 
  if [ ! -d "$PARAM_APP_PATH" ]
  then
    # Directory do not exist, print usage and return non zero value for error
    echo "$CTE_PATTERN_ERR Directory '$PARAM_APP_PATH' do not exist."
    proc_print_usage
    proc_exit $CTE_ERR_WRONG_USAGE
  fi
}


##
# proc_set_app_properties
# 
# FUNCTION 
#  Stock in global variables the application properties needed to create an 
#  icon. These are required to build the desktop file. 
# 
# PARAMETERS
#  None
#
# RETURN
#  None
#
proc_set_app_properties()
{
  # Stock application properties to write in desktop file.
  APP_CATEGORIE="Game"
  APP_COMMENT="Rom organizer with pretty interface"
  APP_EXEC="$PARAM_APP_PATH/$PARAM_APP_NAME"
  APP_GENERIC_NAME="Rom organizer"
  APP_NAME="$PARAM_APP_NAME"
  APP_PATH="$PARAM_APP_PATH"
  APP_TERMINAL="false"
  APP_TRY_EXEC="$PARAM_APP_PATH/$PARAM_APP_NAME"
  APP_TYPE="Application"

  # Stock application properties dependent on the type of path
  # Note that only icon path is concerned with the type of path.
  # This was done to avoid modification of linux $PATH for the 
  # other properties (exec, try_exec)
  if [ "$PARAM_PATH_TYPE" == "a" ]
  then
    # Absolute path must be indicated
    APP_ICON="$PARAM_APP_PATH/$PARAM_ICON_NAME"
  else
    # Relative path must be indicated
    APP_ICON="$PARAM_ICON_NAME"
  fi

  # Desktop file name to create
  APP_DESKTOP_FILENAME="$PARAM_APP_NAME.desktop"
}

##
# proc_build_desktop
# 
# FUNCTION 
#  Build desktop file with properties previoulsy set in 
#  proc_set_app_properties.
# 
# PARAMETERS
#  None
#
# RETURN
#  None
# 
proc_build_desktop()
{
  # Delete previously version of destop file to create
  if [ -f "$APP_DESKTOP_FILENAME" ]
  then
    rm -fr $APP_DESKTOP_FILENAME
  fi

  # Build file
  echo "[Desktop Entry]"               >> $APP_DESKTOP_FILENAME
  echo "Version=$CTE_DESKTOP_VERSION"  >> $APP_DESKTOP_FILENAME
  echo "Type=$APP_TYPE"                >> $APP_DESKTOP_FILENAME
  echo "Name=$APP_NAME"                >> $APP_DESKTOP_FILENAME
  echo "GenericName=$APP_GENERIC_NAME" >> $APP_DESKTOP_FILENAME
  echo "Comment=$APP_COMMENT"          >> $APP_DESKTOP_FILENAME
  echo "TryExec=$APP_TRY_EXEC"         >> $APP_DESKTOP_FILENAME
  echo "Exec=$APP_EXEC"                >> $APP_DESKTOP_FILENAME
  echo "Icon=$APP_ICON"                >> $APP_DESKTOP_FILENAME
  echo "Categories=$APP_CATEGORIE;"    >> $APP_DESKTOP_FILENAME
  echo "Terminal=$APP_TERMINAL"        >> $APP_DESKTOP_FILENAME
  echo "Path=$APP_PATH"                >> $APP_DESKTOP_FILENAME
}

##
# proc_set_env_properties
# 
# FUNCTION 
#  Stock in global variables the directories where files must be copied to 
#  set an icon in this environment.
#  Files to copied are the following: 
#    - Desktop file.
#    - Icon file.
# 
# PARAMETERS
#  None
#
# RETURN
#  None
#
proc_set_env_properties()
{
  # Set directories where files must be copied
  DIR_DESKTOP_FILE="/usr/share/applications"
  DIR_ICON_FILE="/usr/share/pixmaps"
}

##
# proc_copy_files
# 
# FUNCTION 
#  Copy filed in directories previously set in proc_set_env_properties.
#  Files to copied are the following: 
#    - Desktop file.
#    - Icon file.
#
# PARAMETERS
#  None
#
# RETURN
#  None
# 
proc_copy_files()
{
  # Check existence of directory where desktop file must be copied.
  if [ ! -d "$DIR_DESKTOP_FILE" ]
  then
    echo "$CTE_PATTERN_ERR Directory where desktop file must be copied do not exists: '$DIR_DESKTOP_FILE'"

    # Return non zero value for error
    proc_exit $CTE_ERR_DIR_DESKTOP_NOT_FOUND
  fi

  # If relative paths are used in desktop file
  if [ "$PARAM_PATH_TYPE" == "r" ]
  then
    # Check existence of directory where icon file must be copied.
    if [ ! -d "$DIR_ICON_FILE" ]
    then
      echo "$CTE_PATTERN_ERR Directory where icon file must be copied do not exists: '$DIR_ICON_FILE'"

      # Return non zero value for error
      proc_exit $CTE_ERR_DIR_ICON_NOT_FOUND
    fi
  fi

  # Copy desktop file
  mv -f $APP_DESKTOP_FILENAME $DIR_DESKTOP_FILE

  # Control copy
  if [ "$?" -ne "0" ]
  then
    # Failure
    echo "$CTE_PATTERN_ERR Unable to copy '$APP_DESKTOP_FILENAME' in directory '$DIR_DESKTOP_FILE'."
    echo "Check write permission, try to execute this script with root privilege (sudo...)"

    # Return non zero value for error
    proc_exit $CTE_ERR_COPY_DESKTOP
  else
    # Success
    echo "$CTE_PATTERN_INF File '$APP_DESKTOP_FILENAME' has been copied in directory '$DIR_DESKTOP_FILE'."
  fi

  # If relative paths are used in desktop file
  if [ "$PARAM_PATH_TYPE" == "r" ]
  then

    # Check icon file existence
    if [ ! -f "$PARAM_APP_PATH/$PARAM_ICON_NAME" ]
    then
      # Failure
      echo "$CTE_PATTERN_ERR Icon file '$PARAM_APP_PATH/$PARAM_ICON_NAME' do not exists."

      # Return non zero value for error
      proc_exit $CTE_ERR_ICON_NOT_FOUND
    fi

    # Copy icon file
    cp -f "$PARAM_APP_PATH/$PARAM_ICON_NAME" $DIR_ICON_FILE

    # Control copy
    if [ "$?" -ne "0" ]
    then
      # Failure
      echo "$CTE_PATTERN_ERR Unable to copy '$PARAM_APP_PATH/$PARAM_ICON_NAME' in directory '$DIR_ICON_FILE'."
      echo "Check write permission, try to execute this script with root privilege (sudo...)"

      # Return non zero value for error
      proc_exit $CTE_ERR_COPY_ICON
    else
      # Success
      echo "$CTE_PATTERN_INF File '$PARAM_APP_PATH/$PARAM_ICON_NAME' has been copied in directory '$DIR_ICON_FILE'."
    fi
  fi
}

##
# proc_exit
# 
# FUNCTION 
#  Cleanup created files and quit this script with code given in parameter
#
# PARAMETERS
#  [IN] Exit code
#
# RETURN
#  None
#
proc_exit()
{
  # Remove desktop file (if created)
  if [ -f "$APP_DESKTOP_FILENAME" ]
  then
    rm -f $APP_DESKTOP_FILENAME
  fi

  # Exit with code given in parameter
  exit $1
}

#*****************************************************************************
# START
#*****************************************************************************

# Set parameters value
PARAM_APP_NAME=$1
PARAM_ICON_NAME=$2
PARAM_PATH_TYPE=$3
PARAM_APP_PATH=$4

# Check parameters
proc_check_parameters

# Set application properties
proc_set_app_properties

# Build application desktop file with these properties
proc_build_desktop

# Set environment properties
proc_set_env_properties

# Copy matching files to gnome setting icon
proc_copy_files

# Success
echo "$CTE_PATTERN_INF Icon successfully set."
exit 0
