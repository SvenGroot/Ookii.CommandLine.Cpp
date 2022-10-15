//! \file shell_command.h
//! \brief Provides functionality for applications that have multiple subcommands, all with
//!        their own arguments.
#ifndef OOKII_SHELL_COMMAND_H_
#define OOKII_SHELL_COMMAND_H_

#pragma once

#include "command_line_builder.h"
#include "shell_command_usage_options.h"

namespace ookii
{
    //! \brief Abstract base class for all shell commands.
    //! 
    //! When you implement a shell command, you must derive from the basic_shell_command class, and
    //! implement the run() method.
    //! 
    //! In addition, you must provide a constructor that takes a reference to builder_type, which
    //! is a typedef for basic_parser_builder, which creates the arguments accepted by this command.
    //! This basic_parser_builder will have been initialized with the name and description of the
    //! command, as well as basic_parser_builder::case_sensitive() and basic_parser_builder::locale()
    //! values matching the basic_shell_command_manager.
    //! 
    //! To specify a name or description for your command, you can either pass them to
    //! basic_shell_command_manager::add_command(), or you can provide static methods that return them.
    //! If a name is not provided using either method, the name will match the type name of the
    //! shell command class.
    //! 
    //! For example:
    //! 
    //! ```
    //! class some_command : public std::shell_command
    //! {
    //! public:
    //!     some_command(builder_type &builder)
    //!     {
    //!         // ...
    //!     }
    //! 
    //!     virtual int run() override
    //!     {
    //!         // ...
    //!     }
    //!
    //!     static std::string name()
    //!     {
    //!         return "some_name";
    //!     }
    //!
    //!     static std::string description()
    //!     {
    //!         return "The description.";
    //!     }
    //! };
    //! ```
    //! 
    //! Several typedefs for common character types are provided:
    //! 
    //! Type                    | Definition
    //! ----------------------- | -------------------------------------
    //! `ookii::shell_command`  | `ookii::basic_shell_command<char>`
    //! `ookii::wshell_command` | `ookii::basic_shell_command<wchar_t>`
    //! 
    //! \tparam CharType The character type used for arguments and other strings.
    //! \tparam Traits The character traits to use for strings. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use for strings. Defaults to `std::allocator<CharType>`.
    template<typename CharType = details::default_char_type, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class basic_shell_command
    {
    public:
        //! \brief The concrete type of basic_parser_builder used.
        using builder_type = basic_parser_builder<CharType, Traits, Alloc>;

        //! \brief Initializes a new instance of the basic_shell_command class.
        basic_shell_command() = default;

        //! \brief Initializes a new instance of the basic_shell_command class.
        //!
        //! \attention You don't need to call this constructor when implementing a derived class,
        //!            as it does nothing. It exists so `New-ShellCommand.ps1` can generate code
        //!            that supports shell commands with a base class that does have a constructor.
        basic_shell_command(builder_type &) {}

        //! \brief Default destructor.
        virtual ~basic_shell_command() = default;

        //! \brief Runs the command, after argument parsing was successful.
        //! \return The exit code for the command. Typically, this code will be returned from the
        //!         application to the OS.
        virtual int run() = 0;
    };

    //! \brief Typedef for basic_shell_command using `char` as the character type.
    using shell_command = basic_shell_command<char>;
    //! \brief Typedef for basic_shell_command using `wchar_t` as the character type.
    using wshell_command = basic_shell_command<wchar_t>;

