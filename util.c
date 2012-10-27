
#include "picocoin-config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include "util.h"

const char ipv4_mapped_pfx[12] = "\0\0\0\0\0\0\0\0\0\0\xff\xff";

void bu_reverse_copy(unsigned char *dst, const unsigned char *src, size_t len)
{
	unsigned int i;
	for (i = 0; i < len; i++) {
		*dst = *src;

		src++;
		dst--;
	}
}

void bu_Hash(unsigned char *md256, const void *data, size_t data_len)
{
	unsigned char md1[SHA256_DIGEST_LENGTH];

	SHA256(data, data_len, md1);
	SHA256(md1, SHA256_DIGEST_LENGTH, md256);
}

void bu_Hash4(unsigned char *md32, const void *data, size_t data_len)
{
	unsigned char md256[SHA256_DIGEST_LENGTH];

	bu_Hash(md256, data, data_len);
	memcpy(md32, &md256[SHA256_DIGEST_LENGTH - 4], 4);
}

void bu_Hash160(unsigned char *md160, const void *data, size_t data_len)
{
	unsigned char md1[SHA256_DIGEST_LENGTH];

	SHA256(data, data_len, md1);
	RIPEMD160(md1, SHA256_DIGEST_LENGTH, md160);
}

bool bu_read_file(const char *filename, void **data_, size_t *data_len_,
	       size_t max_file_len)
{
	void *data;
	struct stat st;

	*data_ = NULL;
	*data_len_ = 0;

	int fd = open(filename, O_RDONLY);
	if (fd < 0)
		return false;

	if (fstat(fd, &st) < 0)
		goto err_out_fd;

	if (st.st_size > max_file_len)
		goto err_out_fd;

	data = malloc(st.st_size);
	if (!data)
		goto err_out_fd;

	ssize_t rrc = read(fd, data, st.st_size);
	if (rrc != st.st_size)
		goto err_out_mem;

	close(fd);
	fd = -1;

	*data_ = data;
	*data_len_ = st.st_size;

	return true;

err_out_mem:
	free(data);
err_out_fd:
	if (fd >= 0)
		close(fd);
	return false;
}

bool bu_write_file(const char *filename, const void *data, size_t data_len)
{
	char *tmpfn = calloc(1, strlen(filename) + 16);
	strcpy(tmpfn, filename);
	strcat(tmpfn, ".XXXXXX");

	int fd = mkstemp(tmpfn);
	if (fd < 0)
		goto err_out_tmpfn;

	ssize_t wrc = write(fd, data, data_len);
	if (wrc != data_len)
		goto err_out_fd;

	close(fd);
	fd = -1;

	if (rename(tmpfn, filename) < 0)
		goto err_out_fd;

	free(tmpfn);
	return true;

err_out_fd:
	if (fd >= 0)
		close(fd);
	unlink(tmpfn);
err_out_tmpfn:
	free(tmpfn);
	return false;
}
