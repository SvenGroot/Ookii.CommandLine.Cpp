#pragma once

static constexpr ookii::tstring_view c_usageExpected = TEXT(R"(Application description.

Usage: TestCommand [-StringArg] <string>
   -IntArg <int> [-FloatArg <number>]
   [-Help] [-IntArg2 <int>] [-MultiArg
   <string>...] [-OptionalSwitchArg]
   [-SwitchArg]

    -StringArg <string>
        String argument description.

    -FloatArg <number>
        Float argument description that
        is really quite long and
        probably needs to be wrapped.
        Default value: 10.

    -Help [<bool>] (-?, -h)
        Displays this help message.

    -IntArg2 <int>
         Default value: 4.

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
   [-Help] [-IntArg2 <int>] [-MultiArg
   <string>...] [-OptionalSwitchArg]
   [-SwitchArg]

    [32m-StringArg <string>[0m
        String argument description.

    [32m-FloatArg <number>[0m
        Float argument description that
        is really quite long and
        probably needs to be wrapped.
        Default value: 10.

    [32m-Help [<bool>] (-?, -h)[0m
        Displays this help message.

    [32m-IntArg2 <int>[0m
         Default value: 4.

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

static constexpr ookii::tstring_view c_usageExpectedSyntaxOnly = TEXT(R"(Usage: TestCommand [-StringArg] <string>
   -IntArg <int> [-FloatArg <number>]
   [-Help] [-IntArg2 <int>] [-MultiArg
   <string>...] [-OptionalSwitchArg]
   [-SwitchArg]

Run 'TestCommand -Help' for more
information.
)");

static constexpr ookii::tstring_view c_usageExpectedNone = TEXT(R"(Run 'TestCommand -Help' for more
information.
)");

static constexpr ookii::tstring_view c_usageExpectedFilterDescription = TEXT(R"(Application description.

Usage: TestCommand [-StringArg] <string>
   -IntArg <int> [-FloatArg <number>]
   [-Help] [-IntArg2 <int>] [-MultiArg
   <string>...] [-OptionalSwitchArg]
   [-SwitchArg]

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

static constexpr ookii::tstring_view c_usageExpectedFilterAll = TEXT(R"(Application description.

Usage: TestCommand [-StringArg] <string>
   -IntArg <int> [-FloatArg <number>]
   [-Help] [-IntArg2 <int>] [-MultiArg
   <string>...] [-OptionalSwitchArg]
   [-SwitchArg]

    -StringArg <string>
        String argument description.

    -IntArg <int>


    -FloatArg <number>
        Float argument description that
        is really quite long and
        probably needs to be wrapped.
        Default value: 10.

    -Help [<bool>] (-?, -h)
        Displays this help message.

    -IntArg2 <int>
         Default value: 4.

    -MultiArg <string> (-multi, -m)
        Multi-value argument
        description.

    -OptionalSwitchArg [<bool>]
        Optional switch argument.

    -SwitchArg [<bool>] (-s)
        Switch argument description.
        With a new line.

)");

static constexpr ookii::tstring_view c_usageExpectedFilterNone = TEXT(R"(Application description.

Usage: TestCommand [-StringArg] <string>
   -IntArg <int> [-FloatArg <number>]
   [-Help] [-IntArg2 <int>] [-MultiArg
   <string>...] [-OptionalSwitchArg]
   [-SwitchArg]

)");

static constexpr ookii::tstring_view c_usageExpectedAlphabetical = TEXT(R"(Application description.

Usage: TestCommand [-StringArg] <string>
   -IntArg <int> [-FloatArg <number>]
   [-Help] [-IntArg2 <int>] [-MultiArg
   <string>...] [-OptionalSwitchArg]
   [-SwitchArg]

    -FloatArg <number>
        Float argument description that
        is really quite long and
        probably needs to be wrapped.
        Default value: 10.

    -Help [<bool>] (-?, -h)
        Displays this help message.

    -IntArg2 <int>
         Default value: 4.

    -MultiArg <string> (-multi, -m)
        Multi-value argument
        description.

    -OptionalSwitchArg [<bool>]
        Optional switch argument.

    -StringArg <string>
        String argument description.

    -SwitchArg [<bool>] (-s)
        Switch argument description.
        With a new line.

)");

static constexpr ookii::tstring_view c_usageExpectedAlphabeticalDescending = TEXT(R"(Application description.

Usage: TestCommand [-StringArg] <string>
   -IntArg <int> [-FloatArg <number>]
   [-Help] [-IntArg2 <int>] [-MultiArg
   <string>...] [-OptionalSwitchArg]
   [-SwitchArg]

    -SwitchArg [<bool>] (-s)
        Switch argument description.
        With a new line.

    -StringArg <string>
        String argument description.

    -OptionalSwitchArg [<bool>]
        Optional switch argument.

    -MultiArg <string> (-multi, -m)
        Multi-value argument
        description.

    -IntArg2 <int>
         Default value: 4.

    -Help [<bool>] (-?, -h)
        Displays this help message.

    -FloatArg <number>
        Float argument description that
        is really quite long and
        probably needs to be wrapped.
        Default value: 10.

)");

static constexpr ookii::tstring_view c_usageExpectedLongShortAlphabeticalLongName = TEXT(R"(Usage: TestCommand [[--foo] <int>] [[--bar] <int>] [[--Arg2] <int>] [--Arg1 <int>] [--help] [--Switch1] [--Switch2] [-u]

        --Arg1 <int>
            Arg1 description.

    -a, --Arg2 <int> (-b, --Baz)
            Arg2 description.

        --bar <int>
            Bar description. Default value: 0.

    -f, --foo <int>
            Foo description. Default value: 0.

    -?, --help [<bool>] (-h)
            Displays this help message.

    -S, --Switch1 [<bool>]
            Switch1 description.

    -k, --Switch2 [<bool>]
            Switch2 description.

    -u [<bool>]
            Switch3 description.

)");

static constexpr ookii::tstring_view c_usageExpectedLongShortAlphabeticalLongNameDescending = TEXT(R"(Usage: TestCommand [[--foo] <int>] [[--bar] <int>] [[--Arg2] <int>] [--Arg1 <int>] [--help] [--Switch1] [--Switch2] [-u]

    -u [<bool>]
            Switch3 description.

    -k, --Switch2 [<bool>]
            Switch2 description.

    -S, --Switch1 [<bool>]
            Switch1 description.

    -?, --help [<bool>] (-h)
            Displays this help message.

    -f, --foo <int>
            Foo description. Default value: 0.

        --bar <int>
            Bar description. Default value: 0.

    -a, --Arg2 <int> (-b, --Baz)
            Arg2 description.

        --Arg1 <int>
            Arg1 description.

)");

static constexpr ookii::tstring_view c_usageExpectedLongShortAlphabeticalShortName = TEXT(R"(Usage: TestCommand [[--foo] <int>] [[--bar] <int>] [[--Arg2] <int>] [--Arg1 <int>] [--help] [--Switch1] [--Switch2] [-u]

    -?, --help [<bool>] (-h)
            Displays this help message.

    -a, --Arg2 <int> (-b, --Baz)
            Arg2 description.

        --Arg1 <int>
            Arg1 description.

        --bar <int>
            Bar description. Default value: 0.

    -f, --foo <int>
            Foo description. Default value: 0.

    -k, --Switch2 [<bool>]
            Switch2 description.

    -S, --Switch1 [<bool>]
            Switch1 description.

    -u [<bool>]
            Switch3 description.

)");

static constexpr ookii::tstring_view c_usageExpectedLongShortAlphabeticalShortNameDescending = TEXT(R"(Usage: TestCommand [[--foo] <int>] [[--bar] <int>] [[--Arg2] <int>] [--Arg1 <int>] [--help] [--Switch1] [--Switch2] [-u]

    -u [<bool>]
            Switch3 description.

    -S, --Switch1 [<bool>]
            Switch1 description.

    -k, --Switch2 [<bool>]
            Switch2 description.

    -f, --foo <int>
            Foo description. Default value: 0.

        --bar <int>
            Bar description. Default value: 0.

        --Arg1 <int>
            Arg1 description.

    -a, --Arg2 <int> (-b, --Baz)
            Arg2 description.

    -?, --help [<bool>] (-h)
            Displays this help message.

)");
