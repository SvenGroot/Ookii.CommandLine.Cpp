// [arguments]
// [case_sensitive, mode: long_short]
// [name_transform: dash-case]
struct my_arguments
{
    // [argument, short_name, required, positional]
    std::string test_arg;

    // [argument, short_name: s]
    // [short_alias: x, y]
    bool test_arg2;

    // [argument, short_name, no_long_name]
    int foo;
    
    // [argument: other]
    // [short_name, no_long_name]
    int bar;

    OOKII_DECLARE_PARSE_METHOD(my_arguments);
};
