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
export TERM="alacritty"
export EDITOR="vim"
export VISUAL="vscodium"

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
alias pms="sudo pacma -S"
alias pmr="sudo pacman -R"

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
