//! \file owned_or_borrowed_ptr.h
//! \brief Provides a smart pointer that can optionally own the contained pointer.
#ifndef OOKII_OWNED_OR_BORROWED_PTR_H_
#define OOKII_OWNED_OR_BORROWED_PTR_H_

#pragma once

#include <algorithm>

namespace ookii
{
    //! \brief Smart pointer that may or may not own the contained pointer.
    //! 
    //! This type is a smart pointer that will free the contained pointer on destruction if it's
    //! owned, or will do nothing if it's borrowed. Essentially, it acts like a `std::unique_ptr<T>`
    //! if the pointer is owned, and like a raw pointer if not.
    //! 
    //! This type is used to store both owned and duplicate references to the same objects in a
    //! single list.
    //! 
    //! \warning The borrowed pointers don't have any influence on the lifetime of their owned
    //!          counterparts, so it's up to the user to assure that the borrowed pointer doesn't
    //!          outlive the underlying pointer.
    //! 
    //! \tparam T The type of object referred to by the pointer. Should not be an array type.
    template<typename T>
    class owned_or_borrowed_ptr
    {
    public:
        //! \brief The type of object referred to by the pointer.
        using element_type = T;
        //! \brief A pointer to T.
        using pointer = T*;
        //! \brief A reference to T.
        using reference = std::add_lvalue_reference_t<T>;

        //! \brief Initializes a new instance of the owned_or_borrowed_ptr class, which does not
        //!        contain any pointer.
        owned_or_borrowed_ptr() noexcept = default;

        //! \brief Initializes a new instance of the owned_or_borrowed_ptr class, which does not
        //!        contain any pointer.
        owned_or_borrowed_ptr(std::nullptr_t) noexcept {}

        //! \brief Initializes a new instance of the owned_or_borrowed_ptr class, which contains
        //!        the specified pointer.
        //! 
        //! \param ptr The pointer the object should hold.
        //! \param owned `true` if the instance should delete the pointer when destructed;
        //!        otherwise, `false`. The default is `true`.
        owned_or_borrowed_ptr(pointer ptr, bool owned = true) noexcept
            : _ptr{ptr},
              _owned{owned}
        {
        }

        //! \brief Deletes the contained pointer if it is not NULL and if it's owned.
        ~owned_or_borrowed_ptr()
        {
            reset();
        }

        owned_or_borrowed_ptr(const owned_or_borrowed_ptr &) = delete;

        //! \brief Move constructor.
        //! \param other The owned_or_borrowed_ptr to move from.
        owned_or_borrowed_ptr(owned_or_borrowed_ptr &&other) noexcept
            : _ptr{other.release()},
              _owned{other.is_owned()}
        {
        }

        //! \brief Move constructor from a different owned_or_borrowed_ptr if the pointer types
        //!        can be implicitly converted.
        //! \param other The owned_or_borrowed_ptr to move from.
        template<typename T2, typename = std::enable_if_t<std::is_convertible_v<T2*, pointer>>>
        owned_or_borrowed_ptr(owned_or_borrowed_ptr<T2> &&other) noexcept
            : _ptr{other.release()},
              _owned{other.is_owned()}
        {
        }

        owned_or_borrowed_ptr &operator=(const owned_or_borrowed_ptr &) = delete;

        //! \brief Move assignment operator.
        //! \param other The owned_or_borrowed_ptr to move from.
        owned_or_borrowed_ptr &operator=(owned_or_borrowed_ptr &&other) noexcept
        {
            if (this != std::addressof(other))
            {
                reset(other.release(), other.is_owned());
            }

            return *this;
        }

        //! \brief Move assignment operator from a different owned_or_borrowed_ptr if the pointer
        //!        types can be implicitly converted.
        //! \param other The owned_or_borrowed_ptr to move from.
        template<typename T2, typename = std::enable_if_t<std::is_convertible_v<T2*, pointer>>>
        owned_or_borrowed_ptr &operator=(owned_or_borrowed_ptr<T2> &&other) noexcept
        {
            if (this != std::addressof(other))
            {
                reset(other.release(), other.is_owned());
            }

            return *this;
        }

