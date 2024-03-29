//! \file subcommand.h
//! \brief Provides functionality for applications that have multiple subcommands, all with
//!        their own arguments.
#ifndef OOKII_SUBCOMMAND_H_
#define OOKII_SUBCOMMAND_H_

#pragma once

#include "command_line_builder.h"
#include <span>

namespace ookii
{
    //! \brief Abstract base class for all subcommands.
    //! \tparam CharType The character type used for arguments and other strings.
    //! \tparam Traits The character traits to use for strings. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use for strings. Defaults to `std::allocator<CharType>`.
    //! 
    //! When you implement a subcommand, you must derive from the basic_command class, and
    //! implement the run() method.
    //! 
    //! In addition, you must provide a constructor that takes a reference to builder_type, which
    //! is a typedef for basic_parser_builder, which creates the arguments accepted by this command.
    //! This basic_parser_builder will have been initialized with the name and description of the
    //! command, as well as basic_parser_builder::case_sensitive() and basic_parser_builder::locale()
    //! values matching the basic_command_manager.
    //! 
    //! To specify a name or description for your command, you can either pass them to
    //! basic_command_manager::add_command(), or you can provide static methods that return them.
    //! If a name is not provided using either method, the name will match the type name of the
    //! subcommand class.
    //! 
    //! For example:
    //! 
    //! ```
    //! class some_command : public std::command
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
    //! Type              | Definition
    //! ----------------- | -------------------------------
    //! `ookii::command`  | `ookii::basic_command<char>`
    //! `ookii::wcommand` | `ookii::basic_command<wchar_t>`
    template<typename CharType, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class basic_command
    {
    public:
        //! \brief The concrete type of basic_parser_builder used.
        using builder_type = basic_parser_builder<CharType, Traits, Alloc>;

        //! \brief Initializes a new instance of the basic_command class.
        basic_command() = default;

        //! \brief Initializes a new instance of the basic_command class.
        //!
        //! \attention You don't need to call this constructor when implementing a derived class,
        //!            as it does nothing. It exists so `New-Subcommand.ps1` can generate code
        //!            that supports subcommands with a base class that does have a constructor.
        basic_command(builder_type &) {}

        //! \brief Default destructor.
        virtual ~basic_command() = default;

        //! \brief Runs the command, after argument parsing was successful.
        //! \return The exit code for the command. Typically, this code will be returned from the
        //!         application to the OS.
        virtual int run() = 0;
    };

    //! \brief Typedef for basic_command using `char` as the character type.
    using command = basic_command<char>;
    //! \brief Typedef for basic_command using `wchar_t` as the character type.
    using wcommand = basic_command<wchar_t>;

    //! \brief Abstract base class for subcommands that do their own argument parsing.
    //! \tparam CharType The character type used for arguments and other strings.
    //! \tparam Traits The character traits to use for strings. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use for strings. Defaults to `std::allocator<CharType>`.
    //!
    //! Unlike commands that derive directly from the basic_command class, commands that derive from
    //! the basic_command_with_custom_parsing class are not created with the
    //! basic_command_line_parser class. Instead, they must have a public constructor with no
    //! parameters, and must parse the arguments manually by implementing the parse() method.
    //!
    //! Several typedefs for common character types are provided:
    //! 
    //! Type                                  | Definition
    //! ------------------------------------- | ---------------------------------------------------
    //! `ookii::command_with_custom_parsing`  | `ookii::basic_command_with_custom_parsing<char>`
    //! `ookii::wcommand_with_custom_parsing` | `ookii::basic_command_with_custom_parsing<wchar_t>`
    template<typename CharType, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class basic_command_with_custom_parsing : public basic_command<CharType, Traits, Alloc>
    {
    public:
        //! \brief The concrete type of basic_command_manager used.
        using command_manager_type = basic_command_manager<CharType, Traits, Alloc>;
        //! \brief The concrete type of basic_usage_writer used.
        using usage_writer_type = basic_usage_writer<CharType, Traits, Alloc>;

        //! \brief Parses the arguments for the command.
        //! \param args A span containing the arguments for the command.
        //! \param manager The command manager this command was created with.
        //! \param usage The usage writer that was passed to basic_command_manager::create_command().
        //! \return `true` if parsing was successful; otherwise, `false`.
        virtual bool parse(std::span<const CharType *const> args, const command_manager_type &manager, usage_writer_type *usage) = 0;
    };

