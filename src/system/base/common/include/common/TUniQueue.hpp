#include <queue>
#include <chrono>
#include <future>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <type_traits>
#include <boost/optional.hpp>

#include "loger/slog.h"

namespace HBE {

template <typename T>
class is_unique_ptr final : public std::false_type
{
};

template <typename T>
class is_unique_ptr<std::unique_ptr<T>> final : public std::true_type
{
};

//---------------------------------

template <typename T>
class TUniQueue
{
public:

    explicit TUniQueue(
        const std::chrono::milliseconds timeout = std::chrono::milliseconds(100)
    )
    : m_timeout(timeout)
    {

    }

    ~TUniQueue()
    {
        printDebug("TUniQueue/%s:  TUniQueue deleted ...", __FUNCTION__);
    }

    template<typename U = T>
    typename std::enable_if<is_unique_ptr<U>::value>::type push(U&& m)
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_queue.push(std::move(m));
        lock.unlock();
        m_queue_cond_var.notify_one();
    }

    template<typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value>::type push(U m)
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_queue.push(m);
        lock.unlock();
        m_queue_cond_var.notify_one();
    }

    template<typename U = T>
    typename std::enable_if<is_unique_ptr<U>::value, std::future<boost::optional<U>>>::type pop()
    {
        return std::async(std::launch::async,[this]()
        {
            boost::optional<U> blk;
            std::unique_lock<std::mutex> lock(m_queue_mutex);

            if(!m_queue.size())
            {
                m_queue_cond_var.wait_for(lock, m_timeout, [&]() {return !m_queue.empty();});

                if(m_queue.size())
                {
                    blk = std::move(m_queue.front());
                    m_queue.pop();
                }
            }
            else
            {
                blk = std::move(m_queue.front());
                m_queue.pop();
            }
            return blk;
        });
    }

    template<typename U = T>
    typename std::enable_if<std::is_arithmetic<U>::value, std::future<boost::optional<U>>>::type pop()
    {
        return std::async(std::launch::async,[this]()
        {
            boost::optional<U> blk;
            std::unique_lock<std::mutex> lock(m_queue_mutex);

            if(!m_queue.size())
            {
                m_queue_cond_var.wait_for(lock, m_timeout, [&]() {return !m_queue.empty();});
                if(m_queue.size())
                {
                    blk = m_queue.front();
                    m_queue.pop();
                }
            }
            else
            {
                blk = m_queue.front();
                m_queue.pop();
            }
            return blk;
        });
    }

private:
    std::queue<T> m_queue;
    std::mutex m_queue_mutex;
    std::condition_variable m_queue_cond_var;
    std::chrono::milliseconds m_timeout;
};

} // namespace HBE