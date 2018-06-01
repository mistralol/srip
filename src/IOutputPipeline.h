
class IOutputPipeline {
    public:
        virtual ~IOutputPipeline() { };

        virtual void Start() = 0;
        virtual void Stop() = 0;

        virtual void PushBuffer(GstCaps *caps, GstBuffer *buffer) = 0;
        virtual void SetFileName(const std::string filename) = 0;
};
