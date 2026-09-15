#pragma once

#ifndef MEMORYTRACKER_H_
#define MEMORYTRACKER_H_

#define USE_MEMORY_TRACKER
#define USE_SDL_MEMORY_TRACKER
//#define USE_CRT_MEMORY_TRACKER
//#define USE_VLD_MEMORY_TRACKER

#if defined(USE_MEMORY_TRACKER) && defined(_DEBUG)

#include <unordered_map>
#include <cstddef>
#include <utility>
#include <SDL3/SDL_log.h>

/**
* @brief Namespace for in-house memory tracker.
*/
class MemoryTracker
{
private:

    /**
    * @brief Struct to hold metadata about each allocation, including size, source file, and line number.
    */
    struct AllocationInfo
    {
        size_t size;   /// Size of the allocation.
        const char* file;   /// File where the allocation was made.
        int line;           /// Line number in the file where the allocation was made.
    };

    static std::unordered_map<void*, AllocationInfo> allocationMap; 	/// Map to track allocations; [pointer -> metadata].
    static size_t currentMemoryUsage;   /// Current total memory usage (active allocations).
    static size_t peakMemoryUsage;      /// Peak memory usage.

    // No instancing due to static class.
    MemoryTracker() = delete;
    ~MemoryTracker() = delete;

    // No copying or moving allowed due to static class.
    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;
    MemoryTracker(MemoryTracker&&) = delete;
    MemoryTracker& operator=(MemoryTracker&&) = delete;

public:

    /**
    * @brief Initializes the memory tracker.
    *
    * All this does it set up the memory tracker to report leaks at the end of the program.
    */
    static void Init();

    /**
    * @brief Prints the report of memory leaks detected.
    *
    * The folder is defined in ConstantConfiguration::defaultMemoryReportPath. The report file will be named "MemoryReport_[timestamp].log".
    */
    static void ReportLeaks();

    /**
    * @brief Allocates memory of a given size and tracks the allocation with the source file and line number.
    *
    * @param size The size of memory to allocate in bytes.
    * @param file The source file where the allocation was called.
    * @param line The line number in the source file where the allocation was called.
    *
    * @return The pointer to the allocated memory, or nullptr if allocation fails.
    */
    static void* Allocate(size_t size, const char* file, int line);

    /**
    * @brief Deallocates memory pointed to by ptr and removes it from the tracking system.
    *
    * @param ptr The pointer to the memory to deallocate.
    */
    static void Deallocate(void* ptr);

    /**
    * @brief Allocates memory for an object of type T and constructs it with the provided arguments.
    *
    * @param file The source file where the allocation was called.
    * @param line The line number in the source file where the allocation was called.
    * @param args The arguments to forward to the constructor of T.
    *
    * @return The pointer to the object T, or nullptr if allocation fails.
    */
    template<typename T, typename... Args>
    static T* New(const char* file, int line, Args&&... args)
    {
        // Allocate raw memory.
        void* mem = Allocate(sizeof(T), file, line);
        if (!mem)
        {
            return nullptr;
        }

        // Use placement new to construct the object in the allocated memory.
        return ::new (mem) T(std::forward<Args>(args)...);
    }

    /**
    * @brief Deletes an object of type T.
    *
    * @param ptr The pointer to the object to delete.
    */
    template<typename T>
    static void Delete(T* ptr)
    {
        if (ptr)
        {
            ptr->~T();
            Deallocate(ptr);
        }
    }

#if defined(USE_SDL_MEMORY_TRACKER)

