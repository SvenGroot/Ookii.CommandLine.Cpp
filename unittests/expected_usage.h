#pragma once

static constexpr ookii::tstring_view c_usageExpected = TEXT(R"(Application description.

Usage: TestCommand [-StringArg] <string>
   -IntArg <int> [-FloatArg <number>]
   [-Help] [-MultiArg <string>...]
   [-OptionalSwitchArg] [-SwitchArg]

    -StringArg <string>
        String argument description.

    -FloatArg <number>
        Float argument description that
        is really quite long and
        probably needs to be wrapped.
        Default value: 10.

    -Help [<bool>] (-?, -h)
        Displays this help message.

    -MultiArg <string> (-multi, -m)
        Multi-value argument
        description.

    -OptionalSwitchArg [<bool>]
        Optional switch argument.

    -SwitchArg [<bool>] (-s)
        Switch argument description.
        With a new line.

)");

static constexpr ookii::tstring_view c_usageExpectedColor = TEXT(R"(Application description.

[36mUsage:[0m TestCommand [-StringArg] <string>
   -IntArg <int> [-FloatArg <number>]
   [-Help] [-MultiArg <string>...]
   [-OptionalSwitchArg] [-SwitchArg]

    [32m-StringArg <string>[0m
        String argument description.

    [32m-FloatArg <number>[0m
        Float argument description that
        is really quite long and
        probably needs to be wrapped.
        Default value: 10.

    [32m-Help [<bool>] (-?, -h)[0m
        Displays this help message.

    [32m-MultiArg <string> (-multi, -m)[0m
        Multi-value argument
        description.

    [32m-OptionalSwitchArg [<bool>][0m
        Optional switch argument.

    [32m-SwitchArg [<bool>] (-s)[0m
        Switch argument description.
        With a new line.

)");

static constexpr ookii::tstring_view c_usageExpectedLongShort = TEXT(R"(Usage: TestCommand [[--foo] <int>] [[--bar] <int>] [[--Arg2] <int>] [--Arg1 <int>] [--help] [--Switch1] [--Switch2] [-u]

    -f, --foo <int>
            Foo description. Default value: 0.

        --bar <int>
            Bar description. Default value: 0.

    -a, --Arg2 <int> (-b, --Baz)
            Arg2 description.

        --Arg1 <int>
            Arg1 description.

    -?, --help [<bool>] (-h)
            Displays this help message.

    -S, --Switch1 [<bool>]
            Switch1 description.

    -k, --Switch2 [<bool>]
            Switch2 description.

    -u [<bool>]
            Switch3 description.

)");

static constexpr ookii::tstring_view c_usageExpectedLongShortColor = TEXT(R"([36mUsage:[0m TestCommand [[--foo] <int>] [[--bar] <int>] [[--Arg2] <int>] [--Arg1 <int>] [--help] [--Switch1] [--Switch2] [-u]

    [32m-f, --foo <int>[0m
            Foo description. Default value: 0.

    [32m    --bar <int>[0m
            Bar description. Default value: 0.

    [32m-a, --Arg2 <int> (-b, --Baz)[0m
            Arg2 description.

    [32m    --Arg1 <int>[0m
            Arg1 description.

    [32m-?, --help [<bool>] (-h)[0m
            Displays this help message.

    [32m-S, --Switch1 [<bool>][0m
            Switch1 description.

    [32m-k, --Switch2 [<bool>][0m
            Switch2 description.

    [32m-u [<bool>][0m
            Switch3 description.

)");

static constexpr ookii::tstring_view c_usageExpectedLongShortSyntaxShortName = TEXT(R"(Usage: TestCommand [[-f] <int>] [[--bar] <int>] [[-a] <int>] [--Arg1 <int>] [-?] [-S] [-k] [-u]

    -f, --foo <int>
            Foo description. Default value: 0.

        --bar <int>
            Bar description. Default value: 0.

    -a, --Arg2 <int> (-b, --Baz)
            Arg2 description.

        --Arg1 <int>
            Arg1 description.

    -?, --help [<bool>] (-h)
            Displays this help message.

    -S, --Switch1 [<bool>]
            Switch1 description.

    -k, --Switch2 [<bool>]
            Switch2 description.

    -u [<bool>]
            Switch3 description.

)");

static constexpr ookii::tstring_view c_usageExpectedLongShortAbbreviated = TEXT(R"(Usage: TestCommand [[--foo] <int>] [[--bar] <int>] [[--Arg2] <int>] [arguments]

    -f, --foo <int>
            Foo description. Default value: 0.

        --bar <int>
            Bar description. Default value: 0.

    -a, --Arg2 <int> (-b, --Baz)
            Arg2 description.

        --Arg1 <int>
            Arg1 description.

    -?, --help [<bool>] (-h)
            Displays this help message.

    -S, --Switch1 [<bool>]
            Switch1 description.

    -k, --Switch2 [<bool>]
            Switch2 description.

    -u [<bool>]
            Switch3 description.

)");

