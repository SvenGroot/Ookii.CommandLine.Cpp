#include "common.h"
#include "framework.h"
#include "unicode.h"
#include <ookii/line_wrapping_stream.h>
using namespace std;
using namespace ookii;

class LineWrappingStreamTests : public test::TestClass
{
public:
    TEST_CLASS(LineWrappingStreamTests);

    TEST_METHOD(TestWrapping)
    {
        tstringstream stream;
        tline_wrapping_stream wrapStream{stream, 40};
        wrapStream << c_input << endl;
        VERIFY_EQUAL(c_wrapResult, stream.str());
    }

    TEST_METHOD(TestIndent)
    {
        tstringstream stream;
        tline_wrapping_stream wrapStream{stream, 40};
        wrapStream << set_indent(4) << c_input << endl;
        wrapStream << TEXT("Indented.") << endl;
        wrapStream << reset_indent << TEXT("Not indented.") << endl;
        wrapStream << TEXT("Indented again.") << endl;
        wrapStream << set_indent(6) << TEXT("Changed indent.") << endl;
        VERIFY_EQUAL(c_indentResult, stream.str());
    }

    TEST_METHOD(TestIndentNoLimit)
    {
        tstringstream stream;
        tline_wrapping_stream wrapStream{stream, 0};
        wrapStream << set_indent(4) << c_input << endl;
        wrapStream << TEXT("Indented.") << endl;
        wrapStream << reset_indent << TEXT("Not indented.") << endl;
        wrapStream << TEXT("Indented again.") << endl;
        wrapStream << set_indent(6) << TEXT("Changed indent.") << endl;
        VERIFY_EQUAL(c_indentNoLimitResult, stream.str());
    }

    TEST_METHOD(TestWrappingNoSpace)
    {
        tstringstream stream;
        tline_wrapping_stream wrapStream{stream, 40};
        wrapStream << c_noSpaceInput << endl;
        VERIFY_EQUAL(c_noSpaceWrapResult, stream.str());
    }

    TEST_METHOD(TestIndentNoSpace)
    {
        tstringstream stream;
        tline_wrapping_stream wrapStream{stream, 40};
        wrapStream << set_indent(4) << c_noSpaceInput << endl;
        VERIFY_EQUAL(c_noSpaceIndentResult, stream.str());
    }

    TEST_METHOD(TestIndentBlankLine)
    {
        tstringstream stream;
        tline_wrapping_stream wrapStream{stream, 40};
        wrapStream << set_indent(4) << c_blankLineInput << endl;
        VERIFY_EQUAL(c_blankLineIndentResult, stream.str());
    }

    TEST_METHOD(TestIndentBlankLineNoLimit)
    {
        tstringstream stream;
        tline_wrapping_stream wrapStream{stream, 0};
        wrapStream << set_indent(4) << c_blankLineInput << endl;
        VERIFY_EQUAL(c_blankLineNoLimitIndentResult, stream.str());
    }

    TEST_METHOD(TestMove)
    {
        tstringstream stream;
        tline_wrapping_stream wrapStreamSource{stream, 40};
        tline_wrapping_stream wrapStream{std::move(wrapStreamSource)};
        wrapStream << set_indent(4) << c_input << endl;
        wrapStream << TEXT("Indented.") << endl;
        wrapStream << reset_indent << TEXT("Not indented.") << endl;
        wrapStream << TEXT("Indented again.") << endl;
        wrapStream << set_indent(6) << TEXT("Changed indent.") << endl;
        VERIFY_EQUAL(c_indentResult, stream.str());
    }

    TEST_METHOD(TestSkipFormatting)
    {
        TestWrite(c_inputFormatting, c_expectedFormatting, 80, 8);
        TestWrite(c_inputLongFormatting, c_expectedLongFormatting, 80, 8);
    }

    TEST_METHOD(TestSkipFormattingNoMaximum)
    {
        tstringstream stream;
        tline_wrapping_stream wrapStream{stream, 0};
        wrapStream << c_inputFormatting;
        VERIFY_EQUAL(c_inputFormatting, stream.str());
    }

private:
    void TestWrite(tstring_view input, tstring_view expected, size_t max_length, size_t indent)
    {
        tstringstream stream;
        tline_wrapping_stream wrapStream{stream, max_length};
        wrapStream << set_indent(indent) << input << endl;
        VERIFY_EQUAL(expected, stream.str());
    }

