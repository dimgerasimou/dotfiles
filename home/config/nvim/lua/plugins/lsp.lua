-- Mason installer
require('mason').setup()
require('mason-lspconfig').setup()

local cmp = require('cmp')
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
		['<CR>'] = cmp.mapping.confirm({ select = true }),
	}),
	sources = cmp.config.sources({
		{ name = 'nvim_lsp' },
		{ name = 'luasnip' },
    	}, {
      		{ name = 'buffer' },
	})
})

-- Cmp - Luasnip - Supertab
local has_words_before = function()
	unpack = unpack or table.unpack
	local line, col = unpack(vim.api.nvim_win_get_cursor(0))
	return col ~= 0 and vim.api.nvim_buf_get_lines(0, line - 1, line, true)[1]:sub(col, col):match("%s") == nil
end

local luasnip = require("luasnip")
local cmp = require("cmp")

cmp.setup({
	mapping = {
		["<Tab>"] = cmp.mapping(function(fallback)
			if cmp.visible() then
				cmp.select_next_item()
			elseif luasnip.expand_or_jumpable() then
				luasnip.expand_or_jump()
			elseif has_words_before() then
				cmp.complete()
			else
				fallback()
			end
		end, { "i", "s" }),

		["<S-Tab>"] = cmp.mapping(function(fallback)
      			if cmp.visible() then
        			cmp.select_prev_item()
			elseif luasnip.jumpable(-1) then
				luasnip.jump(-1)
			else
				fallback()
			end
		 end, { "i", "s" }),
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


