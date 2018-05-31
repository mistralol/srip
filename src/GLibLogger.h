
class GLibLogger
{
	public:
		GLibLogger();
		~GLibLogger();

    private:

        static void GLibLogHandler(const gchar *domain, GLogLevelFlags level, const gchar *msg, gpointer data);

	private:
		guint m_handle;
};
