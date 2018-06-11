

class OutputManager {
    public:
        OutputManager();
        ~OutputManager();

        void PushBuffer(GstCaps *caps, GstBuffer *buffer);
        void SetFilename(const std::string &Album, const std::string &Artist, const std::string &Song);

        void PipelineAdd(std::shared_ptr<IOutputPipeline> pipeline);
        void PipelineRemoveAll();

    private:
        Mutex m_mutex;
        std::list<std::shared_ptr<IOutputPipeline> > m_pipes;
        std::string m_currentfile;
};