    static constexpr tstring_view c_input{TEXT("Where do you stand so far?\nNow is a good time to check out the progress you've made with your retirement savings, take a look at what you've contributed so far this year, and update your other financial goals as well.")};
    static constexpr tstring_view c_wrapResult{TEXT(R"(Where do you stand so far?
Now is a good time to check out the
progress you've made with your
retirement savings, take a look at what
you've contributed so far this year, and
update your other financial goals as
well.
)")};

    static constexpr tstring_view c_indentResult{TEXT(R"(Where do you stand so far?
    Now is a good time to check out the
    progress you've made with your
    retirement savings, take a look at
    what you've contributed so far this
    year, and update your other
    financial goals as well.
    Indented.
Not indented.
    Indented again.
      Changed indent.
)")};

    static constexpr tstring_view c_indentNoLimitResult{TEXT(R"(Where do you stand so far?
    Now is a good time to check out the progress you've made with your retirement savings, take a look at what you've contributed so far this year, and update your other financial goals as well.
    Indented.
Not indented.
    Indented again.
      Changed indent.
)")};

    static constexpr tstring_view c_noSpaceInput{TEXT("Wheredoyoustandsofar?Nowisagoodtimetocheckouttheprogressyou'vemadewithyour retirementsavings,takealookatwhatyou'vecontributedsofarthisyear,andupdateyourotherfinancialgoalsaswell.")};
    static constexpr tstring_view c_noSpaceWrapResult{TEXT(R"(Wheredoyoustandsofar?Nowisagoodtimetoche
ckouttheprogressyou'vemadewithyour
retirementsavings,takealookatwhatyou'vec
ontributedsofarthisyear,andupdateyouroth
erfinancialgoalsaswell.
)")};

    static constexpr tstring_view c_noSpaceIndentResult{TEXT(R"(Wheredoyoustandsofar?Nowisagoodtimetoche
    ckouttheprogressyou'vemadewithyour
    retirementsavings,takealookatwhatyou
    'vecontributedsofarthisyear,andupdat
    eyourotherfinancialgoalsaswell.
)")};

    static constexpr tstring_view c_blankLineInput{TEXT("Where do you stand so far?\n\nNow is a good time to check out the progress you've made with your retirement savings, take a look at what you've contributed so far this year, and update your other financial goals as well.")};
    static constexpr tstring_view c_blankLineIndentResult{TEXT(R"(Where do you stand so far?

    Now is a good time to check out the
    progress you've made with your
    retirement savings, take a look at
    what you've contributed so far this
    year, and update your other
    financial goals as well.
)")};

    static constexpr tstring_view c_blankLineNoLimitIndentResult{TEXT(R"(Where do you stand so far?

    Now is a good time to check out the progress you've made with your retirement savings, take a look at what you've contributed so far this year, and update your other financial goals as well.
)")};

    static constexpr tstring_view c_inputFormatting{TEXT("\x1b[34mLorem \x1b[34mipsum \x1b[34mdolor \x1b[34msit \x1b[34mamet, \x1b[34mconsectetur \x1b[34madipiscing \x1b[34melit, \x1b]0;new title\x1b\\sed do \x1b]0;new title2\aeiusmod \x1b(Btempor\x1bH incididunt\nut labore et dolore magna aliqua. Donec\x1b[38;2;1;2;3m adipiscing tristique risus nec feugiat in fermentum.\x1b[0m")};

    static constexpr tstring_view c_expectedFormatting{TEXT(R"([34mLorem [34mipsum [34mdolor [34msit [34mamet, [34mconsectetur [34madipiscing [34melit, ]0;new title\sed do ]0;new title2eiusmod (BtemporH
        incididunt
        ut labore et dolore magna aliqua. Donec[38;2;1;2;3m adipiscing tristique risus nec
        feugiat in fermentum.[0m
)")};

    static constexpr tstring_view c_inputLongFormatting{TEXT("Lorem ipsum dolor sit amet, consectetur\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m\x1b[34m adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Donec adipiscing tristique risus nec feugiat in fermentum.")};

    static constexpr tstring_view c_expectedLongFormatting{TEXT(R"(Lorem ipsum dolor sit amet, consectetur[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m[34m adipiscing elit, sed do eiusmod tempor
        incididunt ut labore et dolore magna aliqua. Donec adipiscing tristique
        risus nec feugiat in fermentum.
)")};

};

TEST_CLASS_REGISTRATION(LineWrappingStreamTests);