
/*    Copyright (c) 2010-2015, Delft University of Technology
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without modification, are
 *    permitted provided that the following conditions are met:
 *      - Redistributions of source code must retain the above copyright notice, this list of
 *        conditions and the following disclaimer.
 *      - Redistributions in binary form must reproduce the above copyright notice, this list of
 *        conditions and the following disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *      - Neither the name of the Delft University of Technology nor the names of its contributors
 *        may be used to endorse or promote products derived from this software without specific
 *        prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
 *    OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *    COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 *    GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *    AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *    OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *    Changelog
 *      YYMMDD    Author            Comment
 *      120207    K. Kumar          File created.
 *      120511    K. Kumar          Added unit tests for writeDataMapToTextFile functions.
 *      120511    K. Kumar          Added writeMapDataToFile() template functions.
 *      120712    B. Tong Minh      Updated unit tests to work with iterator-based
 *                                  writeMapDataToFile() functions; updated input file parse
 *                                  function to readLinesFromFile().
 *      130110    K. Kumar          Added unit tests for printInFormattedScientificNotation()
 *                                  function.
 *
 *    References
 *      Press W.H., et al. Numerical Recipes in C++: The Art of Scientific Computing. Cambridge
 *          University Press, February 2002.
 *
 *    Notes
 *
 */

#define BOOST_TEST_MAIN

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/lexical_cast.hpp>
//  #include <boost/test/unit_test.hpp>
#include <boost/throw_exception.hpp>

#include <Eigen/Core>

#include "Tudat/Basics/testMacros.h"
#include "Tudat/InputOutput/streamFilters.h"

#include "Tudat/InputOutput/basicInputOutput.h"
#include "Tudat/InputOutput/aerodynamicCoefficientReader.h"

namespace tudat
{
namespace unit_tests
{

BOOST_AUTO_TEST_SUITE( test_multi_array_reader )

// Test if multi-array file reader is working correctly
BOOST_AUTO_TEST_CASE( testMultiArrayReader )
{
    // Test functionality of 3-dimensional multi-array reader
    {
        std::string fileName = tudat::input_output::getTudatRootPath( )
                + "/Astrodynamics/Aerodynamics/UnitTests/dCDwTest.txt";

        std::vector< std::string > files;
        files.push_back( tudat::input_output::getTudatRootPath( ) + "/Astrodynamics/Propulsion/UnitTests/Tmax_test.txt" );
        files.push_back( tudat::input_output::getTudatRootPath( ) + "/Astrodynamics/Propulsion/UnitTests/Isp_test.txt" );
        files.push_back( tudat::input_output::getTudatRootPath( ) + "/Astrodynamics/Propulsion/UnitTests/Isp_test.txt" );


        std::pair< boost::multi_array< Eigen::Vector3d, 2 >, std::vector< std::vector< double > > > outputArray =
                input_output::AerodynamicCoefficientReader< 2 >::readAerodynamicCoefficients( files );
        std::cout<<outputArray.first.shape( )[ 0 ]<<" "<<outputArray.first.shape( )[ 1 ]<<std::endl;


        std::cout<<outputArray.first[ 0 ][ 0 ].transpose( )<<std::endl;
        std::cout<<outputArray.first[ 1 ][ 0 ].transpose( )<<std::endl;
        std::cout<<outputArray.first[ 1 ][ 1 ].transpose( )<<std::endl;


      }
}

BOOST_AUTO_TEST_SUITE_END( )

} // namespace unit_tests
} // namespace tudat


