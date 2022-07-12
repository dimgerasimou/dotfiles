#  ____   ____ 
# |  _ \ / ___|    Dimitrs Gerasimou (dimgerasimou)
# | | | | |  _     <dimgerasimou@gmail.com>
# | |_| | |_| |    https://github.com/dimgerasimou
# |____/ \____|
#               
# Simple Bash configuration. Nothing strange here.

# If not running interactively, don't do anything
[[ $- != *i* ]] && return

alias ls='ls --color=auto'
PS1='[\u@\h \W]\$ '
alias config='/usr/bin/git --git-dir=/home/dimgerasimou/.cfg/ --work-tree=/home/dimgerasimou'
