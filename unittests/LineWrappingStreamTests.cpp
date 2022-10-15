#include "common.h"
#include "framework.h"
#include "unicode.h"
#include <ookii/line_wrapping_stream.h>
using namespace std;
using namespace ookii;

class LineWrappingStreamTests : public test::TestClass
{
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
};

TEST_CLASS_REGISTRATION(LineWrappingStreamTests);