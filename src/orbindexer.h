#if ((CV_MAJOR_VERSION == 2) && (CV_MINOR_VERSION >=4))
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

class OrbIndexer: public Nan::ObjectWrap {
public:
  static Nan::Persistent<FunctionTemplate> constructor;
  static void Init(Local<Object> target);
  static NAN_METHOD(IndexImage);
  static NAN_METHOD(InitWordIndex);
  static ORBWordIndex * wordIndex;
private:
  static const char * ToCString(const String::Utf8Value& str);
};

#endif