    //! \brief Typedef for basic_command_with_custom_parsing using `char` as the character type.
    using command_with_custom_parsing = basic_command_with_custom_parsing<char>;
    //! \brief Typedef for basic_command_with_custom_parsing using `wchar_t` as the character type.
    using wcommand_with_custom_parsing = basic_command_with_custom_parsing<wchar_t>;

    //! \brief Provides information about a subcommand.
    //! 
    //! \tparam CharType The character type used for arguments and other strings.
    //! \tparam Traits The character traits to use for strings. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use for strings. Defaults to `std::allocator<CharType>`.
    template<typename CharType, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class command_info
    {
    public:
        //! \brief The concrete type of basic_command used.
        using command_type = basic_command<CharType, Traits, Alloc>;
        //! \brief The concrete type of basic_command_with_custom_parsing used.
        using command_with_custom_parsing_type = basic_command_with_custom_parsing<CharType, Traits, Alloc>;
        //! \brief The concrete type of basic_parser_builder used.
        using builder_type = typename command_type::builder_type;
        //! \brief The concrete string type used.
        using string_type = std::basic_string<CharType, Traits, Alloc>;
        //! \brief The type of a function that instantiates a subcommand.
        using creator = std::function<std::unique_ptr<command_type>(builder_type *)>;

    public:

        //! \brief Initializes a new instance of the command_info class.
        //! \param name The name of the subcommand.
        //! \param description The description of the subcommand.
        //! \param creator A function that instantiates the subcommand.
        //! \param use_custom_argument_parsing Indicates whether this command uses
        //!        basic_command_with_custom_parsing as a base type.
        command_info(string_type name, string_type description, creator creator, bool use_custom_argument_parsing = false)
            : _name{name},
              _description{description},
              _creator{creator},
              _use_custom_argument_parsing{use_custom_argument_parsing}
        {
        }

        //! \brief Creates a command_info instance for the specified command type.
        //! 
        //! \tparam T The type of the subcommand, which must derive from basic_command.
        //! \param name The name of the subcommand.
        //! \param description The description of the subcommand.
        template<typename T, std::enable_if_t<!std::is_base_of_v<command_with_custom_parsing_type, T>, int> = 0>
        static command_info create(string_type name, string_type description)
        {
            auto creator = [](builder_type *builder) -> std::unique_ptr<command_type>
            {
                return std::make_unique<T>(*builder);
            };

            return {name, description, creator};
        }

        //! \brief Creates a command_info instance for a command type that uses custom argument
        //!        parsing.
        //! 
        //! \tparam T The type of the subcommand, which must derive from basic_command.
        //! \param name The name of the subcommand.
        //! \param description The description of the subcommand.
        template<typename T, std::enable_if_t<std::is_base_of_v<command_with_custom_parsing_type, T>, int> = 0>
        static command_info create(string_type name, string_type description)
        {
            auto creator = [](builder_type *) -> std::unique_ptr<command_type>
            {
                return std::make_unique<T>();
            };

            return {name, description, creator, true};
        }

        //! \brief Creates an instance of the subcommand type.
        //! 
        //! \param builder The basic_parser_builder to pass to the subcommand type's constructor.
        //! \return An instance of the subcommand type.
        //!
        //! This function returns `nullptr` if this command uses custom argument parsing.
        std::unique_ptr<command_type> create(builder_type &builder) const
        {
            if (_use_custom_argument_parsing)
            {
                return {};
            }

            return _creator(&builder);
        }

        //! \brief Creates an instance of the subcommand type for commands that use custom argument
        //! parsing.
        //! \return An instance of the subcommand type. This can safely be cast to
        //! the basic_command_with_custom_parsing class.
        //!
        //! This function returns `nullptr` if this command does not use custom argument parsing.
        std::unique_ptr<command_type> create_custom_parsing() const
        {
            if (!_use_custom_argument_parsing)
            {
                return {};
            }

            return _creator(nullptr);
        }

        //! \brief Gets a value that indicates whether the command uses custom argument parsing.
        //! \return `true` if the command type derives from the basic_command_with_custom_parsing
        //! class; otherwise, `false`.
        bool use_custom_argument_parsing() const noexcept
        {
            return _use_custom_argument_parsing;
        }

        //! \brief Gets the name of the subcommand.
        const string_type &name() const noexcept
        {
            return _name;
        }

        //! \brief Gets the description of the subcommand.
        const string_type &description() const noexcept
        {
            return _description;
        }

    private:
        string_type _name;
        string_type _description;
        creator _creator;
        bool _use_custom_argument_parsing{};
    };

