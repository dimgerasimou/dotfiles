require('lualine').setup {
	options = { theme = 'rose-pine' }
}

require('nvim-treesitter.configs').setup {
        highlight = {
       	        enable = true,
		disable = { "latex" },
               	additional_vim_regex_highlighting = false,
       	},
       	-- rainbow = {
               	-- enable = true,
               	-- extended_mode = true,
               	-- max_file_lines = nil,
       	-- },
	context_commentstring = {
		enable = true
	},
}
require('gitsigns').setup()
require('treesitter-context').setup()
require('rose-pine').setup({
	---@usage 'main' | 'moon'
	dark_variant = 'moon',
	bold_vert_split = false,
	dim_nc_backgrond = false,
	disable_background = true,
	disable_float_background = true,
	disable_italics = false,
	---@usage string hex value or named color from rosepinetheme.com/palette
	groups = {
		background = 'base',
		panel = 'surface',
		border = 'highlight_med',
		comment= 'muted',
		link = 'iris',
		punctuation = 'subtle',
		error = 'love',
		hint = 'iris',
		info = 'foam',
		warn = 'gold',
		headings = {
			h1 = 'iris',
			h2 = 'foam',
			h3 = 'rose',
			h4 = 'gol',
			h5 = 'pine',
			h6 = 'foam',
		}
		-- or set all headings at once
		-- headings = 'subtle'
	},
	-- Change specific vim highlight groups
	highlight_groups = {
		ColorColumn = { bg = 'rose' } 
	}
})
require('Comment').setup()
require('alpha').setup(require('alpha.themes.startify').config)
