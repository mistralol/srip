

class PipelineBasic {
	protected:
		PipelineBasic();

	public:
		PipelineBasic(const std::string &name, const std::string &pipe);
		PipelineBasic(const std::string &pipe);
		virtual ~PipelineBasic();

		void SetName(const std::string &name);
		void SetRestart(bool value);
		void SetRestartDelay(const struct timespec *ts);
		void SetFastShutdown(bool value);
		void SetDeepNotify(bool value);

		void Start();
		void Stop();
		bool IsRunning();	//Return is pipeline is actually running
		bool HasExited();
		void WaitForExit();
		bool WaitForExit(const struct timespec *timeout);
		void WaitForStart();
		bool WaitForStart(const struct timespec *timeout);

		int GetRestartCount();

		void DumpToDot(const std::string &fname);

	protected:
		virtual void OnParseError(const gchar *msg);
		virtual void OnStart(GstElement *pipeline);
		virtual void OnStartFailure(GstElement *pipeline);

		virtual void OnIdle();
		virtual bool OnBusMessage(GstMessage *msg, bool *stop);
		virtual void OnInfo(const gchar *msg, const gchar *details);
		virtual void OnWarning(const gchar *msg, const gchar *details);
		virtual void OnError(const gchar *msg, const gchar *details);

		virtual void OnStop(GstElement *pipeline);
		virtual void OnExit();

	private:
		static void *Run(void *arg);

		bool SetState(GstElement *pipeline, GstState state);
		bool WaitForEos(GstElement *pipeline, GstBus *bus);
		bool ProcessBusMessage(GstElement *pipeline, GstMessage *msg);

	protected:
		std::string m_name;

		Mutex m_mutex;
		pthread_t m_thread;
		volatile bool m_running;
		std::string m_pipelinestr;
		Mutex m_StartedMutex;
		volatile bool m_started;
		bool m_restart;
		bool m_exited;
		bool m_fastshutdown;
		bool m_deepnotify;
		struct timespec m_restartdelay;
		//If you access m_pipeline it is safe to do so without a lock so long as it is a callback from the pipeline
		//If you access it from outside the pipeline then you must hold m_mutex and also expect it to be a NULL value!
		GstElement *m_pipeline;
		int m_TotalRestarts;
};


