#!/bin/sh
#
while ! pgrep -f "AMS2AVX" > /dev/null; do
  sleep 1 # Wait for 1 second before checking again
done
echo "Automobilista 2 has started"
protontricks --no-runtime --background-wineserver -c "wine ~/git/simshmbridge/assets/pcars2handle.exe ~/git/simshmbridge/assets/pcars2handle" 1066890
