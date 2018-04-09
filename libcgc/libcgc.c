//#include <libcgc.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/syscall.h>
#include <linux/random.h>

//int receive(int fd, void *buf, size_t count, size_t *rx_bytes)
int receive(int fd, void *buf, unsigned int count, void *rx_bytes)
{
	ssize_t r = read(fd, buf, count);
	if (r < 0) return 1;
	if (rx_bytes) *(size_t*)rx_bytes = r;
	return 0;
}

//int transmit(int fd, const void *buf, size_t count, size_t *tx_bytes)
int transmit(int fd, const void *buf, unsigned int count, void *tx_bytes)
{
	ssize_t r = write(fd, buf, count);
	if (r < 0) return 1;
	if (tx_bytes) *(size_t*)tx_bytes = r;
	return 0;
}

void __attribute__((__noreturn__)) _terminate(unsigned int status)
{
	_exit(status);
}

#ifndef MAP_32BIT
	#define MAP_32BIT 0
#endif

int allocate(unsigned int length, int is_X, void **addr)
{
	void *r = mmap(NULL, length, PROT_READ | PROT_WRITE | (is_X ? PROT_EXEC : 0), MAP_32BIT | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (r == MAP_FAILED) return 1;
	if (addr) *addr = r;
	return 0;
}

//int deallocate(void *addr, size_t length)
int deallocate(void *addr, unsigned int length)
{
	return munmap(addr, length) != 0;
}

//int fdwait(int nfds, fd_set *readfds, fd_set *writefds, const struct timeval *timeout, int *readyfds)
int fdwait(int nfds, void *readfds, void *writefds, const void *timeout, int *readyfds)
{
	int r = select(nfds, readfds, writefds, NULL, (struct timeval *)timeout);
	if (r < 0) return 1;
	if (readyfds) *readyfds = r;
	return 0;
}

//int random(void *buf, size_t count, size_t *rnd_bytes)
int random(void *buf, unsigned int count, void *rnd_bytes)
{
#ifdef SYS_getrandom
	int r = syscall(SYS_getrandom, buf, count, 0);
	if (r < 0) return 1;
	if (rnd_bytes) *(size_t*)rnd_bytes = r;
	return 0;
#else
	while (count > 0) ((char *)buf)[--count] = 'A';
	if (rnd_bytes) *(size_t*)rnd_bytes = count;
#endif
}

// almost direct from trailofbits cb-multios
static void __attribute__ ((constructor)) cgc_initialize_flag_page(void) {
  void *mmap_addr = mmap((void*) 0x4347C000, 0x1000, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS,-1,0);

  // Fill the flag page with bytes from the prng
  random(mmap_addr, 0x1000, NULL);
}

int main();
void _start()
{
	cgc_initialize_flag_page();
	_terminate(main());
}
