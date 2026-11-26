#pragma once

#include <string>
#include "raylib.h"

// ImageBuffer: Owns a cpu-side raylib Image copy (Rule of Five)
// Used for safe cropping & filtering without altering originals

class ImageBuffer {
private:
    Image data;
    bool valid;

public:
    ImageBuffer();
    explicit ImageBuffer(const std::string& filePath);
    explicit ImageBuffer(Image img);

    // Rule of Five
    ImageBuffer(const ImageBuffer& other);
    ImageBuffer& operator=(const ImageBuffer& other);
    ImageBuffer(ImageBuffer&& other) ;
    ImageBuffer& operator=(ImageBuffer&& other);
    ~ImageBuffer();

    bool isValid() const;
    int getWidth() const;
    int getHeight() const;
    Image& getRawImage();
    const Image& getRawImage() const;

    // Basic crop & resize helpers
    void cropSquare();
    void resizeToMax(int maxSide);
};

// CachedTexture: Plain struct pairing a path with its GPU texture

struct CachedTexture {
    std::string path;
    Texture2D texture;
    bool active;
};


// ImageStore: Single owner & GPU texture cache for all images
class ImageStore {
private:
    static const int MAX_CACHED_TEXTURES = 256;
    static CachedTexture cache[MAX_CACHED_TEXTURES];
    static int cacheCount;

    static Texture2D fallbackTexture;
    static bool initialized;
    static int nextImageId;

    static void ensureFallback();
    static int findIndexByPath(const std::string& filePath);

public:
    // Pure static service class
    ImageStore() = delete;

    // Import an external image into images/<subFolder>/ ("posts" or "avatars")
    // Returns the saved relative path (e.g., "images/posts/post_1.png")
    static std::string importImage(const std::string& sourcePath, const std::string& subFolder);

    // Save an edited ImageBuffer (after crop/filters) into images/<subFolder>/
    static std::string saveEditedImage(const ImageBuffer& buffer, const std::string& subFolder);

    // Load or retrieve cached GPU Texture2D (returns fallback if missing/invalid)
    static Texture2D getTexture(const std::string& filePath);

    // Remove a single texture from cache (when a post/avatar is updated or deleted)
    static void unloadTexture(const std::string& filePath);

    // Delete file from disk during cascading post deletion (Integration Rule 7)
    static bool deleteImageFile(const std::string& filePath);

    // Unload all cached textures and fallback on app shutdown
    static void clearAll();
};