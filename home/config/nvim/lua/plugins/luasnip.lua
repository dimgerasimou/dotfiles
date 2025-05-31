require('luasnip').setup{
	enable_autosnippets = true,
	cut_selection_keys = '<Tab>',
}

require('luasnip.loaders.from_vscode').lazy_load()
require('luasnip.loaders.from_lua').load({paths = './lua/snippets/'})
