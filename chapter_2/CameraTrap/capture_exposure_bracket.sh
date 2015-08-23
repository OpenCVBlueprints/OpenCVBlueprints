#!/usr/bin/env bash

ev_step=1000
frames=3
while getopts s:f: flag; do
    case $flag in
        s)
            ev_step="$OPTARG"
            ;;
        f)
            frames="$OPTARG"
            ;;
        ?)
            exit
            ;;
    esac
done

min_ev=$((-ev_step * (frames - 1) / 2))
for ((i=0; i<frames; i++)); do
    ev=$((min_ev + i * ev_step))
    gphoto2 --set-config exposurecompensation=$ev \
        --capture-image
done
gphoto2 --set-config exposurecompensation=0