    /**
    * @brief Tracks an SDL allocation by storing its pointer and estimated size.
    *
    * \/!\ DOES NOT ALLOCATE MEMORY. DOES NOT CALL SDL_CreateX(...) OR malloc / calloc / realloc / new. THIS IS MEANT TO BE CALLED *AFTER* AN SDL ALLOCATION. /!\
    *
    * SDL doesn't expose exact internal sizes easily, so you can pass whatever you like as the estimated size.
    *
    * @param ptr The pointer to the SDL allocation.
    * @param estimatedSize The estimated size of the allocation in bytes.
    * @param file The source file where the allocation was called.
    * @param line The line number in the source file where the allocation was called.
    *
    * @return The same pointer that was passed in.
    */
    template<typename T>
    static T TrackSDLCreate(T ptr, size_t estimatedSize, const char* file, int line)
    {
        if (ptr)
        {
            // So basically, cast any pointer or handle to uintptr_t (if it's less than, it will widen it to 8 bytes), then cast to void.
            void* key = (void*)(uintptr_t)(ptr);

            allocationMap[key] = AllocationInfo {
                .size = estimatedSize,
                .file = file,
                .line = line
            };
        }
        return ptr;
    }

    /**
    * @brief Tracks the destruction of an SDL allocation by removing its pointer from the tracking system.
    *
    * \/!\ DOES NOT FREE MEMORY. DOES NOT CALL SDL_DestroyX(...) OR free(...). THIS IS MEANT TO BE CALLED *BEFORE* AN SDL DESTRUCTION. /!\
    *
    * @param ptr The pointer to the SDL allocation to destroy.
    */
    template<typename T>
    static void TrackSDLDestroy(T ptr)
    {
        if (ptr)
        {
            void* key = (void*)(uintptr_t)(ptr);

            auto it = allocationMap.find(key);
            if (it != allocationMap.end())
            {
                allocationMap.erase(it);
            }
            else
            {
                SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "MemoryTracker: Attempted to destroy untracked SDL pointer %p", key);
            }
        }
    }

#endif // defined(USE_SDL_MEMORY_TRACKER)
};

// C style
#define ENG_MALLOC(size)       MemoryTracker::Allocate(size, __FILE__, __LINE__)
#define ENG_FREE(ptr)          MemoryTracker::Deallocate(ptr)

// C++ Style (with constructor arguments)
#define ENG_NEW(Type, ...)     MemoryTracker::New<Type>(__FILE__, __LINE__, ##__VA_ARGS__)
#define ENG_DELETE(ptr)        MemoryTracker::Delete(ptr)

#define ENG_REPORT_LEAKS()     MemoryTracker::ReportLeaks()
#define MEMORY_TRACKER_INIT()  MemoryTracker::Init()

// If the memory tracker isn't used, neither is the SDL one automatically.
#else // !defined(USE_MEMORY_TRACKER) || !defined(_DEBUG)

#include <cstdlib>
#include <new>

#define ENG_MALLOC(size)       std::malloc(size)
#define ENG_FREE(ptr)          std::free(ptr)

#define ENG_NEW(Type, ...)     new Type(__VA_ARGS__)
#define ENG_DELETE(ptr)        delete ptr

#define ENG_REPORT_LEAKS()     ((void)0)
#define MEMORY_TRACKER_INIT()  ((void)0)

#endif // defined(USE_MEMORY_TRACKER) && defined(_DEBUG)

#if defined(USE_SDL_MEMORY_TRACKER) && defined(USE_MEMORY_TRACKER) && defined(_DEBUG)

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>

// Window
#define ENG_SDL_CreateWindow(title, w, h, flags) \
    MemoryTracker::TrackSDLCreate(::SDL_CreateWindow(title, w, h, flags), sizeof(void*), __FILE__, __LINE__)

#define ENG_SDL_DestroyWindow(window) \
    do { MemoryTracker::TrackSDLDestroy(window); ::SDL_DestroyWindow(window); } while(0)

// Renderer
#define ENG_SDL_CreateRenderer(window, name) \
    MemoryTracker::TrackSDLCreate(::SDL_CreateRenderer(window, name), sizeof(void*), __FILE__, __LINE__)

#define ENG_SDL_DestroyRenderer(renderer) \
    do { MemoryTracker::TrackSDLDestroy(renderer); ::SDL_DestroyRenderer(renderer); } while(0)

