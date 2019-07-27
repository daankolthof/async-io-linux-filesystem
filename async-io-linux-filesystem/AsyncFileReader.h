#ifndef ASYNC_FILE_READER
#define ASYNC_FILE_READER

#include <cstddef>

/** Class made to asynchronously read from file descriptors.
*/
class AsyncFileReader
{
	public:
	static constexpr size_t EVENT_BUFFER_SIZE = 64;
	static constexpr size_t READ_BUFFER_SIZE = 512;

	AsyncFileReader();

	~AsyncFileReader();

	/** Adds a file descriptor to the list of file descriptors to asynchronously read from.
	*/
	void addFileDescriptor(const int);

	/** Remove a file descriptor from the list of file descriptors to asynchronously read from.
	*/
	void removeFileDescriptor(const int);

	/** Runs the loop checking for reads on any of the file descriptors.
	 *  Should be ran on a separate thread if anything else needs to be done on the main thread.
	*/
	void runAsyncLoop();

	/** Stops an already started loop from listening to file descriptors and reading in files.
	 *  If this is ran on a separate thread, join() the thread before deconstructing the AsyncFileReader object.
	*/
	void stopAsyncLoop();

	/** Check whether the asynchronous reader has stopped.
	 *  Should only be used to check if the reader stopped because of an error while reading, not to confirm a stop from stopAsyncLoop().
	*/
	bool hasStopped();

	private:
	// An epoll file descriptor, listing the file descriptor(s) for which completion events should be handled.
	int epoll_fd_;
	bool stopped_;
	char read_buffer_[READ_BUFFER_SIZE];

	void handleEventOnFile(const struct epoll_event&);
};

#endif // ASYNC_FILE_READER