    namespace details
    {
        template<typename CharType, typename Traits, typename Alloc>
        class version_command : public basic_command<CharType, Traits, Alloc>
        {
        public:
            using base_type = basic_command<CharType, Traits, Alloc>;
            using builder_type = typename base_type::builder_type;

            version_command(builder_type &builder, typename builder_type::version_function function)
                : base_type{builder},
                  _function{function}
            {
            }

            virtual int run() override
            {
                _function();
                return 0;
            }

        private:
            typename builder_type::version_function _function;
        };
    }

    //! \brief Manages registration, creation and invocation of subcommands for an application.
    //! 
    //! To use subcommands in your application, you define a class that derives from
    //! basic_command for each one. Then, you register each class with the basic_command_manager
    //! using the add_command() method.
    //! 
    //! You can then invoke create_command() to create an instance of a command type based on the
    //! provided arguments, or run_command() to create a command and immediately run it.
    //! 
    //! Several typedefs for common character types are provided:
    //! 
    //! Type                      | Definition
    //! ------------------------- | -------------------------------------
    //! `ookii::command_manager`  | `ookii::basic_command_manager<char>`
    //! `ookii::wcommand_manager` | `ookii::basic_command_manager<wchar_t>`
    //! 
    //! \tparam CharType The character type used for arguments and other strings. Only `char` and
    //!         `wchar_t` are supported. Defaults to `wchar_t` if `_UNICODE` is defined, otherwise
    //!         to `char`.
    //! \tparam Traits The character traits to use for strings. Defaults to `std::char_traits<CharType>`.
    //! \tparam Alloc The allocator to use for strings. Defaults to `std::allocator<CharType>`.
    template<typename CharType = details::default_char_type, typename Traits = std::char_traits<CharType>, typename Alloc = std::allocator<CharType>>
    class basic_command_manager
    {
    public:
        //! \brief The concrete type of command_info used.
        using info_type = command_info<CharType, Traits, Alloc>;
        //! \brief The concrete string type used.
        using string_type = typename info_type::string_type;
        //! \brief The concrete type of basic_parser_builder used.
        using builder_type = typename info_type::builder_type;
        //! \brief The concrete type of basic_command used.
        using command_type = typename info_type::command_type;
        //! \brief The concrete type of basic_command_with_custom_parsing used.
        using command_with_custom_parsing_type = typename info_type::command_with_custom_parsing_type;
        //! \brief The concrete type of basic_usage_writer used.
        using usage_writer_type = basic_usage_writer<CharType, Traits, Alloc>;
        //! \brief The concrete type of output stream used.
        using stream_type = std::basic_ostream<CharType, Traits>;
        //! \brief The type of a function used to configure parser options for every command.
        using configure_function = std::function<void(builder_type&)>;
        //! \brief The type of a function that displays version information.
        using version_function = std::function<void()>;
        //! \brief The specialized type of basic_localized_string_provider used.
        using string_provider_type = basic_localized_string_provider<CharType, Traits, Alloc>;

        //! \brief Initializes a new instance of the basic_command_manager class.
        //! 
        //! \param application_name The name of the application containing the command. This name
        //!        is used when printing usage help.
        //! \param case_sensitive Indicates whether command names and argument names are case
        //!        sensitive. The default is false.
        //! \param locale The locale to use when converting argument values. The default is a copy
        //!        of the current global locale.
        //! \param string_provider A pointer to an implementation of the
        //!        basic_localized_string_provider class that will be used to get strings for error
        //!        messages etc., or `nullptr` to use the default string provider. If not `nullptr`,
        //!        this pointer must remain valid as long as the basic_command_manager exists.
        basic_command_manager(string_type application_name, bool case_sensitive = false, const std::locale &locale = {},
            const string_provider_type *string_provider = nullptr)
            : _commands{string_less{case_sensitive, locale}},
              _application_name{application_name},
              _locale{locale},
              _string_provider{string_provider},
              _case_sensitive{case_sensitive}
        {
            if (_string_provider == nullptr)
            {
                _string_provider = &string_provider_type::get_default();
            }
        }

