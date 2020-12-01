#!/bin/bash

mkdir -p ThirdParty
cd ThirdParty

BLENDER_FOLDER=blender
QT_FOLDER=/opt/qt512

CURRENT_DIR=$(pwd)

# Blender v2.79 installing and unpacking
if [ -d "${CURRENT_DIR}/${BLENDER_FOLDER}" ]; then
    echo "Blender exists"
else
    echo "Blender does not exist"
    echo "downloading..."
    wget -O blender_x86_64.tar.bz2 https://download.blender.org/release/Blender2.79/blender-2.79b-linux-glibc219-x86_64.tar.bz2
    tar -xvjf blender_x86_64.tar.bz2
    mv ./blender-2.79b-linux-glibc219-x86_64 ./blender
    rm -f blender_x86_64.tar.bz2
fi

yes Y | sudo apt-get install libeigen3-dev libassimp-dev cmake libmagick++-dev libboost-all-dev git libgl-dev libfontconfig1 libfreetype6 libxcb1 libxkbcommon* libxkbcommon-x11-*

# checking Linux version
if [ -f /etc/os-release ]; then
    # freedesktop.org and systemd
    . /etc/os-release
    OS=$NAME
    VER=$VERSION_ID
elif type lsb_release >/dev/null 2>&1; then
    # linuxbase.org
    OS=$(lsb_release -si)
    VER=$(lsb_release -sr)
elif [ -f /etc/lsb-release ]; then
    # For some versions of Debian/Ubuntu without lsb_release command
    . /etc/lsb-release
    OS=$DISTRIB_ID
    VER=$DISTRIB_RELEASE
fi

if [ -d "$QT_FOLDER" ]; then
    echo "QT exists"
else
    echo "QT does not exist"
    echo "downloading..."
    if [ "$VER" = "18.04" ] && [ "$OS" = "Ubuntu" ]; then
    	echo "Ubuntu 18.04 is found!"
    	# Qt 5.12.3 Bionic release
		yes Y | sudo add-apt-repository ppa:beineri/opt-qt-5.12.3-bionic
		sudo apt-get update
		yes Y | sudo apt-get install qt512-meta-minimal		
	elif [ "$VER" = "16.04" ] && [ "$OS" = "Ubuntu" ]; then
		echo "Ubuntu 16.04 is found!"
		# Qt 5.12.3 Xenial release
		yes Y | sudo add-apt-repository ppa:beineri/opt-qt-5.12.3-xenial
		sudo apt-get update
		yes Y | sudo apt-get install qt512-meta-minimal
	else
    	echo "Unknown Linux version"
	fi

	USER_FOLDER=$(eval echo ~$USER)
	#echo $USER_FOLDER	

	if [ -f "${USER_FOLDER}/.config/qtchooser/default.conf" ]; then
    	echo "default.conf exists"
	else
		mkdir ~/.config/qtchooser
		touch ~/.config/qtchooser/default.conf
		printf "/opt/qt512/bin\n/opt/qt512/lib" >> ~/.config/qtchooser/default.conf
	fi
	
fi



