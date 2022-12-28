# Samples

Ookii.CommandLine for C++ comes with several samples that demonstrate various aspects of its
functionality.

Two samples demonstrate how to use Ookii.CommandLine for C++ manually, without code generation:

- The [**parser sample**](parser) demonstrates the basic functionality of defining, parsing and
  using arguments.
- The [**subcommand sample**](subcommand) demonstrates how to create a simple application that has
  multiple subcommands.

The remaining samples all utilize the [code generation scripts](../docs/Scripts.md).

- The [**generated parser sample**](generated_parser) is identical to the parser sample, but uses
  code generation.
- The [**generated subcommand sample**](generated_subcommand) is identical to the subcommand sample,
  but uses code generation.
- The [**long/short mode sample**](lonsg_short) demonstrates the POSIX-like long/short parsing mode,
  where arguments can have both a long name with `--` and a short name with `-`.
- The [**custom usage sample**](custom_usage) demonstrates the flexibility of Ookii.CommandLine's
  usage help generation, by customizing it to use completely different formatting.
- The [**nested commands sample**](nested_commands) demonstrates how to create an application where
  commands can contain other commands. It also demonstrates how to create common arguments for
  multiple commands using a common base class, and how to combine generated commands with manually
  written commands.
