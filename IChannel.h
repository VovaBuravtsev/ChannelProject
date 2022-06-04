#include <boost/optional/optional_io.hpp>

template<typename T>
class IChannel{
public:
	virtual void Send(const T& value) = 0;
	virtual boost::optional<T> Recv() = 0;
	virtual void Close() = 0;
	virtual bool getClosed() = 0;
	virtual bool getEmpty() = 0;
	virtual void WaitFree() = 0;
};