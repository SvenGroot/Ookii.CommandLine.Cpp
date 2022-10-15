#include "common.h"
#include "framework.h"
#include <ookii/owned_or_borrowed_ptr.h>
#include "custom_types.h"
using namespace std;
using namespace ookii;

class Value
{
public:
    Value(bool &guard, int data)
        : Data{data},
          _guard{guard}
    {
    }

    ~Value()
    {
        VERIFY_FALSE(_guard);
        _guard = true;
    }

    int Data;

private:
    bool &_guard;
};

class UobPtrTests : public test::TestClass
{
public:
    TEST_CLASS(UobPtrTests);

    TEST_METHOD(TestEmpty)
    {
        owned_or_borrowed_ptr<int> ptr;

        VERIFY_FALSE(ptr.is_owned());
        VERIFY_NULL(ptr.get());
        VERIFY_FALSE(ptr);
    }

    TEST_METHOD(TestOwned)
    {
        bool destructed1{};
        bool destructed2{};
        {
            auto src = make_unique<Value>(destructed1, 42);
            owned_or_borrowed_ptr<Value> ptr{src.release()};
            VERIFY_TRUE(ptr.is_owned());
            VERIFY_NOT_NULL(ptr.get());
            VERIFY_TRUE(ptr);
            VERIFY_EQUAL(42, (*ptr).Data);
            VERIFY_EQUAL(42, ptr->Data);
            VERIFY_EQUAL(42, ptr.get()->Data);

            ptr.reset(new Value{destructed2, 47}, true);
            VERIFY_TRUE(destructed1);
            VERIFY_TRUE(ptr.is_owned());
            VERIFY_NOT_NULL(ptr.get());
            VERIFY_TRUE(ptr);
            VERIFY_EQUAL(47, (*ptr).Data);
            VERIFY_EQUAL(47, ptr->Data);
            VERIFY_EQUAL(47, ptr.get()->Data);
        }

        VERIFY_TRUE(destructed2);
    }

    TEST_METHOD(TestBorrowed)
    {
        bool destructed{};
        {
            auto source = make_unique<Value>(destructed, 42);
            owned_or_borrowed_ptr<Value> ptr{source.get(), false};
            VERIFY_FALSE(ptr.is_owned());
            VERIFY_TRUE(ptr);
            VERIFY_TRUE(source.get() == ptr.get());
            VERIFY_EQUAL(42, ptr->Data);
            ptr.reset();
            VERIFY_FALSE(ptr);
            VERIFY_NULL(ptr.get());
            VERIFY_FALSE(destructed);
        }

        destructed = false;

        {
            auto source = make_owned_ptr<Value>(destructed, 47);
            {
                auto ptr = source.as_borrowed();
                VERIFY_FALSE(ptr.is_owned());
                VERIFY_TRUE(ptr);
                VERIFY_TRUE(source.get() == ptr.get());
                VERIFY_EQUAL(47, ptr->Data);
            }

            VERIFY_FALSE(destructed);
        }

        VERIFY_TRUE(destructed);
    }

    TEST_METHOD(TestMove)
    {
        bool destructed1{};
        bool destructed2{};
        auto source = make_owned_ptr<Value>(destructed1, 42);
        auto target = make_owned_ptr<Value>(destructed2, 47);
        target = std::move(source);
        VERIFY_FALSE(destructed1);
        VERIFY_TRUE(destructed2);
        VERIFY_TRUE(target.is_owned());
        VERIFY_EQUAL(42, target->Data);

        destructed2 = false;
        auto source2 = make_unique<Value>(destructed2, 50);
        source.reset(source2.get(), false);
        target = std::move(source);
        VERIFY_TRUE(destructed1);
        VERIFY_FALSE(destructed2);
        VERIFY_FALSE(target.is_owned());
        VERIFY_EQUAL(50, target->Data);

        destructed1 = false;
        source = make_owned_ptr<Value>(destructed1, 52);
        auto target2{std::move(source)};
        VERIFY_FALSE(destructed1);
        VERIFY_FALSE(destructed2);
        VERIFY_EQUAL(52, target2->Data);
    }
};

TEST_CLASS_REGISTRATION(UobPtrTests);