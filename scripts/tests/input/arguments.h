// [arguments: name]
// Description of the arguments
// with a line break.
//
// And a paragraph.
struct my_arguments
{
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

    OOKII_GENERATED_METHODS(my_arguments);
};
