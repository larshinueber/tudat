/*    Copyright (c) 2010-2019, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 */
//
//#define BOOST_TEST_DYN_LINK
//#define BOOST_TEST_MAIN

#include <limits>
#include <string>

#include <boost/test/unit_test.hpp>


#include "tudat/basics/testMacros.h"

#include "tudat/io/basicInputOutput.h"

#include "tudat/simulation/environment_setup/body.h"
#include "tudat/astro/observation_models/oneWayRangeObservationModel.h"
#include "tudat/simulation/estimation_setup/createObservationModel.h"
#include "tudat/simulation/environment_setup/defaultBodies.h"
#include "tudat/simulation/environment_setup/createBodies.h"
#include "tudat/simulation/environment_setup/createGroundStations.h"
#include "tudat/astro/basic_astro/unitConversions.h"
#include "tudat/interface/spice/spiceInterface.h"

#include "tudat/astro/ground_stations/transmittingFrequencies.h"
#include "tudat/io/readTrackingTxtFile.h"

//namespace tudat
//{
//namespace unit_tests
//{

using namespace tudat;
using namespace tudat::observation_models;
using namespace tudat::spice_interface;
using namespace tudat::ephemerides;
using namespace tudat::simulation_setup;
using namespace tudat::orbital_element_conversions;
using namespace tudat::coordinate_conversions;
using namespace tudat::unit_conversions;

namespace tio = tudat::input_output;


//BOOST_AUTO_TEST_SUITE(test_doppler_measured_frequency)

//BOOST_AUTO_TEST_CASE(testSimpleCase)


const static std::string juiceDataFile = "/home/dominic/Downloads/Fdets.jui2024.08.20.Yg.r2i.txt";

std::shared_ptr<tio::TrackingTxtFileContents> readJuiceFdetsFile(const std::string& fileName)
{
    std::vector<std::string>
        columnTypes({ "utc_datetime_string", "signal_to_noise_ratio", "normalised_spectral_max", "doppler_measured_frequency_hz", "doppler_noise_hz", });

    auto rawFileContents = tio::createTrackingTxtFileContents(fileName, columnTypes, '#', ", \t");
    rawFileContents->addMetaData(tio::TrackingDataType::file_name, "JUICE Fdets Test File");
    return rawFileContents;
}


