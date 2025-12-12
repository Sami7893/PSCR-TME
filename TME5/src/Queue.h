#pragma once

#include <cstdlib>
#include <cstring>
#include <mutex>
#include <condition_variable>

namespace pr {

// MT safe version of the Queue, non blocking.
// Based on circular buffer.
// Store pointers to T, not T itself ; consumer is responsible for deleting them.
// nullptr is returned by pop if the queue is empty.
// push returns false if the queue is full.
template <typename T>
class Queue {
	T ** tab_;
	const size_t allocsize_;
	size_t begin_;
	size_t size_;
	bool isBlocking;
	mutable std::mutex m_;
	std::condition_variable cv;

	// fonctions private, sans protection mutex
	bool empty() const {
		return size_ == 0;
	}
	bool full() const {
		return size_ == allocsize_;
	}
public:
	Queue(size_t size=10) :allocsize_(size), begin_(0), size_(0) {
		tab_ = new T*[size];
		// zero-initialize, not strictly necessary
		memset(tab_, 0, size * sizeof(T*));
	}
	size_t size() const {
		std::unique_lock<std::mutex> lg(m_);
		return size_;
	}

	void setBlocking(bool isBlocking) {
		std::unique_lock<std::mutex> lg(m_);
		this->isBlocking = isBlocking;
		cv.notify_all();
	}


	T* pop() {
		std::unique_lock<std::mutex> lg(m_);
		while (empty() && isBlocking) {
			cv.wait(lg);
		}
		if (empty()) {
			return nullptr;
		}
		if (full()) {
			cv.notify_all();
		}
		auto ret = tab_[begin_];
		tab_[begin_] = nullptr;
		size_--;
		begin_ = (begin_ + 1) % allocsize_;
		return ret;
	}
	bool push(T* elt) {
		std::unique_lock<std::mutex> lg(m_);
		while (full() && isBlocking) {
			cv.wait(lg);
		}
		if (full()) {
			return false;
		}
		if (empty()) {
			cv.notify_all();
		}
		tab_[(begin_ + size_) % allocsize_] = elt;
		size_++;
		return true;
	}
	~Queue() {
		// ?? lock a priori inutile, ne pas detruire si on travaille encore avec
		for (size_t i = 0; i < size_; i++) {
			auto ind = (begin_ + i) % allocsize_;
			// producer allocated, consumer should delete
			// but we are destroyed with some elements still in the queue
			// we must delete them.
			delete tab_[ind];
		}
		delete[] tab_;
	}
};

} /* namespace pr */
