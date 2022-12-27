#pragma once

class Command1 : public ookii::basic_command<ookii::tchar_t>
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

class Command2 : public ookii::basic_command<ookii::tchar_t>
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

    static ookii::tstring name()
    {
        return TEXT("AnotherCommand");
    }

    static ookii::tstring description()
    {
        return TEXT("This is a very long description that probably needs to be wrapped.");
    }

    int value{};
};

class Command3 : public ookii::basic_command<ookii::tchar_t>
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

class CustomParsingCommand : public ookii::basic_command_with_custom_parsing<ookii::tchar_t>
{
public:
    virtual bool parse(int argc, const ookii::tchar_t *const argv[], const command_manager_type &, usage_writer_type *) override
    {
        VERIFY_EQUAL(1, argc);
        Value = argv[0];
        return true;
    }

    virtual int run() override
    {
        return 0;
    }

    ookii::tstring Value{};
};