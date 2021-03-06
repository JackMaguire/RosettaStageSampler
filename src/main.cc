#include <Trajectory.hh>
#include <global.hh>
#include <util.hh>

#include <array>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <chrono>

#define CHECK_FOR_DEAD_END_ELIMINATION double const estimated_runtime = estimate_minimum_runtime_in_hours( trajectories.size(), average_runtime_for_stage_in_hours, fractions_to_keep, true ); if( estimated_runtime > max_cpu_hours ) break;

struct run_results {
  int num_trajectories;
  std::array< double, 6 > fraction_to_keep_for_each_stage;
  double score;
};

run_results run(
  std::vector< Trajectory > const & trajectories,
  std::array< double, 7 > const & average_runtime_for_stage_in_hours,
  double const max_cpu_hours,
  int const ensemble_size,
  std::ofstream & log_stream
);

int main(){

  /*
    This is hardcoded to sample the following 7-stage protocol:

    Stage1: Global Dock
    positive scores are better

    Stage2: Local Dock
    positive scores are better

    Stage3: Relax
    negative scores are better

    ...

    Stage7: Relax

   */


  auto my_pair = load_trajectories( "../run/trajectories.shuf.dat" );
  std::vector< Trajectory > const trajectories = std::move( my_pair.first );
  std::array< double, 7 > const average_runtime_for_stage_in_hours = my_pair.second;

  for( int i = STAGE1; i <= STAGE7; ++i ){
    std::cout << "average time for stage " << i << " is " << average_runtime_for_stage_in_hours[ i ] << "hours\n";
  }

  constexpr std::array< double, 4 > max_cpu_hour_options { 5e3, 1e4, 5e4, 1e5 };
  constexpr std::array< int, 6 > ensemble_size_options { 1, 5, 10, 50, 100, 500 };

  constexpr int num_combos = max_cpu_hour_options.size() * ensemble_size_options.size();

  std::vector< run_results > all_results( num_combos );


#pragma omp parallel for shared( all_results ) schedule( dynamic )
  for( int i = num_combos - 1; i >= 0; --i ){
    int const cpu_ind = i / ensemble_size_options.size();
    int const ensemble_ind = i % ensemble_size_options.size();
    double const max_cpu = max_cpu_hour_options[ cpu_ind ];
    int const ensemble_size = ensemble_size_options[ ensemble_ind ];


    std::ofstream log_stream;
    log_stream.open( std::to_string( i ) + ".log" );    
    auto start = std::chrono::system_clock::now();
    all_results[ i ] = run( trajectories, average_runtime_for_stage_in_hours, max_cpu, ensemble_size, log_stream );
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    log_stream.close();

    std::cout << "For max_cpu = " << max_cpu << " and ensemble_size = " << ensemble_size << " best conditions are:" << "\n";
    std::cout << "\tnum_trajectories = " << all_results[ i ].num_trajectories << "\n";
    for( int i = STAGE1; i < STAGE7; ++i ){
      std::cout << "\tfrac to keep after stage " << i << " = " << all_results[ i ].fraction_to_keep_for_each_stage[ i ] << "\n";
    }
    std::cout << "\t with score " << all_results[ i ].score << "\n";
    std::cout << "\t over " << elapsed_seconds.count() << "seconds" << "\n";

    std::ofstream out_stream;
    out_stream.open( std::to_string( i ) + ".dat" );
    out_stream << "For max_cpu = " << max_cpu << " and ensemble_size = " << ensemble_size << " best conditions are:" << "\n";
    out_stream << "\tnum_trajectories = " << all_results[ i ].num_trajectories << "\n";
    for( int i = STAGE1; i < STAGE7; ++i ){
      out_stream << "\tfrac to keep after stage " << i << " = " << all_results[ i ].fraction_to_keep_for_each_stage[ i ] << "\n";
    }
    out_stream << "\t with score " << all_results[ i ].score << "\n";
    out_stream << "\t over " << elapsed_seconds.count() << "seconds" << "\n";
    out_stream.close();
  }

}//main

double
determine_fraction_of_real_survivors( std::vector< Trajectory > & trajectories, double const fraction_to_keep_for_stage1 ){
  int const num_to_keep = trajectories.size() * fraction_to_keep_for_stage1;
  if( num_to_keep < 1.0 ){
    std::partial_sort(
      trajectories.begin(),//begin
      trajectories.begin() + num_to_keep,//middle
      trajectories.end(),//end
      ReverseTrajectorySorter( STAGE1 )//comparator
    );
  }
  int num_valid = 0;
  for( int i=0; i < num_to_keep; ++i ){
    if( trajectories[ i ].score_at_the_end_of_stage[ STAGE1 ] > -9000 ){
      ++num_valid;
    }
  }

  double const answer = double( num_valid ) / double( num_to_keep );
  if( answer > 1.0 ) std::cout << "\t" << answer << std::endl;
  assert( answer <= 1.0 );
  return answer;
}

