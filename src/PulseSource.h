

class PulseSource : public PipelineBasic {
    public:
        PulseSource();
        ~PulseSource();

        void OnStart(GstElement *pipeline);

        void SetFunction(std::function<void(GstCaps *, GstBuffer *)> func);

    private:
        static void OnOverRun(GstElement* object, gpointer user_data);
        static GstFlowReturn OnNewSample(GstElement* object, gpointer user_data);


        Mutex m_mutex;
        std::function<void(GstCaps *, GstBuffer *)> m_callback;
        int m_idx;
};

