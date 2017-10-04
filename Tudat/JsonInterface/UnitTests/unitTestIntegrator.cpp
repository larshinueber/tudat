/*    Copyright (c) 2010-2017, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 */

#define BOOST_TEST_MAIN

#include "unitTestSupport.h"
#include "Tudat/JsonInterface/Mathematics/integrator.h"

namespace tudat
{

namespace unit_tests
{

#define INPUT( filename ) \
    ( json_interface::inputDirectory( ) / boost::filesystem::path( __FILE__ ).stem( ) / filename ).string( )

BOOST_AUTO_TEST_SUITE( test_json_integrator )

// Test 1: integrator types
BOOST_AUTO_TEST_CASE( test_json_integrator_types )
{
    BOOST_CHECK_EQUAL_ENUM( INPUT( "types" ),
                            numerical_integrators::integratorTypes,
                            numerical_integrators::unsupportedIntegratorTypes );
}

// Test 2: Runge-Kutta coefficient sets
BOOST_AUTO_TEST_CASE( test_json_integrator_rksets )
{
    BOOST_CHECK_EQUAL_ENUM( INPUT( "rksets" ),
                            numerical_integrators::rungeKuttaCoefficientSets,
                            numerical_integrators::unsupportedRungeKuttaCoefficientSets );
}

// Test 3: euler
BOOST_AUTO_TEST_CASE( test_json_integrator_euler )
{
    using namespace tudat::numerical_integrators;
    using namespace tudat::json_interface;

    // Create IntegratorSettings from JSON file
    const boost::shared_ptr< IntegratorSettings< double > > fromFileSettings =
            parseJSONFile< boost::shared_ptr< IntegratorSettings< double > > >( INPUT( "euler" ) );

    // Create IntegratorSettings manually
    const AvailableIntegrators integratorType = euler;
    const double initialTime = 3.0;
    const double stepSize = 1.4;
    const boost::shared_ptr< IntegratorSettings< double > > manualSettings =
            boost::make_shared< IntegratorSettings< double > >( integratorType,
                                                                initialTime,
                                                                stepSize );

    // Compare
    BOOST_CHECK_EQUAL_JSON( fromFileSettings, manualSettings );
}

// Test 4: rungeKutta4
BOOST_AUTO_TEST_CASE( test_json_integrator_rungeKutta4 )
{
    using namespace tudat::numerical_integrators;
    using namespace tudat::json_interface;

    // Create IntegratorSettings from JSON file
    const boost::shared_ptr< IntegratorSettings< double > > fromFileSettings =
            parseJSONFile< boost::shared_ptr< IntegratorSettings< double > > >( INPUT( "rungeKutta4" ) );

    // Create IntegratorSettings manually
    const AvailableIntegrators integratorType = rungeKutta4;
    const double initialTime = 3.0;
    const double stepSize = 1.4;
    const unsigned int saveFrequency = 2;
    const bool assessTerminationConditionDuringIntegrationSubsteps = true;
    const boost::shared_ptr< IntegratorSettings< double > > manualSettings =
            boost::make_shared< IntegratorSettings< double > >( integratorType,
                                                                initialTime,
                                                                stepSize,
                                                                saveFrequency,
                                                                assessTerminationConditionDuringIntegrationSubsteps );

    // Compare
    BOOST_CHECK_EQUAL_JSON( fromFileSettings, manualSettings );
}


// Test 5: rungeKuttaVariableStepSize
BOOST_AUTO_TEST_CASE( test_json_integrator_rungeKuttaVariableStepSize )
{
    using namespace tudat::numerical_integrators;
    using namespace tudat::json_interface;

    // Create IntegratorSettings from JSON file
    const boost::shared_ptr< IntegratorSettings< double > > fromFileSettings =
            parseJSONFile< boost::shared_ptr< IntegratorSettings< double > > >( INPUT( "rungeKuttaVariableStepSize" ) );

    // Create IntegratorSettings manually
    const AvailableIntegrators integratorType = rungeKuttaVariableStepSize;
    const double initialTime = -0.3;
    const double initialStepSize = 1.4;
    const RungeKuttaCoefficients::CoefficientSets rungeKuttaCoefficientSet =
            RungeKuttaCoefficients::rungeKuttaFehlberg78;
    const double minimumStepSize = 0.4;
    const double maximumStepSize = 2.4;
    const double relativeErrorTolerance = 1.0E-4;
    const double absoluteErrorTolerance = 1.0E-2;
    const double safetyFactorForNextStepSize = 2.0;
    const double maximumFactorIncreaseForNextStepSize = 10.0;
    const double minimumFactorDecreaseForNextStepSize = 0.1;
    const boost::shared_ptr< IntegratorSettings< double > > manualSettings =
            boost::make_shared< RungeKuttaVariableStepSizeSettings< double > >( integratorType,
                                                                                initialTime,
                                                                                initialStepSize,
                                                                                rungeKuttaCoefficientSet,
                                                                                minimumStepSize,
                                                                                maximumStepSize,
                                                                                relativeErrorTolerance,
                                                                                absoluteErrorTolerance,
                                                                                1,
                                                                                false,
                                                                                safetyFactorForNextStepSize,
                                                                                maximumFactorIncreaseForNextStepSize,
                                                                                minimumFactorDecreaseForNextStepSize );

    // Compare
    BOOST_CHECK_EQUAL_JSON( fromFileSettings, manualSettings );
}


BOOST_AUTO_TEST_SUITE_END( )

} // namespace unit_tests

} // namespace tudat
