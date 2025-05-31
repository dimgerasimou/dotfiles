/* See LICENSE file for copyright and license details. */

#define SESSION_FILE "/tmp/dwm-session"
#define STATUSBAR "dwmblocks"

/* appearance */

/* bar */
static const unsigned int systraypinning   = 0;    /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft    = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing   = 5;    /* systray spacing */
static const int systraypinningfailfirst   = 1;    /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray               = 1;    /* 0: no systray */

static const unsigned int statuspinning    = 0;    /* 0: sloppy statusbar follows selected monitor, >0: pin statusbar to monitor X */
static const int statuspinningfailfirst    = 1;    /* 1: if pinning fails, display statusbar on the first monitor, False: display statusbar on the last monitor*/
static const unsigned int showstatusallmon = 0;    /* 1: show statusbar on all monitors */
static const unsigned int statusbarlhpad   = 0;    /* right horizontal padding of status bar */

static int showbar                         = 1;    /* 0: no bar */
static int topbar                          = 1;    /* 0: bottom bar */
static const unsigned int hidevacanttags   = 1;    /* 1: hide vacant tags */
static const int vertpad                   = 0;    /* vertical padding above the bar */
static const int sidepad                   = 0;    /* horizontal padding of the bar  */
static const int underpad                  = 0;    /* vertical padding below the bar */
static const unsigned int hidetitlebar     = 0;    /* 1: no window titles */
static const unsigned int hidelayout       = 0;    /* 1: no layout symbol */

/* windows */
static const int swallowfloating    = 1;     /* 1: swallow floating windows by default */
static unsigned int borderpx        = 0;     /* border pixel of windows */
static unsigned int snap            = 32;    /* snap pixel */
static const int focusonwheel       = 1;     /* focus window on scrolling */
static const unsigned int gappih    = 20;    /* horiz inner gap between windows */
static const unsigned int gappiv    = 20;    /* vert inner gap between windows */
static const unsigned int gappoh    = 20;    /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 20;    /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;     /* 1: no outer gap when there is only one window */
static const unsigned int warpmon   = 1;     /* warp cursor to selected monitor */
static const unsigned int warpwin   = 1;     /* warp cursor to selected window */

/* fonts and colors */
static const char *fonts[]          = { "FiraCode Nerd Font:style=Regular:size=11",
                                        "Font Awesome 6 Free:style=Regular:size=11"};

static char normbgcolor[]           = "#323232";
static char systraybg[]             = "#323232";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#296991";
static char urgbordercolor[]        = "#005577";
static char selbgcolor[]            = "#296991";

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static unsigned int scratchtag = 1 << LENGTH(tags);

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class         instance    title           tags mask     isfloating    isterminal    noswallow    monitor */
	{ "firefox",     NULL,       NULL,           0,            0,            0,            0,           -1 },
	{ "thunderbird", NULL,       NULL,           0,            0,            0,            -1,          -1 },
	{ "Alacritty",   NULL,       NULL,           0,            0,            1,            0,           -1 },	
	{ "St",          NULL,       NULL,           0,            0,            1,            0,           -1 },
	{ "vscodium",    NULL,       NULL,           0,            0,            0,            0,           -1 },
	{ NULL,          NULL,       "Event Tester", 0,            0,            0,            1,           -1 }, /* xev */
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int decorhints  = 1;    /* 1 means respect decoration hints */
static const int attachdirection = 2;/* 0 default, 1 above, 2 aside, 3 below, 4 bottom, 5 top */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "H[]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
};

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "normbgcolor",        STRING,  &normbgcolor },
		{ "systraybg",          STRING,  &systraybg },
		{ "normbordercolor",    STRING,  &normbordercolor },
		{ "normfgcolor",        STRING,  &normfgcolor },
		{ "selbgcolor",         STRING,  &selbgcolor },
		{ "selbordercolor",     STRING,  &selbordercolor },
		{ "urgbordercolor",     STRING,  &urgbordercolor },
		{ "selfgcolor",         STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",               INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",              FLOAT,   &mfact },
};

