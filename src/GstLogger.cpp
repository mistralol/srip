
#include "main.h"

GstLogger::GstLogger()
{
	m_enabled = false;
	gst_debug_add_log_function(&GstLogger::GstLogFunction, this, NULL);
    gst_debug_remove_log_function(gst_debug_log_default);
}

GstLogger::~GstLogger()
{
	if (gst_debug_remove_log_function(&GstLogger::GstLogFunction) != 1)
		abort(); //We should have only added 1 instance
    gst_debug_add_log_function(&GstLogger::GstLogFunction, NULL, NULL);
}

bool GstLogger::GetEnabled()
{
	return m_enabled;
}

void GstLogger::SetEnabled(bool value)
{
	m_enabled = value;
    gst_debug_set_active(value);
    //Switch on INFO logging unless the environment variable exists to override it
    if (getenv("GST_DEBUG") == NULL) {
        gst_debug_set_default_threshold(GST_LEVEL_WARNING);
    }
}

void GstLogger::GstLogFunction (GstDebugCategory *category, GstDebugLevel level,
				const gchar *file, const gchar *function, gint line,
				GObject *object, GstDebugMessage *message, gpointer user_data)
{
	GstLogger *self = (GstLogger *) user_data;
	if (self->m_enabled == false)
		return;

	char *msg = NULL;
	const gchar* str = gst_debug_message_get(message);
	if (asprintf(&msg, "%s:%s:%d Msg: %s", file, function, line, str) < 0)
		abort();

	int len = strlen(msg);
	if (len > 0 && msg[len-1] == '\n')
		msg[len-1] = 0;

	switch(level)
	{
		case GST_LEVEL_NONE:
			LogInfo("Gstreamer logging has been disable wit GST_LEVEL_NONE");
			break;
		case GST_LEVEL_ERROR:
			LogError("GST ERROR: %s", msg);
			break;
		case GST_LEVEL_WARNING:
			LogWarning("GST WARN: %s", msg);
			break;
		case GST_LEVEL_FIXME:
			LogWarning("GST FIXME: %s", msg);
			break;
		case GST_LEVEL_INFO:
			LogInfo("GST INFO: %s", msg);
			break;
		case GST_LEVEL_DEBUG:
			LogDebug("GST DEBUG: %s", msg);
			break;
		case GST_LEVEL_LOG:
			LogInfo("GST LOG: %s", msg);
			break;
		case GST_LEVEL_TRACE:
			LogDebug("GST TRACE: %s", msg);
			break;
		case GST_LEVEL_MEMDUMP:
			LogDebug("GST MEMDUMP: %s", msg);
			break;
		case GST_LEVEL_COUNT:
			abort();
			break; //Should not happen
		default:
			abort(); //FIXME: Unhandled case
	}
	free(msg);
}
