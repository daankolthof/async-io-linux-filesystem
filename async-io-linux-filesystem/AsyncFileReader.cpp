
#include "AsyncFileReader.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>

AsyncFileReader::AsyncFileReader()
	: epoll_fd_(epoll_create1(0)),
	stopped_(false)
	{
		static_assert(EVENT_BUFFER_SIZE > 0, "EVENT_BUFFER_SIZE must be greater than 0");
		static_assert(READ_BUFFER_SIZE > 0, "READ_BUFFER_SIZE must be greater than 0");
	}

AsyncFileReader::~AsyncFileReader()
{
	close(epoll_fd_);
}

void AsyncFileReader::addFileDescriptor(const int fd)
{
	struct epoll_event epoll_event;
	epoll_event.events = EPOLLIN;
	epoll_event.data.fd = fd;

	epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &epoll_event);
}

void AsyncFileReader::removeFileDescriptor(const int fd)
{
	struct epoll_event epoll_event; // For the sake of compatibility with older versions of Linux, this struct must exist (pointer to struct cannot be nullptr).

	epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &epoll_event);
}

void AsyncFileReader::runAsyncLoop()
{
	struct epoll_event epoll_event_buffer[EVENT_BUFFER_SIZE];

	while(!stopped_)
	{
		// Call epoll_wait with a timeout of 1000 milliseconds.
		const int fds_ready = epoll_wait(epoll_fd_, epoll_event_buffer, EVENT_BUFFER_SIZE, 1000);

		// Function epoll_wait returned.
		if(-1 == fds_ready)
		{
			// An error occured.
			stopped_ = true;
			continue;
		}

		// Handle any file descriptors with events.
		for(int i = 0; i < fds_ready; i++)
		{
			const struct epoll_event& epoll_event = epoll_event_buffer[i];
			/* To use multiple CPUs, you could execute the handlers on (multiple) worker threads. For the sake of simplicity, polling and handling is done on the same thread now. */
			handleEventOnFile(epoll_event);
		}
	}
}

void AsyncFileReader::stopAsyncLoop()
{
	stopped_ = true;
}

bool AsyncFileReader::hasStopped()
{
	return stopped_;
}

void AsyncFileReader::handleEventOnFile(const struct epoll_event& epoll_event)
{
	const uint32_t events = epoll_event.events;
	const int fd = epoll_event.data.fd;

	if(EPOLLERR & events)
	{
		// An error occured on the file descriptor. Try to close it.
		removeFileDescriptor(fd);
		close(fd);
	}
	else if(EPOLLIN & events)
	{
		// Read is available, read in from the file descriptor and print the message.
		int read_result = read(fd, read_buffer_, READ_BUFFER_SIZE-1);

		if(-1 == read_result)
		{
			// An error occured while reading.
			removeFileDescriptor(fd);
			close(fd);
			return;
		}

		// Successfully read in some bytes.
		read_buffer_[read_result] = '\0';
		printf("Read %d bytes\n", read_result);
		printf("%s", read_buffer_);
	}
}

