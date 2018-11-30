#include <Trajectory.hh>
#include <global.hh>

#include <array>
#include <cassert>
#include <iostream>

#define CHECK_FOR_DEAD_END_ELIMINATION double const estimated_runtime = estimate_minimum_runtime( trajectories.size(), average_runtime_for_stage_in_hours, fractions_to_keep ); if( estimated_runtime > max_cpu_hours ) break

struct run_results {
  int num_trajectories;
  std::array< double, 6 > fraction_to_keep_for_each_stage;
};

run_results run(
  std::vector< Trajectory > const & trajectories,
  std::array< double, 7 > const & average_runtime_for_stage_in_hours,
  double const max_cpu_hours,
  int const ensemble_size
);

std::array< double, 7 > inspect(
  std::vector< Trajectory > const & trajectories
){
  std::array< double, 7 > mean_time_for_stage = { 0, 0, 0, 0, 0, 0, 0 };
  for( Trajectory const & t : trajectories ){
    for( int i = 0; i < 7; ++i ){
      mean_time_for_stage[ i ] += t.cpu_hours_for_stage[ i ];
    }
  }

  for( int i = 0; i < 7; ++i ){
    mean_time_for_stage[ i ] /= double( trajectories.size() );
    std::cout << "mean time for stage " << i << ": " << mean_time_for_stage[ i ] << " hours" << std::endl;
  }

  return mean_time_for_stage;
}

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

  std::vector< Trajectory > trajectories = load_trajectories( "temp_scores_for_development.txt" );
  std::array< double, 7 > average_runtime_for_stage_in_hours = inspect( trajectories );

  std::array< double, 4 > max_cpu_hour_options { 1e3, 1e4, 1e5, 1e6 };
  std::array< int, 6 > ensemble_size_options { 1, 5, 10, 50, 100, 500 };

  for( double max_cpu : max_cpu_hour_options ){
    for( int ensemble_size : ensemble_size_options ){
      run_results const results =
	run( trajectories, average_runtime_for_stage_in_hours, max_cpu, ensemble_size );

      std::cout << "For max_cpu = " << max_cpu << " and ensemble_size = " << ensemble_size << " best conditions are:" << std::endl;
      std::cout << "\tnum_trajectories = " << results.num_trajectories << std::endl;
      for( int i = STAGE1; i < STAGE7; ++i ){
	std::cout << "\tfrac to keep after stage " << i << " = " << results.fraction_to_keep_for_each_stage[ i ] << std::endl;
      }
    }
  }
}

[[nodiscard]]
double estimate_minimum_runtime(
  int num_total_trajectories,
  std::array< double, 7 > const & average_runtime_for_stage_in_hours,
  std::array< double, 6 > const & fractions_to_keep_for_stage
){
  std::array< int, 7 > num_trajectories_for_stage;
  num_trajectories_for_stage[ STAGE1 ] = num_total_trajectories;
  for( int i = STAGE2; i <= STAGE7; ++i ){
    num_trajectories_for_stage[ i ] = num_trajectories_for_stage[ i-1 ] * fractions_to_keep_for_stage[ i-1 ];
  }

  double total_runtime = 0.0;
  for( int i = STAGE1; i <= STAGE7; ++i ){
    total_runtime += num_trajectories_for_stage[ i ] * average_runtime_for_stage_in_hours[ i ];
  }

  return total_runtime;
}

std::vector< Trajectory >
get_final_trajectories(
  std::vector< Trajectory > const & all_trajectories,
  int const num_initial_trajectories,
  std::array< double, 6 > const & fractions_to_keep
) {
  assert( num_initial_trajectories <= all_trajectories.size() );
  std::vector< Trajectory > trajectories;
  std::copy(
    all_trajectories.begin(),
    all_trajectories.begin() + num_initial_trajectories,
    trajectories.begin()
  );

  return trajectories;
}

run_results
run(
  std::vector< Trajectory > const & trajectories,
  std::array< double, 7 > const & average_runtime_for_stage_in_hours,
  double const max_cpu_hours,
  int const ensemble_size
){

  std::array< double, 6 > const step_sizes = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 };

  std::array< double, 4 > num_trajectories { 1e5, 1e6, 1e7, 1e8 };
  int best_num_trajectories;

  std::array< double, 6 > fractions_to_keep { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  std::array< double, 6 > best_fractions_to_keep;

  for( int n_traj : num_trajectories ){

    for(
      fractions_to_keep[ STAGE1 ] = 0.01;
      fractions_to_keep[ STAGE1 ] <= 1.0;
      fractions_to_keep[ STAGE1 ] += step_sizes[ STAGE1 ]
    ){

      //zero-out everything upward
      for( int i = STAGE2; i < NUM_STAGES; ++i )
	fractions_to_keep[ i ] = 0.0;
      CHECK_FOR_DEAD_END_ELIMINATION;

      for(
	fractions_to_keep[ STAGE2 ] = 0.01;
	fractions_to_keep[ STAGE2 ] <= 1.0;
	fractions_to_keep[ STAGE2 ] += step_sizes[ STAGE2 ]
      ){

	//zero-out everything upward
	for( int i = STAGE3; i < NUM_STAGES; ++i )
	  fractions_to_keep[ i ] = 0.0;
	CHECK_FOR_DEAD_END_ELIMINATION;

	for(
	  fractions_to_keep[ STAGE3 ] = 0.01;
	  fractions_to_keep[ STAGE3 ] <= 1.0;
	  fractions_to_keep[ STAGE3 ] += step_sizes[ STAGE3 ]
	){

	  //zero-out everything upward
	  for( int i = STAGE4; i < NUM_STAGES; ++i )
	    fractions_to_keep[ i ] = 0.0;
	  CHECK_FOR_DEAD_END_ELIMINATION;

	  for(
	    fractions_to_keep[ STAGE4 ] = 0.01;
	    fractions_to_keep[ STAGE4 ] <= 1.0;
	    fractions_to_keep[ STAGE4 ] += step_sizes[ STAGE4 ]
	  ){

	    //zero-out everything upward
	    for( int i = STAGE5; i < NUM_STAGES; ++i )
	      fractions_to_keep[ i ] = 0.0;
	    CHECK_FOR_DEAD_END_ELIMINATION;

	    for(
	      fractions_to_keep[ STAGE5 ] = 0.01;
	      fractions_to_keep[ STAGE5 ] <= 1.0;
	      fractions_to_keep[ STAGE5 ] += step_sizes[ STAGE5 ]
	    ){

	      //zero-out everything upward
	      fractions_to_keep[ STAGE6 ] = 0.0;
	      CHECK_FOR_DEAD_END_ELIMINATION;

	      for(
		fractions_to_keep[ STAGE6 ] = 0.01;
		fractions_to_keep[ STAGE6 ] <= 1.0;
		fractions_to_keep[ STAGE6 ] += step_sizes[ STAGE6 ]
	      ){
		CHECK_FOR_DEAD_END_ELIMINATION;

		//evaluate!

	      }//fractions_to_keep[ STAGE6 ]

	    }//fractions_to_keep[ STAGE5 ]

	  }//fractions_to_keep[ STAGE4 ]

	}//fractions_to_keep[ STAGE3 ]

      }//fractions_to_keep[ STAGE2 ]

    }//fractions_to_keep[ STAGE1 ]

  }//ntraj

  run_results results;
  results.num_trajectories = best_num_trajectories;
  results.fraction_to_keep_for_each_stage = best_fractions_to_keep;
  return results;
}