        //! \brief Sets a description that will be shown before the command list usage help.
        //! \param description The description.
        //! \return A reference to the basic_command_manager.
        basic_command_manager &description(string_type description)
        {
            _description = description;
            return *this;
        }

        //! \brief Gets the description that will be shown before the command list usage help.
        const string_type &description() const
        {
            return _description;
        }

        //! \brief Sets the name of a help argument, _including prefix_, that is used by all
        //! subcommands.
        //! 
        //! When set, the command list usage help will include an instruction for the user to
        //! invoke a command with this argument to get information about that command.
        //! 
        //! This must be set manually because the command manager cannot know the name,
        //! capitalization or prefix used by each command without instantiating every command.
        //! Keep in mind that the automatically added help argument will adjust its name based on
        //! the capitalization of the other arguments.
        //! 
        //! \param name_with_prefix The name of the argument, including its prefix. For example,
        //!        "-Help".
        //! \return A reference to the basic_command_manager.
        basic_command_manager& common_help_argument(string_type name_with_prefix)
        {
            _common_help_argument = name_with_prefix;
            return *this;
        }

        //! \brief Gets the name of a help argument, including prefix, that is used by all
        //! subcommands.
        const string_type& common_help_argument() const
        {
            return _common_help_argument;
        }

        //! \brief Sets a function that can be used to configure parser options for every command.
        //!
        //! The configure parser function, if set, is invoked on the basic_parser_builder instance
        //! created before that instance is passed to the command class's constructor.
        //!
        //! Use this to configure global options (such as parsing mode) that apply to all commands.
        //!
        //! While this could be used to define global arguments shared by every command, generally
        //! it's recommended to use a common base class for that.
        //!
        //! \param function The function to invoke to configure the parser.
        //! \return A reference to the basic_command_manager.
        basic_command_manager &configure_parser(configure_function function)
        {
            _configure_function = function;
            return *this;
        }

        //! \brief Adds a command to the basic_command_manager.
        //! 
        //! \tparam The type of the command, which must derive from basic_command<CharType>.
        //! \param name The name used to invoke the command. If left blank, it will be determined
        //!        by the static name() method of the command type, or the type name if there is no
        //!        such method.
        //! \param description The description of the command, used for usage help. If left blank,
        //!        it will be determined by the static description() method of the command type. If
        //!        no such method exist, the description will be blank.
        //! \return A reference to the basic_command_manager.
        template<typename T>
        basic_command_manager &add_command(string_type name = {}, string_type description = {})
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

        //! \brief Adds the standard version command.
        //!
        //! This method adds a command with the default name "version", which invokes the specified
        //! function when invoked.
        //!
        //! You can specify a different name, as well as a custom description, using the
        //! basic_localized_string_provider class.
        //!
        //! \param function A function that displays version information. This will be called when
        //!        the command is invoked.
        //! \return A reference to the basic_command_manager.
        basic_command_manager &add_version_command(version_function function)
        {
            auto creator = [function = std::move(function)](builder_type *builder) -> std::unique_ptr<command_type>
            {
                return std::make_unique<details::version_command<CharType, Traits, Alloc>>(*builder, function);
            };

            auto name = _string_provider->automatic_version_command_name();
            auto description = _string_provider->automatic_version_description();
            auto [it, success] = _commands.emplace(name, info_type{name, description, creator});
            if (!success)
                throw std::logic_error("Duplicate command name");

            return *this;
        }

#if defined(_WIN32) || defined(DOXYGEN)
        //! \brief Adds the standard version argument, using version information from the
        //! [VERSIONINFO](https://learn.microsoft.com/windows/win32/menurc/versioninfo-resource)
        //! resource.
        //!
        //! \warning This function is only available when compiling for Windows.
        //!
        //! This method adds a command with the default name "version", which when invoked will
        //! read the VERSION_INFO resource of the current executable, and print the product name,
        //! version and optionally copyright information to the standard output. If these resources
        //! don't exist, the text "Unknown version" is shown.
        //!
        //! You can specify a different name, as well as a custom description, using the
        //! basic_localized_string_provider class.
        //!
        //! \return A reference to the basic_command_manager.
        basic_command_manager &add_win32_version_command()
        {
            return add_version_command(details::show_win32_version<CharType>);
        }
#endif


