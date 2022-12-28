#include "common.h"
#include "framework.h"
#include <ookii/command_line.h>
#include "custom_types.h"
#include "command_types.h"
using namespace std;
using namespace ookii;

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
        manager.description(TEXT("Application description."))
            .common_help_argument(TEXT("-Help"))
            .add_command<Command1>()
            .add_command<Command2>()
            .add_command<Command3>(TEXT("LastCommand"), TEXT("Foo"));

        {
            tline_wrapping_ostringstream stream{40};
            basic_usage_writer<tchar_t> usage{stream};
            manager.write_usage(&usage);
            VERIFY_EQUAL(c_usageExpected, stream.str());
        }

        {
            tline_wrapping_ostringstream stream{40};
            basic_usage_writer<tchar_t> usage{stream, true};
            manager.write_usage(&usage);
            VERIFY_EQUAL(c_usageExpectedColor, stream.str());
        }
    }

    TEST_METHOD(TestCreate)
    {
        basic_command_manager<tchar_t> manager{TEXT("TestApp")};
        manager.add_command<Command1>()
            .add_command<Command2>()
            .add_command<Command3>(TEXT("LastCommand"), TEXT("Foo"));

        tline_wrapping_ostringstream output{0};
        tline_wrapping_ostringstream error{0};
        basic_usage_writer<tchar_t> options{output, error};
        auto command = create_command(manager, { TEXT("AnotherCommand"), TEXT("-Value"), TEXT("42") }, &options);
        VERIFY_NOT_NULL(command);
        auto actual = dynamic_cast<Command2*>(command.get());
        VERIFY_NOT_NULL(actual);
        VERIFY_EQUAL(42, actual->value);
        VERIFY_EQUAL(TEXT(""), output.str());
        VERIFY_EQUAL(TEXT(""), error.str());

        command = create_command(manager, { TEXT("AnotherCommand") }, &options);
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
        auto result = run_command(manager, { TEXT("AnotherCommand"), TEXT("-Value"), TEXT("42") }, &options);
        VERIFY_NOT_NULL(result);
        VERIFY_EQUAL(42, *result);
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

    TEST_METHOD(TestConfigureParser)
    {
        basic_command_manager<tchar_t> manager{TEXT("TestApp")};
        manager.configure_parser([](auto &parser) { parser.mode(parsing_mode::long_short); })
            .add_command<Command2>();

        auto result = run_command(manager, { TEXT("AnotherCommand"), TEXT("--value"), TEXT("42") });
        VERIFY_NOT_NULL(result);
        VERIFY_EQUAL(42, *result);
    }

    TEST_METHOD(TestCustomParsing)
    {
        basic_command_manager<tchar_t> manager{TEXT("TestApp")};
        manager.add_command<CustomParsingCommand>();

        auto info = manager.get_command(TEXT("CustomParsingCommand"));
        VERIFY_TRUE(info->use_custom_argument_parsing());

        auto command = create_command(manager, { TEXT("CustomParsingCommand"), TEXT("Hello") });
        VERIFY_NOT_NULL(command);
        auto actual = static_cast<CustomParsingCommand*>(command.get());
        VERIFY_EQUAL(TEXT("Hello"), actual->Value);
    }

    static std::optional<int> run_command(const basic_command_manager<tchar_t> &manager, std::initializer_list<const tchar_t*> args, basic_usage_writer<tchar_t> *usage = nullptr)
    {
        std::vector<const tchar_t*> arguments{TEXT("Executable")};
        arguments.insert(arguments.end(), args);
        return manager.run_command(static_cast<int>(arguments.size()), arguments.data(), usage);
    }

    static std::unique_ptr<basic_command<tchar_t>> create_command(const basic_command_manager<tchar_t> &manager, std::initializer_list<const tchar_t*> args, basic_usage_writer<tchar_t> *usage = nullptr)
    {
        std::vector<const tchar_t*> arguments{TEXT("Executable")};
        arguments.insert(arguments.end(), args);
        return manager.create_command(static_cast<int>(arguments.size()), arguments.data(), usage);
    }

    static constexpr tstring_view c_usageExpected = TEXT(R"(Application description.

Usage: TestApp <command> [arguments]

The following commands are available:

    AnotherCommand
        This is a very long description
        that probably needs to be
        wrapped.

    Command1

    LastCommand
        Foo

Run 'TestApp <command> -Help' for more
information about a command.
)");

    static constexpr tstring_view c_usageExpectedColor = TEXT(R"(Application description.

[36mUsage:[0m TestApp <command> [arguments]

The following commands are available:

    [32mAnotherCommand[0m
        This is a very long description
        that probably needs to be
        wrapped.

    [32mCommand1[0m

    [32mLastCommand[0m
        Foo

Run 'TestApp <command> -Help' for more
information about a command.
)");

    static constexpr tstring_view c_commandUsageExpected = TEXT(R"(This is a very long description that probably needs to be wrapped.

Usage: TestApp AnotherCommand -Value <int> [-Help]

    -Help [<bool>] (-?, -h)
        Displays this help message.

)");

    static constexpr tstring_view c_commandErrorExpected = TEXT(R"(The required argument 'Value' was not supplied.

)");

};

TEST_CLASS_REGISTRATION(SubcommandTests);
