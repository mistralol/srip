
class GstLogger
{
	public:
		GstLogger();
		~GstLogger();
		
		bool GetEnabled();
		void SetEnabled(bool value);
		
		static void GstLogFunction (GstDebugCategory *category, GstDebugLevel level,
				const gchar *file, const gchar *function, gint line,
				GObject *object, GstDebugMessage *message, gpointer user_data);
				
	private:
		bool m_enabled;
};
