// [global]
// [mode: long_short]
// [prefixes: -, /]

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
