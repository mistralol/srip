

class OutputManager {
    public:
        OutputManager();
        ~OutputManager();

        void PushBuffer(GstCaps *caps, GstBuffer *buffer);
        void SetFileName(const std::string &filename);

        void PipelineAdd(std::shared_ptr<IOutputPipeline> pipeline);
        void PipelineRemoveAll();

    private:
        Mutex m_mutex;
        std::list<std::shared_ptr<IOutputPipeline> > m_pipes;
        std::string m_currentfile;
};
