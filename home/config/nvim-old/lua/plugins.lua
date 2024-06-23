return require('packer').startup(function(use)
	use 'wbthomason/packer.nvim'

	use "nvim-lua/plenary.nvim"
	use "nvim-tree/nvim-web-devicons"
	use "MunifTanjim/nui.nvim"
	use {
		'nvim-lualine/lualine.nvim',
		requires = { 'kyazdani42/nvim-web-devicons', opt = true }
	}
	use { "iamcco/markdown-preview.nvim", run = "cd app && npm install", setup = function() vim.g.mkdp_filetypes = { "markdown" } end, ft = { "markdown" }, }
	use { "catppuccin/nvim", as = "catppuccin" }
	use {
		'goolord/alpha-nvim',
		requires = { 'nvim-tree/nvim-web-devicons' },
	}
	-- use {
 --        	'lukas-reineke/headlines.nvim',
 --        	after = 'nvim-treesitter',
 --        }
	use 'jbyuki/nabla.nvim'
	use 'lervag/vimtex'
	use 'numToStr/Comment.nvim'
	use 'lewis6991/gitsigns.nvim'
	use 'terryma/vim-multiple-cursors'
	use 'folke/trouble.nvim'
	use 'tpope/vim-surround'
	use 'nvim-treesitter/nvim-treesitter'
	use {
		"nvim-neo-tree/neo-tree.nvim",
		branch = "v3.x",
		requires = {
			"nvim-lua/plenary.nvim",
			"nvim-tree/nvim-web-devicons",
			"MunifTanjim/nui.nvim",
		}
	}
	use 'echasnovski/mini.nvim'
	use {
		"nvim-treesitter/nvim-treesitter-textobjects",
		after = "nvim-treesitter",
		requires = "nvim-treesitter/nvim-treesitter",
	}
	use {
		'JoosepAlviste/nvim-ts-context-commentstring',
		requires = 'nvim-treesitter/nvim-treesitter',
	}
	use {
		"akinsho/toggleterm.nvim", tag = '*',
		config = function()
  			require("toggleterm").setup()
		end
	}
	use "lukas-reineke/indent-blankline.nvim"
	use {
		"folke/which-key.nvim",
		config = function()
			vim.o.timeout = true
			vim.o.timeoutlen = 300
		require("which-key").setup()
		end
	}
	use {
		"max397574/better-escape.nvim",
		config = function()
			require("better_escape").setup()
		end
	}
	use 'mrjones2014/smart-splits.nvim'
	use "kwkarlwang/bufresize.nvim"
	use {
		'neovim/nvim-lspconfig',
		"williamboman/mason.nvim",
		"williamboman/mason-lspconfig.nvim",
	}
	use 'hrsh7th/cmp-nvim-lsp'
	use 'hrsh7th/cmp-buffer'
	use 'hrsh7th/cmp-path'
	use 'hrsh7th/cmp-cmdline'
	use {
		'hrsh7th/nvim-cmp',
		requires = {
			'kdheepak/cmp-latex-symbols'
		}
	}
	use 'hrsh7th/cmp-calc'
	use 'max397574/cmp-greek'
	use 'saadparwaiz1/cmp_luasnip'
	use 'hrsh7th/cmp-emoji'
	use 'ray-x/cmp-treesitter'
	use 'hrsh7th/cmp-nvim-lua'
	use 'onsails/lspkind.nvim'
	use 'p00f/clangd_extensions.nvim'
	use {
		'willothy/nvim-cokeline',
		requires = {
			"nvim-lua/plenary.nvim",
			"kyazdani42/nvim-web-devicons",
    		},
	}
	use 'HiPhish/nvim-ts-rainbow2'
	use 'Shatur/neovim-session-manager'
	use {
		"L3MON4D3/LuaSnip",
		run = "make install_jsregexp"
	}
	use {
		'nvim-telescope/telescope.nvim', tag = '0.1.2',
		requires = { {'nvim-lua/plenary.nvim'} }
	}
	use { 'nvim-telescope/telescope-fzf-native.nvim', run = 'make' }
	use 'stevearc/aerial.nvim'
	use {
		's1n7ax/nvim-window-picker',
		tag = 'v2.*',
       }
end)
