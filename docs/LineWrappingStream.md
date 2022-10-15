# Line wrapping stream

The Ookii.CommandLine library includes a utility class, the `ookii::basic_line_wrapping_ostream`
template and its typedefs `ookii::line_wrapping_ostream` and `ookii::wline_wrapping_ostream`,
which it uses for white-space wrapping usage help and error messages when writing to the console.
This class can also be used in your own code.

The `line_wrapping_ostream` class allows you to write text to another `ostream`, including file
or string streams, wrapping that text at the specified line length, and optionally indenting
subsequent lines.

You can create a `line_wrapping_ostream` for any stream or line length, but for console output you
can use the `line_wrapping_ostream::for_cout()` and `line_wrapping_ostream::for_cerr()` methods,
which return a stream wrapping the standard output and error streams respectively, with their line
lengths set to the console width.

## Controlling indentation

To control the indentation level, use the following IO manipulators:

- `set_indent`: Starting with the next line, each line will be indented with the specified number
  of spaces.
- `reset_indent`: Indicates the next line should not be indented. If the last character written was
  not a new line, one is inserted. Subsequent lines will be indented again.

For example:

```c++
auto stream = ookii::line_wrapping_ostream::for_cout();
stream << ookii::set_indent(4) << "This line is not indented, but it's pretty long so it's going to be wrapped and the overflow will be." << std::endl;
stream << "This line is indented." << std::endl;
stream << ookii::reset_indent << "This line isn't." << std::endl;
stream << "But this one is again." << std::endl;
```

The output of this code would be (assuming an console width of 80 characters):

```text
This line is not indented, but it's pretty long so it's going to be wrapped and
    the overflow will be.
    This line is indented.
This line isn't.
    But this one is again.
```

## Line wrapping stream buffer

The actual line wrapping functionality is implemented in the `ookii::line_wrapping_streambuf` class,
which writes white-space wrapped output to a different `streambuf`. The `line_wrapping_ostream` is
just a convenience class that uses the `line_wrapping_streambuf` with the `streambuf` of another
stream. You can, if you need to, use `line_wrapping_streambuf` with any stream.

The `set_indent` and `reset_indent` IO manipulators will work with any stream that uses a
`line_wrapping_streambuf`. If they are used with a stream that uses a different `streambuf`, they
have no effect, so you can safely write code that uses them that is agnostic of the type of stream
used.

The `set_indent` and `reset_indent` IO manipulators rely on `dynamic_cast` to determine the type of
`streambuf` used by a stream, so will require RTTI to be enabled.
