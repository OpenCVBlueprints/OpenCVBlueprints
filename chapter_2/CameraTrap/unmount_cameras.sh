#!/usr/bin/env bash

if [ "$(uname)" == "Darwin" ]; then
    killall PTPCamera
else
    mounted_cameras=`gvfs-mount -l | grep -Po 'gphoto2://.*/' | uniq`
    for mounted_camera in $mounted_cameras; do
        gvfs-mount -u $mounted_camera
    done
fi
