return require('packer').startup(function(use)

	use 'wbthomason/packer.nvim'
	use {
		'nvim-lualine/lualine.nvim',
		requires = { 'kyazdani42/nvim-web-devicons', opt = true }
	}
	use {
		'iamcco/markdown-preview.nvim',
		run = function() vim.fn["mkdp#util#install"]() end,
	}
	use {
		'nvim-treesitter/nvim-treesitter',
		run = function()
			local ts_update = require('nvim-treesitter.install').update({ with_sync = true })
			ts_update()
		end,
	}
	use {
		"nvim-neo-tree/neo-tree.nvim",
		branch = "v2.x",
		requires = { 
			"nvim-lua/plenary.nvim",
			"nvim-tree/nvim-web-devicons", 
			"MunifTanjim/nui.nvim",
		}
	}
	use 'p00f/nvim-ts-rainbow'
	use 'JoosepAlviste/nvim-ts-context-commentstring'	
	use 'numToStr/Comment.nvim' 
	use 'lervag/vimtex'
	use 'lewis6991/gitsigns.nvim'
	use 'nvim-treesitter/nvim-treesitter-context'
	use 'jbyuki/nabla.nvim'
	use 'Yazeed1s/minimal.nvim'
	use { 'rose-pine/neovim', as = 'rose-pine' }
	use 'terryma/vim-multiple-cursors'
	use 'nvim-tree/nvim-web-devicons'
	use {
		'goolord/alpha-nvim',
		requires = { 'nvim-tree/nvim-web-devicons'}
	}
end)
