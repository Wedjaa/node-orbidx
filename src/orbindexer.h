#if ((CV_MAJOR_VERSION == 2) && (CV_MINOR_VERSION >=4))
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

class OrbIndexer: public Nan::ObjectWrap {
public:
  static Nan::Persistent<FunctionTemplate> constructor;
  static NAN_METHOD(New);
  static void Init(Local<Object> target);
  static NAN_METHOD(IndexImage);
  static NAN_METHOD(InitWordIndex);
  static NAN_METHOD(StartTraining);
  static NAN_METHOD(TrainImage);
  static NAN_METHOD(SaveTraining);
  static NAN_METHOD(LoadTraining);
  ORBWordIndex * wordIndex;
  cv::Mat trainingDescriptors;
  bool trainingStarted;
private:
  static const char * ToCString(const String::Utf8Value& str);
};

#endif
