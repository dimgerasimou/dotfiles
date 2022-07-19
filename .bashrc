#  ____   ____ 
# |  _ \ / ___|    Dimitris Gerasimou (dimgerasimou)
# | | | | |  _     <https://github.com/dimgerasimou>
# | |_| | |_| |    
# |____/ \____|
#               
# Simple Bash configuration. Nothing strange here.

# If not running interactively, don't do anything
[[ $- != *i* ]] && return

alias ls="ls --color=auto"
PS1="[\u@\h \W]\$ "
alias config="/usr/bin/git --git-dir=$HOME/.cfg/ --work-tree=$HOME"
alias update-dotfile="$HOME/.local/bin/git/update-dotfiles"

export QT_QPA_PLATFORMTHEME=qt5ct
