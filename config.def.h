/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const Gap default_gap        = {.isgap = 1, .realgap = 5, .gappx = 5};
static const unsigned int snap      = 5;        /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 0;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10", "NotoColorEmoji:pixelsize=10:antialias=true:autohint=true" };
static const char dmenufont[]       = "monospace:size=10";
static const char nbg[]             = "#020202";
static const char nfg[]             = "#bbbbbb";
static const char sfg[]             = "#bcbcbc";
static const char sbg[]             = "#282828";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { nfg,       nbg,       "#444444" },
	[SchemeSel]  = { sfg,       sbg,       "#aa0000" },
};

static const char *const autostart[] = {
	"dunst", NULL,
	"nm-applet", NULL,
	"picom", "--transparent-clipping", "--unredir-if-possible", NULL,
	"simple-polkit-authentication-agent", NULL, /* polkit_gnome_wrapped */
	"dwmblocks", NULL,
	"gammastep", "-P", "-r", NULL,
	"sh", "-c", "xautolock -lockaftersleep -secure -time 2 -detectsleep -locker 'lock-xorg -u -e'", NULL,

	"sh", "-c", "sleep 1 && amixer -q set Capture 0 nocap; kill -57 $(pidof dwmblocks)", NULL, /* FIXME pipewire keeps toggling mic to 100% unmute*/
	"sh", "-c", "id=$(xinput | grep -i touchpad | sed 's|^.*id=||' | awk '{print $1}');\
        en=$(xinput list-props $id | sed -n '2p' | awk '{print $NF}'); xinput disable $id", NULL,
	"sh", "-c", "sh ~/.fehbg", NULL,

	"setxkbmap", "-option", "caps:swapescape,altwin:swap_alt_win", NULL,
	"xset", "r", "rate", "300", "70", NULL,
	NULL /* terminate */
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

#include "shiftview.c"
#include <X11/XF86keysym.h>

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 *	type following command, and click the window [https://dwm.suckless.org/customisation/rules/]
	 *	xprop | awk '/^WM_CLASS/{sub(/.* =/, "instance:"); sub(/,/, "\nclass:"); print}/^WM_NAME/{sub(/.* =/, "title:"); print}'
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "mpv",      NULL,       NULL,       2,            0,           -1 },
	{ "Virt-manager",NULL,    NULL,       1 << 7,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "><>",      NULL },    /* no layout function means floating behavior */
};

/* key definitions */
#define MODKEY Mod4Mask /* Mod4Mask - logo key, Mod1Mask - Alt */
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", nbg, "-nf", nfg, "-sb", sbg, "-sf", sfg, NULL };
static const char *termcmd[]  = { "alacritty", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY|ControlMask,           XK_b,      spawn,          SHCMD("add-bookmark ~/.m/files/bookmarks.txt") },
	{ MODKEY|ShiftMask,             XK_b,      spawn,          SHCMD("show-bookmark ~/.m/files/bookmarks.txt") },
	{ MODKEY,                       XK_c,      spawn,          SHCMD("\
            id=$(xinput | grep -i touchpad | sed 's|^.*id=||' | awk '{print $1}');\
            en=$(xinput list-props $id | sed -n '2p' | awk '{print $NF}');\
            [ $en -eq 1 ] && xinput disable $id || xinput enable $id") },
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_e,      spawn,          SHCMD("keepassxc") },
	{ MODKEY,                       XK_f,      togglefullscr,  {0} },
	{ MODKEY|ShiftMask,             XK_f,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.01} },
	{ MODKEY|ControlMask,           XK_h,      moveresize,     {.v = "0x 0y -25w 0h"}},  // resize left
	{ MODKEY|ShiftMask,             XK_h,      moveresize,     {.v = "-25x 0y 0w 0h"}},  // move left
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY|ControlMask,           XK_j,      moveresize,     {.v = "0x 0y 0w 25h"}},   // resize down
	{ MODKEY|ShiftMask,             XK_j,      moveresize,     {.v = "0x 25y 0w 0h"}},   // move down
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY|ControlMask,           XK_k,      moveresize,     {.v = "0x 0y 0w -25h"}},  // resize up
	{ MODKEY|ShiftMask,             XK_k,      moveresize,     {.v = "0x -25y 0w 0h"}},  // move up
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.01} },
	{ MODKEY|ControlMask,           XK_l,      moveresize,     {.v = "0x 0y 25w 0h"}},   // resize right
	{ MODKEY|ShiftMask,             XK_l,      moveresize,     {.v = "25x 0y 0w 0h"}},   // move right
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_n,      shiftview,      {.i = +1} },
	{ MODKEY,                       XK_p,      shiftview,      {.i = -1} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ MODKEY|ShiftMask,             XK_r,      spawn,          SHCMD("pkill -x picom || picom --transparent-clipping --unredir-if-possible") },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_w,      spawn,          SHCMD("librewolf -P default") },
	{ MODKEY|ControlMask,           XK_w,      spawn,          SHCMD("librewolf --private-window") },
	{ MODKEY|ShiftMask,             XK_w,      spawn,          SHCMD("librewolf -P") },
	{ MODKEY,                       XK_z,      spawn,          SHCMD("kill $(pidof gammastep) || gammastep -P -r") },
	{ MODKEY,                       XK_Escape, spawn,          SHCMD("lock-xorg -u -e") },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +5 } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = GAP_TOGGLE} },
	{ 0,                            XK_Print,  spawn,          SHCMD("mkdir -p ~/Pictures/flameshot && flameshot full -p ~/Pictures/flameshot/$(date +%Y%m%d-%H%M%S)")},
	{ 0|ShiftMask,                  XK_Print,  spawn,          SHCMD("mkdir -p ~/Pictures/flameshot && flameshot gui -p ~/Pictures/flameshot/$(date +%Y%m%d-%H%M%S)")},
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY,                       XK_minus,  setgaps,        {.i = -5 } },
	{ MODKEY|ShiftMask,             XK_minus,  setgaps,        {.i = GAP_RESET } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ 0,                            XK_F6,     spawn,          SHCMD("amixer -q set Capture nocap 0 && kill -57 $(pidof dwmblocks)")}, /* FIXME use action keys */
	{ 0, XF86XK_AudioLowerVolume,              spawn,          SHCMD("amixer -q set Master 1%- && kill -57 $(pidof dwmblocks)")},
	{ 0, XF86XK_AudioMute,                     spawn,          SHCMD("amixer -q set Master toggle && kill -57 $(pidof dwmblocks)")},
	{ 0, XF86XK_AudioRaiseVolume,              spawn,          SHCMD("amixer -q set Master 1%+ && kill -57 $(pidof dwmblocks)")},
	{ 0, XF86XK_MonBrightnessDown,             spawn,          SHCMD("brightnessctl -q set 1%- && kill -56 $(pidof dwmblocks)")},
	{ 0, XF86XK_MonBrightnessUp,               spawn,          SHCMD("brightnessctl -q set 1%+ && kill -56 $(pidof dwmblocks)")},
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY|ShiftMask,Button1,       resizemouse,    {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