    //! \brief Provides information about a shell command.
    //! 
    //! \tparam CharType The character type used for arguments and other strings.
    //! \tparam Traits The character traits to use for strings. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use for strings. Defaults to `std::allocator<CharType>`.
    template<typename CharType, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class shell_command_info
    {
    public:
        //! \brief The concrete type of basic_shell_command used.
        using command_type = basic_shell_command<CharType, Traits, Alloc>;
        //! \brief The concrete type of basic_parser_builder used.
        using builder_type = typename command_type::builder_type;
        //! \brief The concrete string type used.
        using string_type = std::basic_string<CharType, Traits, Alloc>;

    private:
        using creator = std::function<std::unique_ptr<command_type>(builder_type &)>;

    public:

        //! \brief Creates a shell_command_info instance for the specified type.
        //! 
        //! \tparam T The type of the shell command, which must derive from basic_shell_command.
        //! \param name The name of the shell command.
        //! \param description The description of the shell command.
        template<typename T>
        static shell_command_info create(string_type name, string_type description)
        {
            auto creator = [](builder_type &builder) -> std::unique_ptr<command_type>
            {
                return std::make_unique<T>(builder);
            };

            return {name, description, creator};
        }

        //! \brief Creates an instance of the shell command type.
        //! 
        //! \param builder The basic_parser_builder to pass to the shell command type's constructor.
        std::unique_ptr<command_type> create(builder_type &builder) const
        {
            return _creator(builder);
        }

        //! \brief Gets the name of the shell command.
        const string_type &name() const noexcept
        {
            return _name;
        }

        //! \brief Gets the description of the shell command.
        const string_type &description() const noexcept
        {
            return _description;
        }

    private:
        shell_command_info(string_type name, string_type description, creator creator)
            : _name{name},
              _description{description},
              _creator{creator}
        {
        }

        string_type _name;
        string_type _description;
        creator _creator;
    };

