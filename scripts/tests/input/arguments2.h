// [arguments]
// [prefixes: --, -]
// [case_sensitive, no_whitespace_separator, allow_duplicate_arguments, argument_value_separator: =]
struct other_arguments
{
    // [argument]
    int some_arg;

    OOKII_GENERATED_METHODS(other_arguments);
};

// Tests multiple structs in one file.
// [arguments]
struct third_arguments
{
    // [argument]
    int other_arg;

    OOKII_GENERATED_METHODS(third_arguments);
};