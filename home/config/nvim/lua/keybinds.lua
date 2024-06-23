-- map leader to <Space>
vim.keymap.set('n', ' ', '<Nop>', { silent = true, remap = false })
vim.g.mapleader = ' '

require('which-key').register({
	a = { "<cmd>AerialToggle!<CR>", "Aerial" },
	n = { "<cmd>Neotree toggle<CR>", "Neotree" },
	l = {
		name = "latex",
		c = { "<cmd>VimtexCompile<CR>",         "Compile document (Vimtex)"    },
		C = { "<cmd>VimtexCompileSS<CR>",       "Compile single shot (Vimtex)" },
		o = { "<cmd>VimtexCompileOutput<CR>",   "Compile output (Vimtex)"      },
		O = { "<cmd>VimtexCompileSelected<CR>", "Compile selected (Vimtex)"    },
		r = { "<cmd>VimtexReload<CR>",          "Reload (Vimtex)"              },
		v = { "<cmd>VimtexView<CR>",            "View (Vimtex)"                },
		u = { "<cmd>VimtexClean<CR>",           "Clean (Vimtex)"               },
		I = { "<cmd>VimtexStatus<CR>",          "Status (Vimtex)"              },
		i = { "<cmd>VimtexInfo<CR>",            "Info (Vimtex)"                },
		l = { "<cmd>VimtexLog<CR>",             "Log (Vimtex)"                 },
		s = { "<cmd>VimtexStop<CR>",            "Stop (Vimtex)"                },
		S = { "<cmd>VimtexStopAll<CR>",         "Stop all (Vimtex)"            },
		p = { "<cmd>lua require('nabla').popup({border = 'rounded'})<CR>",   "Preview expression (Nabla)"        },
		P = { "<cmd>lua require('nabla').toggle_virt({autogen = true})<CR>", "Toggle inline expressions (Nabla)" },
	},
	d = {
		name = "diagnostics",
		d = { "<cmd>Trouble diagnostics toggle<cr>",                        "Diagnostics (Trouble)"                        },
		D = { "<cmd>Trouble diagnostics toggle filter.buf=0<cr>",           "Buffer Diagnostics (Trouble)"                 },
		s = { "<cmd>rouble symbols toggle focus=false<cr>",                 "Symbols (Trouble)"                            },
		l = { "<cmd>Trouble lsp toggle focus=false win.position=right<cr>", "LSP Definitions / references / ... (Trouble)" },
		L = { "<cmd>Trouble loclist toggle<cr>",                            "Location List (Trouble)"                      },
		Q = { "<cmd>Trouble qflist toggle<cr>",                             "Quickfix List (Trouble)"                      },
	},
	s = {
		name = "splits",
		h = { "<cmd>SmartSwapLeft<CR>",  "Swap split left"  },
		j = { "<cmd>SmartSwapUp<CR>",    "Swap split up"    },
		k = { "<cmd>SmartSwapDown<CR>",  "Swap split down"  },
		l = { "<cmd>SmartSwapRight<CR>", "Swap split right" },
	},
	["."] = { "<cmd>MultipleCursorsLock<CR>",                      "Multicursors lock",  mode = {"n", "x"} },
	["|"] = { "<cmd>lua require('multiple-cursors').align() <CR>", "Multicursors align", mode = {"n", "x"} },
}, { prefix = "<leader>" })

require('which-key').register({
	['<A-h>'] = { "<cmd>SmartResizeLeft<CR>",    "Resize split left"  },
	['<A-j>'] = { "<cmd>SmartResizeDown<CR>",    "Resize split down"  },
	['<A-k>'] = { "<cmd>SmartResizeUp<CR>",      "Resize split up"    },
	['<A-l>'] = { "<cmd>SmartResizeRight<CR>",   "Resize split right" },

	['<C-h>'] = { "<cmd>SmartCursorMoveLeft<CR>",  "Cursor move left"  },
	['<C-j>'] = { "<cmd>SmartCursorMoveUp<CR>",    "Cursor move up"    },
	['<C-k>'] = { "<cmd>SmartCursorMoveDown<CR>",  "Cursor move down"  },
	['<C-l>'] = { "<cmd>SmartCursorMoveRight<CR>", "Cursor move right" },

	['<C-down>']      = { "<cmd>MultipleCursorsAddDown<CR>",        "Multicursors add down",                  mode = {"n", "i", "x"} },
	['<C-up>']        = { "<cmd>MultipleCursorsAddUp<CR>",          "Multicursors add up",                    mode = {"n", "i", "x"} },
	['<C-LeftMouse>'] = { "<cmd>MultipleCursorsMouseAddDelete<CR>", "Multicursors add-delete at mouse click", mode = {"n", "x"}      },
})
