#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dbus/dbus.h>

#include "../include/colorscheme.h"
#include "../include/common.h"

const char *BLUETUITH_PATH   = "/usr/local/bin/st";
const char *BLUETUITH_ARGS[] = { "st", "-e", "bluetuith", NULL };
const char *MENU_PATH[]      = {"$HOME", ".local", "bin", "dwmblocks", "bluetooth-menu"};
const char *MENU_ARGS[]      = {"bluetooth-menu", NULL};
const char *ICON_LIST[]      = {"󰂲", "󰂯", "󰥰"};
const char *BT_SHOW_CMD      = "bluetoothctl show";
const char *BT_INFO_CMD      = "bluetoothctl info";
const char *BT_CON_CMD       = "bluetoothctl devices Connected";

static int
bluetooth_is_audio_device(char *address)
{
	FILE *ep;
	char cmd[128];
	char buf[128];

	sprintf(cmd, "%s %s", BT_INFO_CMD, address);

	if (!(ep = popen(cmd, "r"))) {
		logwrite("Failed to execute command with popen().", cmd, LOG_ERROR, "dwmblocks-bluetooth");
		exit(errno);
	}

	while(fgets(buf, sizeof(buf), ep)) {
		if (strstr(buf, "audio-headset")) {
			pclose(ep);
			return 1;
		}
	}

	pclose(ep);
	return 0;
}

static int
get_bluetooth_audio_playback(void)
{
	FILE *ep;
	char buffer[256];
	char mac[18];
	char *ptr;

	if (!(ep = popen(BT_CON_CMD, "r"))) {
		logwrite("Failed to execute command with popen().", BT_CON_CMD, LOG_ERROR, "dwmblocks-bluetooth");
		exit(errno);
	}

	while (fgets(buffer, sizeof(buffer), ep)) {
		if (strstr(buffer, "Device")) {
			ptr = buffer;
			ptr += 7;
			trimtonewl(ptr);

			strncpy(mac, ptr, 17);

			if (bluetooth_is_audio_device(mac)) {
				pclose(ep);
				return 1;
			}
		}
	}

	pclose(ep);
	return 0;
}

static int
get_bluetooth_adapter_state(DBusConnection  *conn)
{
	DBusMessage     *msg;
	DBusMessage     *reply;
	DBusMessageIter  args;
	DBusMessageIter  replyArgs;
	DBusError        err;

	const char  *iface   = "org.bluez.Adapter1";
	const char  *prop    = "Powered";
	dbus_bool_t  powered = FALSE;

	msg = dbus_message_new_method_call("org.bluez", "/org/bluez/hci0",
	                                   "org.freedesktop.DBus.Properties", "Get");

	if (!msg) {
		logwrite("Failed to create DBus message.", NULL, LOG_WARN, "dwmblocks-bluetooth-menu");
		return -1;
	}

	dbus_message_iter_init_append(msg, &args);
	dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &iface);
	dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &prop);

	reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
	dbus_message_unref(msg);

	if (!reply) {
		logwrite("Failed to get Bluetooth Powered State.", err.message, LOG_WARN, "dwmblocks-bluetooth-menu");
		dbus_error_free(&err);
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
get_bluetooth_state(void)
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

	state = get_bluetooth_adapter_state(conn);

	if (state != 1) {
		dbus_connection_unref(conn);
		return state;
	}


	state = state && get_bluetooth_audio_playback() ? 2 : state;

	dbus_connection_unref(conn);
	return state;
}

static void
exec_block_button(void)
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
		forkexecv(BLUETUITH_PATH, (char**) BLUETUITH_ARGS, "dwmblocks-bluetooth");
		break;

	default:
		break;
	}
}

int
main(void)
{
	int state;

	exec_block_button();

	state = get_bluetooth_state();
	printf(CLR_4 BG_1" %s " NRM "\n", ICON_LIST[state]);

	return EXIT_SUCCESS;
}
