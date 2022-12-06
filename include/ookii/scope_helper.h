#ifndef OOKII_SCOPE_HELPER_H_
#define OOKII_SCOPE_HELPER_H_

#include <functional>

namespace ookii::details
{
    class scope_exit
    {
    public:
        scope_exit() = default;

        scope_exit(std::function<void()> callback)
            : _callback{callback}
        {
        }

        scope_exit(scope_exit &) = delete;
        scope_exit &operator=(scope_exit &) = delete;

        ~scope_exit()
        {
            reset();
        }

        void reset(std::function<void()> callback = {})
        {
            if (_callback)
            {
                _callback();
            }

            _callback = callback;
        }

        void release()
        {
            _callback = {};
        }

    private:
        std::function<void()> _callback;
    };
}

#endif