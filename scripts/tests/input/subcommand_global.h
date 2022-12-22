// [global]
// [mode: long_short]
// [case_sensitive]
// [prefixes: -, /]
// [win32_version]
// [version_info: Version info]
// [common_help_argument]
// [name_transform: DashCase]
// Description of the application.

// [command: name]
// Description of the command
// with a line break.
//
// And a paragraph.
class my_command : public ookii::command
{
public:
    my_command(builder_type &builder);

    virtual int run() override;

private:
    // [argument, required, positional]
    std::string test_arg;
};
