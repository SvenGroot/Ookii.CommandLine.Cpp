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
    Default
    LongShort
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
    [bool] $MultiValue
    [bool] $CancelParsing
    [string] $ValueDescription
    [string[]] $Aliases
    [char[]] $ShortAliases
    [string] $DefaultValue
    [string] $Description
    [string] $FieldName

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
                $this.MultiValue = $true
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
            $this.FieldName = $Matches.name
        } else {
            throw "Unrecognized field definition: $line"
        }
    }

    [string] GenerateArgument([string]$StringPrefix, [string]$FieldPrefix) {
        if ($this.MultiValue) {
            $method = "add_multi_value_argument"
        } else {
            $method = "add_argument"
        }

        if (-not $this.HasLongName) {
            if (-not $this.HasShortName) {
                throw "Argument for field $($this.FieldName) has neither a short nor a long name."
            }

            $primaryName = "'$($this.ShortName)'"
        }
        else {
            $primaryName = "`"$($this.Name)`""
        }

        $result = "        .$method($FieldPrefix$($this.FieldName), $StringPrefix$primaryName)"
        if ($this.HasLongName -and $this.HasShortName) {
            if ($this.ShortName) {
                $result += ".short_name($StringPrefix'$($this.ShortName)')"
            } else {
                $result += ".short_name()"
            }
        }

        if ($this.Required) {
            $result += ".required()"
        }

        if ($this.Positional) {
            $result += ".positional()"
        }

        if ($this.CancelParsing) {
            $result += ".cancel_parsing()"
        }

        if ($this.DefaultValue) {
            $result += ".default_value($($this.DefaultValue))"
        }

        if ($this.ValueDescription) {
            $result += ".value_description($StringPrefix`"$($this.ValueDescription)`")"
        }

        foreach ($alias in $this.Aliases) {
            $result += ".alias($StringPrefix`"$alias`")"
        }

        foreach ($alias in $this.ShortAliases) {
            $result += ".short_alias($StringPrefix'$alias')"
        }

        if ($this.Description) {
            $result += ".description($StringPrefix`"$($this.Description)`")"
        }

        $result
        return $result;
    }

    [void] GenerateName([NameTransformMode]$Transform) {
        # Don't alter an explicitly specified name.
        if (-not $this.Name) {
            switch ($Transform) {
                PascalCase {
                    $this.Name = [ArgumentInfo]::ToPascalOrCamelCase($this.FieldName, $true)
                }
                CamelCase {
                    $this.Name = [ArgumentInfo]::ToPascalOrCamelCase($this.FieldName, $false)
                }
                SnakeCase {
                    $this.Name = [ArgumentInfo]::ToSnakeOrDashCase($this.FieldName, '_')
                }
                DashCase {
                    $this.Name = [ArgumentInfo]::ToSnakeOrDashCase($this.FieldName, '-')
                }
                Trim {
                    $this.Name = $this.FieldName.Trim('_')
                }
                default {
                    $this.Name = $this.FieldName
                }
            }
        }

        if ((-not $this.HasLongName) -and $this.HasShortName -and (-not $this.ShortName)) {
            $this.ShortName = $this.Name[0]
        }
    }

    [string]hidden static ToPascalOrCamelCase([string]$Name, [bool]$PascalCase) {
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

    [string]hidden static ToSnakeOrDashCase([string]$Name, [char]$separator) {
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
}

class CommandInfo {
    [string] $CommandName
    [string] $Description
    [string] $TypeName
    [string] $BaseClass
    [string[]] $Prefixes
    [bool] $CaseSensitive
    [bool] $Register = $true
    [bool] $AllowWhiteSpaceSeparator = $true
    [bool] $AllowDuplicateArguments
    [char] $Separator
    [ParsingMode] $ParsingMode
    [ArgumentInfo[]] $Arguments

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
                        $this.ParsingMode = [ParsingMode]::Default
                    }
                    "long_short" {
                        $this.ParsingMode = [ParsingMode]::LongShort
                    }
                    default {
                        throw "Unknown parsing mode $($attribute.Value)"
                    }
                }
            }
            default {
                Write-Warning "Unexpected command attribute $($attribute.Name)"
            }
        }
    }

    [string[]] GenerateParser([string]$StringPrefix, [string]$CharType, [NameTransformMode]$NameTransform) {
        $result = @()
        if ($this.CommandName) {
            $result += "    auto name = $StringPrefix`"$($this.CommandName)`";"
        } else {
            $result += "    std::basic_string<$CharType> name;"
            $conversion = if ($CharType -eq "wchar_t") {
                "wstring"
            } else {
                "string"
            }

            $result += "    if (argc > 0) { name = std::filesystem::path{argv[0]}.filename().$conversion(); }"
        }

        $result += "    $($this.TypeName) args{};"
        $result += "    auto parser = ookii::basic_parser_builder<$CharType>{name}"
        if ($this.Description) {
            $result += "        .description($StringPrefix`"$($this.Description)`")"
        }
        
        $result += $this.GenerateParserAttributes($StringPrefix)
        $result += $this.GenerateArguments($StringPrefix, "args.", $NameTransform)

        $result += "        .build();"
        $result += ""
        $result += "    if (parser.parse(argc, argv, options))"
        $result += "    {"
        $result += "        return args;"
        $result += "    }"
        $result += ""
        $result += "    return {};"
        return $result;
    }

    [string[]] GenerateParserAttributes([string]$StringPrefix) {
        $result = @()
        switch ($this.Mode) {
            [ParsingMode]::Default {
                $result += "        .mode(ookii::parsing_mode::default)"
            }
            [ParsingMode]::LongShort {
                $result += "        .mode(ookii::parsing_mode::long_short)"
            }
        }

        if ($this.Prefixes.Length -gt 0) {
            $prefixList = $this.Prefixes -join "`", $StringPrefix`""
            $result += "        .prefixes({ $StringPrefix`"$prefixList`" })"
        }

        if ($this.CaseSensitive) {
            $result += "        .case_sensitive(true)"
        }

        if (-not $this.AllowWhiteSpaceSeparator) {
            $result += "        .allow_whitespace_separator(false)"
        }

        if ($this.AllowDuplicateArguments) {
            $result += "        .allow_duplicate_arguments(true)"
        }

        if ($this.Separator) {
            $result += "        .argument_value_separator($StringPrefix'$($this.Separator)')"
        }

        return $result
    }

    [string[]] GenerateShellCommand([string]$StringPrefix, [NameTransformMode]$NameTransform) {
        $result = @("$($this.TypeName)::$($this.TypeName)($($this.TypeName)::builder_type &builder)")
        if ($this.BaseClass) {
            $result += "    : $($this.BaseClass){builder}"
        }

        $result += "{"
        $result += "    builder"
        $result += $this.GenerateParserAttributes($StringPrefix)
        $result += $this.GenerateArguments($StringPrefix, "this->", $NameTransform)
        $result[-1] += ";"
        $result += "}"
        $result += ""
        $result += ""
        return $result
    }

    [string[]] GenerateArguments([string]$StringPrefix, [string]$FieldPrefix, [NameTransformMode]$NameTransform) {
        $result = @()
        foreach ($arg in $this.Arguments) {
            $arg.GenerateName($NameTransform)
            $result += $arg.GenerateArgument($StringPrefix, $FieldPrefix)
        }

        return $result
    }

    [string] GenerateRegistration([string]$StringPrefix) {
        $name = if ($this.CommandName) {
            "$StringPrefix`"$($this.CommandName)`""
        } else {
            "{}"
        }

        $commandDescription = if ($this.Description) {
            "$StringPrefix`"$($this.Description)`""
        } else {
            "{}"
        }

        return "        .add_command<$($this.TypeName)>($name, $commandDescription)"
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

function Convert-Arguments([string[]]$contents, [string]$argumentsAttributeName = "arguments") {
    $info = [CommandInfo]::new()
    $state = [ParseState]::BeforeStruct
    [bool]$newParagraph = $false
    foreach ($line in $contents) {
        switch ($state) {
            BeforeStruct {
                foreach ($attribute in (Get-CommentAttribute $line)) {
                    if ($state -eq [ParseState]::BeforeStruct) {
                        if ($attribute.Name -ieq $argumentsAttributeName) {
                            $info.CommandName = $attribute.Value
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
                    $info.ParseType($line)
                    $state = [ParseState]::InStruct
                }
            }
            StructComment {
                $comment = Get-Comment $line
                if ($null -ne $comment) {
                    $info.Description += Get-DescriptionLine $comment ([ref]$newParagraph) ($info.Description -gt 0)
                } else {
                    $info.ParseType($line)
                    $state = [ParseState]::InStruct
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
}
