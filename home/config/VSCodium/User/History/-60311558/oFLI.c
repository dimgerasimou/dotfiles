#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dbus/dbus.h>

#include "../include/colorscheme.h"
#include "../include/common.h"

const char *tui_settings_args[] = { "st", "-e", "bluetuith", NULL };
const char *bt_icons[]      = {"󰂲", "󰂯", "󰥰"};

static int
getbtadapterstate(DBusConnection  *conn, DBusError *err)
{
	DBusMessage     *msg, *reply;
	DBusMessageIter  args, replyArgs;

	const char  *iface   = "org.bluez.Adapter1";
	const char  *prop    = "Powered";
	dbus_bool_t  powered = FALSE;

	msg = dbus_message_new_method_call("org.bluez", "/org/bluez/hci0",
	                                   "org.freedesktop.DBus.Properties", "Get");

	if (!msg) {
		logwrite("Failed to create DBus message.", NULL, LOG_WARN, "dwmblocks-bluetooth");
		return -1;
	}

	dbus_message_iter_init_append(msg, &args);
	dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &iface);
	dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &prop);

	reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, err);
	dbus_message_unref(msg);

	if (dbus_error_is_set(err)) {
		logwrite("D-Bus Error:", err->message, LOG_WARN, "dwmblocks-bluetooth");
		dbus_error_free(err);
		return -1;
	}

	if (!reply) {
		logwrite("D-Bus: No reply received.", NULL, LOG_WARN, "dwmblocks-bluetooth");
		return -1;
	}

	if (dbus_message_iter_init(reply, &replyArgs)) {
		DBusMessageIter variant;
		dbus_message_iter_recurse(&replyArgs, &variant);
		dbus_message_iter_get_basic(&variant, &powered);
	}
	dbus_message_unref(reply);

	return powered;
}

static int
getbtstate(void)
{
	DBusConnection *conn  = NULL;
	DBusError       err;
	int             state = 0;

	/* initializing DBus connection */
	dbus_error_init(&err);
	
	if (!(conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err)) || dbus_error_is_set(&err)) {
		logwrite("Failed to connect to the DBus system bus.", err.message, LOG_WARN, "dwmblocks-bluetooth-menu");
		dbus_error_free(&err);
		return -1;
	}

	state = getbtadapterstate(conn, &err);

	if (state != 1) {
		dbus_connection_unref(conn);
		return state;
	}

	// state = get_bluetooth_playback_state(conn, &err);

	dbus_connection_unref(conn);
	return state;
}

static void
execbutton(void)
{
	char *env = NULL;

	if (!(env = getenv("BLOCK_BUTTON")))
		return;

	switch (atoi(env)) {
	case 1:
	{
		char *path = get_path((char**) MENU_PATH, 1);
		forkexecv(path, (char**) MENU_ARGS, "dwmblocks-bluetooth");
		free(path);
		break;
	}

	case 2:
		forkexecv(BLUETUITH_PATH, (char**) tui_settings_args, "dwmblocks-bluetooth");
		break;

	default:
		break;
	}
}

int
main(void)
{
	int state;

	execbutton();

	state = getbtstate();

	if (state < 0)
		return -state;

	printf(CLR_4 BG_1" %s " NRM "\n", bt_icons[state]);

	return EXIT_SUCCESS;
}
