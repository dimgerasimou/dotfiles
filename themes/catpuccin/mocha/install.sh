#!/bin/sh

mkdir -p $HOME/.config/zathura
cp zathurarc $HOME/.config/zathura/zathurarc

mkdir -p $HOME/.config/dunst
cp dunstrc $HOME/.config/dunst/dunstrc

mkdir -p $HOME/.config/picom
cp picom.conf $HOME/.config/picom/picom.conf

mkdir -p $HOME/.config/qt5ct/colors
cp qt5ct $HOME/.config/qt5ct/colors/Catpuccin-Mocha.conf

cp Xresources $HOME/.Xresources

mkdir -p $HOME/.local/share/wallpapers
cp wallpapers/*  $HOME/.local/share/wallpapers/