int main()
{
    // Load Spice kernels
    std::string kernelsPath = paths::getSpiceKernelPath();
    spice_interface::loadStandardSpiceKernels();
    spice_interface::loadSpiceKernelInTudat( "/home/dominic/Downloads/juice_orbc_000074_230414_310721_v01.bsp" );

    // Define bodies to use.
    std::vector< std::string > bodiesToCreate = { "Earth", "Moon", "Sun", "Jupiter" };
    std::string globalFrameOrigin = "SSB";
    std::string globalFrameOrientation = "J2000";

    // Create bodies settings needed in simulation
    BodyListSettings bodySettings = getDefaultBodySettings(
        bodiesToCreate, globalFrameOrigin, globalFrameOrientation);
    bodySettings.at( "Earth" )->shapeModelSettings = fromSpiceOblateSphericalBodyShapeSettings( );
    bodySettings.at( "Earth" )->rotationModelSettings = gcrsToItrsRotationModelSettings(
        basic_astrodynamics::iau_2006, globalFrameOrientation );
    bodySettings.at( "Earth" )->bodyDeformationSettings.push_back( iers2010TidalBodyShapeDeformation( ) );

    std::shared_ptr<GroundStationSettings> nnorciaSettings = std::make_shared<GroundStationSettings>(
        "NWNORCIA", getCombinedApproximateGroundStationPositions( ).at( "NWNORCIA" ) );
    nnorciaSettings->addStationMotionSettings(
        std::make_shared<LinearGroundStationMotionSettings>(
            ( Eigen::Vector3d( ) << -45.00, 10.00, 47.00 ).finished( ) / 1.0E3 / physical_constants::JULIAN_YEAR, 0.0) );

    std::shared_ptr<GroundStationSettings> yarragadeeSettings = std::make_shared<GroundStationSettings>(
        "YARRAGAD", getCombinedApproximateGroundStationPositions( ).at( "YARRAGAD" ) );
    yarragadeeSettings->addStationMotionSettings(
        std::make_shared<LinearGroundStationMotionSettings>(
            ( Eigen::Vector3d( ) << -47.45, 9.12, 51.76).finished( ) / 1.0E3 / physical_constants::JULIAN_YEAR, 0.0) );

    bodySettings.at( "Earth" )->groundStationSettings.push_back( nnorciaSettings );
    bodySettings.at( "Earth" )->groundStationSettings.push_back( yarragadeeSettings );


    // Create Spacecraft
    const std::string spacecraftName = "JUICE";
    bodiesToCreate.push_back(spacecraftName);
    bodySettings.addSettings(spacecraftName);
    bodySettings.get(spacecraftName)->ephemerisSettings = directSpiceEphemerisSettings("Earth", "J2000", false);

    // Create bodies
    SystemOfBodies bodies = createSystemOfBodies(bodySettings);

    // Set turnaround ratios in spacecraft (ground station)
    std::shared_ptr< system_models::VehicleSystems > vehicleSystems = std::make_shared< system_models::VehicleSystems >();
    vehicleSystems->setTransponderTurnaroundRatio(&getDsnDefaultTurnaroundRatios);
    bodies.at(spacecraftName)->setVehicleSystems(vehicleSystems);

    bodies.processBodyFrameDefinitions();


    // Define link ends for observations.
    LinkEnds linkEnds;
    linkEnds[transmitter] = std::make_pair< std::string, std::string >("Earth", static_cast<std::string>( "NWNORCIA" ) );
    linkEnds[retransmitter] = std::make_pair< std::string, std::string >(static_cast<std::string>(spacecraftName), "");
    linkEnds[receiver] = std::make_pair< std::string, std::string >("Earth", static_cast<std::string>( "YARRAGAD" ) );


    std::shared_ptr< ground_stations::StationFrequencyInterpolator > transmittingFrequencyCalculator =
        std::make_shared< ground_stations::ConstantFrequencyInterpolator >(7180.142419E6);

    bodies.at("Earth")->getGroundStation( "NWNORCIA" )->setTransmittingFrequencyCalculator(transmittingFrequencyCalculator);

    // Create observation settings
    std::shared_ptr< DopplerMeasuredFrequencyObservationModel< double, Time > > dopplerFrequencyObservationModel =
        std::dynamic_pointer_cast<DopplerMeasuredFrequencyObservationModel< double, Time>>(
            ObservationModelCreator< 1, double, Time>::createObservationModel(
                std::make_shared< ObservationModelSettings >(doppler_measured_frequency, linkEnds), bodies));

    // Test observable for both fixed link ends

    Time observationTimeUtc = basic_astrodynamics::timeFromIsoString< Time >( "2024-08-20T17:29:51.500" );
    Time observationTime = earth_orientation::defaultTimeConverter->getCurrentTime< Time >(
        basic_astrodynamics::utc_scale, basic_astrodynamics::tdb_scale, observationTimeUtc,
        getCombinedApproximateGroundStationPositions( ).at( "NWNORCIA" ) );
    std::vector< double > linkEndTimes;
    std::vector< Eigen::Vector6d > linkEndStates;

    // Define link end
    LinkEndType referenceLinkEnd = receiver;

    // Ancillary Settings
    std::shared_ptr< ObservationAncilliarySimulationSettings > ancillarySettings =
        std::make_shared< ObservationAncilliarySimulationSettings >();
    ancillarySettings->setAncilliaryDoubleVectorData(frequency_bands, { x_band, x_band });

    // Compute observables
    double dopplerObservable = dopplerFrequencyObservationModel->computeObservationsWithLinkEndData(
        observationTime, referenceLinkEnd, linkEndTimes, linkEndStates, ancillarySettings)(0);

    std::cout << "TEST: Doppler observable: " << dopplerObservable - 8422.49E6 << std::endl;
    std::cout<< dopplerObservable - 8422.49E6 - 13682699.425314944237<<std::endl;
    // std::dynamic_pointer_cast<OneWayDopplerObservationModel< double, double>>(
    //     uplinkDopplerObservationModel)->setNormalizeWithSpeedOfLight(0);
    // std::dynamic_pointer_cast<OneWayDopplerObservationModel< double, double>>(
    //     downlinkDopplerObservationModel)->setNormalizeWithSpeedOfLight(0);

}

//
//BOOST_AUTO_TEST_SUITE_END()
//
//}
//
//}
