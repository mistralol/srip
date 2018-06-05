
class IOutputPipeline : public PipelineBasic {
    public:
        virtual ~IOutputPipeline() { };

        virtual void OnPreStart() = 0;

        virtual void PushBuffer(GstCaps *caps, GstBuffer *buffer) = 0;
        virtual void SetFileName(const std::string filename) = 0;
};
