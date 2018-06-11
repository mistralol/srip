
#include <json-glib/json-glib.h>
#include <json/json.h>

#include "main.h"

DBUSMedia::DBUSMedia(OutputManager *OutputManager) :
    m_running(true),
    m_OutputManager(OutputManager)
{

}

DBUSMedia::~DBUSMedia() {

}

void DBUSMedia::OnSignal(GDBusConnection *conn,
			     const gchar *sender_name,
			     const gchar *object_path,
			     const gchar *interface_name,
			     const gchar *signal_name,
			     GVariant *parameters,
			     gpointer data)
{
    DBUSMedia *self = (DBUSMedia *) data;

    if (g_strcmp0("PropertiesChanged", signal_name) != 0)
        return; /* Not relevent */
    LogDebug("OnSignal Sender: %s path: %s interface: %s signal %s", sender_name, object_path, interface_name, signal_name);
    //gchar *str = g_variant_print(parameters, TRUE);
    //LogInfo("Signal: %s", str);
    //g_free(str);

    /* Get this stuff out of glib's fucked up api's (GVariant) */
    JsonNode *gson = json_gvariant_serialize(parameters);
    JsonGenerator *gen = json_generator_new();
    json_generator_set_root (gen, gson);
    gchar *str = json_generator_to_data (gen, NULL);
    g_object_unref(gen);
    json_node_free(gson);

    Json::Value root;
    try {
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Winline"
        Json::Reader reader;
        reader.parse(str, root);
        #pragma GCC diagnostic pop
    } catch(std::exception &ex) {
        abort(); //FIXME: Do what exactly?
    }
    g_free(str);

    std::stringstream ss;
    ss << root;
    LogDebug("JSON: %s", ss.str().c_str());

    try {
        std::string Album = root[1]["Metadata"]["xesam:album"].asString();
        std::string Artist = root[1]["Metadata"]["xesam:artist"][0].asString();
        std::string Title = root[1]["Metadata"]["xesam:title"].asString();
        LogDebug("New Song Info: %s - %s - %s", Album.c_str(), Artist.c_str(), Title.c_str());
        self->m_OutputManager->SetFilename(Album, Artist, Title);
    } catch(std::exception &ex) {
        LogError("Error reading signal: %s", ex.what());
    }
}

void DBUSMedia::Run() {
    GMainLoop* loop = g_main_loop_new (NULL, FALSE);
    GError *error = NULL;
    GDBusConnection *conn = g_bus_get_sync (G_BUS_TYPE_SESSION, NULL, NULL);
    GDBusProxy *proxy = g_dbus_proxy_new_sync(conn,
        G_DBUS_PROXY_FLAGS_NONE,
        NULL,
        "org.mpris.MediaPlayer2.Player",
        "/org/mpris/MediaPlayer2",
        "org.freedesktop.DBus.Properties",
        NULL,
        &error
    );

    if (error) {
        //FIXME: Make this meaningful
        LogCritical("Unknown DBUS Error");
        g_error_free(error);
    }

    if (!conn || !proxy) {
        LogCritical("Fails to get connection to dbus");
        abort();
    }

    int id = g_dbus_connection_signal_subscribe(conn,
            NULL,
            "org.freedesktop.DBus.Properties",
            "PropertiesChanged",
            "/org/mpris/MediaPlayer2",
            NULL,
            G_DBUS_SIGNAL_FLAGS_NONE,
            OnSignal,
            this,
            NULL
        );

    while(m_running) {
        g_main_loop_run(loop);
    }

    g_main_loop_unref(loop);

    g_dbus_connection_signal_unsubscribe(conn, id);

    gst_object_unref(proxy);
}
