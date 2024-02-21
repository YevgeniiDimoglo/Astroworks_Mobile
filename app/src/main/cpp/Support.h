//
// Created by Yujin on 2024/02/22.
//

#ifndef ASTROWORKS_MOBILE_SUPPORT_H
#define ASTROWORKS_MOBILE_SUPPORT_H

struct ANativeWindowDeleter {
    void operator()(ANativeWindow *window) { ANativeWindow_release(window); }
};

static std::vector<uint8_t> LoadBinaryFileToVector(const char *file_path,
                                                   AAssetManager *assetManager) {
    std::vector<uint8_t> file_content;
    assert(assetManager);
    AAsset *file =
            AAssetManager_open(assetManager, file_path, AASSET_MODE_BUFFER);
    size_t file_length = AAsset_getLength(file);

    file_content.resize(file_length);

    AAsset_read(file, file_content.data(), file_length);
    AAsset_close(file);
    return file_content;
}

#endif //ASTROWORKS_MOBILE_SUPPORT_H
