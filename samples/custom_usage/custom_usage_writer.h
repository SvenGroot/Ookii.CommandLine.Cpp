#ifndef CUSTOM_USAGE_WRITER_H_
#define CUSTOM_USAGE_WRITER_H_

#include <numeric>

#pragma once

class custom_usage_writer : public ookii::usage_writer
{
public:
    // Override some defaults to suit the custom format.
    custom_usage_writer()
    {
        // Only list the positional arguments in the syntax.
        use_abbreviated_syntax = true;

        // Set the indentation to work with the formatting used.
        application_description_indent = 2;
        syntax_indent = 2;

        // Customize some of the colors.
        usage_prefix_color = ookii::vt::text_format::foreground_yellow;
        argument_description_color = ookii::vt::text_format::bold_bright;

        // No blank lines between arguments in the description list.
        blank_line_after_description = false;
    }

protected:
    // Add a header before the description.
    virtual void write_application_description(string_view_type description) override
    {
        output << ookii::set_indent(application_description_indent);
        set_color(usage_prefix_color);
        output << "DESCRIPTION:";
        set_color(color_reset);
        output << std::endl << description << std::endl << std::endl;
    }

    // Use a custom usage prefix with the string "USAGE:" in all caps and on its own line.
    virtual void write_usage_syntax_prefix(string_view_type command_name) override
    {
        set_color(usage_prefix_color);
        output << "USAGE:";
        set_color(color_reset);
        output << std::endl;
        set_color(ookii::vt::text_format::underline);
        output << command_name;
        set_color(color_reset);
    }

    // Add some color to the argument names in the syntax.
    //
    // This doesn't apply to the description list because it uses
    // write_argument_name_for_description.
    virtual void write_argument_name(string_view_type name, string_view_type prefix) override
    {
        // "bright black", aka "gray"...
        set_color(ookii::vt::text_format::bright_foreground_black);
        ookii::usage_writer::write_argument_name(name, prefix);
        set_color(color_reset);
    }

    virtual void write_argument_descriptions() override
    {
        // Calculate the amount of indentation needed based on the longest names, with two spaces
        // before and after. This way the usage dynamically adapts if you change the argument
        // names.
        argument_description_indent = calculate_max_names_length() + 4;
        ookii::usage_writer::write_argument_descriptions();
    }

    // Add a header before the argument description list (normally there is none).
    virtual void write_argument_description_list_header() override
    {
        set_color(usage_prefix_color);
        output << "OPTIONS:";
        set_color(color_reset);
        output << std::endl;
    }

    // Custom format for argument names and aliases.
    virtual void write_argument_description_header(const argument_type &arg) override
    {
        // WriteArgumentDescriptions adjusts the indentation when in long/short mode, which we don't
        // want here, so set it manually.
        output << ookii::set_indent(argument_description_indent) << ookii::reset_indent;
        output << "  ";
        set_color(argument_description_color);

        // Write all the argument's names and aliases, short names and aliases first.
        bool first = true;
        const auto &short_prefix = parser().prefixes()[0];
        if (arg.has_short_name())
        {
            auto short_name = arg.short_name();
            write_argument_name_for_description(string_view_type{&short_name, 1}, short_prefix);
            first = false;
        }

        for (const auto &alias : arg.short_aliases())
        {
            if (!first)
            {
                output << '|';
            }

            write_argument_name_for_description(string_view_type{&alias, 1}, short_prefix);
            first = false;
        }

        if (arg.has_long_name())
        {
            if (!first)
            {
                output << '|';
            }

            write_argument_name_for_description(arg.name(), parser().long_prefix());
            first = false;
        }

        for (const auto &alias : arg.aliases())
        {
            if (!first)
            {
                output << '|';
            }

            write_argument_name_for_description(alias, parser().long_prefix());
            first = false;
        }

        // Unlike the default description format, we just omit the value description entirely
        // if the argument is a switch.
        if (!arg.is_switch())
        {
            output << ' ';
            write_value_description_for_description(arg.value_description());
        }

        set_color(color_reset);

        // Pad until the indentation is reached.
        write_spacing(argument_description_indent - calculate_names_length(arg) - 2);
    }

    // Customize the format of the default values.
    virtual void write_default_value(const argument_type &arg) override
    {
        output << " [default: ";
        arg.write_default_value(output);
        output << "]";
    }

private:
    size_t calculate_max_names_length()
    {
        size_t max = 0;
        for (const auto &arg : parser().arguments())
        {
            auto length = calculate_names_length(arg);
            if (length > max)
            {
                max = length;
            }
        }

        return max;
    }

    size_t calculate_names_length(const argument_type &argument)
    {
        auto short_prefix_length = parser().prefixes()[0].length();
        auto long_prefix_length = parser().long_prefix().length();
        size_t length = 0;
        if (argument.has_short_name())
        {
            // +2 for separator and short name
            length += short_prefix_length + 2;
        }

        // Space for prefix, short name, separator.
        length += argument.short_aliases().size() * (short_prefix_length + 1 + 1);

        if (argument.has_long_name())
        {
            // +1 for separator
            length += argument.name().length() + long_prefix_length + 1;
        }

        // Space for prefix, long name, separator.
        length += std::accumulate(argument.aliases().begin(), argument.aliases().end(), 0,
            [long_prefix_length](size_t left, const auto &right)
            {
                return left + long_prefix_length + right.length() + 1;
            });

        // There is one separator too many
        length -= 1;

        // Length of value description.
        if (!argument.is_switch())
        {
            length += 3 + argument.value_description().length();
        }

        return length;
    }
};

#endif