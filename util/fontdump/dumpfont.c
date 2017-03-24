/*
 * Small ttf to png utility
 * Very ugly code
 *
 * Author: Rafael Roquetto
 */
#include <ft2build.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <png.h>

#include FT_FREETYPE_H

static FT_Library library;
static FT_Face face;

static void
panic(const char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, "FATAL: ");

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	fputc('\n', stderr);

	exit(1);
}

static
void init_ft_library(void)
{
	int error = FT_Init_FreeType(&library);

	if (error)
		panic("Cannot load libfreetype.");
}

static void
load_font_face(const char *path, int size)
{
	int error;

	error = FT_New_Face(library, path,
			0, &face);
	if (error)
		panic("Cannot load '%s'.", path);

	error = FT_Set_Char_Size(face,
			0,
			size*64,
			300, 300);

	if (error)
		panic("Cannot set character size.");
}

static void
load_glyph(char c)
{
	int error;

	error = FT_Load_Glyph(face,
			FT_Get_Char_Index(face, c),
			FT_LOAD_DEFAULT);

	if (error)
		panic("Cannot load glyph. Error %d", error);

	if (face->glyph->format == FT_GLYPH_FORMAT_BITMAP)
		return;

	error = FT_Render_Glyph(face->glyph,
			FT_RENDER_MODE_NORMAL);

	if (error)
		panic("Cannot render glyph");
}

static void
write_png(const char *filename, FT_Bitmap bitmap)
{
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;
	int i, j;
	unsigned char *row;
	unsigned char *q;
       
	row = malloc(bitmap.width*3);

	if ((fp = fopen(filename, "wb")) == NULL)
		panic("Cannot open '%s'.", filename);

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
			NULL, NULL, NULL);

	if (png_ptr == NULL)
		panic("png_create_write_struct failed.");

	info_ptr = png_create_info_struct(png_ptr);

	if (png_ptr == NULL) {
		png_destroy_write_struct(&png_ptr, NULL);
		panic("png_create_info_struct failed.");
	}

	if (setjmp(png_jmpbuf(png_ptr))) 
		panic("png_setjmpbuf failed.");

	png_init_io(png_ptr, fp);

	if (setjmp(png_jmpbuf(png_ptr))) 
		panic("png_setjmpbuf failed.");

	png_set_IHDR(png_ptr, info_ptr,
			bitmap.width, bitmap.rows, 8,
			PNG_COLOR_TYPE_RGB,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT);

	png_write_info(png_ptr, info_ptr);

	if (setjmp(png_jmpbuf(png_ptr))) 
		panic("png_setjmpbuf failed.");

	q = bitmap.buffer;

	for (i = 0; i < bitmap.rows; i++) {
		unsigned char  *p = row;
		for (j = 0; j < bitmap.width; j++) {
			*p++ = *q;
			*p++ = *q;
			*p++ = *q++;
		}
		png_write_row(png_ptr, (png_bytep) row);
	}

	free(row);

	png_write_end(png_ptr, NULL);

	png_destroy_write_struct(&png_ptr, &info_ptr);
}

static void
extract_char_range(int i, int j, const char *path)
{
	char fname[128];
	int k;
	FT_GlyphSlot slot;

	for (k = i; k <= j; k++) {
		load_glyph((char) k);
		slot = face->glyph;
		snprintf(fname, sizeof(fname),
			"%s/%c.png", path, k);
		write_png(fname, slot->bitmap);
	}
}

static void
extract_char(char ch, const char *path, const char *filename)
{
	char fname[128];
	FT_GlyphSlot slot;

	load_glyph(ch);
	slot = face->glyph;

	snprintf(fname, sizeof (fname),
			"%s/%s.png", path, filename);
	write_png(fname, slot->bitmap);
}

static void
process_font(const char *path)
{
	extract_char_range('0', '9', path);
	extract_char_range('a', 'z', path);
	extract_char_range('A', 'Z', path);
}

int main(int argc, char *argv[])
{
	char *path;
	int size;

	if (argc < 4)
		panic("Usage: dumpfont ttfile size destdir");

	size = atoi(argv[2]);
	path = argv[3];

	init_ft_library();
	load_font_face(argv[1], size);

	mkdir(path, 0777);

	process_font(path);

	return 0;
}
