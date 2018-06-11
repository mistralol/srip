
class OutputManager;

class DBUSMedia {
    public:
        DBUSMedia(OutputManager *OutputManager);
        ~DBUSMedia();

        void Run();
        static void OnSignal(GDBusConnection *conn, const gchar *sender_name,
			     const gchar *object_path, const gchar *interface_name,
			     const gchar *signal_name, GVariant *parameters,
			     gpointer data);



    private:
        volatile bool m_running;
        OutputManager *m_OutputManager;
};

