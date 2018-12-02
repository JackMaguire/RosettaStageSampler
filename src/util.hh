#pragma once

#include <global.hh>

#include <array>
#include <algorithm>
#include <cassert>
#include <iostream>


std::array< double, 7 > get_mean_time_for_stage_in_hours (
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


[[nodiscard]]
double estimate_minimum_runtime_in_hours(
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
  std::vector< Trajectory > trajectories( all_trajectories.begin(), all_trajectories.begin() + num_initial_trajectories );

  for( int stage = STAGE1; stage < STAGE7; ++stage ){
    int const num_survivors = trajectories.size() * fractions_to_keep[ stage ];
    if( stage < 2 ){//positive scores are better for the first two
      std::sort( trajectories.begin(), trajectories.end(), ReverseTrajectorySorter( stage ) );
    } else {
      std::sort( trajectories.begin(), trajectories.end(), TrajectorySorter( stage ) );
    }
    trajectories.resize( num_survivors );
  }

  std::sort( trajectories.begin(), trajectories.end(), TrajectorySorter( STAGE7 ) );

  return trajectories;
}

double
evaluate(
  std::vector< Trajectory > const & trajectories,
  int const ensemble_size
){
  //average over the top ensemble_size elements
  double sum = 0;
  for( int i = 0; i < ensemble_size && i < trajectories.size(); ++i ){
    sum += trajectories[ i ].score_at_the_end_of_stage[ STAGE7 ];
  }
  return sum / ensemble_size;
}
