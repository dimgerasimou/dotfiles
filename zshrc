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
export VISUAL="nvim"
export GPG_TTY=$(tty)

# Aliases --------------------------------------------------

# Navigation
alias ...="../.."
alias ....="../../.."
alias h="cd $HOME"
alias d="cd $HOME/Desktop"
alias D="cd $HOME/Downloads"
alias home="cd $HOME"
alias desktop="cd $HOME/Desktop"
alias downloads="cd $HOME/Downloads"

# Pacman
alias pmsyu="sudo pacman -Syu"
alias pms="sudo pacman -S"
alias pmr="sudo pacman -R"
alias pmq="pacman -Q"
alias pmqi="pacman -Qi"

# Neovim
alias nv="nvim"
alias v="nvim"

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

# cups
alias cups-web="firefox https://localhost:631/"

# zathura
alias z="zathura"

# vscodium
alias vs="vscodium"


n ()
{
    # Block nesting of nnn in subshells
    if [[ "${NNNLVL:-0}" -ge 1 ]]; then
        echo "nnn is already running"
        return
    fi

    # The behaviour is set to cd on quit (nnn checks if NNN_TMPFILE is set)
    # If NNN_TMPFILE is set to a custom path, it must be exported for nnn to
    # see. To cd on quit only on ^G, remove the "export" and make sure not to
    # use a custom path, i.e. set NNN_TMPFILE *exactly* as follows:
    #     NNN_TMPFILE="${XDG_CONFIG_HOME:-$HOME/.config}/nnn/.lastd"
    export NNN_TMPFILE="${XDG_CONFIG_HOME:-$HOME/.config}/nnn/.lastd"

    # Unmask ^Q (, ^V etc.) (if required, see `stty -a`) to Quit nnn
    # stty start undef
    # stty stop undef
    # stty lwrap undef
    # stty lnext undef

    # The backslash allows one to alias n to nnn if desired without making an
    # infinitely recursive alias
    \nnn "$@"

    if [ -f "$NNN_TMPFILE" ]; then
            . "$NNN_TMPFILE"
            rm -f "$NNN_TMPFILE" > /dev/null
    fi
}

#NNN
export NNN_PLUG='i:imgview;m:nmount;d:dragdrop;x:xdgdefault;t:preview-tabbed'
export NNN_FIFO=/tmp/nnn.fifo
