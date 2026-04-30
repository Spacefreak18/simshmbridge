#!/usr/bin/env bash

findProtonPath() {
  local APPID=$1
  local CONFIG_FILE="$HOME/.local/share/Steam/config/config.vdf"

  if [ ! -f "$CONFIG_FILE" ]; then
    echo "Error: Steam config not found." >&2
    return 1
  fi

  local PROTON_NAME
  PROTON_NAME=$(awk '/"CompatToolMapping"/ {flag=1} flag && /"'$APPID'"/ {app=1} app && /"name"/ {print $2; exit}' "$CONFIG_FILE" | tr -d '"')

  if [ -z "$PROTON_NAME" ]; then
    echo "-1"
    return
  fi

  local CUSTOM_PATH="$HOME/.local/share/Steam/compatibilitytools.d/$PROTON_NAME"
  local OFFICIAL_PATH="$HOME/.local/share/Steam/steamapps/common/$PROTON_NAME"

  if [ -d "$CUSTOM_PATH" ]; then
    echo "$CUSTOM_PATH"
  elif [ -d "$OFFICIAL_PATH" ]; then
    echo "$OFFICIAL_PATH"
  else
    echo "-1"
  fi
}

echo -n "This script will install simshmbridge by compiling it and moving binaries to /opt/simshmbridge/. Continue [Y/n]? "
read -r continue

case "$continue" in
[nN]*) exit ;;
*) ;;
esac

echo 'Select the game for simshmbridge installation:'
echo '1) Assetto Corsa'
echo '2) Automobilista 2'
echo '3) LeMans Ultimate'
echo '4) RaceRoom'
read -r game

case $game in
1)
  ID="244210"
  MAKE_FILE="Makefile.ac"
  BIN="acbridge.exe"
  SHM="acshm"
  ;;
2)
  ID="1066890"
  MAKE_FILE="Makefile.pcars2"
  BIN="pcars2bridge.exe"
  SHM="pcars2shm"
  ;;
3)
  ID="2399420"
  MAKE_FILE="Makefile.lmu"
  BIN="lmubridge.exe"
  SHM="lmushm"
  ;;
4)
  ID="211500"
  MAKE_FILE="Makefile.r3e"
  BIN="r3ebridge.exe"
  SHM="r3eshm"
  ;;
*)
  echo "Invalid selection. Exiting..."
  exit 1
  ;;
esac

echo -e "\nEnsuring repository is synced...\n"
git submodule sync --recursive >/dev/null
git submodule update --init --recursive >/dev/null || {
  echo "Git update failed"
  exit 1
}

echo -e "Compiling simshmbridge for your game...\n"
if make -f "$MAKE_FILE" >/dev/null 2>&1; then
  echo "Compilation successful."
else
  echo "Compilation failed! Check your build dependencies."
  exit 1
fi

echo -e "Moving files to /opt/simshmbridge/...\n"
sudo mkdir -p /opt/simshmbridge/
sudo mv "./assets/$BIN" /opt/simshmbridge/
sudo mv "./assets/$SHM" /opt/simshmbridge/

protonPath=$(findProtonPath "$ID")

if [ "$protonPath" == "-1" ] || [ -z "$protonPath" ]; then
  echo "Warning: Could not find Proton path. Please locate your Proton 'proton' script manually."
  protonPath="[PATH_TO_PROTON]"
fi

echo "-------------------------------------------------------"
echo "Simshmbridge installed successfully!"
echo -e "\nPlease append the following to your Steam Launch Options:"
echo "&& sleep 5 && \"$protonPath/proton\" run /opt/simshmbridge/$BIN"
echo -e "\nIf you have no existing launch options, use this:"
echo "%%command%% && sleep 5 && \"$protonPath/proton\" run /opt/simshmbridge/$BIN"
echo -e "\nAlso before starting the game make sure to run /opt/simshmbridge/$SHM. You can also just make it run on startup."
echo "-------------------------------------------------------"
