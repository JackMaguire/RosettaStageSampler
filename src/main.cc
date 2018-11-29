#include <Trajectory.hh>

#include <array>
#include <iostream>
enum STAGE_TAG {
  STAGE1 = 0,
  STAGE2,
  STAGE3,
  STAGE4,
  STAGE5,
  STAGE6,
  STAGE7,
  NUM_STAGES
};

void run(
  std::vector< Trajectory > const & trajectories,
  std::array< double, 7 > const & average_runtime_for_stage_in_hours,
  double const max_cpu_hours,
  int const ensemble_size
);

int main(){

  /*
    This is hardcoded to sample the following 7-stage protocol:

    Stage1: Global Dock
    keep: 0.001 to X

    Stage2: Local Dock
    keep: X to X

    Stage3: Relax
    keep X to X

    ...

    Stage7: Relax

   */

  std::vector< Trajectory > trajectories;
  std::array< double, 7 > average_runtime_for_stage_in_hours;

  std::array< double, 4 > max_cpu_hour_options { 1e3, 1e4, 1e5, 1e6 };
  std::array< int, 6 > ensemble_size_options { 1, 5, 10, 50, 100, 500 };

  for( double max_cpu : max_cpu_hour_options ){
    for( int ensemble_size : ensemble_size_options ){
      run( trajectories, average_runtime_for_stage_in_hours, max_cpu, ensemble_size );
    }
  }
}

double estimate_minimum_runtime(
  int num_total_trajectories,
  std::array< double, 7 > const & average_runtime_for_stage_in_hours,
  std::array< double, 6 > const & percents_to_keep_for_stage
){
  std::array< int, 7 > num_trajectories_for_stage;
  num_trajectories_for_stage[ STAGE1 ] = num_total_trajectories;
  for( int i = STAGE2; i <= STAGE7; ++i ){
    num_trajectories_for_stage[ i ] = num_trajectories_for_stage[ i-1 ] * percents_to_keep_for_stage[ i-1 ];
  }

  double total_runtime = 0.0;
  for( int i = STAGE1; i <= STAGE7; ++i ){
    total_runtime += num_trajectories_for_stage[ i ] * average_runtime_for_stage_in_hours[ i ];
  }

  return total_runtime;
}

void run(
  std::vector< Trajectory > const & trajectories,
  std::array< double, 7 > const & average_runtime_for_stage_in_hours,
  double const max_cpu_hours,
  int const ensemble_size
){

  std::array< double, 6 > percents_to_keep { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

  for( percents_to_keep[ STAGE1 ] = 0.0; percents_to_keep[ STAGE1 ] <= 1.0; percents_to_keep[ STAGE1 ] += 0.01 ){

    //zero-out everything upward
    for( int i = STAGE2; i < NUM_STAGES; ++i ) percents_to_keep[ i ] = 0.0;

    for( percents_to_keep[ STAGE2 ] = 0.0; percents_to_keep[ STAGE2 ] <= 1.0; percents_to_keep[ STAGE2 ] += 0.01 ){

      //zero-out everything upward
      for( int i = STAGE3; i < NUM_STAGES; ++i ) percents_to_keep[ i ] = 0.0;

      for( percents_to_keep[ STAGE3 ] = 0.0; percents_to_keep[ STAGE3 ] <= 1.0; percents_to_keep[ STAGE3 ] += 0.01 ){

	//zero-out everything upward
	for( int i = STAGE4; i < NUM_STAGES; ++i ) percents_to_keep[ i ] = 0.0;

	for( percents_to_keep[ STAGE4 ] = 0.0; percents_to_keep[ STAGE4 ] <= 1.0; percents_to_keep[ STAGE4 ] += 0.01 ){

	  //zero-out everything upward
	  for( int i = STAGE5; i < NUM_STAGES; ++i ) percents_to_keep[ i ] = 0.0;

	  for( percents_to_keep[ STAGE5 ] = 0.0; percents_to_keep[ STAGE5 ] <= 1.0; percents_to_keep[ STAGE5 ] += 0.01 ){

	    //zero-out everything upward
	    for( int i = STAGE6; i < NUM_STAGES; ++i ) percents_to_keep[ i ] = 0.0;

	    for( percents_to_keep[ STAGE6 ] = 0.0; percents_to_keep[ STAGE6 ] <= 1.0; percents_to_keep[ STAGE6 ] += 0.01 ){




	    }//percents_to_keep[ STAGE6 ]

	  }//percents_to_keep[ STAGE5 ]

	}//percents_to_keep[ STAGE4 ]

      }//percents_to_keep[ STAGE3 ]

    }//percents_to_keep[ STAGE2 ]

  }//percents_to_keep[ STAGE1 ]

}
