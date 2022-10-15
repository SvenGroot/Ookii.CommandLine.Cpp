# Parsing command line arguments

When you have [defined the command line arguments](DefiningArguments.md), you
can now parse the command line to determine the argument values.

After you call `build()` on the `ookii::parser_builder()`, you are given an instance of the
`ookii::command_line_parser` class. You can use this instance to inspect information about all the
arguments you created, and to [generate usage help](UsageHelp.md), but most importantly you can
call one of the overloads of the `parse()` method. Which overload you want to use depends on how
you wish to handle errors.

## Letting `command_line_parser` handle errors

The `parse()` method has a number of overloads, and some of those overloads take a last argument of
type `ookii::usage_options`. These overloads will handle errors, and print an error message and
usage help to the console if necessary.

There are several of these overloads, but the one you will usually use is probably this one:

```c++
result_type parse(int argc, const CharType *const argv[], const usage_options_type &options);
```

This one takes the arguments as they are passed to your `main()` function. It assumes that `argv[0]`
contains the application name, and therefore skips it, checking the remainder for your defined
arguments.

A typical usage of this method is as follows:

```c++
int main(int argc, char *argv[])
{
    /* Argument variables go here. */
    auto parser = /* parser builder argument definitions go here. */;
    if (!parser.parse(argc, argv, {}))
    {
        return 1; // Or a suitable error code.
    }

    // Run the application, using your arguments.
    do_something();
    return 0;
}
```

See [here](DefiningArguments.md) for an example of the omitted code to build the parser.

A default instance of `usage_options` is passed to the method in this example, but you can of course
create one and set its members to customize the error and usage help output. If the default instance
is used, usage help is written to the standard output, using the default format, and white-space
wrapping the output at the console width.

The `parse()` method returns a type that can be tested to indicate whether parsing was successful.
That type also provides additional information, but since the error was already handled, and usage
help was printed to the console, there's nothing else you need to do here.

**Note:** argument parsing is not atomic. If it fails, some of the variables may already have been
set by arguments supplied before the error occurred.

## Manual error handling

In most cases, the above method should be sufficient for your needs, but sometimes you may want
more control over how errors are handled (for example, to print custom localized error messages
instead of the default ones).

In this case, call an overload that doesn't take a `usage_options`. This will return a value of
type `parse_result` (the overloads that do take `usage_options` also return that, actually), which
contains more information about the error.

The `parse_result` type can be converted to a boolean to easily check if parsing was successful.
If you want more information, check the `parse_result::error` field, which contains an enumeration
indicating the type of error that occurred. If no error occurred, the error will be `parse_error::none`.

The the `parse_error::error_arg_name` field indicates the argument that caused the error. Note that
not all categories of error will have this value set. If an error wasn't related to a specific
argument, this field will be blank. The name may also refer to a non-existent argument, if the user
specified an unknown one.

You can call `parse_result::get_error_message()` to get the default error message for the error,
or use the `error` and `error_arg_name` fields to create your own. Note that `parse_error::none`
and `parse_error::parsing_cancelled` don't have an error message and will return an empty string;
the latter is returned by an argument that used `parser_builder::argument_builder::cancel_parsing()`.

Here is a small sample of this kind of usage:

```c++
int main(int argc, char *argv[])
{
    /* Argument variables go here. */
    auto parser = /* parser builder argument definitions go here. */;
    auto result = parser.parse(argc, argv);
    if (!result)
    {
        if (result.error != ookii::parse_error::parsing_cancelled)
        {
            auto err = ookii::line_wrapping_ostream::for_cerr();
            err << result.get_error_message() << std::endl << std::endl;
        }

        parser.write_usage();
        return 1;
    }

    // Run the application, using your arguments.
    do_something();
    return 0;
}
```

The `write_usage()` method optionally takes the same `usage_options` parameter to customize the
appearance of the usage help.

Speaking of usage help, let's take [a detailed look at how that works next](UsageHelp.md).
