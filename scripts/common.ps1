enum ParseState {
    BeforeStruct
    StructAttributes
    StructComment
    InStruct
    ArgumentAttributes
    ArgumentComment
}

enum NameTransformMode {
    None
    PascalCase
    CamelCase
    DashCase
    SnakeCase
    Trim
}

enum ParsingMode {
    Unspecified
    DefaultMode
    LongShort
}

enum ArgumentKind {
    Regular
    MultiValue
    Action
}

enum UsageHelpRequest {
    Unspecified
    Full
    SyntaxOnly
    None
}

class CodeGenContext {
    [string] $CharType
    [string] $FieldPrefix
    [string] $StringPrefix
    [string] $TypeAttribute
    [string] $GlobalAttribute
    [string] $ExtraIndent
    [NameTransformMode] $NameTransform
    [System.IO.TextWriter] $Writer
}

class AttributeInfo {
    AttributeInfo([string]$Name, [string]$Value = $null) {
        $this.Name = $Name
        $this.Value = $Value
    }

    [string] $Name
    [string] $Value
}

class ArgumentInfo {
    [string] $Name
    [char] $ShortName
    [bool] $HasShortName
    [bool] $HasLongName = $true
    [bool] $Required
    [bool] $Positional
    [ArgumentKind] $Kind
    [bool] $CancelParsing
    [string] $ValueDescription
    [string[]] $Aliases
    [char[]] $ShortAliases
    [string] $DefaultValue
    [string] $Description
    [string] $MemberName
    [bool] $Static
    [string] $ArgumentType

    [void] ProcessAttribute([AttributeInfo]$attribute) {
        switch ($attribute.Name) {
            "required" {
                $this.Required = $true
            }
            "positional" {
                $this.Positional = $true
            }
            "value_description" {
                $this.ValueDescription = $attribute.Value
            }
            "alias" {
                $attribute.Value.Split(",") | ForEach-Object {
                    $this.Aliases += $_.Trim()
                }
            }
            "short_alias" {
                $attribute.Value.Split(",") | ForEach-Object {
                    $this.ShortAliases += $_.Trim()
                }
            }
            "default" {
                $this.DefaultValue = $attribute.Value
            }
            "multi_value" {
                $this.Kind = [ArgumentKind]::MultiValue
            }
            "cancel_parsing" {
                $this.CancelParsing = $true
            }
            "short_name" {
                $this.HasShortName = $true
                if ($attribute.Value) {
                    $this.ShortName = $attribute.Value
                }
            }
            "no_long_name" {
                $this.HasLongName = $false
            }
            default {
                Write-Warning "Unexpected field attribute $($attribute.Name)"
            }
        }
    }

    [void] ParseField([string]$line) {
        if ($line -match "(.*?)\s+(?<name>\w+)\s*[;{=]") {
            $this.MemberName = $Matches.name
        } elseif ($line -match "(?<static>static\s+)?bool (?<name>\w+)\s*\((?<params>.*)\)") {
            $this.MemberName = $Matches.name
            $this.Kind = [ArgumentKind]::Action;
            $this.Static = $null -ne $Matches.static
            if ($this.Kind -eq [ArgumentKind]::MultiValue) {
                Write-Warning "[multi_value] attribute ignored for action argument member $($this.MemberName)"
            }

            if ($this.DefaultValue) {
                Write-Warning "[default_value] attribute ignored for action argument member $($this.MemberName)"
            }

            $this.ArgumentType = Get-ArgumentType $Matches.params
        } else {
            throw "Unrecognized field definition: $line"
        }
    }

