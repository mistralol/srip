
class OutputLame : public IOutputPipeline {
    public:
        OutputLame();
        ~OutputLame();

        void OnPreStart();
        void OnStart(GstElement *pipeline);
        void OnStop(GstElement *pipeline);

        void PushBuffer(GstCaps *caps, GstBuffer *buffer);
        void SetFileName(const std::string filename);

    private:
        static void OnOverRun(GstElement* object, gpointer user_data);
        static GstFlowReturn OnNewSample(GstElement* object, gpointer user_data);

        Mutex m_mutex;
        std::string m_filename;
        GstAppSrc *m_thesrc;
};
