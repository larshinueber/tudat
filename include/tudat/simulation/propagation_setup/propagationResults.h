//
// Created by dominic on 22-12-22.
//

#ifndef TUDAT_PROPAGATIONRESULTS_H
#define TUDAT_PROPAGATIONRESULTS_H

#include <map>
#include <string>

#include "tudat/simulation/propagation_setup/propagationProcessingSettings.h"
#include "tudat/simulation/propagation_setup/propagationTermination.h"

namespace tudat
{

    namespace propagators
    {
        template<typename StateScalarType = double, typename TimeType = double>
        class SimulationResults
        {
        public:
            SimulationResults() {}

            virtual ~SimulationResults() {}

        };


        template<typename StateScalarType, typename TimeType>
        class SingleArcDynamicsSimulator;


        template<typename StateScalarType = double, typename TimeType = double, int NumberOfStateColumns = 1 >
        class SingleArcSimulationResults : public SimulationResults<StateScalarType, TimeType>
        {
        public:

            SingleArcSimulationResults(const std::map <std::pair<int, int>, std::string> &dependentVariableIds,
                                       const std::map <std::pair<int, int>, std::string> &stateIds,
                                       const std::shared_ptr <SingleArcPropagatorProcessingSettings> &outputSettings) :
                    SimulationResults<StateScalarType, TimeType>(),
                    dependentVariableIds_(dependentVariableIds),
                    stateIds_(stateIds),
                    outputSettings_(outputSettings),
                    propagationIsPerformed_(false),
                    propagationTerminationReason_(
                            std::make_shared<PropagationTerminationDetails>(propagation_never_run)) {
            }

            void reset() {
                equationsOfMotionNumericalSolution_.clear();
                equationsOfMotionNumericalSolutionRaw_.clear();
                dependentVariableHistory_.clear();
                cumulativeComputationTimeHistory_.clear();
                cumulativeNumberOfFunctionEvaluations_.clear();
                propagationIsPerformed_ = false;
                propagationTerminationReason_ = std::make_shared<PropagationTerminationDetails>(propagation_never_run);
            }

            void reset(
                    const std::map <TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, NumberOfStateColumns >>& equationsOfMotionNumericalSolutionRaw,
                    const std::map <TimeType, Eigen::VectorXd>& dependentVariableHistory,
                    const std::map<TimeType, double>& cumulativeComputationTimeHistory,
                    const std::map<TimeType, unsigned int>& cumulativeNumberOfFunctionEvaluations,
                    std::shared_ptr <PropagationTerminationDetails> propagationTerminationReason )
            {
                reset( );
                equationsOfMotionNumericalSolutionRaw_ = equationsOfMotionNumericalSolutionRaw;
                dependentVariableHistory_ = dependentVariableHistory;
                cumulativeComputationTimeHistory_ = cumulativeComputationTimeHistory;
                cumulativeNumberOfFunctionEvaluations_ = cumulativeNumberOfFunctionEvaluations;
                propagationTerminationReason_ = propagationTerminationReason;
            }

            std::map <TimeType, Eigen::Matrix<StateScalarType, Eigen::Dynamic, 1>> &
            getEquationsOfMotionNumericalSolution() {
                return equationsOfMotionNumericalSolution_;
            }

            std::map <TimeType, Eigen::Matrix<StateScalarType, Eigen::Dynamic, NumberOfStateColumns>> &
            getEquationsOfMotionNumericalSolutionRaw() {
                return equationsOfMotionNumericalSolutionRaw_;
            }

            std::map <TimeType, Eigen::VectorXd> &getDependentVariableHistory() {
                return dependentVariableHistory_;
            }

            std::map<TimeType, double> &getCumulativeComputationTimeHistory() {
                return cumulativeComputationTimeHistory_;
            }

            std::map<TimeType, unsigned int> &getCumulativeNumberOfFunctionEvaluations() {
                return cumulativeNumberOfFunctionEvaluations_;
            }

            std::shared_ptr <PropagationTerminationDetails> getPropagationTerminationReason() {
                return propagationTerminationReason_;
            }

            bool integrationCompletedSuccessfully() const {
                return (propagationTerminationReason_->getPropagationTerminationReason() ==
                        termination_condition_reached);
            }


            std::map <std::pair<int, int>, std::string> getDependentVariableId() {
                return dependentVariableIds_;
            }

            std::map <std::pair<int, int>, std::string> getStateIds() {
                return stateIds_;
            }

            std::shared_ptr <SingleArcPropagatorProcessingSettings> getOutputSettings( )
            {
                return outputSettings_;
            }

