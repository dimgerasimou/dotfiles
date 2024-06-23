-- Mason installer
require('mason').setup()
require('mason-lspconfig').setup()

require('luasnip').setup()
require('luasnip.loaders.from_lua').load({paths = '~/.config/nvim/lua/snippets/'})
require('luasnip').config.set_config({
	enable_autosnippets = true,
	store_selection_keys = '<Tab>',
	update_events = 'TextChanged,TextChangedI',
})

local cmp = require('cmp')
local lspkind = require('lspkind')
local capabilities = require('cmp_nvim_lsp').default_capabilities()
local ls = require('luasnip')

cmp.setup({
	snippet = {
		expand = function(args)
        		require('luasnip').lsp_expand(args.body)
		end,
	},
	mapping = {
		['<Up>'] = cmp.mapping.select_prev_item(select_opts),
		['<Down>'] = cmp.mapping.select_next_item(select_opts),

		['<C-p>'] = cmp.mapping.select_prev_item(select_opts),
		['<C-n>'] = cmp.mapping.select_next_item(select_opts),

		['<C-u>'] = cmp.mapping.scroll_docs(-4),
		['<C-d>'] = cmp.mapping.scroll_docs(4),

		['<C-e>'] = cmp.mapping.abort(),
		['<C-y>'] = cmp.mapping.confirm({select = true}),
		['<CR>'] = cmp.mapping.confirm({select = false}),

		['<C-f>'] = cmp.mapping(function(fallback)
			if ls.jumpable(1) then
				ls.jump(1)
      			else
        			fallback()
      			end
    		end, {'i', 's'}),

    		['<C-b>'] = cmp.mapping(function(fallback)
      			if ls.jumpable(-1) then
       				ls.jump(-1)
      			else
        			fallback()
      			end
    		end, {'i', 's'}),

		-- ['<Tab>'] = cmp.mapping(function(fallback)
		-- 	local col = vim.fn.col('.') - 1
		--
		-- 	if cmp.visible() then
		-- 		cmp.select_next_item(select_opts)
		-- 	elseif ls.jumpable(1) then
		-- 		ls.jump(1)
		-- 	elseif col == 0 or vim.fn.getline('.'):sub(col, col):match('%s') then
		-- 		fallback()
		-- 	else
		-- 		cmp.complete()
		-- 	end
		-- end, {'i', 's'}),
		--
		-- ['<S-Tab>'] = cmp.mapping(function(fallback)
		-- 	if cmp.visible() then
		-- 		cmp.select_prev_item(select_opts)
		-- 	elseif ls.jumpable(-1) then
		-- 		ls.jump(-1)
		-- 	else
		-- 		fallback()
		-- 	end
		-- end, {'i', 's'}),
	},
  	sources = cmp.config.sources({
		{ name = 'nvim_lsp' },
		{ name = 'luasnip' },
      		{ name = 'calc' },
      		{ name = 'greek' },
      		{ name = 'emoji' },
      		{ name = 'nvim_lua' },
      		{ name = 'treesitter' },
		{
			name = 'latex_symbols',
			option = { strategy = 2 },
		}, {
			{ name = 'buffer' },
		}
	}),
	enabled = function()
		-- disable completion in comments
		local context = require 'cmp.config.context'
		-- keep command mode completion enabled when cursor is in a comment
		if vim.api.nvim_get_mode().mode == 'c' then
			return true
		else
			return not context.in_treesitter_capture("comment")
			       and not context.in_syntax_group("Comment")
		end
	end,
	formatting = {
		format = lspkind.cmp_format({
			mode = 'symbol',
			maxwidth = 50,
			ellipsis_char = '...',
		})
	},
	sorting = {
		comparators = {
        		cmp.config.compare.offset,
			cmp.config.compare.exact,
			cmp.config.compare.recently_used,
			require("clangd_extensions.cmp_scores"),
			cmp.config.compare.kind,
			cmp.config.compare.sort_text,
			cmp.config.compare.length,
			cmp.config.compare.order,
		},
	},
	window = {
		documentation = cmp.config.window.bordered()
	},
})

cmp.setup.filetype('gitcommit', {
	sources = cmp.config.sources({
		{ name = 'git' },
	}, {
		{ name = 'buffer' },
	})
})

cmp.setup.cmdline({ '/', '?' }, {
	mapping = cmp.mapping.preset.cmdline(),
	sources = {
		{ name = 'buffer' }
	}
})

cmp.setup.cmdline(':', {
	mapping = cmp.mapping.preset.cmdline(),
	sources = cmp.config.sources({
		{ name = 'path' }
	}, {
		{ name = 'cmdline' }
	})
})

-- Language servers
require("clangd_extensions").setup {
	server = {
		capabilities = capabilities
	}
}
require('lspconfig').jdtls.setup{
	capabilities = capabilities
}
require('lspconfig').ltex.setup{
	capabilities = capabilities
}
require('lspconfig').marksman.setup{
	capabilities = capabilities
}
require('lspconfig').lua_ls.setup{
	capabilities = capabilities
}
require('lspconfig').bashls.setup{
	capabilities = capabilities
}
require('lspconfig').html.setup{
	capabilities = capabilities
}
require('lspconfig').pylsp.setup{
	capabilities = capabilities
}
require('lspconfig').neocmake.setup{
	capabilities = capabilities
}


