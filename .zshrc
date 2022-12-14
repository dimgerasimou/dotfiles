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
plugins=(git zsh-syntax-highlighting zsh-autosuggestions sudo dirhistory copybuffer dirhistory)
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

# Git
alias gs="git status"
alias gp="git push"
alias gpull="git pull"
alias gc="git commit -m"
alias gclone="git clone"
alias gaa="git add -A"
alias ga="git add"
alias gd="git diff"
alias gca="git commit -a -m"

# Translation
alias λσ="ls"
alias ψδ="cd"

# 7zip
alias 7ze="7z a -p -mhe=on"

# dwm
alias dwm-build="cd $HOME/.local/src/dwm && ./install.sh"
alias dwm-config="$EDITOR $HOME/.local/src/dwm/src/config.h"