// Texture
#define ENG_SDL_CreateTexture(renderer, format, access, w, h) \
    MemoryTracker::TrackSDLCreate(::SDL_CreateTexture(renderer, format, access, w, h), sizeof(void*), __FILE__, __LINE__)

#define ENG_SDL_DestroyTexture(texture) \
    do { MemoryTracker::TrackSDLDestroy(texture); ::SDL_DestroyTexture(texture); } while(0)

#define ENG_SDL_CreateTextureFromSurface(renderer, surface) \
    MemoryTracker::TrackSDLCreate(::SDL_CreateTextureFromSurface(renderer, surface), sizeof(void*), __FILE__, __LINE__)

// Surface
#define ENG_SDL_CreateSurface(w, h, format) \
    MemoryTracker::TrackSDLCreate(::SDL_CreateSurface(w, h, format), sizeof(void*), __FILE__, __LINE__)

#define ENG_SDL_DestroySurface(surface) \
    do { MemoryTracker::TrackSDLDestroy(surface); ::SDL_DestroySurface(surface); } while(0)

// Properties
#define ENG_SDL_CreateProperties() \
    MemoryTracker::TrackSDLCreate(::SDL_CreateProperties(), sizeof(uint32_t), __FILE__, __LINE__)

#define ENG_SDL_DestroyProperties(props) \
    do { MemoryTracker::TrackSDLDestroy(props); ::SDL_DestroyProperties(props); } while(0)

// Image
#define ENG_IMG_Load(path) \
    MemoryTracker::TrackSDLCreate(::IMG_Load(path), sizeof(void*), __FILE__, __LINE__)

// TTF
#define ENG_TTF_OpenFont(file, ptsize) \
    MemoryTracker::TrackSDLCreate(::TTF_OpenFont(file, ptsize), sizeof(void*), __FILE__, __LINE__)

#define ENG_TTF_CloseFont(font) \
    do { MemoryTracker::TrackSDLDestroy(font); ::TTF_CloseFont(font); } while (0)

#define ENG_TTF_RenderText_Blended(font, text, length, textColor) \
    MemoryTracker::TrackSDLCreate(::TTF_RenderText_Blended(font, text, length, textColor), sizeof(void*), __FILE__, __LINE__)

#define ENG_TTF_RenderText_Blended_Wrapped(font, text, length, textColor, wrapWidth) \
    MemoryTracker::TrackSDLCreate(::TTF_RenderText_Blended_Wrapped(font, text, length, textColor, wrapWidth), sizeof(void*), __FILE__, __LINE__)

// Mixer
#define ENG_MIX_CreateMixerDevice(devid, spec) \
    MemoryTracker::TrackSDLCreate(::MIX_CreateMixerDevice(devid, spec), sizeof(void*), __FILE__, __LINE__)

#define ENG_MIX_DestroyMixer(mixer) \
    do { MemoryTracker::TrackSDLDestroy(mixer); ::MIX_DestroyMixer(mixer); } while(0)

#define ENG_MIX_CreateTrack(mixer) \
    MemoryTracker::TrackSDLCreate(::MIX_CreateTrack(mixer), sizeof(void*), __FILE__, __LINE__)

#define ENG_MIX_DestroyTrack(track) \
    do { MemoryTracker::TrackSDLDestroy(track); ::MIX_DestroyTrack(track); } while(0)

#define ENG_MIX_LoadAudio(mixer, path, predecode) \
    MemoryTracker::TrackSDLCreate(::MIX_LoadAudio(mixer, path, predecode), sizeof(void*), __FILE__, __LINE__)

#define ENG_MIX_DestroyAudio(audio) \
        do { MemoryTracker::TrackSDLDestroy(audio); ::MIX_DestroyAudio(audio); } while(0)

#else // !defined(USE_SDL_MEMORY_TRACKER) || !defined(USE_MEMORY_TRACKER) || !defined(_DEBUG)

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>

// Window
#define ENG_SDL_CreateWindow(title, w, h, flags) (::SDL_CreateWindow(title, w, h, flags))