        //! \brief Assigns a NULL value to this instance.
        owned_or_borrowed_ptr &operator=(std::nullptr_t) noexcept
        {
            reset();
            return *this;
        }

        //! \brief Gets a value that indicates if the contained pointer is not NULL.
        //! \return `true` if the contained pointer is not NULL; otherwise, `false`.
        explicit operator bool() const noexcept
        {
            return _ptr != nullptr;
        }

        //! \brief Swaps this owned_or_borrowed_ptr with another.
        //! \param other The owned_or_borrowed_ptr to swap with.
        void swap(owned_or_borrowed_ptr &other) noexcept
        {
            std::swap(_ptr, other._ptr);
            std::swap(_owned, other._owned);
        }

        //! \brief Gets the contained pointer.
        pointer get() const noexcept
        {
            return _ptr;
        }

        //! \brief Gets a value that indicates whether the contained pointer is owned.
        //! 
        //! If this method returns `true`, the reset() method and the destructor will delete the
        //! contained pointer. If it's `false`, they will do nothing to that pointer.
        //! 
        //! This method may return `true` if the contained pointer is NULL, in which case it has
        //! no significance.
        //! 
        //! \return `true` if the contained pointer is owned; otherwise, `false`.
        bool is_owned() const noexcept
        {
            return _owned;
        }

        //! \brief Dererences the contained pointer.
        //! 
        //! \warning This method does not check if the contained pointer is NULL.
        //! 
        //! \return A reference to the object refered to by the pointer.
        reference operator*() const noexcept
        {
            return *get();
        }

        //! \brief Member access operator.
        pointer operator->() const noexcept
        {
            return get();
        }

        //! \brief Resets this instance with a new pointer value.
        //! 
        //! If this instance currently contains a non-NULL owned pointer, it is deleted.
        //! 
        //! \param ptr The pointer the object should hold.
        //! \param owned `true` if the instance should delete the pointer when destructed;
        //!        otherwise, `false`. The default is `true`.
        void reset(pointer ptr = nullptr, bool owned = true) noexcept
        {
            if (_owned && _ptr != nullptr)
                delete _ptr;

            _ptr = ptr;
            _owned = owned;
        }

        //! \brief Resets this instance with a NULL pointer value.
        //! 
        //! If this instance currently contains a non-NULL owned pointer, it is deleted.
        void reset(std::nullptr_t) noexcept
        {
            reset();
        }

        //! \brief Releases the contained pointer without deleting it, even if it was owned.
        //! 
        //! After calling this method, this instance will refer to a NULL pointer. The value of
        //! is_owned() will not change.
        //! 
        //! \return The previously contained pointer.
        pointer release() noexcept
        {
            return std::exchange(_ptr, nullptr);
        }

        //! \brief Creates a borrowed pointer that refers to the same pointer as this instance.
        owned_or_borrowed_ptr as_borrowed() const noexcept
        {
            return {_ptr, false};
        }

    private:
        pointer _ptr{};
        bool _owned{};
    };

    //! \brief Swaps two owned_or_borrowed_ptr instances.
    //! \param left The first instance to swap.
    //! \param right The second instance to swap.
    template<typename T>
    void swap(owned_or_borrowed_ptr<T> &left, owned_or_borrowed_ptr<T> &right) noexcept
    {
        left.swap(right);
    }

    //! \brief Creates a new instance of the owned_or_borrowed_ptr that owns the contained pointer,
    //!        by constructing a new object of type T with the specified arguments.
    //! \tparam T The type of the object to construct.
    //! \tparam Args The types of the constructor arguments.
    //! \param args The constructor arguments.
    //! \return A owned_or_borrowed_ptr that owns the constructed object.
    template<typename T, typename... Args>
    [[nodiscard]] auto make_owned_ptr(Args&&... args)
    {
        return owned_or_borrowed_ptr<T>(new T{std::forward<Args>(args)...});
    }
}

#endif