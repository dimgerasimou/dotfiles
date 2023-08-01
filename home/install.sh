#!/bin/sh

configs="dmenu easyeffects nvim"

install() {
	mkdir -p $HOME/.config/

	for config in $configs; do
		cp -r config/$config $HOME/.config/$config
	done
	
	cp bashrc $HOME/.bashrc

	cp zshrc $HOME/.zshrc
}

update() {
	for config in $configs; do
		rm -rf config/$config
		cp -r $HOME/.config/$config config/$config
	done
	
	cp $HOME/.bashrc bashrc

	cp $HOME/.zshrc zshrc
}

uninstall() {
	for config in $configs; do
		rm -rf $HOME/.config/$config
	done
	
	rm -f $HOME/.bashrc

	rm -f $HOME/.zshrc
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
