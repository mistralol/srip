
#include "main.h"

PipelineBasic::PipelineBasic() {
	m_name = "";
	m_running = false;
	m_pipelinestr = "";
	m_started = false;
	m_restart = false;
	m_restartdelay.tv_sec = 0;
	m_restartdelay.tv_nsec = 0;
	m_exited = false;
	m_fastshutdown = true;
	m_deepnotify = false;
	m_pipeline = NULL;
	m_TotalRestarts = 0;
}

PipelineBasic::PipelineBasic(const std::string &name, const std::string &pipe)
{
	m_name = name;
	m_running = false;
	m_pipelinestr = pipe;
	m_started = false;
	m_restart = true;
	m_restartdelay.tv_sec = 30;
	m_restartdelay.tv_nsec = 0;
	m_exited = false;
	m_fastshutdown = true;
	m_deepnotify = false;
	m_pipeline = NULL;
	m_TotalRestarts = 0;
}

PipelineBasic::PipelineBasic(const std::string &pipe)
{
	m_name = "";
	m_running = false;
	m_pipelinestr = pipe;
	m_started = false;
	m_restart = true;
	m_restartdelay.tv_sec = 30;
	m_restartdelay.tv_nsec = 0;
	m_exited = false;
	m_fastshutdown = true;
	m_deepnotify = false;
	m_pipeline = NULL;
	m_TotalRestarts = 0;
}

PipelineBasic::~PipelineBasic()
{
	if (m_running == true)
		Stop();
}

void PipelineBasic::SetName(const std::string &name)
{
	m_name = name;
}

void PipelineBasic::SetRestart(bool value)
{
	m_restart = value;
}

void PipelineBasic::SetRestartDelay(const struct timespec *ts)
{
	m_restartdelay = *ts;
}

void PipelineBasic::SetFastShutdown(bool value) {
	m_fastshutdown = value;
}

void PipelineBasic::SetDeepNotify(bool value) {
	m_deepnotify = value;
}

void PipelineBasic::Start()
{
	m_running = true;
	if(pthread_create(&m_thread, NULL, &Run, this))
		abort();
}

void PipelineBasic::Stop()
{
	void *retval = NULL;
	ScopedLock lock(&m_mutex);
	if (m_pipeline)
	{
		//Kick Pipeline
		SetState(m_pipeline, GST_STATE_PAUSED);
	}
	m_running = false;
	m_mutex.WakeUpAll();
	lock.Unlock();

	if (pthread_join(m_thread, &retval))
		abort();
}

bool PipelineBasic::IsRunning() {
	ScopedLock lock(&m_mutex);
	if (m_pipeline)
		return true;
	return false;
}

bool PipelineBasic::HasExited()
{
	ScopedLock lock(&m_mutex);
	if (m_restart == true)
	{
		LogCritical("Pipeline '%s' It does not make sense to call HasExited when restart it set to true", m_name.c_str());
		abort();
	}
	return m_exited;
}

void PipelineBasic::WaitForExit()
{
	ScopedLock lock(&m_mutex);
	while(m_exited == false)
	{
		m_mutex.Wait();
	}
}

bool PipelineBasic::WaitForExit(const struct timespec *timeout)
{
	ScopedLock lock(&m_mutex);
	while(m_exited == false)
	{
		if (m_mutex.Wait(timeout) < 0)
			return false;
	}
	return true;
}

void PipelineBasic::WaitForStart() {
	ScopedLock lock(&m_StartedMutex);
	while(m_started == false)
	{
		m_StartedMutex.Wait();
	}
}

bool PipelineBasic::WaitForStart(const struct timespec *timeout)
{
	ScopedLock lock(&m_StartedMutex);
	while(m_started == false)
	{
		if (m_StartedMutex.Wait(timeout) < 0)
			return m_started;
	}
	return true;
}

int PipelineBasic::GetRestartCount() {
	return m_TotalRestarts;
}

