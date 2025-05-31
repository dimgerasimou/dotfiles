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
get_bluetooth_adapter_state(DBusConnection  *conn, DBusError *err)
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

static char**
get_bluetooth_connected_devices(DBusConnection *conn, DBusError *err, unsigned int *ndev)
{
	DBusMessage      *msg, *reply;
	DBusMessageIter   args, element;
	char            **ret = NULL;

	*ndev = 0;

	msg = dbus_message_new_method_call("org.bluez", "/org/bluez/hci0",
	                                   "org.freedesktop.DBus.Properties", "Get");

	const char  *iface   = "org.bluez.Adapter1";
	const char  *prop    = "Devices";

	dbus_message_append_args(msg, DBUS_TYPE_STRING, &iface,
	                         DBUS_TYPE_STRING, &prop, DBUS_TYPE_INVALID);

	reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, err);
	dbus_message_unref(msg);

	if (dbus_error_is_set(err)) {
		logwrite("D-Bus Error:", err->message, LOG_WARN, "dwmblocks-bluetooth");
		dbus_error_free(err);
		return NULL;
	}

	if (!reply) {
		logwrite("D-Bus: No reply received.", NULL, LOG_WARN, "dwmblocks-bluetooth");
		return NULL;
	}

	if (dbus_message_iter_init(reply, &args) && DBUS_TYPE_VARIANT == dbus_message_iter_get_arg_type(&args)) {
		dbus_message_iter_recurse(&args, &element);
		if (DBUS_TYPE_ARRAY == dbus_message_iter_get_arg_type(&element)) {
			DBusMessageIter array_iter;
			dbus_message_iter_recurse(&element, &array_iter);

			while (dbus_message_iter_get_arg_type(&array_iter) == DBUS_TYPE_OBJECT_PATH) {
				const char *device_path;
				dbus_message_iter_get_basic(&array_iter, &device_path);

				strncpy(devices[*device_count], device_path, 256);
				(*device_count)++;

				dbus_message_iter_next(&array_iter);
			}
		}
	} else {
		fprintf(stderr, "Failed to parse the reply message.\n");
	}

	dbus_message_unref(reply);
}

static int
get_bluetooth_state(void)
{
	DBusConnection *conn  = NULL;
	DBusError       err;
	char          **dev   = NULL;
	unsigned int    ndev  = 0;
	int             state = 0;

	/* initializing DBus connection */
	dbus_error_init(&err);
	
	if (!(conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err)) || dbus_error_is_set(&err)) {
		logwrite("Failed to connect to the DBus system bus.", err.message, LOG_WARN, "dwmblocks-bluetooth-menu");
		dbus_error_free(&err);
		return -1;
	}

	state = get_bluetooth_adapter_state(conn, &err);

	if (state != 1) {
		dbus_connection_unref(conn);
		return state;
	}

	dev = get_bluetooth_connected_devices(conn, &err, &ndev);

	state = get_bluetooth_audio_playback();

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
