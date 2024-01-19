//
// Created by gomkyung2 on 1/19/24.
//

module;

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

export module stb_image;

export namespace stbi {
    constexpr int VERSION = STBI_VERSION;

    constexpr int eDefault       = STBI_default;
    constexpr int eGrey          = STBI_grey;
    constexpr int eGreyAlpha     = STBI_grey_alpha;
    constexpr int eRgb           = STBI_rgb;
    constexpr int eRgbAlpha      = STBI_rgb_alpha;

    using uc = stbi_uc;
    using us = stbi_us;

    ////////////////////////////////////
    //
    // 8-bits-per-channel interface
    //

    stbi_uc *load_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *channels_in_file,
                              int desired_channels) {
        return stbi_load_from_memory(buffer, len, x, y, channels_in_file, desired_channels);
    }

    stbi_uc *load_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *channels_in_file,
                                 int desired_channels) {
        return stbi_load_from_callbacks(clbk, user, x, y, channels_in_file, desired_channels);
    }

#ifndef STBI_NO_STDIO
    stbi_uc *load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels) {
        return stbi_load(filename, x, y, channels_in_file, desired_channels);
    }

    stbi_uc *load_from_file(FILE *f, int *x, int *y, int *channels_in_file, int desired_channels) {
        return stbi_load_from_file(f, x, y, channels_in_file, desired_channels);
    }

    // for stbi_load_from_file, file pointer is left pointing immediately after image
#endif

#ifndef STBI_NO_GIF
    stbi_uc *load_gif_from_memory(stbi_uc const *buffer, int len, int **delays, int *x, int *y, int *z, int *comp,
                                  int req_comp) {
        return stbi_load_gif_from_memory(buffer, len, delays, x, y, z, comp, req_comp);
    }
#endif

#ifdef STBI_WINDOWS_UTF8
int convert_wchar_to_utf8(char *buffer, size_t bufferlen, const wchar_t* input) {
    return stbi_convert_wchar_to_utf8(buffer, bufferlen, input);
}
#endif

    ////////////////////////////////////
    //
    // 16-bits-per-channel interface
    //

    stbi_us *load_16_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *channels_in_file,
                                 int desired_channels) {
        return stbi_load_16_from_memory(buffer, len, x, y, channels_in_file, desired_channels);
    }

    stbi_us *load_16_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *channels_in_file,
                                    int desired_channels) {
        return stbi_load_16_from_callbacks(clbk, user, x, y, channels_in_file, desired_channels);
    }

#ifndef STBI_NO_STDIO
    stbi_us *load_16(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels) {
        return stbi_load_16(filename, x, y, channels_in_file, desired_channels);
    }

    stbi_us *load_from_file_16(FILE *f, int *x, int *y, int *channels_in_file, int desired_channels) {
        return stbi_load_from_file_16(f, x, y, channels_in_file, desired_channels);
    }
#endif

    ////////////////////////////////////
    //
    // float-per-channel interface
    //
#ifndef STBI_NO_LINEAR
    float *loadf_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *channels_in_file,
                             int desired_channels) {
        return stbi_loadf_from_memory(buffer, len, x, y, channels_in_file, desired_channels);
    }

    float *loadf_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *channels_in_file,
                                int desired_channels) {
        return stbi_loadf_from_callbacks(clbk, user, x, y, channels_in_file, desired_channels);
    }

#ifndef STBI_NO_STDIO
    float *loadf(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels) {
        return stbi_loadf(filename, x, y, channels_in_file, desired_channels);
    }

    float *loadf_from_file(FILE *f, int *x, int *y, int *channels_in_file, int desired_channels) {
        return stbi_loadf_from_file(f, x, y, channels_in_file, desired_channels);
    }
#endif
#endif

#ifndef STBI_NO_HDR
    void hdr_to_ldr_gamma(float gamma) {
        return stbi_hdr_to_ldr_gamma(gamma);
    }

    void hdr_to_ldr_scale(float scale) {
        return stbi_hdr_to_ldr_scale(scale);
    }
#endif // STBI_NO_HDR

#ifndef STBI_NO_LINEAR
    void ldr_to_hdr_gamma(float gamma) {
        return stbi_ldr_to_hdr_gamma(gamma);
    }

    void ldr_to_hdr_scale(float scale) {
        return stbi_ldr_to_hdr_scale(scale);
    }
#endif // STBI_NO_LINEAR

    // stbi_is_hdr is always defined, but always returns false if STBI_NO_HDR
    int is_hdr_from_callbacks(stbi_io_callbacks const *clbk, void *user) {
        return stbi_is_hdr_from_callbacks(clbk, user);
    }

    int is_hdr_from_memory(stbi_uc const *buffer, int len) {
        return stbi_is_hdr_from_memory(buffer, len);
    }
