#include <Trajectory.hh>
#include <global.hh>
#include <util.hh>

#include <array>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <math.h>

#define PASS true
#define FAIL false

bool vals_are_very_close(
  double const val1,
  double const val2,
  double const lambda = 0.01
){
  double const diff = std::abs( val1 - val2 );
  return diff < lambda;
}

bool run_test_1(){
  //Tests get_mean_time_for_stage_in_hours()

  //Expected Results:
  //Stage   Hours
  // 1      1
  // 2      2
  // 3      2.5
  // 4      3
  // 5      7
  // 6      -1
  // 7      0

  std::array< double, 7 > const expected_mean_times =
    { 1.0, 2.0, 1.5, 3.0, 7.0, -1.0, 0.0 };

  std::vector< Trajectory > trajectories( 10 );
  for( int i = 0; i < 10; ++i ){
    Trajectory & t = trajectories[ i ];
    double offset = 0.0;

    switch( i ){
      case STAGE1:
	offset = 1.0;
	break;
      case STAGE2:
	offset = -1.0;
	break;
      case STAGE3:
	offset = 0.25;
	break;
      case STAGE4:
	offset = -0.25;
	break;
      case STAGE5:
	offset = 3.5;
	break;
      case STAGE6:
	offset = -3.5;
	break;
      default:
	break;
    }

    for( int j = STAGE1; j <= STAGE7; ++j ){
      t.cpu_hours_for_stage[ j ] = expected_mean_times[ j ] + offset;
    }
  }

  std::array< double, 7 > const mean_times =
    get_mean_time_for_stage_in_hours( trajectories );

  bool return_status = PASS;

  for( int stage = STAGE1; stage <= STAGE7; ++stage ){
    if( ! vals_are_very_close( expected_mean_times[ stage ], mean_times[ stage ] ) ){
      std::cout << "run_test_1 failed!\n"
      << "stage " << stage << " was measured at " << mean_times[ stage ]
      << " hours instead of " << expected_mean_times[ stage ] << std::endl;

      return_status = FAIL;
    }
  }

  return return_status;
}

int main(){
  if( run_test_1() ){
    std::cout << "All tests pass" << std::endl;
    return 0;
  } else {
    return 1;
  }
}
