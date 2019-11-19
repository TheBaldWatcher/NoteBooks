#include "AndroidAssetFileHelper.hpp"

std::tuple<std::shared_ptr<unsigned char>, long unsigned>ReadFileFromAsset(AAssetManager* mManeger, const char *data_file)
{
    std::shared_ptr<unsigned char> data = nullptr;
    // unsigned char *data = nullptr;
    long unsigned size = 0;

    if (nullptr == mManeger || nullptr == data_file) {
        LOGI_SDK("load from asset fails, no assetmanager or datafile");
        return std::make_tuple(data, size);
    }

    AAsset* dataAsset = AAssetManager_open(mManeger, data_file, AASSET_MODE_UNKNOWN);//get file read AAsset
    if (nullptr == dataAsset) {
        LOGI_SDK("load from asset fails: %s", data_file);
    } else {
        size = AAsset_getLength(dataAsset);
        // data = new unsigned char [size];
        data = std::shared_ptr<unsigned char> (new unsigned char[size], [](unsigned char *p){delete [] p;});
        int numBytesRead = AAsset_read(dataAsset, data.get(), size);  //begin to read data once time
        AAsset_close(dataAsset);
    }
    
    // LOGE_SDK("=====%s==%d==%p %d=\n", __FUNCTION__,__LINE__,data,size);
    return std::make_tuple(data, size);
}
