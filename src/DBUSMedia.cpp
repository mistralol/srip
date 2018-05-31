
#include <dbus/dbus.h>

#include "main.h"

DBUSMedia::DBUSMedia() :
    m_running(true)
{

}

DBUSMedia::~DBUSMedia() {

}


void DBUSMedia::Run() {
    DBusMessage* msg;
    DBusMessageIter args;
    DBusConnection* conn;
    DBusError err;
    int ret;
    char* sigvalue;

    LogDebug("Listening for signals\n");

    // initialise the errors
    dbus_error_init(&err);

    // connect to the bus and check for errors
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
      LogError("Connection Error (%s)", err.message);
      dbus_error_free(&err);
    }
    if (NULL == conn) {
        LogCritical("dbus_bus_get failed!!");
        abort();
    }

    // request our name on the bus and check for errors
    ret = dbus_bus_request_name(conn, "srip.sniffer", DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
    if (dbus_error_is_set(&err)) {
        LogError("Name Error (%s)", err.message);
        dbus_error_free(&err);
    }
    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
        LogCritical("I don't own my name on dbus!!");
        abort();
    }

    // add a rule for which messages we want to see
    dbus_bus_add_match(conn, "type='signal',interface='org.freedesktop.DBus.Properties'", &err); // see signals from the given interface
    dbus_connection_flush(conn);
    if (dbus_error_is_set(&err)) {
        LogError("Match Error (%s)\n", err.message);
        abort();
    }

    // loop listening for signals being emmitted
    while (m_running) {
        // non blocking read of the next available message
        dbus_connection_read_write(conn, 0);
        msg = dbus_connection_pop_message(conn);

        // loop again if we haven't read a message
        if (NULL == msg) {
            //usleep(10000);
            continue;
        }

        LogDebug("Read some signal Sender: %s Interface: %s Member: %s Path: %s",
                dbus_message_get_sender(msg),
                dbus_message_get_interface(msg),
                dbus_message_get_member(msg),
                dbus_message_get_path(msg)
            );

        // check if the message is a signal from the correct interface and with the correct name
        if (dbus_message_is_signal(msg, "org.freedesktop.DBus.Properties", "PropertiesChanged")) {
            if (!dbus_message_iter_init(msg, &args)) {
                LogError("Message Has No Parameters");
            } else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args)) {
                LogError("Argument is not string!");
            } else {
                dbus_message_iter_get_basic(&args, &sigvalue);
            }
            LogInfo("Got Signal with value %s\n", sigvalue);
            if (strcmp(sigvalue, "org.mpris.MediaPlayer2.Player") == 0) {
                LogInfo("Got Some Song INFO!!!");
            }
        }

      // free the message
      dbus_message_unref(msg);
   }

}
