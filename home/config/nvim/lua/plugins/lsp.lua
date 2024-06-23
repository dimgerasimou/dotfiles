-- vars
local ls      = require('luasnip')
local cmp     = require('cmp')
local capabilities = require('cmp_nvim_lsp').default_capabilities()
local lspkind = require('lspkind')

-- luasnip setup
ls.setup {
	enable_autosnippets = true,
	store_selection_keys = '<Tab>',
	update_events = 'TextChanged,TextChangedI'
}
require('luasnip.loaders.from_lua').load({paths = './lua/snippets/'})

-- cmp setup
cmp.setup({
	snippet = {
		expand = function(args)
			require('luasnip').lsp_expand(args.body)
		end,
	},
	mapping = {
		['<Up>']   = cmp.mapping.select_prev_item(),
		['<Down>'] = cmp.mapping.select_next_item(),
		['<C-e>']  = cmp.mapping.abort(),
		['<C-s>']  = cmp.mapping(function(fallback)
			if ls.jumpable(1) then
				ls.jump(1)
			else
				fallback()
			end
		end, {'i', 's'}),
		['<C-a>'] = cmp.mapping(function(fallback)
			if ls.jumpable(-1) then
				ls.jump(-1)
			else
				fallback()
			end
		end, {'i', 's'}),
		['<CR>'] = cmp.mapping(function(fallback)
			if cmp.visible() then
				if ls.expandable() then
					ls.expand()
				else
					cmp.confirm({
						select = true,
					})
				end
			else
				fallback()
			end
		end),
		["<Tab>"] = cmp.mapping(function(fallback)
			if cmp.visible() then
				cmp.select_next_item()
			elseif ls.locally_jumpable(1) then
				ls.jump(1)
			else
				fallback()
			end
		end, { "i", "s" }),

		["<S-Tab>"] = cmp.mapping(function(fallback)
			if cmp.visible() then
				cmp.select_prev_item()
			elseif ls.locally_jumpable(-1) then
				ls.jump(-1)
			else
				fallback()
			end
		end, { "i", "s" }),
	},
	sources = cmp.config.sources({
		{ name = 'nvim_lsp' },
		{ name = 'luasnip' },
		{ name = 'calc' },
		{ name = 'emoji' },
		{ name = 'nvim_lua' },
		{ name = 'buffer' },
		{ name = 'path' },
		{ name = 'plugins' },
		{ name = 'neorg' },
	}),
	enabled = function()
		local context = require('cmp.config.context')
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

cmp.setup.filetype({ "tex", "plaintex" }, {
	sources = {
		{ name = "lua-latex-symbols"}
	}
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

require('cmp_git').setup()

require('cmp-plugins').setup({
	files = { "~/.config/nvim/" }
})

-- Language servers
require('clangd_extensions').setup ({
	server = {
		capabilities = capabilities
	}
})

require('lspconfig').clangd.setup{
	capabilities = capabilities
}

require('lspconfig').lua_ls.setup{
	capabilities = capabilities,
	settings = {
		Lua = {
			runtime = {
				version = "Lua 5.1"
			},
			diagnostics = {
				globals = { "vim" }
			}
		}
	}
}

require('lspconfig').ltex.setup{
	capabilities = capabilities
}

require('lspconfig').bashls.setup{
	capabilities = capabilities
}

require('lspconfig').marksman.setup{
	capabilities = capabilities
}

require('lspconfig').pylsp.setup{
	capabilities = capabilities
}