#ifndef STBI_NO_STDIO
    int is_hdr(char const *filename) {
        return stbi_is_hdr(filename);
    }

    int is_hdr_from_file(FILE *f) {
        return stbi_is_hdr_from_file(f);
    }
#endif // STBI_NO_STDIO


    // get a VERY brief reason for failure
    // on most compilers (and ALL modern mainstream compilers) this is threadsafe
    const char *failure_reason(void) {
        return stbi_failure_reason();
    }

    // free the loaded image -- this is just free()
    void image_free(void *retval_from_stbi_load) {
        return stbi_image_free(retval_from_stbi_load);
    }

    // get image dimensions & components without fully decoding
    int info_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp) {
        return stbi_info_from_memory(buffer, len, x, y, comp);
    }

    int info_from_callbacks(stbi_io_callbacks const *clbk, void *user, int *x, int *y, int *comp) {
        return stbi_info_from_callbacks(clbk, user, x, y, comp);
    }

    int is_16_bit_from_memory(stbi_uc const *buffer, int len) {
        return stbi_is_16_bit_from_memory(buffer, len);
    }

    int is_16_bit_from_callbacks(stbi_io_callbacks const *clbk, void *user) {
        return stbi_is_16_bit_from_callbacks(clbk, user);
    }

#ifndef STBI_NO_STDIO
    int info(char const *filename, int *x, int *y, int *comp) {
        return stbi_info(filename, x, y, comp);
    }

    int info_from_file(FILE *f, int *x, int *y, int *comp) {
        return stbi_info_from_file(f, x, y, comp);
    }

    int is_16_bit(char const *filename) {
        return stbi_is_16_bit(filename);
    }

    int is_16_bit_from_file(FILE *f) {
        return stbi_is_16_bit_from_file(f);
    }
#endif


    // for image formats that explicitly notate that they have premultiplied alpha,
    // we just return the colors as stored in the file. set this flag to force
    // unpremultiplication. results are undefined if the unpremultiply overflow.
    void set_unpremultiply_on_load(int flag_true_if_should_unpremultiply) {
        return stbi_set_unpremultiply_on_load(flag_true_if_should_unpremultiply);
    }

    // indicate whether we should process iphone images back to canonical format,
    // or just pass them through "as-is"
    void convert_iphone_png_to_rgb(int flag_true_if_should_convert) {
        return stbi_convert_iphone_png_to_rgb(flag_true_if_should_convert);
    }

    // flip the image vertically, so the first pixel in the output array is the bottom left
    void set_flip_vertically_on_load(int flag_true_if_should_flip) {
        return stbi_set_flip_vertically_on_load(flag_true_if_should_flip);
    }

    // as above, but only applies to images loaded on the thread that calls the function
    // this function is only available if your compiler supports thread-local variables;
    // calling it will fail to link if your compiler doesn't
    void set_unpremultiply_on_load_thread(int flag_true_if_should_unpremultiply) {
        return stbi_set_unpremultiply_on_load_thread(flag_true_if_should_unpremultiply);
    }

    void convert_iphone_png_to_rgb_thread(int flag_true_if_should_convert) {
        return stbi_convert_iphone_png_to_rgb_thread(flag_true_if_should_convert);
    }

    void set_flip_vertically_on_load_thread(int flag_true_if_should_flip) {
        return stbi_set_flip_vertically_on_load_thread(flag_true_if_should_flip);
    }

    // ZLIB client - used by PNG, available for other purposes

    char *zlib_decode_malloc_guesssize(const char *buffer, int len, int initial_size, int *outlen) {
        return stbi_zlib_decode_malloc_guesssize(buffer, len, initial_size, outlen);
    }

    char *zlib_decode_malloc_guesssize_headerflag(const char *buffer, int len, int initial_size, int *outlen,
                                                  int parse_header) {
        return stbi_zlib_decode_malloc_guesssize_headerflag(buffer, len, initial_size, outlen, parse_header);
    }

    char *zlib_decode_malloc(const char *buffer, int len, int *outlen) {
        return stbi_zlib_decode_malloc(buffer, len, outlen);
    }

    int zlib_decode_buffer(char *obuffer, int olen, const char *ibuffer, int ilen) {
        return stbi_zlib_decode_buffer(obuffer, olen, ibuffer, ilen);
    }

    char *zlib_decode_noheader_malloc(const char *buffer, int len, int *outlen) {
        return stbi_zlib_decode_noheader_malloc(buffer, len, outlen);
    }

    int zlib_decode_noheader_buffer(char *obuffer, int olen, const char *ibuffer, int ilen) {
        return stbi_zlib_decode_noheader_buffer(obuffer, olen, ibuffer, ilen);
    }
}
