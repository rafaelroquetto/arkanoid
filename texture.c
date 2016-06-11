#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <png.h>
#include <errno.h>
#include <string.h>
#include <GL/glew.h>

#include "texture.h"
#include "panic.h"


struct image {
    int width;
    int height;
    unsigned *bits;
};

static struct image *
image_make_from_png(const char *png_filename)
{
    FILE *fp;
    struct image *img;
    png_structp png_ptr;
    png_bytep *rows;
    png_byte *p;
    unsigned *q;
    int i, j, color_type, bit_depth;

    if ((fp = fopen(png_filename, "rb")) == NULL)
        panic("image_make_from_png: failed to open `%s': %s", png_filename, strerror(errno));

    if ((png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                    NULL, NULL)) == NULL)
        panic("png_create_read_struct");

    png_infop info_ptr;

    if ((info_ptr = png_create_info_struct(png_ptr)) == NULL)
        panic("png_create_info_struct");

    if (setjmp(png_jmpbuf(png_ptr)))
        panic("png error");

    png_init_io(png_ptr, fp);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    color_type = png_get_color_type(png_ptr, info_ptr);

    bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if (color_type != PNG_COLOR_TYPE_RGBA
            && color_type != PNG_COLOR_TYPE_RGB)
        panic("invalid color type");

    img = malloc(sizeof *img);

    img->width = png_get_image_width(png_ptr, info_ptr);
    img->height = png_get_image_height(png_ptr, info_ptr);

    img->bits = malloc(img->width*img->height*sizeof *img->bits);

    rows = png_get_rows(png_ptr, info_ptr);

    for (i = 0; i < img->height; i++) {
        if (color_type == PNG_COLOR_TYPE_RGBA) {
            if (bit_depth == 8) {
                memcpy(&img->bits[i*img->width], rows[i],
                        img->width*sizeof *img->bits);
            } else {
                assert(bit_depth == 16);

                /* !#$#@! GIMP uses 16-bit pixels */

                p = rows[i];
                q = &img->bits[i*img->width];

                for (j = 0; j < img->width; j++) {
                    *q++ = p[0] | (p[2] << 8) |
                        (p[4] << 16) | (p[6] << 24);
                    p += 8;
                }
            }
        } else if (color_type == PNG_COLOR_TYPE_RGB) {
            p = rows[i];
            q = &img->bits[i*img->width];

            for (j = 0; j < img->width; j++) {
                if (bit_depth == 8) {
                    *q++ = p[0] | (p[1] << 8) | (p[2] << 16) |
                        0xff000000;
                    p += 3;
                } else {
                    *q++ = p[0] | (p[2] << 8) | (p[4] << 16) |
                        0xff000000;
                    p += 6;
                }
            }
        } else {
            assert(0);
        }
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    fclose(fp);

    return img;
}

static void
image_free(struct image *img)
{
    free(img->bits);
    free(img);
}

static GLuint
image_to_opengl_texture(const struct image *image)
{
    GLuint texture_id;

    glGenTextures(1, &texture_id);

    glBindTexture(GL_TEXTURE_2D, texture_id);

#if 0
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#endif

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height,
            0, GL_RGBA, GL_UNSIGNED_BYTE, image->bits);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture_id;
}

GLuint
load_texture_from_png(const char *filename, int *width, int *height)
{
    struct image *image;
    GLuint texture;

    image = image_make_from_png(filename);

    texture = image_to_opengl_texture(image);

    if (width)
        *width = image->width;

    if (height)
        *height = image->height;

    image_free(image);

    return texture;
}
