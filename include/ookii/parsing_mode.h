//! \file parsing_mode.h
//! \brief Provides the ookii::parsing_mode enumeration.
#ifndef OOKII_PARSING_MODE_H_
#define OOKII_PARSING_MODE_H_

#pragma once

namespace ookii
{
    //! \brief Indicates what argument parsing rules should be used to interpret the command line.
    //!
    //! To set the parsing mode, use the basic_parser_builder::mode() method.
    enum class parsing_mode
    {
        //! \brief Use the normal Ookii.CommandLine parsing rules.
        default_mode,
        //! \brief Use POSIX-like rules where arguments have separate long and short names.
        long_short,
    };
}

#endif