static char *colors[][3] = {
	/*               fg           bg           border   */
	[SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
	[SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
	[SchemeUrg]  = { selfgcolor,  selbgcolor,  urgbordercolor  },
	[SchemeTray] = { normfgcolor, systraybg,   normbordercolor },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static const char scratchpadname[] = "scratchpad";
static const char layoutmenu_cmd[] = "layoutmenu";
static const char binpath[]        = ".local/bin/dwm";

/* application launch */
static const char *dmenucmd[]      = { "dmenu_appmenu", "-nn", "-c", "-i", "-n", "-h", "12", "-l", "20", NULL };
static const char *dmenuaudio[]    = { "/home/dimgerasimou/.local/bin/dmenu/dmenu-audio-source-select", NULL};
static const char *clipcmd[]       = { "clipmenu", "-a", "-c", "-i", "-n", "-h", "12", "-l", "20", NULL };
static const char *dmenuall[]      = { "dmenu_run", "-c", "-i", "-n", "-h", "12", "-l", "20", NULL };
static const char *xrandrset[]     = { "xrandr-setup", "-s", "-c", "-i", "-nn", "-h", "12", "-l", "5", "-p", "Select XRandR layout:", NULL};
static const char *termcmd[]       = { "st", NULL };
static const char *browsercmd[]    = { "firefox", NULL };
static const char *emailcmd[]      = { "thunderbird", NULL };
static const char *explrcmd[]      = { "pcmanfm", NULL };
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "120x34", NULL };
static const char *lockcmd[]       = { "slock", NULL };

/* volume control */
static const char *volumeup[]   = { "audiocontrol", "sink",   "increase", NULL };
static const char *volumedown[] = { "audiocontrol", "sink",   "decrease", NULL };
static const char *volumemute[] = { "audiocontrol", "sink",   "mute",     NULL };
static const char *micmute[]    = { "audiocontrol", "source", "mute",     NULL };

/* playback control via dbus */
static const char *dbustogg[] = { "mediacontrol", "toggle", NULL };
static const char *dbusstop[] = { "mediacontrol", "stop",   NULL };
static const char *dbusnext[] = { "mediacontrol", "next",   NULL };
static const char *dbusprev[] = { "mediacontrol", "prev",   NULL };


/* brightness control */
static const char *brightup[]   = { "brightnessctl", "--class=backlight", "set", "+5%", NULL };
static const char *brightdown[] = { "brightnessctl", "--class=backlight", "set", "5%-", NULL };

static const char *switchlang[] = { "keyboard.sh", NULL };
static const char *screenshot[] = { "takescreenshot", NULL };

/* keys */
static const Key keys[] = {
	/* modifier                     key              function        argument                    description */
	/* spawn */
	{ MODKEY,                       XK_a,            spawn,          {.v = dmenucmd}      }, /* Spawn dmenu */
	{ MODKEY|ShiftMask,             XK_a,            spawn,          {.v = dmenuall}      }, /* Spawn dmenu (path) */
	{ MODKEY,                       XK_Return,       spawn,          {.v = termcmd}       }, /* Spawn terminal */
	{ MODKEY,                       XK_w,            spawn,          {.v = browsercmd}    }, /* Spawn browser */
	{ MODKEY,                       XK_e,            spawn,          {.v = emailcmd}      }, /* Spawn e-mail client */
	{ MODKEY,                       XK_s,            spawn,          {.v = explrcmd}      }, /* Spawn file explorer */
	{ MODKEY,                       XK_space,        spawn,          {.v = switchlang}    }, /* Toggle language */
	{ MODKEY,                       XK_grave,        togglescratch,  {.v = scratchpadcmd} }, /* Toggle sratchpad */
	{ MODKEY,                       XK_p,            spawn,          {.v = lockcmd}       }, /* Lock */
	{ MODKEY,                       XK_c,            spawn,          {.v = clipcmd}       }, /* Open clipboard */

	/* layouts */
	{ MODKEY,                       XK_b,            togglebar,      {0}                  }, /* Toggle the status bar */
	{ MODKEY,                       XK_j,            focusstack,     {.i = +1}            }, /* Focus next in stack */
	{ MODKEY,                       XK_k,            focusstack,     {.i = -1}            }, /* Focus previous in stack */
	{ MODKEY|ShiftMask,             XK_j,            rotatestack,    {.i = +1}            }, /* Rotate stack next*/
	{ MODKEY|ShiftMask,             XK_k,            rotatestack,    {.i = -1}            }, /* Rotate stack previous */
	{ MODKEY,                       XK_bracketright, incnmaster,     {.i = +1}            }, /* Increase master stack */
	{ MODKEY,                       XK_bracketleft,  incnmaster,     {.i = -1}            }, /* Decrease master stack */
	{ MODKEY,                       XK_h,            setmfact,       {.f = -0.05}         }, /* Widen to left */
	{ MODKEY,                       XK_l,            setmfact,       {.f = +0.05}         }, /* Widen to right */
	{ MODKEY|ShiftMask,             XK_Return,       zoom,           {0}                  }, /* IDK */
	{ MODKEY,                       XK_Tab,          view,           {0}                  }, /* Switch to last tag */
	{ MODKEY|ShiftMask,             XK_BackSpace,    killclient,     {0}                  }, /* Kill active Window */
	{ MODKEY|ControlMask,           XK_space,        setlayout,      {0}                  }, /* Focus window */
	{ MODKEY|ShiftMask,             XK_space,        togglefloating, {0}                  }, /* Toggle floating layout */
	{ MODKEY,                       XK_0,            view,           {.ui = ~0}           }, /* View all tags */
	{ MODKEY|ShiftMask,             XK_0,            tag,            {.ui = ~0}           }, /* Tag window in all tags */
	{ MODKEY,                       XK_comma,        focusmon,       {.i = -1}            }, /* Focus previous monitor */
	{ MODKEY,                       XK_period,       focusmon,       {.i = +1}            }, /* Focus next monitor */
	{ MODKEY|ShiftMask,             XK_comma,        tagmon,         {.i = -1}            }, /* Move window to previous monitor */
	{ MODKEY|ShiftMask,             XK_period,       tagmon,         {.i = +1}            }, /* Move window to next monitor */

	/* vanity gaps */
	/* { MODKEY|Mod1Mask,              XK_u,      incrgaps,       {.i = +1 } }, */
	/* { MODKEY|Mod1Mask|ShiftMask,    XK_u,      incrgaps,       {.i = -1 } }, */
	/* { MODKEY|Mod1Mask,              XK_i,      incrigaps,      {.i = +1 } }, */
	/* { MODKEY|Mod1Mask|ShiftMask,    XK_i,      incrigaps,      {.i = -1 } }, */
	/* { MODKEY|Mod1Mask,              XK_o,      incrogaps,      {.i = +1 } }, */
	/* { MODKEY|Mod1Mask|ShiftMask,    XK_o,      incrogaps,      {.i = -1 } }, */
	/* { MODKEY|Mod1Mask,              XK_6,      incrihgaps,     {.i = +1 } }, */
	/* { MODKEY|Mod1Mask|ShiftMask,    XK_6,      incrihgaps,     {.i = -1 } }, */
	/* { MODKEY|Mod1Mask,              XK_7,      incrivgaps,     {.i = +1 } }, */
	/* { MODKEY|Mod1Mask|ShiftMask,    XK_7,      incrivgaps,     {.i = -1 } }, */
	/* { MODKEY|Mod1Mask,              XK_8,      incrohgaps,     {.i = +1 } }, */
	/* { MODKEY|Mod1Mask|ShiftMask,    XK_8,      incrohgaps,     {.i = -1 } }, */
	/* { MODKEY|Mod1Mask,              XK_9,      incrovgaps,     {.i = +1 } }, */
	/* { MODKEY|Mod1Mask|ShiftMask,    XK_9,      incrovgaps,     {.i = -1 } }, */
	/* { MODKEY|Mod1Mask,              XK_0,      togglegaps,     {0} }, */
	/* { MODKEY|Mod1Mask|ShiftMask,    XK_0,      defaultgaps,    {0} }, */

	/* { MODKEY|ShiftMask|ControlMask, XK_k,      viewnext,       {0} }, */
	/* { MODKEY|ShiftMask|ControlMask, XK_j,      viewprev,       {0} }, */
	/* { MODKEY|ShiftMask|ControlMask, XK_l,      tagtonext,      {0} }, */
	/* { MODKEY|ShiftMask|ControlMask, XK_h,      tagtoprev,      {0} }, */
	/* { MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} }, */
	/* { MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} }, */
	/* { MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} }, */

	/* function keys */
	{ 0,                            XF86XK_AudioRaiseVolume,  spawnbin, {.v = volumeup}   }, /* Increase volume */
	{ 0,                            XF86XK_AudioLowerVolume,  spawnbin, {.v = volumedown} }, /* Decrease volume */
	{ 0,                            XF86XK_AudioMute,         spawnbin, {.v = volumemute} }, /* Mute audio */
	{ 0,                            XF86XK_AudioMicMute,      spawnbin, {.v = micmute}    }, /* Mute microphone */
	{ MODKEY,                       XK_F5,                    spawnbin, {.v = micmute}    }, /* Mute microphone */
	{ MODKEY,                       XK_F6,                    spawnbin, {.v = dmenuaudio} }, /* Spawn audio source menu */
	{ MODKEY,                       XK_F7,                    spawnbin, {.v = xrandrset}  }, /* Spawn monitor menu */
	{ 0,                            XF86XK_AudioPlay,         spawnbin, {.v = dbustogg}   }, /* Toggle media player */
	{ 0,                            XF86XK_AudioStop,         spawnbin, {.v = dbusstop}   }, /* Toggle media player */
	{ 0,                            XF86XK_AudioPause,        spawnbin, {.v = dbusstop}   }, /* Stop media player */
	{ 0,                            XF86XK_AudioNext,         spawnbin, {.v = dbusnext}   }, /* Media player next */
	{ 0,                            XF86XK_AudioPrev,         spawnbin, {.v = dbusprev}   }, /* Media player previous */
	{ 0,                            XF86XK_MonBrightnessUp,   spawn,    {.v = brightup}   }, /* Brightness up */
	{ 0,                            XF86XK_MonBrightnessDown, spawn,    {.v = brightdown} }, /* Brightness down */
	{ 0,                            XK_Print,                 spawnbin, {.v = screenshot} }, /* Take screenshot */
	{ MODKEY|ShiftMask,             XK_q,                     quit,     {0}               }, /* Quit dwm */
	{ MODKEY|ControlMask|ShiftMask, XK_q,                     quit,     {1}            }, /* Restart dwm */

	TAGKEYS(                        XK_1,                         0) /* General tags */
	TAGKEYS(                        XK_2,                         1) /* General tags */
	TAGKEYS(                        XK_3,                         2) /* General tags */
	TAGKEYS(                        XK_4,                         3) /* General tags */
	TAGKEYS(                        XK_5,                         4) /* General tags */
	TAGKEYS(                        XK_6,                         5) /* General tags */
	TAGKEYS(                        XK_7,                         6) /* General tags */
	TAGKEYS(                        XK_8,                         7) /* General tags */
	TAGKEYS(                        XK_9,                         8) /* General tags */
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkLtSymbol,          0,              Button1,        layoutmenu,     {0} },
	{ ClkStatusText,        0,              Button1,        sigstatusbar,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigstatusbar,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigstatusbar,   {.i = 3} },
	{ ClkStatusText,        0,              Button4,        sigstatusbar,   {.i = 4} },
	{ ClkStatusText,        0,              Button5,        sigstatusbar,   {.i = 5} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