    [void] GenerateArgument([CodeGenContext]$Context) {
        $method = "add_argument"
        switch ($this.Kind) {
            MultiValue { $method = "add_multi_value_argument" }
            Action { $method = "add_action_argument" }
        }

        if (-not $this.HasLongName) {
            if (-not $this.HasShortName) {
                throw "Argument for field $($this.MemberName) has neither a short nor a long name."
            }

            $primaryName = "'$($this.ShortName)'"
        }
        else {
            $primaryName = "`"$($this.Name)`""
        }

        if ($this.Kind -eq [ArgumentKind]::Action) {
            if ($this.Static) {
                $target = $this.MemberName
            } else {
                if ($Context.FieldPrefix.StartsWith("this")) {
                    $capture = "this"
                } else {
                    $capture = "&$($Context.FieldPrefix.Substring(0, $Context.FieldPrefix.Length - 1))"
                }
                $target = "[$capture]($($this.ArgumentType) value, ookii::basic_command_line_parser<$($Context.CharType)> &parser) { return $($Context.FieldPrefix)$($this.MemberName)(value, parser); }"
            }
        } else {
            $target = "$($Context.FieldPrefix)$($this.MemberName)"
        }

        $Context.Writer.Write("        .$method($target, $($Context.StringPrefix)$primaryName)")
        if ($this.HasLongName -and $this.HasShortName) {
            if ($this.ShortName) {
                $Context.Writer.Write(".short_name($($Context.StringPrefix)'$($this.ShortName)')")
            } else {
                $Context.Writer.Write(".short_name()")
            }
        }

        if ($this.Required) {
            $Context.Writer.Write(".required()")
        }

        if ($this.Positional) {
            $Context.Writer.Write(".positional()")
        }

        if ($this.CancelParsing) {
            $Context.Writer.Write(".cancel_parsing()")
        }

        if ($this.DefaultValue) {
            $Context.Writer.Write(".default_value($($this.DefaultValue))")
        }

        if ($this.ValueDescription) {
            $Context.Writer.Write(".value_description($($Context.StringPrefix)`"$($this.ValueDescription)`")")
        }

        foreach ($alias in $this.Aliases) {
            $Context.Writer.Write(".alias($($Context.StringPrefix)`"$alias`")")
        }

        foreach ($alias in $this.ShortAliases) {
            $Context.Writer.Write(".short_alias($($Context.StringPrefix)'$alias')")
        }

        if ($this.Description) {
            $Context.Writer.Write(".description($($Context.StringPrefix)`"$($this.Description)`")")
        }

        $Context.Writer.WriteLine()
    }

    [void] GenerateName([NameTransformMode]$Transform) {
        # Don't alter an explicitly specified name.
        if (-not $this.Name) {
            $this.Name = Convert-Name $this.MemberName $Transform
        }

        if ((-not $this.HasLongName) -and $this.HasShortName -and (-not $this.ShortName)) {
            $this.ShortName = $this.Name[0]
        }
    }
}

class CommandInfo {
    [string] $CommandName
    [string] $Description
    [string] $TypeName
    [string] $BaseClass
    [string[]] $Prefixes
    [string] $LongPrefix
    [bool] $CaseSensitive
    [bool] $Register = $true
    [bool] $AllowWhiteSpaceSeparator = $true
    [bool] $AllowDuplicateArguments
    [char] $Separator
    [ParsingMode] $ParsingMode
    [ArgumentInfo[]] $Arguments
    [Nullable[NameTransformMode]] $OverrideNameTransform = $null
    [string[]] $VersionInfo
    [bool] $Win32VersionInfo
    [bool] $NoAutoHelp
    [bool] $IsGlobal
    [string] $CommonHelpArgument
    [bool] $AutoCommonHelpArgument
    [UsageHelpRequest] $ShowUsageOnError

    [bool] NeedFileSystem() {
        return -not ([bool]$this.CommandName)
    }

    [void] ParseType([string]$line) {
        if ($line -match "^\s*(struct|class)\s+(?<name>\w+)(\s*:\s*((virtual|public|private|protected)\s+)*(?<base>(\w|:)+))?") {
            $this.TypeName = $Matches.name
            $this.BaseClass = $Matches.base
        } else {
            throw "Unrecognized type definition: $line"
        }
    }

