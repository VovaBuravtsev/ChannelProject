#include <thread>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <condition_variable>
#include <boost/optional/optional_io.hpp>
#include <boost/none.hpp>
#include <gtest/gtest.h>
#include "IChannel.h"


template<typename T>
class BufferedChannel : public IChannel<T>{
	std::queue<T> m_inner_buffer;
	std::mutex m_mutex;
	std::condition_variable m_is_empty_var;
	std::condition_variable m_is_full_var;
	int m_size;
	bool m_que_is_closed = false;
public:
	BufferedChannel(int input_size) : m_size(input_size){}

	BufferedChannel(const BufferedChannel& parent_channel);

	void Send(const T& value)override;

	boost::optional<T> Recv()override;

	void Close()override;

	bool getClosed()override;

	bool getEmpty()override;

	void WaitFree()override;
};

template<typename T>
boost::optional<T> BufferedChannel<T>::Recv()
{
	std::unique_lock<std::mutex> recv_unique_lock(m_mutex);

	while (m_inner_buffer.empty() && !m_que_is_closed){
   		m_is_empty_var.wait(recv_unique_lock);
	}

	if(m_que_is_closed && m_inner_buffer.empty())return boost::none;

	T tempValue = m_inner_buffer.front();
	m_inner_buffer.pop();
	m_is_full_var.notify_one();
	return tempValue;
}

template<typename T>
void BufferedChannel<T>::Send(const T& value)
{
	std::unique_lock<std::mutex> send_unique_lock(m_mutex);

	while ((m_inner_buffer.size() == m_size) && !m_que_is_closed){
   		m_is_full_var.wait(send_unique_lock);
	}
	
	if(m_que_is_closed)throw std::logic_error("BufferedChannel's inner queue is closed");

	m_inner_buffer.push(value);
	m_is_empty_var.notify_one();
} 

template<typename T>
void BufferedChannel<T>::Close(){
	std::unique_lock<std::mutex> close_unique_lock(m_mutex);
	m_que_is_closed = true;
	m_is_empty_var.notify_all();
	m_is_full_var.notify_all(); 
}

template<typename T>
bool BufferedChannel<T>::getClosed(){
	std::unique_lock<std::mutex> getClosed_unique_lock(m_mutex);
	return m_que_is_closed;
}

template<typename T>
bool BufferedChannel<T>::getEmpty(){
	std::unique_lock<std::mutex> getEmpty_unique_lock(m_mutex);
	return m_inner_buffer.empty();
}

template<typename T>
void BufferedChannel<T>::WaitFree(){
	while(!this->getEmpty()){
		continue;
	}
}

template<typename T>
BufferedChannel<T>::BufferedChannel(const BufferedChannel& parent_channel){
	m_inner_buffer = std::queue<T>(parent_channel.m_size);
	for(int i = parent_channel.m_size; i > 0; --i){
		m_inner_buffer[i] = parent_channel.m_inner_buffer[i];
	}
	m_is_empty_var = parent_channel.m_is_empty_var;
	m_is_full_var = parent_channel.m_is_full_var;
	m_que_is_closed = parent_channel.m_que_is_closed;		
	m_size = parent_channel.m_size;
}