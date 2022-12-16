#pragma once

#include <vector>
#include <functional>
#include <regex>
#include <mutex>
#include <ookii/format_helper.h>
#include <ookii/type_info.h>
#include <ookii/string_helper.h>
#include "unicode.h"

#define TEST_CLASS(className) \
    using TestClassType = className; \
    const ::ookii::tchar_t *TestClassName() const override { return TEXT(#className); }

#define TEST_METHOD(methodName) \
    TestMethodRegistration methodName##_Registration{this, TEXT(#methodName), std::bind(&TestClassType::methodName, this)}; \
    void methodName()

#define TEST_SETUP_METHOD(methodName) \
    SetupMethodRegistration methodName##_Registration{this, std::bind(&TestClassType::methodName, this)}; \
    void methodName()

#define TEST_CLEANUP_METHOD(methodName) \
    CleanupMethodRegistration methodName##_Registration{this, std::bind(&TestClassType::methodName, this)}; \
    void methodName()

#define TEST_CLASS_REGISTRATION(className) \
    ::ookii::test::TestClassRegistration<className> className##_Registration

namespace ookii::test
{

extern bool g_verbose;

enum class LogMessageType
{
    Start,
    Pass,
    Fail,
    End,
    Error,
    Info
};

namespace details
{

    extern std::mutex g_logMutex;

    // It would be preferable to use std::basic_format_string, but MSVC does not implement
    // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2508r1.html. The type is available
    // in libfmt.
    template<typename... Args>
    void LogMessagePart(LogMessageType type, const std::basic_string_view<tchar_t> format, Args&&... args)
    {
        switch (type)
        {
        case LogMessageType::Start:
            tcout << TEXT("START: ");
            break;

        case LogMessageType::Pass:
            tcout << TEXT("PASS:  ");
            break;

        case LogMessageType::Fail:
            tcout << TEXT("FAIL:  ");
            break;

        case LogMessageType::End:
            tcout << TEXT("END:   ");
            break;

        case LogMessageType::Error:
            tcout << TEXT("ERROR: ");
            break;

        case LogMessageType::Info:
            tcout << TEXT("INFO:  ");
            break;
        }

        tcout << format::ncformat({}, format, std::forward<Args>(args)...);
    }

}

template<typename... Args>
void LogMessage(LogMessageType type, const std::basic_string_view<tchar_t> format, Args&&... args)
{
    std::lock_guard<std::mutex> lock{details::g_logMutex};
    details::LogMessagePart(type, format, std::forward<Args>(args)...);
    tcout << std::endl;
}

class TestFailureException : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class TestResult
{
public:
    void AddFailure(tstring name)
    {
        m_failedTests.push_back(name);
    }

    void AddPass()
    {
        ++m_passed;
    }

    TestResult &operator+=(const TestResult &right)
    {
        m_passed += right.m_passed;
        m_failedTests.insert(m_failedTests.end(), right.m_failedTests.begin(), right.m_failedTests.end());
        return *this;
    }

    int Passed() const
    {
        return m_passed;
    }

    const std::vector<tstring> Failed() const
    {
        return m_failedTests;
    }

private:
    int m_passed{};
    std::vector<tstring> m_failedTests;
};

class TestClass
{
public:
    virtual ~TestClass() = default;

    TestResult RunTests(const std::basic_regex<tchar_t> *pattern)
    {
        TestResult result;
        bool ranTests = false;
        auto className = TestClassName();
        for (auto &test : m_testMethods)
        {
            auto fullName = OOKII_FMT_NS format(TEXT("{}::{}"), className, test.Name);
            if (pattern != nullptr && !std::regex_match(fullName, *pattern))
            {
                continue;
            }

            // Run the setup method if this is the first test in the class. Doing this here avoids
            // running the setup method if no tests in the class match the filter.
            if (!ranTests)
            {
                if (m_setupMethod)
                {
                    LogMessage(LogMessageType::Info, TEXT("{}::<test setup>"), className);
                    if (!RunTestMethod(m_setupMethod))
                    {
                        LogMessage(LogMessageType::Fail, TEXT("{}::<test setup>"), className);
                        result.AddFailure(OOKII_FMT_NS format(TEXT("{}::<test setup>"), className));
                        break;
                    }
                }

                ranTests = true;
            }

            LogMessage(LogMessageType::Start, TEXT("{}"), fullName);
            if (RunTestMethod(test.Method))
            {
                result.AddPass();
                LogMessage(LogMessageType::Pass, TEXT("{}"), fullName);
            }
            else
            {
                result.AddFailure(fullName);
                LogMessage(LogMessageType::Fail, TEXT("{}"), fullName);
            }
        }

        if (ranTests && m_cleanupMethod)
        {
            LogMessage(LogMessageType::Info, TEXT("{}::<test cleanup>"), className);
            if (!RunTestMethod(m_cleanupMethod))
            {
                LogMessage(LogMessageType::Fail, TEXT("{}::<test cleanup>"), className);
                result.AddFailure(OOKII_FMT_NS format(TEXT("{}::<test cleanup>"), className));
            }
        }

        return result;
    }

protected:
    class TestMethodRegistration
    {
    public:
        TestMethodRegistration(TestClass *testClass, tstring name, std::function<void()> method)
        {
            testClass->m_testMethods.push_back({name, method});
        }
    };

    class SetupMethodRegistration
    {
    public:
        SetupMethodRegistration(TestClass *testClass, std::function<void()> method)
        {
            testClass->m_setupMethod = method;
        }
    };

    class CleanupMethodRegistration
    {
    public:
        CleanupMethodRegistration(TestClass *testClass, std::function<void()> method)
        {
            testClass->m_cleanupMethod = method;
        }
    };

    virtual const ::ookii::tchar_t *TestClassName() const = 0;

private:
    struct TestMethodInfo
    {
        tstring Name;
        std::function<void()> Method;
    };

    static bool RunTestMethod(const std::function<void()> &method)
    {
        try
        {
            method();
            return true;
        }
        catch (const TestFailureException &)
        {
        }
        catch (const std::exception &ex)
        {
            LogMessage(LogMessageType::Error, TEXT("Unexpected exception of type {}: {}"), get_type_name<tchar_t>(typeid(ex)), string_convert<tchar_t>::from_bytes(ex.what()));
        }
        catch (...)
        {
            LogMessage(LogMessageType::Error, TEXT("Unknown exception"));
        }

        return false;
    }

    std::vector<TestMethodInfo> m_testMethods;
    std::function<void()> m_setupMethod;
    std::function<void()> m_cleanupMethod;
};

class TestDriver
{
public:
    static TestDriver &Instance()
    {
        static TestDriver instance;
        return instance;
    }

    void RegisterTestClass(tstring name, std::function<std::unique_ptr<TestClass>()> creationMethod)
    {
        m_testClasses.push_back({name, creationMethod});
    }

    bool RunTests(const std::basic_regex<tchar_t> *pattern)
    {
        TestResult result;
        for (auto &testClass : m_testClasses)
        {
            auto instance = testClass.CreationMethod();
            result += instance->RunTests(pattern);
        }

        tcout << std::endl;

        tcout << OOKII_FMT_NS format(TEXT("Passed: {}; Failed: {}"), result.Passed(), result.Failed().size());
        tcout << std::endl;
        bool testResult;
        if (result.Failed().size() > 0)
        {
            tcout << std::endl;
            tcout << TEXT("Failed tests:") << std::endl;
            for (auto &test : result.Failed())
            {
                tcout << TEXT("    ") << test << std::endl;
            }

            testResult = false;
        }
        else
        {
            testResult = true;
        }

        return testResult;
    }

private:
    struct TestClassInfo
    {
        tstring Name;
        std::function<std::unique_ptr<TestClass>()> CreationMethod;
    };

    TestDriver() = default;

    std::vector<TestClassInfo> m_testClasses;
};

template<typename T>
class TestClassRegistration
{
public:
    TestClassRegistration()
    {
        TestDriver::Instance().RegisterTestClass(get_short_type_name<T, tchar_t>(), []()
            {
                return std::make_unique<T>();
            });
    }
};

namespace details
{

    struct VerifyInfo
    {
        const char *FunctionName;
        const char *File;
        int Line;
    };

#define OOKII_TEST_VERIFY_INFO ::ookii::test::details::VerifyInfo{__func__, __FILE__, __LINE__}
#define OOKII_TEST_THROW_FAILED() throw TestFailureException("Test failed.")
#define OOKII_TEST_THROW_FAILED_IF(condition) if (condition) { OOKII_TEST_THROW_FAILED(); }

    template<typename... Args>
    void LogVerify(const VerifyInfo &info, bool success, std::basic_string_view<tchar_t> format, Args... args)
    {
        auto olderrno = errno;
        std::lock_guard<std::mutex> lock{g_logMutex};
        if (success)
        {
            if (g_verbose)
                LogMessagePart(LogMessageType::Info, TEXT("{}:{}: Verify: "), string_convert<tchar_t>::from_bytes(info.FunctionName), info.Line);
        }
        else
        {
            LogMessagePart(LogMessageType::Error, TEXT("{}:{}: Failed: "), string_convert<tchar_t>::from_bytes(info.FunctionName), info.Line);
        }

        if (!success || g_verbose)
            tcout << format::ncformat({}, format, std::forward<Args>(args)...) << std::endl;

        errno = olderrno;
    }

    template<typename T1, typename T2>
    void VerifyEqual(const T1 &expected, const T2 &actual, const tchar_t *expectedName, const tchar_t *actualName, const VerifyInfo &info)
    {
        bool success = (expected == actual);
        LogVerify(info, success, TEXT("{} == {} (<{}> == <{}>)"), expectedName, actualName, expected, actual);
        OOKII_TEST_THROW_FAILED_IF(!success);
    }

    template<typename T1, typename T2>
    void VerifyNotEqual(const T1 &expected, const T2 &actual, const tchar_t *expectedName, const tchar_t *actualName, const VerifyInfo &info)
    {
        bool success = (expected != actual);
        LogVerify(info, success, TEXT("{} != {} (<{}> != <{}>)"), expectedName, actualName, expected, actual);
        OOKII_TEST_THROW_FAILED_IF(!success);
    }

    inline void VerifyTrue(bool condition, const tchar_t *conditionName, const VerifyInfo &info)
    {
        LogVerify(info, condition, TEXT("{} == true"), conditionName);
        OOKII_TEST_THROW_FAILED_IF(!condition);
    }

    inline void VerifyFalse(bool condition, const tchar_t *conditionName, const VerifyInfo &info)
    {
        LogVerify(info, !condition, TEXT("{} == false"), conditionName);
        OOKII_TEST_THROW_FAILED_IF(condition);
    }

    template<typename T>
    inline void VerifyNull(const T &value, const tchar_t *valueName, const VerifyInfo &info)
    {
        bool success = value == nullptr;
        LogVerify(info, success, TEXT("{} == nullptr"), valueName);
        OOKII_TEST_THROW_FAILED_IF(!success);
    }

    template<typename T>
    inline void VerifyNull(const std::optional<T> &value, const tchar_t *valueName, const VerifyInfo &info)
    {
        bool success = value == std::nullopt;
        LogVerify(info, success, TEXT("{} == nullptr"), valueName);
        OOKII_TEST_THROW_FAILED_IF(!success);
    }

    template<typename T>
    inline void VerifyNotNull(const T &value, const tchar_t *valueName, const VerifyInfo &info)
    {
        bool success = value != nullptr;
        LogVerify(info, success, TEXT("{} != nullptr"), valueName);
        OOKII_TEST_THROW_FAILED_IF(!success);
    }

    template<typename T>
    inline void VerifyNotNull(const std::optional<T> &value, const tchar_t *valueName, const VerifyInfo &info)
    {
        bool success = value != std::nullopt;
        LogVerify(info, success, TEXT("{} != nullptr"), valueName);
        OOKII_TEST_THROW_FAILED_IF(!success);
    }

    inline void VerifyMemoryEqual(const void *expected, const void *actual, size_t size, const tchar_t *expectedName, const tchar_t *actualName, const VerifyInfo &info)
    {
        bool success = memcmp(expected, actual, size) == 0;
        LogVerify(info, success, TEXT("MemoryEqual({}, {})"), expectedName, actualName);
        OOKII_TEST_THROW_FAILED_IF(!success);
    }

    template<typename T>
    concept NotPointer = requires(T param)
    {
        requires !std::is_pointer_v<T>;
    };

    template<NotPointer T1, NotPointer T2>
    void VerifyReferenceEqual(const T1 &expected, const T2 &actual, const tchar_t *expectedName, const tchar_t *actualName, const VerifyInfo &info)
    {
        bool success = (std::addressof(expected) == std::addressof(actual));
        LogVerify(info, success, TEXT("std::addressof({}) == std::addressof({})"), expectedName, actualName);
        OOKII_TEST_THROW_FAILED_IF(!success);
    }

    template<typename T1, typename T2>
    void VerifyReferenceEqual(const T1 *expected, const T2 *actual, const tchar_t *expectedName, const tchar_t *actualName, const VerifyInfo &info)
    {
        bool success = (expected == actual);
        LogVerify(info, success, TEXT("{} == {}"), expectedName, actualName);
        OOKII_TEST_THROW_FAILED_IF(!success);
    }

    inline void VerifyExpectedException(bool thrown, const tchar_t *exceptionType, const tchar_t *operation, const VerifyInfo &info)
    {
        LogVerify(info, thrown, TEXT("Operation {} throws exception {}"), operation, exceptionType);
        OOKII_TEST_THROW_FAILED_IF(!thrown);
    }

    template<typename Range>
    inline void VerifyRangeEqual(const Range &expected, const Range &actual, const tchar_t *expectedName, const tchar_t *actualName, const VerifyInfo &info)
    {
        
        // Alow for ADL with using.
        using std::begin;
        using std::end;
        using std::size;
        if (size(expected) != size(actual))
        {
            LogVerify(info, false, TEXT("Range: {} == {} (size differs: {} != {})"), expectedName, actualName, size(expected), size(actual));
            OOKII_TEST_THROW_FAILED();
        }

        int index{};
        for (auto item1 = begin(expected), item2 = begin(actual); item1 != end(expected); ++item1, ++item2, ++index)
        {
            if (*item1 != *item2)
            {
                LogVerify(info, false, TEXT("Range: {} == {} (items at index {} differ: <{}> != <{}>)"), expectedName, actualName,
                          index, *item1, *item2);

                OOKII_TEST_THROW_FAILED();
            }
        }

        LogVerify(info, true, TEXT("Range: {} == {} (size {})"), expectedName, actualName, size(expected));
    }

    inline void VerifyUnexpectedException(const tchar_t *operation, const VerifyInfo &info)
    {
        try
        {
            throw;
        }
        catch (const TestFailureException &)
        {
            throw;
        }
        catch (const std::exception &ex)
        {
            LogVerify(info, false, TEXT("Operation {} threw unexpected exception {}: {}"), operation,
                      get_type_name<tchar_t>(typeid(ex)), string_convert<tchar_t>::from_bytes(ex.what()));
        }
        catch (...)
        {
            LogVerify(info, false, TEXT("Operation {} threw an unknown exception."), operation);
        }

        OOKII_TEST_THROW_FAILED();
    }

}

#define VERIFY_EQUAL(expected, actual) \
    ::ookii::test::details::VerifyEqual((expected), (actual), TEXT(#expected), TEXT(#actual), OOKII_TEST_VERIFY_INFO)

#define VERIFY_NOT_EQUAL(expected, actual) \
    ::ookii::test::details::VerifyNotEqual((expected), (actual), TEXT(#expected), TEXT(#actual), OOKII_TEST_VERIFY_INFO)

#define VERIFY_TRUE(condition) \
    ::ookii::test::details::VerifyTrue(static_cast<bool>(condition), TEXT(#condition), OOKII_TEST_VERIFY_INFO)

#define VERIFY_FALSE(condition) \
    ::ookii::test::details::VerifyFalse(static_cast<bool>(condition), TEXT(#condition), OOKII_TEST_VERIFY_INFO)

#define VERIFY_NULL(value) \
    ::ookii::test::details::VerifyNull(value, TEXT(#value), OOKII_TEST_VERIFY_INFO)

#define VERIFY_NOT_NULL(value) \
    ::ookii::test::details::VerifyNotNull(value, TEXT(#value), OOKII_TEST_VERIFY_INFO)

#define VERIFY_RANGE_EQUAL(expected, actual) \
    ::ookii::test::details::VerifyRangeEqual((expected), (actual), TEXT(#expected), TEXT(#actual), OOKII_TEST_VERIFY_INFO)

#define VERIFY_MEMORY_EQUAL(expected, actual, size) \
    ::ookii::test::details::VerifyMemoryEqual((expected), (actual), (size), TEXT(#expected), TEXT(#actual), OOKII_TEST_VERIFY_INFO)

#define VERIFY_REFERENCE_EQUAL(expected, actual) \
    ::ookii::test::details::VerifyReferenceEqual((expected), (actual), TEXT(#expected), TEXT(#actual), OOKII_TEST_VERIFY_INFO)

#define VERIFY_THROWS(operation, expectedException)                                                                             \
    {                                                                                                                           \
        bool ookiiTestExceptionThrown = false;                                                                                  \
        try                                                                                                                     \
        {                                                                                                                       \
            operation;                                                                                                          \
        }                                                                                                                       \
        catch (const expectedException &)                                                                                       \
        {                                                                                                                       \
            ookiiTestExceptionThrown = true;                                                                                    \
        }                                                                                                                       \
        catch (...)                                                                                                             \
        {                                                                                                                       \
            ::ookii::test::details::VerifyUnexpectedException(TEXT(#operation), OOKII_TEST_VERIFY_INFO);                              \
        }                                                                                                                       \
                                                                                                                                \
        ::ookii::test::details::VerifyExpectedException(ookiiTestExceptionThrown, TEXT(#expectedException), TEXT(#operation), OOKII_TEST_VERIFY_INFO); \
    }

#define VERIFY_THROWS_TEST(operation, expectedException, condition)                                                             \
    {                                                                                                                           \
        bool ookiiTestExceptionThrown = false;                                                                                  \
        try                                                                                                                     \
        {                                                                                                                       \
            operation;                                                                                                          \
        }                                                                                                                       \
        catch (const expectedException &ex)                                                                                     \
        {                                                                                                                       \
            VERIFY_TRUE((condition));                                                                                           \
            ookiiTestExceptionThrown = true;                                                                                    \
        }                                                                                                                       \
        catch (...)                                                                                                             \
        {                                                                                                                       \
            ::ookii::test::details::VerifyUnexpectedException(#operation, OOKII_TEST_VERIFY_INFO);                              \
        }                                                                                                                       \
                                                                                                                                \
        ::ookii::test::details::VerifyExpectedException(ookiiTestExceptionThrown, #expectedException, #operation, OOKII_TEST_VERIFY_INFO); \
    }

#define VERIFY_NO_THROW(operation)                                                                                              \
    try                                                                                                                         \
    {                                                                                                                           \
        operation;                                                                                                              \
        LogVerify(OOKII_TEST_VERIFY_INFO, true, TEXT("Operation does not throw: {}"), #operation);                              \
    }                                                                                                                           \
    catch (...)                                                                                                                 \
    {                                                                                                                           \
        ::ookii::test::details::VerifyUnexpectedException(#operation, OOKII_TEST_VERIFY_INFO);                                  \
    }

}