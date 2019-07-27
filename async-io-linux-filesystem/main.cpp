
#include "AsyncFileReader.h"

int main()
{
	AsyncFileReader afr;

	// Read from stdin.
	const int fd = 0;

	afr.addFileDescriptor(fd);

	afr.runAsyncLoop();

	return 0;
}
