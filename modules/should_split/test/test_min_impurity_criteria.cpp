#include <boost/test/unit_test.hpp>

#include "MinImpurityCriteria.h"

BOOST_AUTO_TEST_SUITE( MinImpurityCriteriaTests )

BOOST_AUTO_TEST_CASE(test_ShouldSplit)
{
    const float minImpurity = 0.2;
    MinImpurityCriteria MinImpurityCriteria(minImpurity);
    BufferCollection bc;
    BOOST_CHECK( !MinImpurityCriteria.ShouldSplit(0, minImpurity, 0, 0, 0, bc, 0, true));
    BOOST_CHECK( MinImpurityCriteria.ShouldSplit(0, minImpurity+0.1, 0, 0, 0, bc, 0, true));
    BOOST_CHECK( !MinImpurityCriteria.ShouldSplit(0, minImpurity-0.1, 0, 0, 0, bc, 0, true));

}

BOOST_AUTO_TEST_CASE(test_Clone)
{
    const float minImpurity = 0.1;
    ShouldSplitCriteriaI* minImpurityCriteria = new MinImpurityCriteria(minImpurity);
    ShouldSplitCriteriaI* clone = minImpurityCriteria->Clone();
    delete minImpurityCriteria;
    BufferCollection bc;
    BOOST_CHECK( !clone->ShouldSplit(0, minImpurity, 0, 0, 0, bc, 0, true));
    BOOST_CHECK( clone->ShouldSplit(0, minImpurity+0.1, 0, 0, 0, bc, 0, true));
    BOOST_CHECK( !clone->ShouldSplit(0, minImpurity-0.1, 0, 0, 0, bc, 0, true));

    delete clone;
}

BOOST_AUTO_TEST_SUITE_END()