            bool getPropagationIsPerformed( )
            {
                return propagationIsPerformed_;
            }


        private:

            //! Map of state history of numerically integrated bodies.
            /*!
             *  Map of state history of numerically integrated bodies, i.e. the result of the numerical integration, transformed
             *  into the 'conventional form' (\sa SingleStateTypeDerivative::convertToOutputSolution). Key of map denotes time,
             *  values are concatenated vectors of integrated body states (order defined by propagatorSettings_).
             *  NOTE: this map is empty if clearNumericalSolutions_ is set to true.
             */
            std::map <TimeType, Eigen::Matrix<StateScalarType, Eigen::Dynamic, 1>> equationsOfMotionNumericalSolution_;

            //! Map of state history of numerically integrated bodies.
            /*!
            *  Map of state history of numerically integrated bodies, i.e. the result of the numerical integration, in the
            *  original propagation coordinates. Key of map denotes time, values are concatenated vectors of integrated body
            * states (order defined by propagatorSettings_).
            *  NOTE: this map is empty if clearNumericalSolutions_ is set to true.
            */
            std::map <TimeType, Eigen::Matrix<StateScalarType, Eigen::Dynamic, NumberOfStateColumns>> equationsOfMotionNumericalSolutionRaw_;

            //! Map of dependent variable history that was saved during numerical propagation.
            std::map <TimeType, Eigen::VectorXd> dependentVariableHistory_;

            //! Map of cumulative computation time history that was saved during numerical propagation.
            std::map<TimeType, double> cumulativeComputationTimeHistory_;

            //! Map of cumulative number of function evaluations that was saved during numerical propagation.
            std::map<TimeType, unsigned int> cumulativeNumberOfFunctionEvaluations_;

            //! Map listing starting entry of dependent variables in output vector, along with associated ID.
            std::map <std::pair<int, int>, std::string> dependentVariableIds_;

            std::map <std::pair<int, int>, std::string> stateIds_;

            std::shared_ptr <SingleArcPropagatorProcessingSettings> outputSettings_;

            bool propagationIsPerformed_;


            //! Event that triggered the termination of the propagation
            std::shared_ptr <PropagationTerminationDetails> propagationTerminationReason_;

            friend class SingleArcDynamicsSimulator<StateScalarType, TimeType>;

        };

        template<typename StateScalarType = double, typename TimeType >
        std::shared_ptr< SingleArcSimulationResults<StateScalarType, TimeType, Eigen::Dynamic > > createVariationalSimulationResults(
                const std::shared_ptr< SingleArcSimulationResults<StateScalarType, TimeType, 1 > > simulationResults )
        {
            return std::make_shared< SingleArcSimulationResults<StateScalarType, TimeType, Eigen::Dynamic > >(
                    simulationResults->getDependentVariableId( ),
                    simulationResults->getStateIds(),
                    simulationResults->getOutputSettings( ) );
        }

        template<typename StateScalarType = double, typename TimeType >
        void setSimulationResultsFromVariationalResults(
                const std::shared_ptr< SingleArcSimulationResults<StateScalarType, TimeType, Eigen::Dynamic > > variationalResults,
                const std::shared_ptr< SingleArcSimulationResults<StateScalarType, TimeType, 1 > > simulationResults,
                const int parameterVectorSize_,
                const int stateTransitionMatrixSize_ )
        {
            std::map <TimeType, Eigen::Matrix<StateScalarType, Eigen::Dynamic, 1 >> equationsOfMotionNumericalSolutionRaw_;
            utilities::createVectorBlockMatrixHistory(
                    variationalResults->getEquationsOfMotionNumericalSolutionRaw( ), equationsOfMotionNumericalSolutionRaw_,
                std::make_pair( 0, parameterVectorSize_ ), stateTransitionMatrixSize_ );
            simulationResults->reset(
                    equationsOfMotionNumericalSolutionRaw_,
                    variationalResults->getDependentVariableHistory( ),
                    variationalResults->getCumulativeComputationTimeHistory(),
                    variationalResults->getCumulativeNumberOfFunctionEvaluations( ),
                    variationalResults->getPropagationTerminationReason() );
        }

        template<typename StateScalarType = double, typename TimeType = double, int NumberOfStateColumns = 1 >
        class MultiArcSimulationResults : public SimulationResults<StateScalarType, TimeType> {

