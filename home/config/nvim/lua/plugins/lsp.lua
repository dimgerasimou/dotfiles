local capabilities = require('blink.cmp').get_lsp_capabilities{
	textDocument = { completion = { completionItem = { snippetSupport = false } } },
}

require('clangd_extensions').setup {
	server = {
		capabilities = capabilities
	}
}

require('lspconfig').clangd.setup{
	capabilities = capabilities
}

require('lspconfig').lua_ls.setup{
	capabilities = capabilities,
	settings = {
		Lua = {
			runtime = { version = "Lua 5.1" },
			diagnostics = { globals = { "vim" } }
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