void PipelineBasic::DumpToDot(const std::string &fname)
{
	ScopedLock lock(&m_mutex);
	if (m_pipeline) {
		LogNotice("Dumping Pipeline '%s' to dot file '%s'", m_name.c_str(), fname.c_str());
		gchar *data = gst_debug_bin_to_dot_data((GstBin *) m_pipeline, GST_DEBUG_GRAPH_SHOW_ALL);
		std::string cmd = "dot -Tpng -o" + fname;
		FILE *fp = popen(cmd.c_str(), "w");
		if (!fp) {
			LogError("Cannot write to command '%s'", cmd.c_str());
			return;
		}
		fprintf(fp, "%s", data);
		pclose(fp);
		gst_debug_bin_to_dot_file((GstBin *) m_pipeline, GST_DEBUG_GRAPH_SHOW_ALL, fname.c_str());
	} else {
		LogWarning("Cannot dump '%s' to dot file as pipeline does not exist", m_name.c_str());
	}
}

void PipelineBasic::OnParseError(const gchar *msg)
{
	LogError("Pipeline '%s' Cannot Parse: %s", m_name.c_str(), m_pipelinestr.c_str());
	LogError("Pipeline '%s' Parse error: %s", m_name.c_str(), msg);
}

void PipelineBasic::OnPreStart() {

}

void PipelineBasic::OnStart(GstElement *pipeline)
{
	LogInfo("Pipeline '%s' Started Pipe: %s", m_name.c_str(), m_pipelinestr.c_str());
}

void PipelineBasic::OnIdle() {
	//Nothing would be noisy
}

void PipelineBasic::OnStartFailure(GstElement *pipeline)
{
	LogError("Pipeline '%s' Failed To Start", m_name.c_str());
}

void PipelineBasic::OnStop(GstElement *pipeline)
{
	LogInfo("Pipeline '%s' Stopped", m_name.c_str());
}

//If this is overriden and you return true and set *stop to true
//It will cause the pipeline to stop
bool PipelineBasic::OnBusMessage(GstMessage *msg, bool *stop)
{
#if 0
	LogDebug("Pipeline '%s' Message(%u) From '%s' Of Type: '%s'",
			m_name.c_str(), gst_message_get_seqnum(msg),
			GST_MESSAGE_SRC_NAME(msg),
			gst_message_type_get_name(GST_MESSAGE_TYPE(msg))
	);
#endif

	return false;
}

void PipelineBasic::OnInfo(const gchar *msg, const gchar *details)
{
	LogInfo("Pipeline '%s', INFO: %s Details: %s", m_name.c_str(), msg, details);
}

void PipelineBasic::OnWarning(const gchar *msg, const gchar *details)
{
	LogWarning("Pipeline '%s', WARNING: %s Details: %s", m_name.c_str(), msg, details);
}

void PipelineBasic::OnError(const gchar *msg, const gchar *details)
{
	LogError("Pipeline '%s', ERROR: %s Details: %s", m_name.c_str(), msg, details);
}

void PipelineBasic::OnExit()
{
	LogDebug("Pipeline '%s' Exited", m_name.c_str());
}