#define ENG_SDL_DestroyWindow(window) (::SDL_DestroyWindow(window))

// Renderer
#define ENG_SDL_CreateRenderer(window, name) (::SDL_CreateRenderer(window, name))

#define ENG_SDL_DestroyRenderer(renderer) (::SDL_DestroyRenderer(renderer))

// Texture
#define ENG_SDL_CreateTexture(renderer, format, access, w, h) (::SDL_CreateTexture(renderer, format, access, w, h))

#define ENG_SDL_DestroyTexture(texture) (::SDL_DestroyTexture(texture))

#define ENG_SDL_CreateTextureFromSurface(renderer, surface) (::SDL_CreateTextureFromSurface(renderer, surface))

// Surface
#define ENG_SDL_CreateSurface(w, h, format) (::SDL_CreateSurface(w, h, format))

#define ENG_SDL_DestroySurface(surface) (::SDL_DestroySurface(surface))

// Properties
#define ENG_SDL_CreateProperties() (::SDL_CreateProperties())

#define ENG_SDL_DestroyProperties(props) (::SDL_DestroyProperties(props))

// Image
#define ENG_IMG_Load(path) (::IMG_Load(path))

// TTF
#define ENG_TTF_OpenFont(file, ptsize) (::TTF_OpenFont(file, ptsize))

#define ENG_TTF_CloseFont(font) (::TTF_CloseFont(font))

#define ENG_TTF_RenderText_Blended(font, text, length, color) (::TTF_RenderText_Blended(font, text, length, color))

#define ENG_TTF_RenderText_Blended_Wrapped(font, text, length, textColor, wrapWidth) (::TTF_RenderText_Blended_Wrapped(font, text, length, textColor, wrapWidth))

// Mixer
#define ENG_MIX_CreateMixerDevice(devid, spec) (::MIX_CreateMixerDevice(devid, spec))

#define ENG_MIX_DestroyMixer(mixer) (::MIX_DestroyMixer(mixer))

#define ENG_MIX_CreateTrack(mixer) (::MIX_CreateTrack(mixer))

#define ENG_MIX_DestroyTrack(track) (::MIX_DestroyTrack(track))

#define ENG_MIX_LoadAudio(mixer, path, predecode) (::MIX_LoadAudio(mixer, path, predecode))

#define ENG_MIX_DestroyAudio(audio) (::MIX_DestroyAudio(audio))

#endif // defined(USE_SDL_MEMORY_TRACKER) && defined(USE_MEMORY_TRACKER) && defined(_DEBUG)


#if defined(USE_CRT_MEMORY_TRACKER) && defined(_DEBUG)

#define _CRTDBG_MAP_ALLOC

#include <crtdbg.h>

/**
* @brief Namespace for CRT memory tracking initialization.
*/
namespace CRTMemoryTracker
{   
    /**
	* @brief Initializes the CRT memory tracker.
    */
    void Init();
}

#define CRT_MEMORY_TRACKER_INIT() CRTMemoryTracker::Init()

#else // !defined(USE_CRT_MEMORY_TRACKER) || !defined(_DEBUG)

#define CRT_MEMORY_TRACKER_INIT() ((void)0)

#endif // defined(USE_CRT_MEMORY_TRACKER) && defined(_DEBUG)


#if defined(USE_VLD_MEMORY_TRACKER) && defined(_DEBUG)

/**
* @brief Namespace for VLD memory tracking initialization.
*/
namespace VLDMemoryTracker
{
    /**
	* @brief Initializes the VLD memory tracker.
    */
    void Init();
}

#define VLD_MEMORY_TRACKER_INIT() VLDMemoryTracker::Init()

#else // !defined(USE_VLD_MEMORY_TRACKER) || !defined(_DEBUG)

#define VLD_MEMORY_TRACKER_INIT() ((void)0)

#endif // defined(USE_VLD_MEMORY_TRACKER) && defined(_DEBUG)

#endif // MEMORYTRACKER_H_