    [void] ProcessAttribute([AttributeInfo]$attribute) {
        switch ($attribute.Name) {
            "no_register" {
                $this.Register = $false
            }
            "prefixes" {
                $attribute.Value.Split(",") | ForEach-Object {
                    $this.Prefixes += $_.Trim()
                }
            }
            "long_prefix" {
                $this.Prefix = $attribute.Value
            }
            "case_sensitive" {
                $this.CaseSensitive = $true
            }
            "argument_value_separator" {
                $this.Separator = $attribute.Value
            }
            "no_whitespace_separator" {
                $this.AllowWhiteSpaceSeparator = $false
            }
            "allow_duplicate_arguments" {
                $this.AllowDuplicateArguments = $true
            }
            "mode" {
                switch ($attribute.Value) {
                    "default" {
                        $this.ParsingMode = [ParsingMode]::DefaultMode
                    }
                    "long_short" {
                        $this.ParsingMode = [ParsingMode]::LongShort
                    }
                    default {
                        throw "Unknown parsing mode $($attribute.Value)"
                    }
                }
            }
            "name_transform" {
                $this.OverrideNameTransform = Convert-NameTransform $attribute.Value
            }
            "version_info" {
                $this.VersionInfo += $attribute.Value
            }
            "win32_version" {
                $this.Win32VersionInfo = $true
            }
            "no_auto_help" {
                $this.NoAutoHelp = $true
            }
            "common_help_argument" {
                if (-not $this.IsGlobal) {
                    Write-Warning "Attribute $($attribute.Name) can only be used with the global context for subcommands."
                } else {
                    if ($attribute.Value) {
                        $this.CommonHelpArgument = $attribute.Value
                    } else {
                        $this.AutoCommonHelpArgument = $true
                    }
                }
            }
            "show_usage_on_error" {
                switch ($attribute.Value) {
                    "full" {
                        $this.ShowUsageOnError = [UsageHelpRequest]::Full
                    }
                    "syntax_only" {
                        $this.ShowUsageOnError = [UsageHelpRequest]::SyntaxOnly
                    }
                    "none" {
                        $this.ShowUsageOnError = [UsageHelpRequest]::None
                    }
                    default {
                        throw "Unknown show_usage_on_error value $($attribute.Value)"
                    }
                }
            }
            default {
                Write-Warning "Unexpected command attribute $($attribute.Name)."
            }
        }
    }

    [void] GenerateParser([CodeGenContext]$Context) {
        if ($this.CommandName) {
            $Context.Writer.WriteLine("    command_name = $($Context.StringPrefix)`"$($this.CommandName)`";")
        }

        $Context.FieldPrefix = "this->"
        $Context.Writer.WriteLine("    ookii::basic_parser_builder<$($Context.CharType)> builder{command_name, string_provider};")
        $Context.Writer.WriteLine("    builder")
        $Context.Writer.WriteLine("        .locale(locale)")
        if ($this.Description) {
            $Context.Writer.WriteLine("        .description($($Context.StringPrefix)`"$($this.Description)`")")
        }
        
        $this.GenerateParserAttributes($Context)
        $this.GenerateArguments($Context)
        $Context.Writer.WriteLine("    ;")
        $Context.Writer.WriteLine("")
        $Context.Writer.WriteLine("    return builder;")
    }

