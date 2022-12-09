#include "common.h"
#include "framework.h"
#include <ookii/command_line.h>
#include "custom_types.h"
using namespace std;
using namespace ookii;

class Command1 : public basic_command<tchar_t>
{
public:
    Command1(builder_type &)
    {
    }

    int run() override
    {
        return 0;
    }
};

class Command2 : public basic_command<tchar_t>
{
public:
    Command2(builder_type &builder)
    {
        builder.add_argument(value, TEXT("Value")).required();
    }

    int run() override
    {
        return value;
    }

    static tstring name()
    {
        return TEXT("AnotherCommand");
    }

    static tstring description()
    {
        return TEXT("This is a very long description that probably needs to be wrapped.");
    }

    int value{};
};

class Command3 : public basic_command<tchar_t>
{
public:
    Command3(builder_type &)
    {
    }

    int run() override
    {
        return 0;
    }
};

class SubcommandTests : public test::TestClass
{
public:
    TEST_CLASS(SubcommandTests);

    TEST_METHOD(TestCommands)
    {
        basic_command_manager<tchar_t> manager{TEXT("TestApp")};
        manager.add_command<Command1>()
            .add_command<Command2>()
            .add_command<Command3>(TEXT("LastCommand"), TEXT("Foo"));

        size_t index = 0;
        for (const auto &command : manager.commands())
        {
            VERIFY_TRUE(index < 3);
            switch (index)
            {
            case 0:
                VERIFY_EQUAL(TEXT("AnotherCommand"), command.name());
                VERIFY_EQUAL(TEXT("This is a very long description that probably needs to be wrapped."), command.description());
                break;

            case 1:
                VERIFY_EQUAL(TEXT("Command1"), command.name());
                VERIFY_EQUAL(TEXT(""), command.description());
                break;

            case 2:
                VERIFY_EQUAL(TEXT("LastCommand"), command.name());
                VERIFY_EQUAL(TEXT("Foo"), command.description());
                break;
            }

            ++index;
        }

        auto command = manager.get_command(TEXT("Command1"));
        VERIFY_NOT_NULL(command);
        VERIFY_EQUAL(TEXT("Command1"), command->name());
        VERIFY_EQUAL(TEXT(""), command->description());

        command = manager.get_command(TEXT("AnotherCommand"));
        VERIFY_NOT_NULL(command);
        VERIFY_EQUAL(TEXT("AnotherCommand"), command->name());
        VERIFY_EQUAL(TEXT("This is a very long description that probably needs to be wrapped."), command->description());

        command = manager.get_command(TEXT("LastCommand"));
        VERIFY_NOT_NULL(command);
        VERIFY_EQUAL(TEXT("LastCommand"), command->name());
        VERIFY_EQUAL(TEXT("Foo"), command->description());

        command = manager.get_command(TEXT("Command2"));
        VERIFY_NULL(command);

        // Case insensitive.
        command = manager.get_command(TEXT("lastcommand"));
        VERIFY_NOT_NULL(command);
        VERIFY_EQUAL(TEXT("LastCommand"), command->name());
    }

    TEST_METHOD(TestCommandListUsage)
    {
        basic_command_manager<tchar_t> manager{TEXT("TestApp")};
        manager.add_command<Command1>()
            .add_command<Command2>()
            .add_command<Command3>(TEXT("LastCommand"), TEXT("Foo"));

        tstringstream stream;
        tline_wrapping_stream wrap_stream{stream, 40};
        basic_usage_writer<tchar_t> usage{wrap_stream};
        manager.write_usage(&usage);
        VERIFY_EQUAL(c_usageExpected, stream.str());
    }

    TEST_METHOD(TestCreate)
    {
        basic_command_manager<tchar_t> manager{TEXT("TestApp")};
        manager.add_command<Command1>()
            .add_command<Command2>()
            .add_command<Command3>(TEXT("LastCommand"), TEXT("Foo"));

        tstringstream output;
        tstringstream error;
        basic_usage_writer<tchar_t> options{output, error};
        auto command = manager.create_command({ TEXT("AnotherCommand"), TEXT("-Value"), TEXT("42") }, &options);
        VERIFY_NOT_NULL(command);
        auto actual = dynamic_cast<Command2*>(command.get());
        VERIFY_NOT_NULL(actual);
        VERIFY_EQUAL(42, actual->value);
        VERIFY_EQUAL(TEXT(""), output.str());
        VERIFY_EQUAL(TEXT(""), error.str());

        command = manager.create_command({ TEXT("AnotherCommand") }, &options);
        VERIFY_NULL(command);
        VERIFY_EQUAL(c_commandUsageExpected, output.str());
        VERIFY_EQUAL(c_commandErrorExpected, error.str());
    }

    TEST_METHOD(TestRun)
    {
        basic_command_manager<tchar_t> manager{TEXT("TestApp")};
        manager.add_command<Command1>()
            .add_command<Command2>()
            .add_command<Command3>(TEXT("LastCommand"), TEXT("Foo"));

        tstringstream output;
        tstringstream error;
        basic_usage_writer<tchar_t> options{output, error};
        auto result = manager.run_command({ TEXT("AnotherCommand"), TEXT("-Value"), TEXT("42") }, &options);
        VERIFY_EQUAL(42, result);
    }

    TEST_METHOD(TestCaseSensitive)
    {
        basic_command_manager<tchar_t> manager{TEXT("TestApp"), true};
        manager.add_command<Command1>()
            .add_command<Command2>()
            .add_command<Command3>(TEXT("LastCommand"), TEXT("Foo"));

        auto command = manager.get_command(TEXT("LastCommand"));
        VERIFY_NOT_NULL(command);
        VERIFY_EQUAL(TEXT("LastCommand"), command->name());

        command = manager.get_command(TEXT("lastcommand"));
        VERIFY_NULL(command);
    }

    static constexpr tstring_view c_usageExpected = TEXT(R"(Usage: TestApp <command> [arguments]

The following commands are available:

    AnotherCommand
        This is a very long description
        that probably needs to be
        wrapped.

    Command1

    LastCommand
        Foo

)");

    static constexpr tstring_view c_commandUsageExpected = TEXT(R"(This is a very long description that probably needs to be wrapped.

Usage: TestApp AnotherCommand -Value <int>

)");

    static constexpr tstring_view c_commandErrorExpected = TEXT(R"(The required argument 'Value' was not supplied.

)");

};

TEST_CLASS_REGISTRATION(SubcommandTests);
