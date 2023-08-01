#!/bin/sh

install() {
	mkdir -p $HOME/.config/zathura
	cp zathurarc $HOME/.config/zathura/zathurarc
	
	mkdir -p $HOME/.config/dunst
	cp dunstrc $HOME/.config/dunst/dunstrc
	
	mkdir -p $HOME/.config/picom
	cp picom.conf $HOME/.config/picom/picom.conf
	
	mkdir -p $HOME/.config/qt5ct/colors
	cp qt5ct $HOME/.config/qt5ct/colors/Catpuccin-Mocha.conf
	
	cp Xresources $HOME/.Xresources
	cp -r Xresources.d $HOME/.Xresources.d/
	
	mkdir -p $HOME/.local/share/wallpapers
	cp wallpapers/*  $HOME/.local/share/wallpapers/
}

update() {
	cp $HOME/.config/zathura/zathurarc zathurarc

	cp $HOME/.config/dunst/dunstrc dunstrc

	cp $HOME/.config/picom/picom.conf picom.conf

	cp $HOME/.config/qt5ct/colors/Catpuccin-Mocha.conf qt5ct

	cp $HOME/.Xresources Xresources

	rm -r Xresources.d
	cp -r $HOME/.Xresources.d/ Xresources.d
}

uninstall() {
	rm $HOME/.config/zathura/zathurarc

	rm $HOME/.config/dunst/dunstrc

	rm $HOME/.config/picom/picom.conf

	rm $HOME/.config/qt5ct/colors/Catpuccin-Mocha.conf

	rm $HOME/.Xresources
	rm -r $HOME/.Xresources.d

	list="$(ls wallpapers)"
	for image in $list; do
		rm $HOME/.local/share/wallpapers/$image
	done
}

case $1 in
	-r|--uninstall)
		uninstall;;

	-u|--update)
		update;;

	-h|--help)
		echo "Usage: install.sh [-r | --uninstall] [-u | --update]"
		echo "default: install";;
	
	-*|--*)
		echo "Unknown option $1"
		exit 1;;
	*)
		install;;
esac
	

