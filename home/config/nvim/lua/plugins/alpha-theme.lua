local if_nil = vim.F.if_nil

local default_terminal = {
    type = "terminal",
    command = nil,
    width = 69,
    height = 8,
    opts = {
        redraw = true,
        window_config = {},
    },
}

local default_header = {
    type = "text",
    -- val = {
    --     [[                                  __]],
    --     [[     ___     ___    ___   __  __ /\_\    ___ ___]],
    --     [[    / _ `\  / __`\ / __`\/\ \/\ \\/\ \  / __` __`\]],
    --     [[   /\ \/\ \/\  __//\ \_\ \ \ \_/ |\ \ \/\ \/\ \/\ \]],
    --     [[   \ \_\ \_\ \____\ \____/\ \___/  \ \_\ \_\ \_\ \_\]],
    --     [[    \/_/\/_/\/____/\/___/  \/__/    \/_/\/_/\/_/\/_/]],
    -- },
    val = {
	    [[       _                        ]],
	    [[       \`*-.                    ]],
	    [[        )  _`-.                 ]],
	    [[       .  : `. .                ]],
	    [[       : _   '  \               ]],
	    [[       ; *` _.   `*-._          ]],
	    [[       `-.-'          `-.       ]],
	    [[         ;       `       `.     ]],
	    [[         :.       .        \    ]],
	    [[         . \  .   :   .-'   .   ]],
	    [[         '  `+.;  ;  '      :   ]],
	    [[         :  '  |    ;       ;-. ]],
	    [[         ; '   : :`-:     _.`* ;]],
	    [[      .*' /  .*' ; .*`- +'  `*' ]],
	    [[      `*-*   `*-*  `*-*'        ]],

    },
    opts = {
        position = "center",
        hl = "Include",
        -- wrap = "overflow";
    },
}

local footer = {
    type = "text",
    val = '\nNow I will have less distraction.\n- Leonhard Euler',
    opts = {
        position = "center",
        hl = "Type",
    },
}

local leader = "SPC"

--- @param sc string
--- @param txt string
--- @param keybind string? optional
--- @param keybind_opts table? optional
local function button(sc, txt, keybind, keybind_opts)
    local sc_ = sc:gsub("%s", ""):gsub(leader, "<leader>")

    local opts = {
        position = "center",
        shortcut = sc,
        cursor = 3,
        width = 50,
        align_shortcut = "right",
        hl_shortcut = "Structure",
	hl = "Array"
    }
    if keybind then
        keybind_opts = if_nil(keybind_opts, { noremap = true, silent = true, nowait = true })
        opts.keymap = { "n", sc_, keybind, keybind_opts }
    end

    local function on_press()
        local key = vim.api.nvim_replace_termcodes(keybind or sc_ .. "<Ignore>", true, false, true)
        vim.api.nvim_feedkeys(key, "t", false)
    end

    return {
        type = "button",
        val = txt,
        on_press = on_press,
        opts = opts,
    }
end

local buttons = {
    type = "group",
    val = {
        button("e", "  New", "<cmd>ene <BAR> startinsert <CR>"),
        button("f", "  Open", ":lua require('telescope.builtin').find_files()<CR>"),
        button("r", "  Recents", ":lua require('telescope.builtin').oldfiles()<CR>"),
        button("g", "  Find text", ":lua require('telescope.builtin').live_grep({initial_mode = 'insert'})<CR>"),
        button("l", "  Restore", "<cmd>SessionManager load_last_session<CR>"),
	button("c", "  Configuration", "<cmd>cd ~/.config/nvim <BAR> e ~/.config/nvim/init.lua <BAR> Neotree <CR>"),
	button("p", "  Plugins", "<cmd>PackerUpdate<CR>"),
	button("m", "󰺾  Mason", "<cmd>Mason<CR>"),
	button("q", "  Quit", "<cmd>qa<CR>"),
	},
    opts = {
        spacing = 1,
    },
}

local section = {
    terminal = default_terminal,
    header = default_header,
    buttons = buttons,
    footer = footer,
}

local config = {
    layout = {
        { type = "padding", val = 1 },
        section.header,
        { type = "padding", val = 2 },
        section.buttons,
	{ type = "padding", val = 2 },
        section.footer,
    },
    opts = {
        margin = 0,
    },
}

return {
    button = button,
    section = section,
    config = config,
    -- theme config
    leader = leader,
    -- deprecated
    opts = config,
}