void *PipelineBasic::Run(void *arg)
{
	PipelineBasic *self = (class PipelineBasic *) arg;
	GstElement *pipeline = NULL;

	self->OnPreStart();

	while(self->m_running)
	{
		GError *error = NULL;
		pipeline = gst_parse_launch (self->m_pipelinestr.c_str(), &error);
		if (!pipeline) {
			self->OnParseError(error->message);

			//Not going to ever work bail so exit
			ScopedLock lock(&self->m_mutex);
			self->m_exited = true;
			self->m_mutex.WakeUpAll();
			g_error_free(error);
			return NULL;
		}

		//We also can get an error where pipeline != NULL
		if (error != NULL) {
			gst_object_unref(pipeline);
			pipeline = NULL;

			self->OnParseError(error->message);

			//Not going to ever work bail so exit
			ScopedLock lock(&self->m_mutex);
			self->m_exited = true;
			self->m_mutex.WakeUpAll();

			g_error_free(error);
			return NULL;
		}

		if (self->m_deepnotify) {
			gst_element_add_property_deep_notify_watch (pipeline, NULL, TRUE);
		}

		//Export pipeline
		self->m_mutex.Lock();
		self->m_pipeline = pipeline;
		self->m_mutex.Unlock();

		self->OnStart(pipeline);

		{
			ScopedLock StartLock(&self->m_StartedMutex);
			self->m_started = true;
			self->m_StartedMutex.WakeUpAll();
		}

		GstBus *bus = gst_element_get_bus(pipeline);
		if (self->SetState(pipeline, GST_STATE_PLAYING) == true)
		{
			while (self->WaitForEos(pipeline, bus) == false && self->m_running == true)
			{
				self->OnIdle();
			}
		}
		else
		{
			self->OnStartFailure(pipeline);
		}

		self->OnStop(pipeline);

		if (self->m_fastshutdown == false) {
			if (self->SetState(pipeline, GST_STATE_PAUSED) == true) {
				bool Done = false;
				GstClockTime timeout = GST_SECOND * 3;
				do {
					GstMessage *msg = gst_bus_timed_pop (bus, timeout); //We have to poll because this gstreamer function sometimes never returns
					if (msg == NULL) {
						GstState state;
						if (gst_element_get_state(pipeline, &state, NULL, GST_CLOCK_TIME_NONE) != GST_STATE_CHANGE_SUCCESS)
							Done = true;
						if (state != GST_STATE_PAUSED) {
							LogError("Pipeline '%s' Failed to Set pipeline state to PAUSED", self->m_name.c_str());
							Done = true;
						} else {
							LogDebug("Pipeline '%s' Pipeline state not yet changed to PAUSED", self->m_name.c_str());
							Done = true;
						}
					} else {
							if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_PROGRESS) {
							GstProgressType type;
							gchar *code;
							gchar *text;
							gst_message_parse_progress (msg, &type, &code, &text);
							switch(type) {
								case GST_PROGRESS_TYPE_START:
								case GST_PROGRESS_TYPE_CONTINUE:
									break;
								case GST_PROGRESS_TYPE_COMPLETE:
									LogDebug("Pipeline '%s' Change to PAUSED is Complete", self->m_name.c_str());
									Done = true;
									break;
								case GST_PROGRESS_TYPE_CANCELED:
									LogDebug("Pipeline '%s' Change to PAUSED is Cancelled", self->m_name.c_str());
									Done = true;
									break;
								case GST_PROGRESS_TYPE_ERROR:
									LogDebug("Pipeline '%s' Change to PAUSED is Error: %s", self->m_name.c_str(), text);
									Done = true;
									break;
								default:
									LogCritical("Pipeline '%s' Unknown Case Code: %s Text: %s", self->m_name.c_str(), code, text);
									abort();
									break;
							}
							g_free(code);
							g_free(text);
						} else {
							self->ProcessBusMessage(pipeline, msg);
						}
						gst_message_unref(msg);
					}
				} while(Done == false);
			}

			if (self->SetState(pipeline, GST_STATE_READY) == true) {
				bool Done = false;
				GstClockTime timeout = GST_SECOND * 3;
				do {
					GstMessage *msg = gst_bus_timed_pop (bus, timeout); //We have to poll because this gstreamer function sometimes never returns
					if (msg == NULL) {
						GstState state;
						if (gst_element_get_state(pipeline, &state, NULL, GST_CLOCK_TIME_NONE) != GST_STATE_CHANGE_SUCCESS)
							Done = true;
						if (state != GST_STATE_READY) {
							LogError("Pipeline '%s' Failed to Set pipeline state to READY", self->m_name.c_str());
							Done = true;
						} else {
							LogDebug("Pipeline '%s' Pipeline state not yet changed to READY", self->m_name.c_str());
							Done = true;
						}
					} else {
						if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_PROGRESS) {
							GstProgressType type;
							gchar *code;
							gchar *text;
							gst_message_parse_progress (msg, &type, &code, &text);
							switch(type) {
								case GST_PROGRESS_TYPE_START:
								case GST_PROGRESS_TYPE_CONTINUE:
									break;
								case GST_PROGRESS_TYPE_COMPLETE:
									LogDebug("Pipeline '%s' Change to READY is Complete", self->m_name.c_str());
									Done = true;
									break;
								case GST_PROGRESS_TYPE_CANCELED:
									LogDebug("Pipeline '%s' Change to READY is Cancelled", self->m_name.c_str());
									Done = true;
									break;
								case GST_PROGRESS_TYPE_ERROR:
									LogDebug("Pipeline '%s' Change to READY is Error: %s", self->m_name.c_str(), text);
									Done = true;
									break;
								default:
									LogCritical("Pipeline '%s' Unknown Case Code: %s Text: %s", self->m_name.c_str(), code, text);
									abort();
									break;
							}
							g_free(code);
							g_free(text);
						} else {
							self->ProcessBusMessage(pipeline, msg);
						}
						gst_message_unref(msg);
					}
				} while(Done == false);
			}
		} //End of m_fastshutdown == false

		if (self->SetState(pipeline, GST_STATE_NULL) == false)
		{
			LogCritical("Pipeline '%s' Failed to Set pipeline state to NULL", self->m_name.c_str());
			abort();
		}

		self->m_started = false;

		//Un export pipeline
		self->m_mutex.Lock();
		self->m_pipeline = NULL;
		self->m_TotalRestarts++;
		self->m_mutex.Unlock();

		gst_object_unref(bus);
		gst_object_unref(pipeline);
		pipeline = NULL;

		//Take lock and see if we need to restart
		do {
			ScopedLock lock(&self->m_mutex);
			if (self->m_restart == false)
			{
				self->m_exited = true;
				self->m_mutex.WakeUpAll();
				return NULL;
			}

			if (self->m_running)
				self->m_mutex.Wait(&self->m_restartdelay);
		} while(0);
	}

	do {
		ScopedLock lock(&self->m_mutex);
		self->m_exited = true;
		self->m_mutex.WakeUpAll();
	} while(0);

	return NULL;
}

