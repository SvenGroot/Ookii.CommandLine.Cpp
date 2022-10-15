// [shell_command: name]
// Description of the command
// with a line break.
//
// And a paragraph.
class my_command : public ookii::shell_command
{
public:
    my_command(builder_type &builder);

    virtual int run() override;

private:
    // [argument, required, positional]
    // Argument description
    // with a line break.
    //
    // And another paragraph.
    std::string test_arg;
    
    /// [argument, positional, default: 1]
    //// [alias: test]
    /// [value_description: desc]
    /// Short description.
    int __test__arg2__ = 2;
    
    // [argument: foo]
    // [alias: t, v]
    // [multi_value]
    std::vector<float> test_arg3;

    // [argument, cancel_parsing]
    bool _testArg4{};

    // [argument, default: "foo"]
    std::string TestArg5;

    int not_an_arg{5};
};
