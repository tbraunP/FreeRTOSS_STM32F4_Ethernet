#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include <reent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>


#ifdef __cplusplus
 extern "C" {
#endif

void _exit(int code);

ssize_t _read_r(struct _reent *r, int fd, void *ptr, size_t len);

ssize_t _write_r(struct _reent *r, int fd, const void *ptr, size_t len);

off_t _lseek_r(struct _reent *r, int fd, _off_t ptr, int dir);

int _close_r(struct _reent *r, int fd);

int _fstat_r(struct _reent *r, int fd, struct stat *st);

int _isatty_r(struct _reent *r, int fd);

void *_sbrk_r(struct _reent *r, ptrdiff_t incr);

int _kill_r(struct _reent *r, int pid, int signal);

int _getpid_r(struct _reent *r);

void __malloc_lock(struct _reent *r);
void __malloc_unlock(struct _reent *r);

#ifdef __cplusplus
 }
#endif

#endif