bool PipelineBasic::SetState(GstElement *pipeline, GstState state)
{
	GstStateChangeReturn ret = gst_element_set_state (pipeline, state);
	if (ret == GST_STATE_CHANGE_SUCCESS)
		return true;
	if (ret == GST_STATE_CHANGE_FAILURE)
		return false;
	if (ret == GST_STATE_CHANGE_NO_PREROLL)
	{
		//This isn't really an error
		//LogError("Pipeline '%s' GstUtil::SetState Unhandled Error Code GST_STATE_CHANGE_NO_PREROLL", m_name.c_str());
		return true;
	}

	if (ret == GST_STATE_CHANGE_ASYNC)
	{
		GstState curstate;
		GstClockTime timeout = GST_SECOND * 15;
		int retries = 1;

		for(int i =0;i<retries;i++)
		{
			ret = gst_element_get_state(pipeline, &curstate, NULL, timeout);
			if (ret == GST_STATE_CHANGE_SUCCESS)
				return true;
			if (ret == GST_STATE_CHANGE_FAILURE)
				return false;
			LogWarning("Pipeline '%s' gst_element_get_state timeout: %s", m_name.c_str(), gst_element_state_change_return_get_name(ret));
		}

		LogError("Pipeline '%s' GstUtil::SetState Lockup detected", m_name.c_str());
		return false;
	}
	else
	{
		LogCritical("Pipeline '%s' GstUtil::SetState Invalid Return from gst_element_set_state", m_name.c_str());
		abort(); //Accoridng to gstreamer documentation this is not reachable
	}
}

