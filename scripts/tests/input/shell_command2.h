// [shell_command, no_register]
// [prefixes: --, -]
// [case_sensitive, no_whitespace_separator, allow_duplicate_arguments, argument_value_separator: =]
class other_command : public ookii::shell_command
{
public:
    other_command(builder_type &builder);

    virtual int run() override;

private:
    // [argument]
    int _some_arg;
};

// Tests multiple classes in one file.
// [shell_command]
class third_command : public other_command
{
public:
    other_command(builder_type &builder);

    virtual int run() override;

private:
    // [argument]
    int _other_arg;
};