run_results
run(
  std::vector< Trajectory > const & all_trajectories,
  std::array< double, 7 > const & average_runtime_for_stage_in_hours,
  double const max_cpu_hours,
  int const ensemble_size,
  std::ofstream & log_stream
){

  //constexpr std::array< double, 6 > step_sizes = { 0.01, 0.01, 0.01, 0.01, 0.01, 0.01 };
  constexpr std::array< double, 6 > step_sizes = { 0.01, 0.025, 0.033, 0.04, 0.04, 0.05 };


  //constexpr std::array< double, 9 > num_trajectories { 1000, 5000, 10000, 15000, 20000, 25000, 50000, 75000, 94800 };
  constexpr std::array< double, 12 > num_trajectories { 1000, 2000, 3000, 4000, 5000, 7500, 10000, 15000, 20000, 25000, 50000, 75000 };


  int best_num_trajectories = 0;
  double best_score = 99999;

  std::array< double, 6 > fractions_to_keep { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  std::array< double, 6 > best_fractions_to_keep{ 1,1,1,1,1,1 };

  std::array< double, 10 > best_score_for_stage = { 999, 999, 999, 999, 999, 999, 999, 999};

  log_stream << "ntraj\tnum_repeats\t";
  for( int i=1; i<=6; ++i ){
    log_stream << "fractions_to_keep_after_stage" << i << "\t";
  }
  log_stream << "score" << std::endl;

  
  for( int n_traj : num_trajectories ){
    std::vector< Trajectory > trajectories( all_trajectories.begin(), all_trajectories.begin() + n_traj );

    for(
      fractions_to_keep[ STAGE1 ] = 0.01;
      fractions_to_keep[ STAGE1 ] <= 0.6;
      fractions_to_keep[ STAGE1 ] += step_sizes[ STAGE1 ]
    ){

      //zero-out everything upward
      for( int i = STAGE2; i < NUM_STAGES; ++i )
	fractions_to_keep[ i ] = 0.0;
      CHECK_FOR_DEAD_END_ELIMINATION;

      double const stage_2_max =
	determine_fraction_of_real_survivors( trajectories, fractions_to_keep[ STAGE1 ] );

      for(
	fractions_to_keep[ STAGE2 ] = 0.01;
	fractions_to_keep[ STAGE2 ] <= stage_2_max;
	fractions_to_keep[ STAGE2 ] += step_sizes[ STAGE2 ]
      ){

	//zero-out everything upward
	for( int i = STAGE3; i < NUM_STAGES; ++i )
	  fractions_to_keep[ i ] = 0.0;
	CHECK_FOR_DEAD_END_ELIMINATION;

	std::stringstream ss;

	{//evaluate!
	  fractions_to_keep[ STAGE3 ] = 1.0;
	  fractions_to_keep[ STAGE4 ] = 1.0;
	  fractions_to_keep[ STAGE5 ] = 1.0;
	  fractions_to_keep[ STAGE6 ] = 1.0;
	  std::vector< Trajectory > survivors =
	    get_final_trajectories(
	      trajectories,
	      fractions_to_keep
	    );
	  fractions_to_keep[ STAGE6 ] = -1.0;
	  fractions_to_keep[ STAGE5 ] = -1.0;
	  fractions_to_keep[ STAGE4 ] = -1.0;
	  fractions_to_keep[ STAGE3 ] = -1.0;
	  double const score = evaluate( survivors, ensemble_size, STAGE3 );

	  if( score < best_score_for_stage[ STAGE3 ] ){
	    best_score_for_stage[ STAGE3 ] = score;
	    ss << n_traj << "\t1\t";
	    for( double d : fractions_to_keep ){
	      ss << d << "\t";
	    }
	    ss << score << "\n";
	  }

	  if( score < best_score ){
	    best_score = score;
	    best_num_trajectories = n_traj;
	    best_fractions_to_keep = fractions_to_keep;
	  }
	}

	for(
	  fractions_to_keep[ STAGE3 ] = 0.01;
	  fractions_to_keep[ STAGE3 ] <= 1.0;
	  fractions_to_keep[ STAGE3 ] += step_sizes[ STAGE3 ]
	){

	  //zero-out everything upward
	  for( int i = STAGE4; i < NUM_STAGES; ++i )
	    fractions_to_keep[ i ] = 0.0;
	  CHECK_FOR_DEAD_END_ELIMINATION;

	  {//evaluate!
	    fractions_to_keep[ STAGE4 ] = 1.0;
	    fractions_to_keep[ STAGE5 ] = 1.0;
	    fractions_to_keep[ STAGE6 ] = 1.0;
	    std::vector< Trajectory > survivors =
	      get_final_trajectories(
		trajectories,
		fractions_to_keep
	      );
	    fractions_to_keep[ STAGE6 ] = -1.0;
	    fractions_to_keep[ STAGE5 ] = -1.0;
	    fractions_to_keep[ STAGE4 ] = -1.0;
	    double const score = evaluate( survivors, ensemble_size, STAGE4 );

	    if( score < best_score_for_stage[ STAGE4 ] ){
	      best_score_for_stage[ STAGE4 ] = score;
	      ss << n_traj << "\t2\t";
	      for( double d : fractions_to_keep ){
		ss << d << "\t";
	      }
	      ss << score << "\n";
	    }

	    if( score < best_score ){
	      best_score = score;
	      best_num_trajectories = n_traj;
	      best_fractions_to_keep = fractions_to_keep;
	    }
	  }


	  for(
	    fractions_to_keep[ STAGE4 ] = 0.01;
	    fractions_to_keep[ STAGE4 ] <= 1.0;
	    fractions_to_keep[ STAGE4 ] += step_sizes[ STAGE4 ]
	  ){

	    //zero-out everything upward
	    for( int i = STAGE5; i < NUM_STAGES; ++i )
	      fractions_to_keep[ i ] = 0.0;
	    CHECK_FOR_DEAD_END_ELIMINATION;

	    {//evaluate!
	      fractions_to_keep[ STAGE5 ] = 1.0;
	      fractions_to_keep[ STAGE6 ] = 1.0;
	      std::vector< Trajectory > survivors =
		get_final_trajectories(
		  trajectories,
		  fractions_to_keep
		);
	      fractions_to_keep[ STAGE6 ] = -1.0;
	      fractions_to_keep[ STAGE5 ] = -1.0;
	      double const score = evaluate( survivors, ensemble_size, STAGE5 );

	      if( score < best_score_for_stage[ STAGE5 ] ){
		best_score_for_stage[ STAGE5 ] = score;
		ss << n_traj << "\t3\t";
		for( double d : fractions_to_keep ){
		  ss << d << "\t";
		}
		ss << score << "\n";
	      }

	      if( score < best_score ){
		best_score = score;
		best_num_trajectories = n_traj;
		best_fractions_to_keep = fractions_to_keep;
	      }
	    }


	    for(
	      fractions_to_keep[ STAGE5 ] = 0.01;
	      fractions_to_keep[ STAGE5 ] <= 1.0;
	      fractions_to_keep[ STAGE5 ] += step_sizes[ STAGE5 ]
	    ){

	      //zero-out everything upward
	      fractions_to_keep[ STAGE6 ] = 0.0;
	      CHECK_FOR_DEAD_END_ELIMINATION;

	      {//evaluate!
		fractions_to_keep[ STAGE6 ] = 1.0;
		std::vector< Trajectory > survivors =
		  get_final_trajectories(
		    trajectories,
		    fractions_to_keep
		  );
		fractions_to_keep[ STAGE6 ] = -1.0;
		double const score = evaluate( survivors, ensemble_size, STAGE6 );

		if( score < best_score_for_stage[ STAGE6 ] ){
		  best_score_for_stage[ STAGE6 ] = score;
		  ss << n_traj << "\t4\t";
		  for( double d : fractions_to_keep ){
		    ss << d << "\t";
		  }
		  ss << score << "\n";
		}

		if( score < best_score ){
		  best_score = score;
		  best_num_trajectories = n_traj;
		  best_fractions_to_keep = fractions_to_keep;
		}
	      }


	      for(
		fractions_to_keep[ STAGE6 ] = 0.01;
		fractions_to_keep[ STAGE6 ] <= 1.0;
		fractions_to_keep[ STAGE6 ] += step_sizes[ STAGE6 ]
	      ){
		CHECK_FOR_DEAD_END_ELIMINATION;

		//evaluate!
		std::vector< Trajectory > survivors =
		  get_final_trajectories(
		    trajectories,
		    fractions_to_keep
		  );
		double const score = evaluate( survivors, ensemble_size );


		if( score < best_score_for_stage[ STAGE7 ] ){
		  best_score_for_stage[ STAGE7 ] = score;
		  ss << n_traj << "\t5\t";
		  for( double d : fractions_to_keep ){
		    ss << d << "\t";
		  }
		  ss << score << "\n";
		}

		if( score < best_score ){
		  best_score = score;
		  best_num_trajectories = n_traj;
		  best_fractions_to_keep = fractions_to_keep;
		}

	      }//fractions_to_keep[ STAGE6 ]

	    }//fractions_to_keep[ STAGE5 ]

	  }//fractions_to_keep[ STAGE4 ]

	}//fractions_to_keep[ STAGE3 ]

	log_stream << ss.str();

      }//fractions_to_keep[ STAGE2 ]

    }//fractions_to_keep[ STAGE1 ]

  }//ntraj

  log_stream << std::endl;

  run_results results;
  results.num_trajectories = best_num_trajectories;
  results.fraction_to_keep_for_each_stage = best_fractions_to_keep;
  results.score = best_score;
  return results;
}