bool PipelineBasic::WaitForEos(GstElement *pipeline, GstBus *bus)
{
	GstClockTime timeout = GST_SECOND; //1 Seconds
	struct timespec started;

	Time::MonoTonic(&started);

	GstMessage *msg = gst_bus_timed_pop (bus, timeout); //We have to poll because this gstreamer function sometimes never returns
	if (msg == NULL)
	{
		GstState state;
		if (gst_element_get_state(pipeline, &state, NULL, GST_CLOCK_TIME_NONE) != GST_STATE_CHANGE_SUCCESS)
			return true;
		if (state != GST_STATE_PLAYING)
		{
			return true;
		}

		//Sometimes gst_bus_timed_pop gets into a state where it constantly wakes up
		//Deal with this to prevent 100% cpu being used
		struct timespec now;
		Time::MonoTonic(&now);
		GstClockTime gstarted = GST_TIMESPEC_TO_TIME(started);
		GstClockTime gnow = GST_TIMESPEC_TO_TIME(now);
		GstClockTime gdiff = GST_CLOCK_DIFF(gstarted, gnow);
		if (gdiff < timeout)
		{
			LogAlert("Pipeline '%s' gst_bus_timed_pop no sleep bug is active", m_name.c_str());
			usleep(gdiff); //Sleep for remaining time
		}

		return false;
	}

	bool Flag = false;
	if (OnBusMessage(msg, &Flag))
	{
		gst_message_unref (msg);
		return Flag;
	}

	Flag = ProcessBusMessage(pipeline, msg);
	gst_message_unref (msg);
	return Flag;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
bool PipelineBasic::ProcessBusMessage(GstElement *pipeline, GstMessage *msg) {
	switch (GST_MESSAGE_TYPE (msg)) {
		case GST_MESSAGE_EOS:
			return true;
			break;
		case GST_MESSAGE_INFO:
		{
			GError *info = NULL; /* info to show to users                 */
			gchar *dbg = NULL;  /* additional debug string for developers */
			gst_message_parse_info(msg, &info, &dbg);
			if (info && dbg)
				OnInfo(info->message, dbg);
			g_error_free(info);
			g_free(dbg);
			break;
		}
		case GST_MESSAGE_WARNING:
		{
			GError *warn = NULL; /* info to show to users                 */
			gchar *dbg = NULL;  /* additional debug string for developers */
			gst_message_parse_warning(msg, &warn, &dbg);
			if (warn && dbg)
				OnWarning(warn->message, dbg);
			g_error_free(warn);
			g_free(dbg);
			break;
		}
		case GST_MESSAGE_ERROR:
		{
			GError *err = NULL; /* error to show to users                 */
			gchar *dbg = NULL;  /* additional debug string for developers */
			gst_message_parse_error(msg, &err, &dbg);
			if (err && dbg)
				OnError(err->message, dbg);
			g_error_free (err);
			g_free(dbg);
			return true;
			break;
		}
		case GST_MESSAGE_PROGRESS:
		{
                GstProgressType type;
                gchar *code;
                gchar *text;
				gst_message_parse_progress (msg, &type, &code, &text);
				switch(type) {
					case GST_PROGRESS_TYPE_ERROR:
					case GST_PROGRESS_TYPE_CANCELED:
						LogError("Stream '%s' Got Progress Message Type: %d Code: %s Text: %s", m_name.c_str(), type, code, text);
						break;
					default:
						//LogDebug("Stream '%s' Got Progress Message Type: %d Code: %s Text: %s", m_name.c_str(), type, code, text);
						break;
				}
                g_free(code);
                g_free(text);
				break;
        }
		case GST_MESSAGE_PROPERTY_NOTIFY:
		{
			GstObject *object = NULL;
			const gchar *name = NULL;
			const GValue *value = NULL;
			gst_message_parse_property_notify(msg, &object, &name, &value);

			gchar *obj_name = gst_object_get_path_string(GST_OBJECT(object));
			gchar *val_str = NULL;
			if (value != NULL) {
				if (G_VALUE_HOLDS_STRING(value))
					val_str = g_value_dup_string(value);
				else if (G_VALUE_TYPE(value) == GST_TYPE_CAPS)
					val_str = gst_caps_to_string((GstCaps *) g_value_get_boxed(value));
				else if (G_VALUE_TYPE (value) == GST_TYPE_TAG_LIST)
					val_str = gst_tag_list_to_string((GstTagList *) g_value_get_boxed(value));
				else if (G_VALUE_TYPE (value) == GST_TYPE_STRUCTURE)
					val_str = gst_structure_to_string((GstStructure *) g_value_get_boxed(value));
				else
					val_str = gst_value_serialize (value);
			} else {
				val_str = g_strdup ("(no value)");
			}

			LogDebug("Pipeline '%s' Notify From %s %s", m_name.c_str(), name, val_str);

			g_free(obj_name);
			g_free(val_str);
		}
		//Suppress Noise
		case GST_MESSAGE_TAG:
		case GST_MESSAGE_STATE_CHANGED:
		case GST_MESSAGE_STREAM_START:
		case GST_MESSAGE_STREAM_STATUS:
		case GST_MESSAGE_LATENCY:
		case GST_MESSAGE_ASYNC_START:
		case GST_MESSAGE_ASYNC_DONE:
		case GST_MESSAGE_NEW_CLOCK:
		case GST_MESSAGE_ELEMENT:
			break;

		default:
			LogDebug("Pipeline '%s' Unexpected message of type %s from %s", m_name.c_str(), GST_MESSAGE_TYPE_NAME(msg), GST_MESSAGE_SRC_NAME(msg));
			break;
	}
	return false;
}
#pragma GCC diagnostic pop

