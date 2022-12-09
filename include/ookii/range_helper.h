#ifndef OOKII_RANGE_HELPER_H_
#define OOKII_RANGE_HELPER_H_

#pragma once

#include <iterator>

namespace ookii::details
{
    template<typename T, typename IteratorType>
    class range_filter
    {
        using filter_function_type = std::function<bool(const typename IteratorType::reference)>;
        using transform_function_type = std::function<T(const typename IteratorType::reference)>;

        class iterator
        {
        public:
            using iterator_concept = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = typename IteratorType::difference_type;
            using pointer = std::remove_reference_t<value_type>*;
            using reference = value_type&;

            iterator(IteratorType it, IteratorType end, filter_function_type filter, transform_function_type transform)
                : _current{it},
                  _end{end},
                  _filter{filter},
                  _transform{transform}
            {
                next_value(true);
            }

            iterator &operator++()
            {
                next_value(false);
                return *this;
            }

            iterator operator++(int)
            {
                iterator temp = *this;
                next_value(false);
                return temp;
            }

            value_type operator*() const noexcept
            {
                return _transform(*_current);
            }

            bool operator==(const iterator &other) const noexcept
            {
                return _current == other._current;
            }

        private:
            void next_value(bool check_current)
            {
                if (_current == _end || (check_current && (!_filter || _filter(*_current))))
                {
                    return;
                }

                do
                {
                    ++_current;

                } while (_current != _end && _filter && !_filter(*_current));
            }

            IteratorType _current;
            IteratorType _end;
            filter_function_type _filter;
            transform_function_type _transform;
        };

    public:
        range_filter(IteratorType begin, IteratorType end, transform_function_type transform,
            filter_function_type filter = {})
            : _begin{begin},
              _end{end},
              _transform{transform},
              _filter{filter}
        {
        }

        iterator begin() const noexcept
        {
            return iterator{_begin, _end, _filter, _transform};
        }

        iterator end() const noexcept
        {
            return iterator{_end, _end, _filter, _transform};
        }

    private:
        IteratorType _begin;
        IteratorType _end;
        transform_function_type _transform;
        filter_function_type _filter;
    };
}

#endif