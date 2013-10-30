/*
 * signal.h
 *
 *  Created on: 17/09/2012
 *      Author: nicholas
 */

#ifndef SIGNAL_H_
#define SIGNAL_H_
#include <map>
#include <cstdint>
#include <functional>

namespace cxxide
{

template<typename... Values>
class signal
{
private:
	typedef std::function<void(Values...)> callback_t;
	std::map<uint64_t, callback_t> m_Callbacks;
	uint64_t m_ID = 0;
public:

	uint64_t connect(callback_t fn)
	{
		uint64_t index = ++m_ID;
		m_Callbacks.insert(std::make_pair(++index, fn));
		return index;
	}

    void disconnect(uint64_t id)
    {
        m_Callbacks.erase(id);
    }

	void operator()(Values... values) const
	{
		for (auto it : m_Callbacks)
			it.second(values...);
	}
};

} /* namespace UI */
#endif /* sIGNAL_H_ */
