#  ____   ____ 
# |  _ \ / ___|    Dimitris Gerasimou (dimgerasimou)
# | | | | |  _     <https://github.com/dimgerasimou>
# | |_| | |_| |    
# |____/ \____|
#               
# Almost default zsh config with omz.

# ----------------------------------------------------------
#                  Oh My Zsh configuration
# ----------------------------------------------------------

# Main settings
export ZSH="$HOME/.oh-my-zsh"            # Path to your oh-my-zsh installation.
ZSH_THEME="spaceship"                    # Oh My Zsh theme.
zstyle ':omz:update' mode auto           # Oh My Zsh update mode.
zstyle ':omz:update' frequency 13        # Oh My Zsh update frequency in days.
DISABLE_AUTO_TITLE="false"               # Disable terminal autotitle.
ENABLE_CORRECTION="true"                 # Enable command autocorrection.

# Plugins
plugins=(git zsh-syntax-highlighting zsh-autosuggestions sudo dirhistory)
source $ZSH/oh-my-zsh.sh
# ----------------------------------------------------------
#                     Zsh configuration
# ----------------------------------------------------------

# Export ---------------------------------------------------
export TERM="st"
export EDITOR="nvim"
export VISUAL="vscodium"
export GPG_TTY=$(tty)
# Aliases --------------------------------------------------

# Navigation
alias ...="../.."
alias ....="../../.."
alias h="cd $HOME"
alias d="cd $HOME/Desktop"
alias home="cd $HOME"
alias desktop="cd $HOME/Desktop"

# Pacman
alias pmsyu="sudo pacman -Syu"
alias pms="sudo pacman -S"
alias pmr="sudo pacman -R"
alias pmq="pacman -Q"
alias pmqi="pacman -Qi"

# Neovim
alias nv="nvim"

# Emacs
alias em="emacs -nw"

# Git
alias config="/usr/bin/git --git-dir=$HOME/.cfg/ --work-tree=$HOME"
alias update-dotfiles="$HOME/.local/bin/git/update-dotfiles"
alias stat="git status"
alias push="git push"
alias pull="git pull"
alias commit="git commit -m"
alias clone="git clone"
alias addall="git add ."

# Translation
alias λσ="ls"
alias ψδ="cd"

# 7zip
alias 7ze="7z a -p -mhe=on"

# dwm
alias dwm-build="cd $HOME/.local/src/dwm && ./install.sh"
alias dwm-config="$EDITOR $HOME/.local/src/dwm/build/config.h"