    //! \brief Manages registration, creation and invocation of shell commands for an application.
    //! 
    //! To use shell commands in your application, you define a class that derives from
    //! basic_shell_command for each one. Then, you register each class with the basic_shell_command_manager
    //! using the add_command() method.
    //! 
    //! You can then invoke create_command() to create an instance of a command type based on the
    //! provided arguments, or run_command() to create a command and immediately run it.
    //! 
    //! Several typedefs for common character types are provided:
    //! 
    //! Type                            | Definition
    //! ------------------------------- | -------------------------------------
    //! `ookii::shell_command_manager`  | `ookii::basic_shell_command_manager<char>`
    //! `ookii::wshell_command_manager` | `ookii::basic_shell_command_manager<wchar_t>`
    //! 
    //! \tparam CharType The character type used for arguments and other strings. Only `char` and
    //!         `wchar_t` are supported. Defaults to `wchar_t` if `_UNICODE` is defined, otherwise
    //!         to `char`.
    //! \tparam Traits The character traits to use for strings. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use for strings. Defaults to `std::allocator<CharType>`.
    template<typename CharType = details::default_char_type, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class basic_shell_command_manager
    {
    public:
        //! \brief The concrete type of shell_command_info used.
        using info_type = shell_command_info<CharType, Traits, Alloc>;
        //! \brief The concrete string type used.
        using string_type = typename info_type::string_type;
        //! \brief The concrete type of basic_parser_builder used.
        using builder_type = typename info_type::builder_type;
        //! \brief The concrete type of basic_shell_command used.
        using command_type = typename info_type::command_type;
        //! \brief The concrete type of basic_shell_command_usage_options used.
        using usage_options_type = basic_shell_command_usage_options<CharType, Traits, Alloc>;
        //! \brief The concrete type of output stream used.
        using stream_type = std::basic_ostream<CharType, Traits>;

        //! \brief The error exit code used by run_shell_command() if no command name was supplied
        //!        or thesupplied command could not be found.
        static constexpr int error_return_code = 1;

        //! \brief Initializes a new instance of the basic_shell_command_manager class.
        //! 
        //! \param application_name The name of the application containing the command. This name
        //!        is used when printing usage help.
        //! \param case_sensitive Indicates whether command names and argument names are case
        //!        sensitive. The default is false.
        //! \param locale The locale to use when converting argument values. The default is a copy
        //!        of the current global locale.
        basic_shell_command_manager(string_type application_name, bool case_sensitive = false, const std::locale &locale = {})
            : _commands{string_less{case_sensitive, locale}},
              _application_name{application_name},
              _locale{locale},
              _case_sensitive{case_sensitive}
        {
        }

        //! \brief Adds a command to the basic_shell_command_manager.
        //! 
        //! \tparam The type of the command, which must derive from basic_shell_command<CharType>.
        //! \param name The name used to invoke the command. If left blank, it will be determined
        //!        by the static name() method of the command type, or the type name if there is no
        //!        such method.
        //! \param description The description of the command, used for usage help. If left blank,
        //!        it will be determined by the static description() method of the command type. If
        //!        no such method exist, the description will be blank.
        template<typename T>
        basic_shell_command_manager &add_command(string_type name = {}, string_type description = {})
        {
            if (name.empty())
                name = name_helper<T>::name();

            if (description.empty())
                description = description_helper<T>::description();

            auto [it, success] = _commands.emplace(name, info_type::template create<T>(name, description));
            if (!success)
                throw std::logic_error("Duplicate command name");

            return *this;
        }

        //! \brief Gets a view of all the commands.
        //! 
        //! \warning Calling this function may cause build errors on certain versions of clang.
        auto commands() const noexcept
        {
            return _commands | std::views::values;
        }

        //! \brief Gets information about a shell command by name.
        //! \param name The name of the shell command.
        //! \return An instance of shell_command_info describing the command, or `nullptr` if there
        //!         is no command with that name.
        const info_type *get_command(const string_type &name) const
        {
            auto it = _commands.find(name);
            if (it == _commands.end())
                return nullptr;

            return &it->second;
        }

        //! \brief Creates an instance of the specified command, parsing the specified arguments.
        //! 
        //! If the command could not be found, a list of commands will be written. If an error
        //! occurred parsing the arguments, an error message and usage help for the command will
        //! be written.
        //! 
        //! \warning The passed arguments must not include the application name or the command name.
        //! 
        //! \tparam Iterator The type of iterator for the arguments.
        //! \param name The name of the shell command.
        //! \param begin An iterator to the first argument.
        //! \param end An iterator after the last argument.
        //! \param options A basic_shell_command_usage_options instance that will be used to format errors
        //!        and usage help.
        //! \returns An instance of the shell command type, or `nullptr` if the an error occurred.
        template<typename Iterator>
        std::unique_ptr<command_type> create_command(const string_type &name, Iterator begin, Iterator end, const usage_options_type &options = {}) const
        {
            auto info = get_command(name);
            if (info == nullptr)
            {
                write_usage(options);
                return {};
            }

            auto builder = create_parser_builder(*info);
            auto command = info->create(builder);
            auto parser = builder.build();
            if (!parser.parse(begin, end, options))
                return {};

            return command;
        }

        //! \brief Creates an instance of the specified command, parsing the specified arguments.
        //! 
        //! If the command could not be found, a list of commands will be written. If an error
        //! occurred parsing the arguments, an error message and usage help for the command will
        //! be written.
        //! 
        //! \warning The passed arguments must not include the application name or the command name.
        //! 
        //! \tparam Range The type of a range containing the arguments.
        //! \param name The name of the shell command.
        //! \param range A range containing the arguments.
        //! \param options A basic_shell_command_usage_options instance that will be used to format errors
        //!        and usage help.
        //! \returns An instance of the shell command type, or `nullptr` if the an error occurred.
        template<typename Range>
        std::unique_ptr<command_type> create_command(const string_type &name, Range range, const usage_options_type &options = {}) const
        {
            // For ADL
            using std::begin;
            using std::end;
            return create_command(name, begin(range), end(range), options);
        }

        //! \brief Creates an instance of the specified command, parsing the specified arguments.
        //! 
        //! If the command could not be found, a list of commands will be written. If an error
        //! occurred parsing the arguments, an error message and usage help for the command will
        //! be written.
        //! 
        //! \warning The passed arguments must not include the application name or the command name.
        //! 
        //! \tparam T The type of the elements in the initializer list. This must be a string type
        //!         that can be converted to std::basic_string<CharType, Traits, Alloc>.
        //! \param name The name of the shell command.
        //! \param args An initializer list containing the arguments.
        //! \param options A basic_shell_command_usage_options instance that will be used to format errors
        //!        and usage help.
        //! \returns An instance of the shell command type, or `nullptr` if the an error occurred.
        template<typename T>
        std::unique_ptr<command_type> create_command(const string_type &name, std::initializer_list<T> args, const usage_options_type &options = {}) const
        {
            return create_command(name, args.begin(), args.end(), options);
        }

        //! \brief Creates an instance of a command based on the specified arguments.
        //! 
        //! If no command was specified or the command could not be found, a list of commands will
        //! be written. If an error occurred parsing the arguments, an error message and usage help
        //! for the command will be written.
        //! 
        //! \warning The passed arguments must not include the application name, but the first
        //!          argument must be the command name.
        //! 
        //! \tparam Iterator The type of iterator for the arguments.
        //! \param begin An iterator to the first argument.
        //! \param end An iterator after the last argument.
        //! \param options A basic_shell_command_usage_options instance that will be used to format errors
        //!        and usage help.
        //! \returns An instance of the shell command type, or `nullptr` if the an error occurred.
        template<typename Iterator>
        std::unique_ptr<command_type> create_command(Iterator begin, Iterator end, const usage_options_type &options = {}) const
        {
            if (begin == end)
            {
                write_usage(options);
                return {};
            }

            auto &name = *begin;
            return create_command(name, ++begin, end, options);
        }

        //! \brief Creates an instance of a command based on the specified arguments.
        //! 
        //! If no command was specified or the command could not be found, a list of commands will
        //! be written. If an error occurred parsing the arguments, an error message and usage help
        //! for the command will be written.
        //! 
        //! \warning The passed arguments must not include the application name, but the first
        //!          argument must be the command name.
        //! 
        //! \tparam Range The type of a range containing the arguments.
        //! \param range A range containing the arguments.
        //! \param options A basic_shell_command_usage_options instance that will be used to format errors
        //!        and usage help.
        //! \returns An instance of the shell command type, or `nullptr` if the an error occurred.
        template<typename Range>
        std::unique_ptr<command_type> create_command(Range range, const usage_options_type &options = {}) const
        {
            // For ADL
            using std::begin;
            using std::end;
            return create_command(begin(range), end(range), options);
        }

        //! \brief Creates an instance of a command based on the specified arguments.
        //! 
        //! If no command was specified or the command could not be found, a list of commands will
        //! be written. If an error occurred parsing the arguments, an error message and usage help
        //! for the command will be written.
        //! 
        //! \warning The passed arguments must not include the application name, but the first
        //!          argument must be the command name.
        //! 
        //! \tparam T The type of the elements in the initializer list. This must be a string type
        //!         that can be converted to std::basic_string<CharType, Traits, Alloc>.
        //! \param args An initializer list containing the arguments.
        //! \param options A basic_shell_command_usage_options instance that will be used to format errors
        //!        and usage help.
        //! \returns An instance of the shell command type, or `nullptr` if the an error occurred.
        template<typename T>
        std::unique_ptr<command_type> create_command(std::initializer_list<T> args, const usage_options_type &options = {}) const
        {
            return create_command(args.begin(), args.end(), options);
        }

        //! \brief Creates an instance of a command based on the specified arguments.
        //! 
        //! If no command was specified or the command could not be found, a list of commands will
        //! be written. If an error occurred parsing the arguments, an error message and usage help
        //! for the command will be written.
        //! 
        //! \warning The first argument is assumed to be the application executable name, and is
        //!          skipped. The second argument must be the command name.
        //! 
        //! \param argc The number of arguments.
        //! \param argv The arguments..
        //! \param options A basic_shell_command_usage_options instance that will be used to format errors
        //!        and usage help.
        //! \returns An instance of the shell command type, or `nullptr` if the an error occurred.
        std::unique_ptr<command_type> create_command(int argc, const CharType *const argv[], const usage_options_type &options = {}) const
        {
            if (argc < 2)
            {
                write_usage(options);
                return {};
            }

            return create_command(argv + 1, argv + argc, options);
        }

        //! \brief Creates an instance of the specified command, parsing the specified arguments,
        //!        and runs the command.
        //! 
        //! If the command could not be found, a list of commands will be written. If an error
        //! occurred parsing the arguments, an error message and usage help for the command will
        //! be written.
        //! 
        //! \warning The passed arguments must not include the application name or the command name.
        //! 
        //! \tparam Iterator The type of iterator for the arguments.
        //! \param name The name of the shell command.
        //! \param begin An iterator to the first argument.
        //! \param end An iterator after the last argument.
        //! \param options A basic_shell_command_usage_options instance that will be used to format errors
        //!        and usage help.
        //! \returns The exit code of the command, or error_return_code if the command could not
        //!          be created.
        template<typename Iterator>
        int run_command(const string_type &name, Iterator begin, Iterator end, const usage_options_type &options = {}) const
        {
            auto command = create_command(name, begin, end, options);
            if (!command)
                return error_return_code;

            return command->run();
        }

        //! \brief Creates an instance of the specified command, parsing the specified arguments,
        //!        and runs the command.
        //! 
        //! If the command could not be found, a list of commands will be written. If an error
        //! occurred parsing the arguments, an error message and usage help for the command will
        //! be written.
        //! 
        //! \warning The passed arguments must not include the application name or the command name.
        //! 
        //! \tparam Range The type of a range containing the arguments.
        //! \param name The name of the shell command.
        //! \param range A range containing the arguments.
        //! \param options A basic_shell_command_usage_options instance that will be used to format errors
        //!        and usage help.
        //! \returns The exit code of the command, or error_return_code if the command could not
        //!          be created.
        template<typename Range>
        int run_command(const string_type &name, Range range, const usage_options_type &options = {}) const
        {
            // For ADL
            using std::begin;
            using std::end;
            return run_command(name, begin(range), end(range), options);
        }

        //! \brief Creates an instance of the specified command, parsing the specified arguments,
        //!        and runs the command.
        //! 
        //! If the command could not be found, a list of commands will be written. If an error
        //! occurred parsing the arguments, an error message and usage help for the command will
        //! be written.
        //! 
        //! \warning The passed arguments must not include the application name or the command name.
        //! 
        //! \tparam T The type of the elements in the initializer list. This must be a string type
        //!         that can be converted to std::basic_string<CharType, Traits, Alloc>.
        //! \param name The name of the shell command.
        //! \param args An initializer list containing the arguments.
        //! \param options A basic_shell_command_usage_options instance that will be used to format errors
        //!        and usage help.
        //! \returns The exit code of the command, or error_return_code if the command could not
        //!          be created.
        template<typename T>
        int run_command(const string_type &name, std::initializer_list<T> args, const usage_options_type &options = {}) const
        {
            return run_command(name, args.begin(), args.end(), options);
        }

        //! \brief Creates an instance of a command based onthe specified arguments, and runs the
        //!        command.
        //! 
        //! If no command was specified or the command could not be found, a list of commands will
        //! be written. If an error occurred parsing the arguments, an error message and usage help
        //! for the command will be written.
        //! 
        //! \warning The passed arguments must not include the application name, but the first
        //!          argument must be the command name.
        //! 
        //! \tparam Iterator The type of iterator for the arguments.
        //! \param begin An iterator to the first argument.
        //! \param end An iterator after the last argument.
        //! \param options A basic_shell_command_usage_options instance that will be used to format errors
        //!        and usage help.
        //! \returns The exit code of the command, or error_return_code if the command could not
        //!          be created.
        template<typename Iterator>
        int run_command(Iterator begin, Iterator end, const usage_options_type &options = {}) const
        {
            auto command = create_command(begin, end, options);
            if (!command)
                return error_return_code;

            return command->run();
        }

        //! \brief Creates an instance of a command based onthe specified arguments, and runs the
        //!        command.
        //! 
        //! If no command was specified or the command could not be found, a list of commands will
        //! be written. If an error occurred parsing the arguments, an error message and usage help
        //! for the command will be written.
        //! 
        //! \warning The passed arguments must not include the application name, but the first
        //!          argument must be the command name.
        //! 
        //! \tparam Range The type of a range containing the arguments.
        //! \param range A range containing the arguments.
        //! \param options A basic_shell_command_usage_options instance that will be used to format errors
        //!        and usage help.
        //! \returns The exit code of the command, or error_return_code if the command could not
        //!          be created.
        template<typename Range>
        int run_command(Range range, const usage_options_type &options = {}) const
        {
            // For ADL
            using std::begin;
            using std::end;
            return run_command(begin(range), end(range), options);
        }

        //! \brief Creates an instance of a command based onthe specified arguments, and runs the
        //!        command.
        //! 
        //! If no command was specified or the command could not be found, a list of commands will
        //! be written. If an error occurred parsing the arguments, an error message and usage help
        //! for the command will be written.
        //! 
        //! \warning The passed arguments must not include the application name, but the first
        //!          argument must be the command name.
        //! 
        //! \tparam T The type of the elements in the initializer list. This must be a string type
        //!         that can be converted to std::basic_string<CharType, Traits, Alloc>.
        //! \param args An initializer list containing the arguments.
        //! \param options A basic_shell_command_usage_options instance that will be used to format errors
        //!        and usage help.
        //! \returns The exit code of the command, or error_return_code if the command could not
        //!          be created.
        template<typename T>
        int run_command(std::initializer_list<T> args, const usage_options_type &options = {}) const
        {
            return run_command(args.begin(), args.end(), options);
        }

        //! \brief Creates an instance of a command based onthe specified arguments, and runs the
        //!        command.
        //! 
        //! If no command was specified or the command could not be found, a list of commands will
        //! be written. If an error occurred parsing the arguments, an error message and usage help
        //! for the command will be written.
        //! 
        //! \warning The first argument is assumed to be the application executable name, and is
        //!          skipped. The second argument must be the command name.
        //! 
        //! \param argc The number of arguments.
        //! \param argv The arguments..
        //! \param options A basic_shell_command_usage_options instance that will be used to format errors
        //!        and usage help.
        //! \returns The exit code of the command, or error_return_code if the command could not
        //!          be created.
        int run_command(int argc, const CharType *const argv[], const usage_options_type &options = {}) const
        {
            auto command = create_command(argc, argv, options);
            if (!command)
                return error_return_code;

            return command->run();
        }

        //! \brief Writes usage help about the available commands.
        //! \param options A basic_shell_command_usage_options instance that will be used to format the
        //!        usage help.
        void write_usage(const usage_options_type &options = {}) const
        {
            auto &stream = options.output;
            auto usage = format::ncformat(_locale, options.command_usage_format, _application_name);
            usage = format::ncformat(_locale, options.usage_prefix_format, usage);
            stream << usage << std::endl << std::endl;
            stream << options.available_commands_header << std::endl << std::endl;
            stream << set_indent(options.command_indent);
            for (const auto &command : _commands)
            {
                stream << reset_indent;
                stream << format::ncformat(_locale, options.command_format, command.second.name(), command.second.description()) << std::endl;
            }
        }

        //! \brief Creates a basic_parser_builder for a specified command.
        //! 
        //! The parser builder will be initialized with the name and description of the command,
        //! as well the locale and case sensitive value of the basic_shell_command_manager.
        //! 
        //! \param command The shell_command_info for the command.
        //! \return A parser builder with suitable defaults for the command.
        builder_type create_parser_builder(const info_type &command) const
        {
            // Include the application name so usage will be correct.
            string_type full_name = _application_name + static_cast<CharType>(' ') + command.name();
            builder_type builder{full_name};
            builder.locale(_locale)
                .case_sensitive(_case_sensitive)
                .description(command.description());

            return builder;
        }

    private:
        template<typename T, typename = void>
        struct name_helper
        {
            static string_type name()
            {
                return get_short_type_name<T, CharType, Traits, Alloc>();
            }
        };

        template<typename T>
        struct name_helper<T, std::void_t<decltype(T::name())>>
        {
            static string_type name()
            {
                return T::name();
            }
        };

        template<typename T, typename = void>
        struct description_helper
        {
            static string_type description()
            {
                return {};
            }
        };

        template<typename T>
        struct description_helper<T, std::void_t<decltype(T::description())>>
        {
            static string_type description()
            {
                return T::description();
            }
        };

        std::map<string_type, info_type, string_less> _commands;
        string_type _application_name;
        std::locale _locale;
        bool _case_sensitive;
    };

    //! \brief Typedef for basic_shell_command_manager using `char` as the character type.
    using shell_command_manager = basic_shell_command_manager<char>;
    //! \brief Typedef for basic_shell_command_manager using `wchar_t` as the character type.
    using wshell_command_manager = basic_shell_command_manager<wchar_t>;

}

#endif