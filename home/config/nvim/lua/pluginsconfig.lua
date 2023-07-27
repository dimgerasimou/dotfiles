-- Include outsorced setups
require('plugins.cokeline')
require('plugins.catppuccin')
require('plugins.lsp')
require('plugins.neotree')

vim.g.vimtex_view_method = 'zathura'

 -- Setup Session_manager
local session_manager_path = require('plenary.path')
local session_manager_config = require('session_manager.config')
require('session_manager').setup({
	sessions_dir = session_manager_path:new(vim.fn.stdpath('data'), 'sessions'),
	autoload_mode = session_manager_config.AutoloadMode.Disabled,
})


require('lualine').setup {
	options = {
		theme = "catppuccin"
	}
}
require('trouble').setup()
require('gitsigns').setup()
require('mini.bufremove').setup()
require('mini.ai').setup()
require('mini.pairs').setup()
require('nvim-treesitter.configs').setup {
	context_commentstring = {
		enable = true,
		enable_autocmd = false,
	},
	rainbow = {
    		enable = false,
    		query = 'rainbow-parens',
    		strategy = require('ts-rainbow').strategy.global,
  	},
}

require('Comment').setup {
	pre_hook = require('ts_context_commentstring.integrations.comment_nvim').create_pre_hook(),
}
require("indent_blankline").setup {
	space_char_blankline = " ",
	show_current_context = true,
	show_current_context_start = true,
}
require('smart-splits').setup({
	resize_mode = {
		hooks = {
			on_leave = require('bufresize').register,
		},
	},
})
require('bufresize').setup()

-- require('headlines').setup({
-- 	markdown = {
-- 		fat_headline_lower_string = "▀",
-- 	},
-- })

require('alpha').setup(require('plugins.alpha-theme').config)

require('telescope').setup {
  	extensions = {
		fzf = {
			fuzzy = true,
			override_generic_sorter = true,
			override_file_sorter = true,
			case_mode = "smart_case",
		},
		aerial = {
			show_nesting = {
				['_'] = false,
				json = true,
				yaml = true,
			}
		}
	},
	defaults = {
		mappings = {
			i = { ["<c-t>"] = require("trouble.providers.telescope").open_with_trouble },
			n = { ["<c-t>"] = require("trouble.providers.telescope").open_with_trouble },
		},
	},
}
require('telescope').load_extension('fzf')
require('aerial').setup({
  on_attach = function(bufnr)
    vim.keymap.set('n', '{', '<cmd>AerialPrev<CR>', {buffer = bufnr})
    vim.keymap.set('n', '}', '<cmd>AerialNext<CR>', {buffer = bufnr})
  end
})
