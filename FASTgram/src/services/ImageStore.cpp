#include "ImageStore.h"
#include <filesystem>
#include <cstdio>

CachedTexture ImageStore::cache[ImageStore::MAX_CACHED_TEXTURES];
int ImageStore::cacheCount = 0;
Texture2D ImageStore::fallbackTexture = { 0 };
bool ImageStore::initialized = false;
int ImageStore::nextImageId = 1;

ImageBuffer::ImageBuffer() {
    data = { 0 };
    valid = false;
}

ImageBuffer::ImageBuffer(const std::string& filePath) {
    data = LoadImage(filePath.c_str());
    valid = IsImageValid(data);
}

ImageBuffer::ImageBuffer(Image img) {
    this->data = img;
    this->valid = IsImageValid(this->data);
}

ImageBuffer::ImageBuffer(const ImageBuffer& other) {
    if (other.valid) {
        data = ImageCopy(other.data);
        valid = IsImageValid(data);
    }
    else {
        data = { 0 };
        valid = false;
    }
}

ImageBuffer& ImageBuffer::operator=(const ImageBuffer& other) {
    if (this == &other) {
        return *this;
    }

    if (valid) {
        UnloadImage(data);
    }

    if (other.valid) {
        data = ImageCopy(other.data);
        valid = true;
    }
    else {
        data = { 0 };
        valid = false;
    }

    return *this;
}

ImageBuffer::ImageBuffer(ImageBuffer&& other) {
    this->data = other.data;
    this->valid = other.valid;

    other.data = { 0 };
    other.valid = false;
}

ImageBuffer& ImageBuffer::operator=(ImageBuffer&& other) {
    if (this != &other) {
        if (this->valid) {
            UnloadImage(this->data);
        }

        this->data = other.data;
        this->valid = other.valid;

        other.data = { 0 };
        other.valid = false;
    }

    return *this;
}

Image& ImageBuffer::getRawImage() {
    return this->data;
}

const Image& ImageBuffer::getRawImage() const {
    return this->data;
}

bool ImageBuffer::isValid() const {
    return this->valid;
}

int ImageBuffer::getWidth() const {
    return this->data.width;
}

int ImageBuffer::getHeight() const {
    return this->data.height;
}

void ImageBuffer::cropSquare() {
    if (!valid) {
        return;
    }

    int side = (data.width < data.height) ? data.width : data.height;
    float offsetX = (data.width - side) / 2.0f;
    float offsetY = (data.height - side) / 2.0f;

    Rectangle cropRect = { offsetX, offsetY, (float)side, (float)side };
    ImageCrop(&data, cropRect);
}

void ImageBuffer::resizeToMax(int maxSide) {
    if (!valid || maxSide <= 0) {
        return;
    }

    int largerSide = (data.width > data.height) ? data.width : data.height;
    if (largerSide <= maxSide) {
        return; // already fits within maxSide, no resize needed
    }

    float scale = (float)maxSide / (float)largerSide;
    int newWidth = (int)(data.width * scale);
    int newHeight = (int)(data.height * scale);

    ImageResize(&data, newWidth, newHeight);
}

ImageBuffer::~ImageBuffer() {
    if (valid) {
        UnloadImage(this->data);
        this->data = { 0 };
        valid = false;
    }
}

//image store
void ImageStore::ensureFallback() {
    if (initialized) {
        return;
    }

    Image placeholder = GenImageColor(64, 64, DARKGRAY);
    fallbackTexture = LoadTextureFromImage(placeholder);
    UnloadImage(placeholder);

    initialized = true;
}

int ImageStore::findIndexByPath(const std::string& filePath) {
    for (int i = 0; i < cacheCount; i++) {
        if (cache[i].path == filePath) {
            return i;
        }
    }
    return -1;
}

std::string ImageStore::importImage(const std::string& sourcePath, const std::string& subFolder) {
    Image image = LoadImage(sourcePath.c_str());
    if (!IsImageValid(image)) {
        return "";
    }

    // Create target directories if they do not exist yet
    std::string dirPath = "images/" + subFolder;
    std::filesystem::create_directories(dirPath);

    // Build a unique relative path (e.g., "images/posts/post_1.png")
    std::string prefix = (subFolder == "avatars") ? "avatar_" : "post_";
    std::string destPath = dirPath + "/" + prefix + std::to_string(nextImageId++) + ".png";

    bool saved = ExportImage(image, destPath.c_str());
    UnloadImage(image); // Free RAM copy immediately after saving to disk

    return saved ? destPath : "";
}

std::string ImageStore::saveEditedImage(const ImageBuffer& buffer, const std::string& subFolder) {
    if (!buffer.isValid()) {
        return "";
    }

    std::string dirPath = "resources/images/" + subFolder;
    std::filesystem::create_directories(dirPath);

    std::string prefix = (subFolder == "avatars") ? "avatar_" : "post_";
    std::string destPath = dirPath + "/" + prefix + std::to_string(nextImageId++) + ".png";

    bool saved = ExportImage(buffer.getRawImage(), destPath.c_str());
    return saved ? destPath : "";
}

Texture2D ImageStore::getTexture(const std::string& filePath) {
    ensureFallback();

    if (filePath.empty()) {
        return fallbackTexture;
    }

    // Return immediately if already cached in GPU memory
    int existingIndex = findIndexByPath(filePath);
    if (existingIndex != -1) {
        return cache[existingIndex].texture;
    }

    // Not cached yet load from disk once
    Texture2D tex = LoadTexture(filePath.c_str());
    if (!IsTextureValid(tex)) {
        return fallbackTexture;
    }

    // Evict oldest entry if cache is full
    if (cacheCount >= MAX_CACHED_TEXTURES) {
        UnloadTexture(cache[0].texture);
        for (int i = 0; i < MAX_CACHED_TEXTURES - 1; i++) {
            cache[i] = cache[i + 1];
        }
        cacheCount = MAX_CACHED_TEXTURES - 1;
    }

    // Store in cache and return
    cache[cacheCount].path = filePath;
    cache[cacheCount].texture = tex;
    cacheCount++;

    return tex;
}

void ImageStore::unloadTexture(const std::string& filePath) {
    int index = findIndexByPath(filePath);
    if (index == -1) {
        return; // Not in cache, nothing to unload
    }

    // Free the texture from GPU memory
    UnloadTexture(cache[index].texture);

    // Shift remaining items left to close the gap in the array
    for (int i = index; i < cacheCount - 1; i++) {
        cache[i] = cache[i + 1];
    }

    //Clear the last duplicate slot and decrease count
    cache[cacheCount - 1].path = "";
    cache[cacheCount - 1].texture = { 0 };
    cacheCount--;
}

bool ImageStore::deleteImageFile(const std::string& filePath) {
    if (filePath.empty()) {
        return false;
    }

    //Unload from GPU cache first (Windows blocks deleting files still in use)
    unloadTexture(filePath);

    //Delete the physical file from disk (std::remove returns 0 on success)
    return std::remove(filePath.c_str()) == 0;
}

void ImageStore::clearAll() {
    //Unload all cached post/avatar textures
    for (int i = 0; i < cacheCount; i++) {
        UnloadTexture(cache[i].texture);
        cache[i].path = "";
        cache[i].texture = { 0 };
    }
    cacheCount = 0;

    //Unload the fallback placeholder texture if it was created
    if (initialized) {
        UnloadTexture(fallbackTexture);
        fallbackTexture = { 0 };
        initialized = false;
    }
}