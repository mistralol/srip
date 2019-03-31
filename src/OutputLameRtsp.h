
class OutputLameRtsp : public IOutputPipeline {
    public:
        OutputLameRtsp(const std::string &uri);
        ~OutputLameRtsp();

        void Stop();

        void OnPreStart();
        void OnStart(GstElement *pipeline);
        void OnStop(GstElement *pipeline);

        void PushBuffer(GstCaps *caps, GstBuffer *buffer);
        void SetFileName(const std::string filename) { };
        bool StopOnFileChange() { return false; }

    private:
        static void OnOverRun(GstElement* object, gpointer user_data);
        static GstFlowReturn OnNewSample(GstElement* object, gpointer user_data);

        Mutex m_mutex;
        std::string m_uri;
        GstAppSrc *m_thesrc;
        GstClockTime m_basetime;
};