        public:
            MultiArcSimulationResults(
                    const std::vector< std::shared_ptr< SingleArcSimulationResults< StateScalarType, TimeType, NumberOfStateColumns > > > singleArcResults ):
                    singleArcResults_( singleArcResults ), propagationIsPerformed_( false ){ }

            ~MultiArcSimulationResults() {}

            bool getPropagationIsPerformed( )
            {
                return propagationIsPerformed_;
            }

            void restartPropagation( )
            {
                propagationIsPerformed_ = false;
                arcStartTimes_.clear( );
                for( unsigned int i = 0; i < singleArcResults_.size( ); i++ )
                {
                    singleArcResults_.at( i )->reset( );
                }
            }

            void setPropagationIsPerformed( )
            {
                propagationIsPerformed_ = true;
                if( arcStartTimes_.size( ) != 0 )
                {
                    throw std::runtime_error( "Error, arc start times not 0 when resetting" );
                }
                for( unsigned int i = 0; i < singleArcResults_.size( ); i++ )
                {
                    arcStartTimes_.push_back( singleArcResults_.at( i )->getEquationsOfMotionNumericalSolution( ).begin( )->first );
                }
            }

            std::vector< std::shared_ptr< SingleArcSimulationResults< StateScalarType, TimeType, NumberOfStateColumns > > > getSingleArcResults( )
            {
                return singleArcResults_;
            }

            std::vector< double > getArcStartTimes( )
            {
                return arcStartTimes_;
            }

            std::vector< std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > > > getConcatenatedEquationsOfMotionResults( )
            {
                std::vector< std::map< TimeType, Eigen::Matrix< StateScalarType, Eigen::Dynamic, 1 > > > concatenatedResults;
                for( unsigned int i = 0; i < singleArcResults_.size( ); i++ )
                {
                    concatenatedResults.push_back( singleArcResults_.at( i )->getEquationsOfMotionNumericalSolution( ) );
                }
                return concatenatedResults;
            }

            std::vector< std::map< TimeType, Eigen::VectorXd > > getConcatenatedDependentVariableResults( )
            {
                std::vector< std::map< TimeType, Eigen::VectorXd > > concatenatedResults;
                for( unsigned int i = 0; i < singleArcResults_.size( ); i++ )
                {
                    concatenatedResults.push_back( singleArcResults_.at( i )->getDependentVariableHistory( ) );
                }
                return concatenatedResults;
            }

            std::vector< std::map< TimeType, double > > getConcatenatedConcatenatedCumulativeComputationTimeHistory( )
            {
                std::vector< std::map< TimeType, double > > concatenatedResults;
                for( unsigned int i = 0; i < singleArcResults_.size( ); i++ )
                {
                    concatenatedResults.push_back( singleArcResults_.at( i )->getCumulativeComputationTimeHistory( ) );
                }
                return concatenatedResults;
            }

            std::vector< std::shared_ptr< PropagationTerminationDetails > > getConcatenatedTerminationReasons( )
            {
                std::vector< std::shared_ptr< PropagationTerminationDetails > > concatenatedResults;
                for( unsigned int i = 0; i < singleArcResults_.size( ); i++ )
                {
                    concatenatedResults.push_back( singleArcResults_.at( i )->getPropagationTerminationReason( ) );
                }
                return concatenatedResults;
            }

        private:
            const std::vector< std::shared_ptr< SingleArcSimulationResults< StateScalarType, TimeType, NumberOfStateColumns > > > singleArcResults_;

            bool propagationIsPerformed_;

            //! List of start times of each arc. NOTE: This list is updated after every propagation.
            std::vector< double > arcStartTimes_;

        };

        template<typename StateScalarType = double, typename TimeType = double, int NumberOfStateColumns = 1 >
        class HybridArcSimulationResults : public SimulationResults<StateScalarType, TimeType> {
            HybridArcSimulationResults(
                    const std::shared_ptr< SingleArcSimulationResults< StateScalarType, TimeType, NumberOfStateColumns > > singleArcResults,
                    const std::shared_ptr< MultiArcSimulationResults< StateScalarType, TimeType, NumberOfStateColumns > > multiArcResults ):
                    singleArcResults_( singleArcResults ), multiArcResults_( multiArcResults ){ }

            ~HybridArcSimulationResults() {}

            std::shared_ptr< SingleArcSimulationResults< StateScalarType, TimeType, NumberOfStateColumns > > singleArcResults_;

            std::shared_ptr< MultiArcSimulationResults< StateScalarType, TimeType, NumberOfStateColumns > > multiArcResults_;
        };

    }

}
#endif //TUDAT_PROPAGATIONRESULTS_H
