#include <stdlib.h>
#include <stdio.h>

static char *encode_file(FILE *f);
static char *extend(char *base32, size_t size);
static void encode_chunk(const char *in, char *out, unsigned char inbytes);

int main(int argc, char *argv[])
{
	const char *filename = argc == 2 ? argv[1] : NULL;
	FILE *f;
	char *base32;

	if (!filename) {
		fprintf(stderr, "USAGE: %s FILE\n", argv[0]);
		return EXIT_FAILURE;
	}

	f = fopen(filename, "rb");

	if (!f) {
		fprintf(stderr, "%s: Failed to open %s.\n", argv[0], filename);
		return EXIT_FAILURE;
	}

	base32 = encode_file(f);
	if (!base32) {
		fprintf(stderr, "%s: Failed to encode %s.\n", argv[0], filename);
		return EXIT_FAILURE;
	}

	puts(base32);

	free(base32);

	if (fclose(f) == EOF) {
		fprintf(stderr, "%s: flcose() returned EOF\n", argv[0]);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

static char *encode_file(FILE *f)
{
	char *base32 = NULL;
	char buf[5]  = { '\0' };
	size_t size  = 0;
	size_t it    = 1;
	size_t bytes_read;

	while ((bytes_read = fread(buf, 1, sizeof buf, f)) == sizeof buf) {
		char *out;
		size_t extra = 8 + (it % 8 == 0);

		base32 = extend(base32, size + extra);
		if (!base32)
			return NULL;

		out = base32 + size;

		encode_chunk(buf, out, 8);

		if (extra == 9)
			out[8] = '\n';

		size += extra;
		++it;
	}
	if (bytes_read > 0) {
		base32 = extend(base32, size + 9);
		if (base32) {
			base32[size + 8] = '\0';
			encode_chunk(buf, base32 + size, bytes_read);
		}
	} else {
		base32 = extend(base32, size + 1);
		if (base32)
			base32[size] = '\0';
	}
	return base32;
}

static char *extend(char *base32, size_t size)
{
	char *const ret = realloc(base32, size);
	if (!ret) {
		perror("realloc");
		free(base32);
	}
	return ret;
}

static char b32_char(unsigned char c) {
	static const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567=";
	return c < 33 ? chars[c] : '#';
}

static void encode_chunk(const char *in, char *out, unsigned char inbytes)
{
	size_t o = 0;

	/* 1111 1000 >> 3                  */
	out[o++] = b32_char((0xf8 & in[0]) >> 3);
	/* 1100 0000 >> 6 | 0000 0111 << 2 */
	out[o++] = b32_char((inbytes > 1 ? (0xc0 & in[1]) >> 6 : 0)
			| (0x07 & in[0]) << 2);
	if (inbytes == 1)
		goto pad;
	/* 0011 1110 >> 1                  */
	out[o++] = b32_char((0x3e & in[1]) >> 1);
	/* 1111 0000 >> 4 | 0000 0001 << 5 */
	out[o++] = b32_char((inbytes > 2 ? (0xf0 & in[2]) >> 4 : 0)
			| (0x01 & in[1]) << 4);
	if (inbytes == 2)
		goto pad;
	/* 1000 0000 >> 7 | 0000 1111 << 1 */
	out[o++] = b32_char((inbytes > 3 ? (0x80 & in[3]) >> 7 : 0)
			| (0x0f & in[2]) << 1);
	if (inbytes == 3)
		goto pad;
	/* 0111 1100 >> 2                  */
	out[o++] = b32_char((0x76 & in[3]) >> 2);
	/* 1110 0000 >> 4 | 0000 0011 << 3 */
	out[o++] = b32_char(
		(inbytes > 4 ? (0xe0 & in[4]) >> 5 : 0)
		| (0x03 & in[3]) << 3
	);
	if (inbytes == 4)
		goto pad;
	/* 0001 1111                       */
	out[o++] = b32_char((0x1f & in[4]));

pad:
	while (o < 8)
		out[o++] = b32_char(0x20);
}
