-- Mason installer
require('mason').setup()
require('mason-lspconfig').setup()

local cmp = require('cmp')
local lspkind = require('lspkind')
local capabilities = require('cmp_nvim_lsp').default_capabilities()
require('cmp').setup({
	snippet = {
		expand = function(args)
        		require('luasnip').lsp_expand(args.body)
		end,
	},
	mapping = cmp.mapping.preset.insert({
		['<C-b>'] = cmp.mapping.scroll_docs(-4),
		['<C-f>'] = cmp.mapping.scroll_docs(4),
		['<C-Space>'] = cmp.mapping.complete(),
		['<C-e>'] = cmp.mapping.abort(),
		['<CR>'] = cmp.mapping({
			i = function(fallback)
        			if cmp.visible() and cmp.get_active_entry() then
					cmp.confirm({ behavior = cmp.ConfirmBehavior.Replace, select = false })
				else
					fallback()
				end
			end,
			-- s = cmp.mapping.confirm({ select = true }),
			-- c = cmp.mapping.confirm({ behavior = cmp.ConfirmBehavior.Replace, select = true }),
     		}),
	}),
	sources = cmp.config.sources({
		{ name = 'nvim_lsp' },
		{ name = 'luasnip' },
      		{ name = 'buffer' },
      		{ name = 'calc' },
      		{ name = 'greek' },
      		{ name = 'emoji' },
      		{ name = 'nvim_lua' },
      		{ name = 'treesitter' },
		{
			name = 'latex_symbols',
			option = 2,
		},
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
			mode = 'symbol', -- show only symbol annotations
			maxwidth = 50, -- prevent the popup from showing more than provided characters (e.g 50 will not show more than 50 characters)
			ellipsis_char = '...', -- when popup menu exceed maxwidth, the truncated part would show ellipsis_char instead (must define maxwidth first)

			-- The function below will be called before any actual modifications from lspkind
			-- so that you can provide more controls on popup customization. (See [#30](https://github.com/onsails/lspkind-nvim/pull/30))
			-- before = function (entry, vim_item)
			-- 	...
			-- 	return vim_item
			-- end
		})
	}
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
require('lspconfig').clangd.setup{
	capabilities = capabilities
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


