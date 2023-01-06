# Utility types

Ookii.CommandLine comes with a few utilities that it uses internally, but which may be of use to
anyone writing console applications. These are the `ookii::line_wrapping_ostream` class and virtual
terminal support.

## Line wrapping stream

The `line_wrapping_ostream` class is a `std::ostream` implementation that allows you to write text
to another `std::ostream`, white-space wrapping the text at the specified line length, and
supporting hanging indents.

Ookii.CommandLine uses this class to wrap and indent error messages and usage help when writing to
the console.

The `line_wrapping_ostream` can be created to wrap any `std::ostream` and with any line length, using
its constructor. If you use a line length of less than 1 or greater than 65536, this is
treated as an infinite length, and lines will not be wrapped. The `line_wrapping_ostream` can still
be used to create indented text if you use an unrestricted line length.

When writing to the console, you can pass the `ookii::use_console_width` constant as the line length
to use the console width. You can also manually call the `ookii::get_console_width()` function to
determine the width.

Most of the time, you will probably want to use the `line_wrapping_ostream::for_cout()` or
`line_wrapping_ostream::for_cerr()` methods to create a stream for the standard output or error
streams, automatically wrapping at the console width.

> Both methods, as well as the `ookii::use_console_width` constant, actually use one character less
> than the console width for their maximum line length, because using the width exactly can lead to
> extra blank lines if a line is exactly the width of the console.

Lines will be wrapped at white-space characters only. If a line does not have a suitable place to
wrap, it will be wrapped at the maximum line length regardless.

If you write virtual terminal sequences to a `line_wrapping_ostream`, by default these will not be
included when calculating the length of the current line, so inserting VT sequences, e.g. for
colors, will not affect how the text is wrapped.

### Line wrapping stream buffer

The actual line wrapping functionality is implemented in the `ookii::line_wrapping_streambuf` class,
which writes white-space wrapped output to a different `std::streambuf`. The `line_wrapping_ostream`
is just a convenience class that uses the `line_wrapping_streambuf` with the `std::streambuf` of
another stream. You can, if you need to, use `line_wrapping_streambuf` with any stream.

Another stream that uses the `line_wrapping_streambuf` is provided, the `line_wrapping_ostringstream`
class. This class writes to a `std::ostringstream`, and provides `str()` methods to access the
written text similar to a regular `std::ostringstream`.

### Indentation

The `line_wrapping_ostream` class uses hanging indents, also called negative indents, where all
lines except the first one are indented. The indentation level can be set using the
`ookii::set_indent()` stream manipulator, which indicates the number of spaces to indent by.

When this manipulator is written to a stream, it will apply to the next line that needs to be
indented. The first line of text, and any line after a blank line, are not indented. Indentation is
applied both to lines that were wrapped, and lines created by explicit new lines in the text.

You can use `set_indent` at any time to change the size of the indentation to use.

Additionally, you can use the `ookii::reset_indent` stream manipulator to indicate you do not want
to indent the current line, even if it didn't follow a blank line. This will only apply to the
current line, not any subsequent lines. Note that `ookii::reset_indent` will insert a line break if
the current line is not empty.

For example:

```c++
auto stream = ookii::line_wrapping_ostream::for_cout();
stream << ookii::set_indent(4) << "The first line is not indented. This line is pretty long, so it'll probably be wrapped, and the wrapped portion will be indented." << std::endl;
stream << "A line after an explicit line break is also indented." << std::endl;
stream << std::endl;
stream << "After a blank line, no indentation is used.";
stream << "The next line is indented again.";
stream << ookii::reset_indent << "This line isn't." << std::endl;
stream << "But this one is again." << std::endl;
```

The output of this code would be (assuming an console width of 80 characters):

```text
The first line is not indented. This line is pretty long, so it'll probably be
    wrapped, and the wrapped portion will be indented.
    A line after an explicit line break is also indented.

After a blank line, no indentation is used.The next line is indented again.
This line isn't.
    But this one is again.
```

The `set_indent` and `reset_indent` stream manipulators will work with any stream that uses a
`line_wrapping_streambuf`. If they are used with a stream that uses a different `std::streambuf`,
they have no effect, so you can safely write code that uses them that is agnostic of the type of
stream used.

The  manipulators rely on `dynamic_cast` to determine the type of `std::streambuf` used by a stream,
so will require RTTI to be enabled.

## Virtual terminal support

Virtual terminal (VT) sequences are a method to manipulate the console utilized, supported by many
console applications on many operating systems. It is supported by the console host on recent
versions of Windows, by [Windows Terminal](https://learn.microsoft.com/windows/terminal/install),
and many console applications on other platforms.

A VT sequence consists of an escape character, followed by a string that specifies what action to
take. They can be used to set colors and other formatting options, but also to do things like move
the cursor.

Ookii.CommandLine uses VT sequences to add color to the usage help and error messages. To let you
customize the colors used by the `usage_writer`, and to use color in your own console
applications, a few types are provided in the `ookii::vt` namespace.

The `virtual_terminal_support` class allows you to determine whether virtual terminal sequences are
supported, and to enable them. The `usage_writer` class uses this internally to enable color output
when possible.

The `ookii::vt::text_format` namespace provides a number of constants for the predefined background
and foreground colors and formats supported by the console, as well as a method to create a VT
sequence for any 24-bit color. These can be used to change the default usage help colors, or to
apply color to your own text.

For example, you can use the following to write in color when supported:

```c++
auto support = ookii::vt::virtual_terminal_support::enable_color(ookii::standard_stream::output);
if (support)
{
    std::cout << ookii::vt::text_format::foreground_green << ookii::vt::text_format::underline;
}

std::cout << "This text is green and underlined.";

if (support)
{
    std::cout << ookii::vt::text_format::default_format;
}

std::cout << std::endl;
```

On Windows, VT support must be enabled for a process. In addition to checking for support, the
`virtual_terminal_support::enable()` and `virtual_terminal_support::enable_color()` methods also
enable it if necessary, and the returned object will revert the console mode when destructed. On
other platforms, it only checks for support and destructing the returned instance does nothing.
