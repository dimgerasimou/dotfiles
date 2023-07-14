#!/bin/sh

mkdir -p $HOME/.config/zathura
cp zathurarc $HOME/.config/zathura/zathurarc

mkdir -p $HOME/.config/dunst
cp dunstrc $HOME/.config/zathura/dunstrc

mkdir -p $HOME/.config/picom
cp picom.conf $HOME/.config/picom/picom.conf

cp Xresources $HOME/.Xresources

mkdir -p $HOME/.local/share/wallpapers
cp wallpapers/*  $HOME/.local/share/wallpapers/

