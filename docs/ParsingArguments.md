# Parsing command line arguments

When you have [defined the command line arguments](DefiningArguments.md), you can now parse the
command line to determine the argument values.

After you call `build()` on the `ookii::parser_builder()` class, you are given an instance of the
`ookii::command_line_parser` class. You can use this instance to inspect information about all the
arguments you created, and to [generate usage help](UsageHelp.md), but most importantly you can
call one of the overloads of the `command_line_parser::parse()` method. Which overload you want to
use depends on how you wish to handle errors.

## Letting `command_line_parser` handle errors

The `parse()` method has a number of overloads, and some of those overloads take a last argument
that is a pointer to an `ookii::usage_writer` class. These overloads will handle errors, and print
an error message and usage help to the console if necessary. They will do this even if you pass
`nullptr` for the `usage_writer` argument.

There are several of these overloads, but the one you will usually use is probably this one:

```c++
ookii::parse_result parse(int argc, const char *const argv[], ookii::usage_writer *writer);
```

This overload takes the arguments as they are passed to your `main()` function. It assumes that
`argv[0]` contains the application name, and therefore skips it, checking the remainder for your
defined arguments.

A typical usage of this method is as follows:

```c++
int main(int argc, char *argv[])
{
    auto parser = /* parser_builder code goes here. */;
    if (!parser.parse(argc, argv, nullptr))
    {
        return 1; // Or a suitable error code.
    }

    // Run the application, using your arguments.
    do_something();
    return 0;
}
```

See [here](DefiningArguments.md) for information about the omitted code to build the parser.

The `parse()` method returns an instance of the `parse_result` class, which can be implicitly
converted to a boolean for testing. If parsing is successful, the result will use
`parse_error::success`, which evaluates as `true`. At that point, all the variables you used for
your arguments will be set to the values specified on the command line.

Argument parsing can fail for a number of reason, including:

- Not all required arguments were specified.
- An argument name was supplied without a value for an argument that’s not a switch.
- An unknown argument name was supplied.
- A non-multi-value argument was specified more than once.
- Too many positional argument values were supplied.
- Argument value conversion failed for one of the arguments.

See the `parse_error` enumeration for all possible error categories. In addition, parsing could
have been canceled by an argument using the `parser_builder::argument_builder_common::cancel_parsing()`
method, an action argument, or the automatic `-Help` or `-Version` argument.

If an error does occur, this overload of the `parse()` method will handle it by print an error
message, and displaying the usage help if requested. This is done according to the passed
`usage_writer`.

If you pass `nullptr` for the `usage_writer`, as in this example, it means the default
`usage_writer` will be used. The default instance will write usage help to the standard output,
using the default format, with color enabled if the output supports it, and white-space wrapping the
output at the console width. Error messages will be written to the standard error stream.

You can customize the output by specifying a pointer to a `usage_writer` instance. See
[usage help](UsageHelp.md) for more information.

The `parse()` method still returns the `parse_result` indicating the result of the operation, but
since errors were already handled, you usually don't need to do anything other than exit the
application if it indicates an error.

**Note:** argument parsing is not atomic. If it fails, some of the variables may already have been
set by arguments supplied before the error occurred.

### Custom error messages

If you wish to customize the error messages shown to the user if parsing fails, for example to
localize them in another language, you can do that using the `localized_string_provider` class.
This class is used as the source for all error messages, as well as a number of other strings used
by Ookii.CommandLine.

Create a class that derives from the `localized_string_provider` class and override its members to
customize any strings you wish to change. You can specify a custom string provider using the
constructor of the `parser_builder` class.

Alternatively, if you need more error information, you can use the manual parsing method below, and
use the `parse_result::error` field to determine the cause of the error and create your own error
message.

## Manual error handling

In most cases, the above method should be sufficient for your needs, but sometimes you may want
more fine-grained control.

In this case, call an overload that doesn't take a `usage_writer` pointer. These overloads do not
handle errors or print help. They return the same `parse_result` class that all overloads do, which
you can use to determine how to handle the error.

The `parse_result` type can be converted to a boolean to easily check if parsing was successful.
If you want more information, check the `parse_result::error` field, which contains an enumeration
indicating the type of error that occurred. If no error occurred, the error will be
`parse_error::none`, which is the only value that converts to `true`.

A special error category is `parse_error::parsing_cancelled`, which isn't necessarily an error, but
indicates that parsing was cancelled by an argument using the
`parser_builder::argument_builder_common::cancel_parsing()` method, an action argument, or the
automatic `-Help` or `-Version` argument.

The the `parse_error::error_arg_name` field indicates the argument that caused the error. Note that
not all categories of error will have this value set. If an error wasn't related to a specific
argument, this field will be blank. The name may also refer to a non-existent argument, if the user
specified an unknown one.

You can call `parse_result::get_error_message()` to get the default error message for the error,
or use the `error` and `error_arg_name` fields to create your own. Note that `parse_error::none`
and `parse_error::parsing_cancelled` don't have an error message and will return an empty string.

If an error occurred, it does not necessarily mean that help must be shown, as an action argument,
including the `-Version` argument, can cancel parsing without requesting help. Therefore, you
should always check the `command_line_parser::help_requested()` method to see if you should show
help. This method will _always_ return `true` for any error other than
`parse_error::parsing_cancelled`, and _always_ return `false` on success.

Here is a small sample of this kind of usage:

```c++
int main(int argc, char *argv[])
{
    auto parser = /* parser_builder code goes here. */;
    auto result = parser.parse(argc, argv);
    if (!result)
    {
        if (result.error != ookii::parse_error::parsing_cancelled)
        {
            auto err = ookii::line_wrapping_ostream::for_cerr();
            err << result.get_error_message() << std::endl << std::endl;
        }

        if (parser.help_requested())
        {
            parser.write_usage();
        }

        return 1;
    }

    // Run the application, using your arguments.
    do_something();
    return 0;
}
```

The `write_usage()` method optionally takes a `usage_writer` parameter to customize the appearance
of the usage help.

Speaking of usage help, let's take [a detailed look at how that works next](UsageHelp.md).