        //! \brief Gets a view of all the commands.
        auto commands() const noexcept
        {
            return details::range_filter<const info_type &, typename std::map<string_type, info_type, string_less>::const_iterator>{
                _commands.begin(),
                _commands.end(),
                [](const auto &a) -> auto&
                {
                    return a.second;
                },
                {}
            };
        }

        //! \brief Gets the name of the application containing the command
        const string_type &application_name() const noexcept
        {
            return _application_name;
        }

        //! \brief Gets the locale used to parse argument values and to format strings.
        const std::locale &locale() const noexcept
        {
            return _locale;
        }

        //! \brief Gets the basic_localized_string_provider implementation used to get strings for
        //! error messages etc.
        //! \return An instance of a class derived from the basic_localized_string_provider class.
        const string_provider_type &string_provider() const noexcept
        {
            return *_string_provider;
        }

        //! \brief Gets a value that indicates whether command names are case sensitive.
        //! \return `true` if command names are case sensitive; otherwise, `false`.
        //!
        //! If command names are case sensitive, argument names for the commands are too unless this
        //! is overridden by the command.
        bool case_sensitive() const noexcept
        {
            return _case_sensitive;
        }

        //! \brief Gets information about a subcommand by name.
        //! \param name The name of the subcommand.
        //! \return An instance of command_info describing the command, or `nullptr` if there
        //!         is no command with that name.
        const info_type *get_command(const string_type &name) const
        {
            auto it = _commands.find(name);
            if (it == _commands.end())
                return nullptr;

            return &it->second;
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
        //! \param argv The arguments.
        //! \param usage A basic_usage_writer instance that will be used to format errors
        //!        and usage help.
        //! \returns An instance of the subcommand type, or `nullptr` if the an error occurred.
        std::unique_ptr<command_type> create_command(int argc, const CharType *const argv[], usage_writer_type *usage = nullptr) const
        {
            if (argc < 2)
            {
                write_usage(usage);
                return {};
            }

            std::span args{argv, static_cast<size_t>(argc)};
            return create_command(argv[1], args.subspan(2), usage);
        }

        //! \brief Creates an instance of a command based on the specified arguments.
        //! 
        //! If no command was specified or the command could not be found, a list of commands will
        //! be written. If an error occurred parsing the arguments, an error message and usage help
        //! for the command will be written.
        //! 
        //! \warning The args span must not contain the application name; the first argument must
        //!          be the command name.
        //! 
        //! \param args A span containing the arguments.
        //! \param usage A basic_usage_writer instance that will be used to format errors
        //!        and usage help.
        //! \returns An instance of the subcommand type, or `nullptr` if the an error occurred.
        std::unique_ptr<command_type> create_command(std::span<const CharType *const> args, usage_writer_type *usage = nullptr) const
        {
            if (args.size() < 1)
            {
                write_usage(usage);
                return {};
            }

            return create_command(args[0], args.subspan(1), usage);
        }

        //! \brief Creates an instance of a command based on the specified arguments.
        //! 
        //! If the command could not be found, a list of commands will be written. If an error
        //! occurred parsing the arguments, an error message and usage help for the command will be
        //! written.
        //! 
        //! The args span must contain only the arguments for the command; the application name
        //! and command name are assumed to be stripped already.
        //! 
        //! \param name The name of the command.
        //! \param args A span containing the arguments for the command.
        //! \param usage A basic_usage_writer instance that will be used to format errors
        //!        and usage help.
        //! \returns An instance of the subcommand type, or `nullptr` if the an error occurred.
        std::unique_ptr<command_type> create_command(const string_type &name, std::span<const CharType *const> args, usage_writer_type *usage = nullptr) const
        {
            auto info = get_command(name);
            if (info == nullptr)
            {
                write_usage(usage);
                return {};
            }

            std::unique_ptr<command_type> command;
            if (info->use_custom_argument_parsing())
            {
                command = info->create_custom_parsing();
                auto custom_command = static_cast<command_with_custom_parsing_type*>(command.get());
                if (!custom_command->parse(args, *this, usage))
                    return {};
            }
            else
            {
                auto builder = create_parser_builder(*info);
                command = info->create(builder);
                auto parser = builder.build();
                if (!parser.parse(args, usage))
                    return {};
            }

            return command;
        }

        //! \brief Creates an instance of a command based on the specified arguments, and runs the
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
        //! \param argv The arguments.
        //! \param usage A basic_usage_writer instance that will be used to format errors
        //!        and usage help.
        //! \returns The exit code of the command, or `std::nullopt` if the command could not be
        //!          created.
        std::optional<int> run_command(int argc, const CharType *const argv[], usage_writer_type *usage = nullptr) const
        {
            auto command = create_command(argc, argv, usage);
            if (!command)
                return {};

            return command->run();
        }

        //! \brief Creates an instance of a command based on the specified arguments, and runs the
        //!        command.
        //! 
        //! If no command was specified or the command could not be found, a list of commands will
        //! be written. If an error occurred parsing the arguments, an error message and usage help
        //! for the command will be written.
        //! 
        //! \warning The args span must not contain the application name; the first argument must
        //!          be the command name.
        //! 
        //! \param args A span containing the arguments.
        //! \param usage A basic_usage_writer instance that will be used to format errors
        //!        and usage help.
        //! \returns The exit code of the command, or `std::nullopt` if the command could not be
        //!          created.
        std::optional<int> run_command(std::span<const CharType *const> args, usage_writer_type *usage = nullptr) const
        {
            auto command = create_command(args, usage);
            if (!command)
                return {};

            return command->run();
        }

        //! \brief Creates an instance of a command based on the specified arguments, and runs the
        //!        command.
        //! 
        //! If the command could not be found, a list of commands will be written. If an error
        //! occurred parsing the arguments, an error message and usage help for the command will be
        //! written.
        //! 
        //! The args span must contain only the arguments for the command; the application name
        //! and command name are assumed to be stripped already.
        //! 
        //! \param name The name of the command.
        //! \param args A span containing the arguments for the command.
        //! \param usage A basic_usage_writer instance that will be used to format errors
        //!        and usage help.
        //! \returns The exit code of the command, or `std::nullopt` if the command could not be
        //!          created.
        std::optional<int> run_command(const string_type &name, std::span<const CharType *const> args, usage_writer_type *usage = nullptr) const
        {
            auto command = create_command(name, args, usage);
            if (!command)
                return {};

            return command->run();
        }

        //! \brief Writes usage help about the available commands.
        //! \param usage A basic_usage_writer instance that will be used to format the
        //!        usage help.
        void write_usage(usage_writer_type *usage = nullptr) const
        {
            if (usage == nullptr)
            {
                usage_writer_type{}.write_command_list_usage(*this);
            }
            else
            {
                usage->write_command_list_usage(*this);
            }
        }

        //! \brief Creates a basic_parser_builder for a specified command.
        //! 
        //! The parser builder will be initialized with the name and description of the command,
        //! as well the locale and case sensitive value of the basic_command_manager.
        //! 
        //! \param command The command_info for the command.
        //! \return A parser builder with suitable defaults for the command.
        builder_type create_parser_builder(const info_type &command) const
        {
            // Include the application name so usage will be correct.
            string_type full_name = _application_name + static_cast<CharType>(' ') + command.name();
            builder_type builder{full_name, _string_provider};
            builder.locale(_locale)
                .case_sensitive(_case_sensitive)
                .description(command.description());

            if (_configure_function)
            {
                _configure_function(builder);
            }

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
        string_type _description;
        string_type _common_help_argument;
        std::locale _locale;
        configure_function _configure_function;
        const string_provider_type *_string_provider;
        bool _case_sensitive;
    };

    //! \brief Typedef for basic_command_manager using `char` as the character type.
    using command_manager = basic_command_manager<char>;
    //! \brief Typedef for basic_command_manager using `wchar_t` as the character type.
    using wcommand_manager = basic_command_manager<wchar_t>;

}

#endif