    [void] GenerateParserAttributes([CodeGenContext]$Context) {
        switch ($this.ParsingMode) {
            DefaultMode {
                $Context.Writer.WriteLine("$($Context.ExtraIndent)        .mode(ookii::parsing_mode::default)")
            }
            LongShort {
                $Context.Writer.WriteLine("$($Context.ExtraIndent)        .mode(ookii::parsing_mode::long_short)")
            }
        }

        switch ($this.ShowUsageOnError) {
            Full {
                $Context.Writer.WriteLine("$($Context.ExtraIndent)        .show_usage_on_error(ookii::usage_help_request::full)")
            }
            SyntaxOnly {
                $Context.Writer.WriteLine("$($Context.ExtraIndent)        .show_usage_on_error(ookii::usage_help_request::syntax_only)")
            }
            None {
                $Context.Writer.WriteLine("$($Context.ExtraIndent)        .show_usage_on_error(ookii::usage_help_request::none)")
            }
        }

        if ($this.Prefixes.Length -gt 0) {
            $prefixList = $this.Prefixes -join "`", $($Context.StringPrefix)`""
            $Context.Writer.WriteLine("$($Context.ExtraIndent)        .prefixes({ $($Context.StringPrefix)`"$prefixList`" })")
        }

        if ($this.LongPrefix) {
            $Context.Writer.WriteLine("$($Context.ExtraIndent)        .long_prefix($($Context.StringPrefix)`"$($this.LongPrefix)`")")
        }

        if (-not $this.IsGlobal -and $this.CaseSensitive) {
            $Context.Writer.WriteLine("$($Context.ExtraIndent)        .case_sensitive(true)")
        }

        if (-not $this.AllowWhiteSpaceSeparator) {
            $Context.Writer.WriteLine("$($Context.ExtraIndent)        .allow_whitespace_separator(false)")
        }

        if ($this.AllowDuplicateArguments) {
            $Context.Writer.WriteLine("$($Context.ExtraIndent)        .allow_duplicate_arguments(true)")
        }

        if ($this.Separator) {
            $Context.Writer.WriteLine("$($Context.ExtraIndent)        .argument_value_separator($($Context.StringPrefix)'$($this.Separator)')")
        }

        if ($this.NoAutoHelp) {
            $Context.Writer.WriteLine("$($Context.ExtraIndent)        .automatic_help_argument(false)")
        }

        if (-not $this.IsGlobal) {
            if ($this.Win32VersionInfo) {
                $Context.Writer.WriteLine("#ifdef _WIN32")
                $Context.Writer.WriteLine("$($Context.ExtraIndent)        .add_win32_version_argument()")
                if ($this.VersionInfo) {
                    $Context.Writer.WriteLine("#else")
                }
            }

            if ($this.VersionInfo) {
                $Context.Writer.WriteLine("$($Context.ExtraIndent)        .add_version_argument([]()")
                $Context.Writer.WriteLine("$($Context.ExtraIndent)        {")
                foreach ($line in $this.VersionInfo) {
                    $Context.Writer.WriteLine("$($Context.ExtraIndent)            ookii::console_stream<$($Context.CharType)>::cout() << $($Context.StringPrefix)`"$line`" << std::endl;")
                }

                $Context.Writer.WriteLine("$($Context.ExtraIndent)        })")
            }

            if ($this.Win32VersionInfo) {
                $Context.Writer.WriteLine("#endif")
            }
        }
    }

    [void] GenerateSubcommand([CodeGenContext]$Context) {
        $Context.Writer.WriteLine("$($this.TypeName)::$($this.TypeName)($($this.TypeName)::builder_type &builder)")
        if ($this.BaseClass) {
            $Context.Writer.WriteLine("    : $($this.BaseClass){builder}")
        }

        $Context.FieldPrefix = "this->"
        $Context.Writer.WriteLine("{")
        $oldWriter = $Context.Writer
        $Context.Writer = [System.IO.StringWriter]::new();
        $this.GenerateParserAttributes($Context)
        $this.GenerateArguments($Context)
        $builder = $Context.Writer.ToString()
        $Context.Writer = $oldWriter
        if ($builder.Length -gt 0) {
            $Context.Writer.WriteLine("    builder")
            $Context.Writer.Write($builder)
            $Context.Writer.WriteLine("    ;")
        }

        $Context.Writer.WriteLine("}")
        $Context.Writer.WriteLine()
    }

    [void] GenerateArguments([CodeGenContext]$Context) {
        $nameTransform = $Context.NameTransform
        if ($null -ne $this.OverrideNameTransform) {
            $nameTransform = $this.OverrideNameTransform
        }

        foreach ($arg in $this.Arguments) {
            $arg.GenerateName($nameTransform)
            $arg.GenerateArgument($Context)
        }
    }

    [void] GenerateRegistration([CodeGenContext]$Context) {
        $name = if ($this.CommandName) {
            "$($Context.StringPrefix)`"$($this.CommandName)`""
        } else {
            "{}"
        }

        $commandDescription = if ($this.Description) {
            "$($Context.StringPrefix)`"$($this.Description)`""
        } else {
            "{}"
        }

        $Context.Writer.WriteLine("        .add_command<$($this.TypeName)>($name, $commandDescription)")
    }

    [void] GenerateGlobal([CodeGenContext]$Context) {
        if ($this.Description) {
            $Context.Writer.WriteLine("        .description($($Context.StringPrefix)`"$($this.Description)`")")
        }

        $helpArgument = $this.CommonHelpArgument
        if ($this.AutoCommonHelpArgument) {
            $nameTransform = $Context.NameTransform
            if ($null -ne $this.OverrideNameTransform) {
                $nameTransform = $this.OverrideNameTransform
            }

            if ($this.ParsingMode -eq [ParsingMode]::LongShort) {
                $prefix = $this.LongPrefix
                if (-not $prefix) {
                    $prefix = "--"
                }
            } else {
                $prefix = "-"
                if ($this.Prefixes.Length -gt 0) {
                    $prefix = $this.Prefixes[0]
                }
            }

            $helpArgument = $prefix + (Convert-Name "Help" $nameTransform)
        }

        if ($helpArgument) {
            $Context.Writer.WriteLine("        .common_help_argument($($Context.StringPrefix)`"$helpArgument`")")
        }

        $oldWriter = $Context.Writer
        $Context.Writer = [System.IO.StringWriter]::new()
        $Context.ExtraIndent = "        "
        $this.GenerateParserAttributes($Context)
        $attributes = $Context.Writer.ToString();
        $Context.Writer = $oldWriter
        $Context.ExtraIndent = $null
        if ($attributes.Length -gt 0) {
            $Context.Writer.WriteLine("        .configure_parser([](auto &parser)")
            $Context.Writer.WriteLine("        {")
            $Context.Writer.WriteLine("            parser")
            $Context.Writer.Write($attributes)
            $Context.Writer.WriteLine("            ;")
            $Context.Writer.WriteLine("        })")
        }

        if ($this.Win32VersionInfo) {
            $Context.Writer.WriteLine("#ifdef _WIN32")
            $Context.Writer.WriteLine("$($Context.ExtraIndent)        .add_win32_version_command()")
            if ($this.VersionInfo) {
                $Context.Writer.WriteLine("#else")
            }
        }

        if ($this.VersionInfo) {
            $Context.Writer.WriteLine("$($Context.ExtraIndent)        .add_version_command([]()")
            $Context.Writer.WriteLine("$($Context.ExtraIndent)        {")
            foreach ($line in $this.VersionInfo) {
                $Context.Writer.WriteLine("$($Context.ExtraIndent)            ookii::console_stream<$($Context.CharType)>::cout() << $($Context.StringPrefix)`"$line`" << std::endl;")
            }

            $Context.Writer.WriteLine("$($Context.ExtraIndent)        })")
        }

        if ($this.Win32VersionInfo) {
            $Context.Writer.WriteLine("#endif")
        }
    }
}

# Get the text of a comment stripped of leading and trailing spaces.
function Get-Comment([string]$line) {
    if ($line -imatch "^\s*//+\s*(?<comment>.*?)\s*$") {
        return $Matches.comment
    } else {
        return $null
    }
}

function Get-Attribute([string]$value) {
    if ($value -notmatch "^\[\s*(?<attribute>.*)\]$") {
        return $null
    }

    $attribute = $Matches.attribute
    while ($attribute) {
        if ($attribute -match "^(?<name>[^:,]*?)\s*:\s*(?<value>.*)$") {
            [AttributeInfo]::new($Matches.name, $Matches.value)
            return           
        } elseif ($attribute -match "^(?<name>[^:,]*?)\s*,\s*(?<remainder>.*)$") {
            [AttributeInfo]::new($Matches.name, $null)
            $attribute = $Matches.remainder
        } else {
            [AttributeInfo]::new($attribute, $null)
            return
        }
    }
}

function Get-CommentAttribute([string]$line) {
    $comment = Get-Comment $line
    if ($comment) {
        Get-Attribute $comment
    }
}

function Get-DescriptionLine([string]$comment, [ref]$newParagraph, [bool]$allowWhiteSpace) {
    if ($comment.Length -eq 0) {
        $newParagraph.Value = $allowWhiteSpace
    } else {
        $comment = $comment.Replace("\", "\\")
        if ($newParagraph.Value) {
            $comment = "\n\n$comment"
            $newParagraph.Value = $false
        } elseif ($allowWhiteSpace) {
            $comment = " $comment"
        }

        $comment
    }
}

function Convert-Arguments([string[]]$contents, [CodeGenContext]$Context) {
    $info = [CommandInfo]::new()
    $state = [ParseState]::BeforeStruct
    [bool]$newParagraph = $false
    foreach ($line in $contents) {
        switch ($state) {
            BeforeStruct {
                foreach ($attribute in (Get-CommentAttribute $line)) {
                    if ($state -eq [ParseState]::BeforeStruct) {
                        if ($attribute.Name -ieq $context.TypeAttribute) {
                            $info.CommandName = $attribute.Value
                            $state = [ParseState]::StructAttributes
                            $newParagraph = $false
                        } elseif ($context.GlobalAttribute -and $attribute.Name -ieq $context.GlobalAttribute) {
                            $info.CommandName = "[global]"
                            $info.IsGlobal = $true
                            $state = [ParseState]::StructAttributes
                            $newParagraph = $false
                        } else {
                            Write-Warning "Unexpected global attribute: $($attribute.Name)"
                        }
                    } else {
                        # The state was changed to StructAttributes above during this loop
                        $info.ProcessAttribute($attribute)
                    }
                }
            }
            StructAttributes {
                $comment = Get-Comment $line
                if ($null -ne $comment) {
                    $hasAttributes = $false
                    foreach ($attribute in (Get-Attribute $comment)) {
                        $info.ProcessAttribute($attribute)
                        $hasAttributes = $true
                    }

                    if (-not $hasAttributes) {
                        $newParagraph = $false
                        $info.Description += Get-DescriptionLine $comment ([ref]$newParagraph) $false
                        $state = [ParseState]::StructComment
                    }
                } else {
                    if ($info.IsGlobal) {
                        $info
                        $info = [CommandInfo]::new()
                        $state = [ParseState]::BeforeStruct
                    } else {
                        $info.ParseType($line)
                        $state = [ParseState]::InStruct
                    }
                }
            }
            StructComment {
                $comment = Get-Comment $line
                if ($null -ne $comment) {
                    $info.Description += Get-DescriptionLine $comment ([ref]$newParagraph) ($info.Description -gt 0)
                } else {
                    if ($info.IsGlobal) {
                        $info
                        $info = [CommandInfo]::new()
                        $state = [ParseState]::BeforeStruct
                    } else {
                        $info.ParseType($line)
                        $state = [ParseState]::InStruct
                    }
                }
            }
            InStruct {
                if ($line -match "^\s*};\s*$") {
                    $info
                    $info = [CommandInfo]::new()
                    $state = [ParseState]::BeforeStruct
                } else {
                    foreach($attribute in (Get-CommentAttribute $line)) {
                        if ($state -eq [ParseState]::InStruct) {
                            if ($attribute.Name -ieq "argument") {
                                $currentArg = [ArgumentInfo]::new()
                                $currentArg.Name = $attribute.Value
                                $state = [ParseState]::ArgumentAttributes
                            } else {
                                Write-Warning "Unexpected attribute in struct: $($attribute.Name)"
                            }
                        } else {
                            # The state was changed to ArgumentAttributes above during this loop
                            $currentArg.ProcessAttribute($attribute)
                        }
                    }
                }
            }
            ArgumentAttributes {
                $comment = Get-Comment $line
                if ($null -ne $comment) {
                    $hasAttributes = $false
                    foreach ($attribute in (Get-Attribute $comment)) {
                        $currentArg.ProcessAttribute($attribute)
                        $hasAttributes = $true
                    }

                    if (-not $hasAttributes) {
                        $newParagraph = $false
                        $currentArg.Description += Get-DescriptionLine $comment ([ref]$newParagraph) $false
                        $state = [ParseState]::ArgumentComment
                    }
                } else {
                    $currentArg.ParseField($line)
                    $info.Arguments += $currentArg
                    $currentArg = $null
                    $state = [ParseState]::InStruct
                }
            }
            ArgumentComment {
                $comment = Get-Comment $line
                if ($null -ne $comment) {
                    $currentArg.Description += Get-DescriptionLine $comment ([ref]$newParagraph) ($currentArg.Description.Length -gt 0)
                } else {
                    $currentArg.ParseField($line)
                    $info.Arguments += $currentArg
                    $currentArg = $null
                    $state = [ParseState]::InStruct
                }
            }
            Default {}
        }
    }

    if (($state -eq [ParseState]::StructAttributes -or $state -eq [ParseState]::StructComment) -and $info.IsGlobal)
    {
        $info
        $info = [CommandInfo]::new()
        $state = [ParseState]::BeforeStruct
    }

    if ($state -ne [ParseState]::BeforeStruct) {
        Write-Warning "Unexpected end of file; some information may have been discarded."
    }
}

function Convert-NameTransform([string]$value) {
    switch ($value) {
        "None" { [NameTransformMode]::None }
        "PascalCase" { [NameTransformMode]::PascalCase }
        "CamelCase" { [NameTransformMode]::CamelCase }
        { $_ -eq "SnakeCase" -or $_ -eq "snake_case" } { [NameTransformMode]::SnakeCase }
        { $_ -eq "DashCase" -or $_ -eq "dash-case" } { [NameTransformMode]::DashCase }
        default { throw "Invalid name transform: $value" }
    }
}

# Extract the type of the first parameter for an action argument.
function Get-ArgumentType([string]$parameters) {
    $level = 0
    for ($index = 0; $index -lt $parameters.Length; $index += 1) {
        $ch = $parameters[$index]
        switch ($ch) {
            '<' { $level += 1 }
            '>' { $level -= 1 }
            {$_ -eq ' ' -or $_ -eq ','} {
                if ($level -eq 0) {
                    return $parameters.Substring(0, $index)
                }
            }
        }
    }

    throw "Unable to determine argument type from action argument parameters: $parameters"
}

function Convert-Name([string]$Name, [NameTransformMode]$Transform) {
    switch ($Transform) {
        PascalCase {
            $Name = ConvertTo-PascalOrCamelCase $Name $true
        }
        CamelCase {
            $Name = ConvertTo-PascalOrCamelCase $Name $false
        }
        SnakeCase {
            $Name = ConvertTo-SnakeOrDashCase $Name '_'
        }
        DashCase {
            $Name = ConvertTo-SnakeOrDashCase $Name '-'
        }
        Trim {
            $Name = $Name.Trim('_')
        }
    }

    return $Name
}

function ConvertTo-PascalOrCamelCase([string]$Name, [bool]$PascalCase) {
    # Remove any underscores, and the first letter (if pascal case) and any letter after an
    # underscore is converted to uppercase. Other letters are unchanged.
    $toUpper = $PascalCase
    $toLower = -not $PascalCase # Only for the first character.
    return [string]::new(($Name.Trim("_").ToCharArray() | ForEach-Object {
        if ($_ -eq "_") {
            $toUpper = $true
        } elseif ($toUpper) {
            [char]::ToUpperInvariant($_)
            $toUpper = $false
        } elseif ($toLower) {
            [char]::ToLowerInvariant($_)
            $toLower = $false
        } else {
            $_
        }
    }))
}

function ConvertTo-SnakeOrDashCase([string]$Name, [char]$separator) {
    $needSeparator = $false
    $first = $true
    return [string]::new(($Name.Trim("_").ToCharArray() | ForEach-Object {
        if ($_ -eq "_") {
            $needSeparator = $true
        } else {
            if ($needSeparator -or ([char]::IsUpper($_) -and -not $first)) {
                $separator
                $needSeparator = $false
            }

            [char]::ToLowerInvariant($_)
        }

        $first = $false
    }))
}
