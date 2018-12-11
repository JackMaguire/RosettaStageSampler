#include <Trajectory.hh>
#include <global.hh>
#include <util.hh>

#include <array>
#include <algorithm>
#include <cassert>
#include <iostream>
//#include <math.h>
#include <cmath>

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

bool run_test_2(){
  //Tests estimate_minimum_runtime()

  //Case 1
  {
    //conditions:
    //Stage n_traj hours
    //1     16     1
    //2-7   0      100
    int const num_total_trajectories = 16;
    std::array< double, 7 > const average_runtime_for_stage_in_hours =
      { 1, 100, 100, 100, 100, 100, 100 };
    std::array< double, 6 > const fractions_to_keep_for_stage =
      { 0, 0, 0, 0, 0, 0 };

    //expected results:
    //16 hours
    constexpr double expectation = 16.0;
    double const estimation = estimate_minimum_runtime_in_hours(
      num_total_trajectories,
      average_runtime_for_stage_in_hours,
      fractions_to_keep_for_stage
    );

    if( ! vals_are_very_close( estimation, expectation ) ){
      std::cout << "run_test_2 case 1 failed!\n"
      << "expecation: " << expectation
      << "\nestimation: " << estimation << std::endl;
      return FAIL;
    }

  }

  //Case 2
  {
    //conditions:
    //Stage n_traj hours
    //1     64     1
    //2     32     3
    //3     16     9
    //4     8      27
    //5     4      9
    //6     2      3
    //7     1      1
    int const num_total_trajectories = 64;
    constexpr std::array< double, 7 > average_runtime_for_stage_in_hours =
      { 1, 3, 9, 27, 9, 3, 1 };
    constexpr std::array< double, 6 > fractions_to_keep_for_stage =
      { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 };

    //expected results:
    //16 hours
    constexpr double expectation =
      64 * average_runtime_for_stage_in_hours[ STAGE1 ] +
      32 * average_runtime_for_stage_in_hours[ STAGE2 ] +
      16 * average_runtime_for_stage_in_hours[ STAGE3 ] +
      8  * average_runtime_for_stage_in_hours[ STAGE4 ] +
      4  * average_runtime_for_stage_in_hours[ STAGE5 ] +
      2  * average_runtime_for_stage_in_hours[ STAGE6 ] +
      1  * average_runtime_for_stage_in_hours[ STAGE7 ];

    double const estimation = estimate_minimum_runtime_in_hours(
      num_total_trajectories,
      average_runtime_for_stage_in_hours,
      fractions_to_keep_for_stage
    );

    if( ! vals_are_very_close( estimation, expectation ) ){
      std::cout << "run_test_2 case 2 failed!\n"
      << "expecation: " << expectation
      << "\nestimation: " << estimation << std::endl;
      return FAIL;
    }

  }

  return PASS;
}

bool run_test_3(){
  //Tests get_final_trajectories();

  //Case 1: Just sort on last value
  {
    //conditions:
    //num_initial_trajectories = 8
    //fractions_to_keep[ n ] = 1.0 for all n except final, which is 0.5
    
    // # scores                     kept?
    // 0 { 0, 0, 0, 0, 0, 0, -1 }
    // 1 { 0, 0, 0, 0, 0, 0, -8 }
    // 2 { 0, 0, 0, 0, 0, 0, -2 }
    // 3 { 0, 0, 0, 0, 0, 0, -7 }
    // 4 { 0, 0, 0, 0, 0, 0, -3 }
    // 5 { 0, 0, 0, 0, 0, 0, -6 }
    // 6 { 0, 0, 0, 0, 0, 0, -4 }
    // 7 { 0, 0, 0, 0, 0, 0, -4 }

    std::array< double, 6 > fractions_to_keep;
    for( int stage = STAGE1; stage < STAGE6; ++stage ){
      fractions_to_keep[ stage ] = 1.0;
    }
    fractions_to_keep[ STAGE6 ] = 0.5;



    std::vector< Trajectory > trajectories( 8 );
    for( int i=0; i<trajectories.size(); ++i ){
      Trajectory & t = trajectories[ i ];
      for( int stage = STAGE1; stage < STAGE6; ++stage ){
	t.score_at_the_end_of_stage[ stage ] = 0;
      }

      switch( i ){
	case 0:
	  t.score_at_the_end_of_stage[ STAGE6 ] = -1.0;
	  break;
	case 1:
	  t.score_at_the_end_of_stage[ STAGE6 ] = -8.0;
	  break;
	case 2:
	  t.score_at_the_end_of_stage[ STAGE6 ] = -2.0;
	  break;
	case 3:
	  t.score_at_the_end_of_stage[ STAGE6 ] = -7.0;
	  break;
	case 4:
	  t.score_at_the_end_of_stage[ STAGE6 ] = -3.0;
	  break;
	case 5:
	  t.score_at_the_end_of_stage[ STAGE6 ] = -6.0;
	  break;
	case 6:
	  t.score_at_the_end_of_stage[ STAGE6 ] = -4.0;
	  break;
	case 7:
	  t.score_at_the_end_of_stage[ STAGE6 ] = -4.0;
	  break;
      }
    }

    std::vector< Trajectory > const results =
      get_final_trajectories( trajectories, fractions_to_keep );

    if( results.size() != 4 ){
      std::cout << "run_test_3 case 1 failed!\n" <<
	"results.size() == " << results.size() << " instead of 4" << std::endl;
      return FAIL;
    }

    double final_score = 0;
    for( Trajectory const & t : results ){
      final_score += t.score_at_the_end_of_stage[ STAGE6 ];
    }

    constexpr double expectation = -8.0 - 7.0 - 6.0 - 4.0;

    if( ! vals_are_very_close( final_score, expectation ) ){
      std::cout << "run_test_3 case 1 failed!\n" <<
	"final_score == " << final_score << " instead of " << expectation << std::endl;
      return FAIL;
    }

  }


  //Case 2: Just sort on two value
  {
    //conditions:
    //num_initial_trajectories = 8
    
    // # scores
    // 0 { 0, 0, -1, 0, 0, -1 }
    // 1 { 0, 0, -2, 0, 0, -2 }
    // 2 { 0, 0, -1, 0, 0, -3 }
    // 3 { 0, 0, -2, 0, 0, -4 }
    // 4 { 0, 0, -1, 0, 0, -5 }
    // 5 { 0, 0, -2, 0, 0, -6 }
    // 6 { 0, 0, -1, 0, 0, -7 }
    // 7 { 0, 0, -2, 0, 0, -8 }

    //After first filtration:
    // # scores
    // 1 { 0, 0, -2, 0, 0, -2 }
    // 3 { 0, 0, -2, 0, 0, -4 }
    // 5 { 0, 0, -2, 0, 0, -6 }
    // 7 { 0, 0, -2, 0, 0, -8 }

    //After second filtration:
    // # scores
    // 7 { 0, 0, -2, 0, 0, -8 }
    // 5 { 0, 0, -2, 0, 0, -6 }

    std::array< double, 6 > fractions_to_keep;
    for( int stage = STAGE1; stage <= STAGE6; ++stage ){
      fractions_to_keep[ stage ] = 1.0;
    }
    fractions_to_keep[ STAGE3 ] = 0.5;
    fractions_to_keep[ STAGE6 ] = 0.5;

    std::vector< Trajectory > trajectories( 8 );
    for( int i=0; i<trajectories.size(); ++i ){
      Trajectory & t = trajectories[ i ];
      for( int stage = STAGE1; stage < STAGE6; ++stage ){
	t.score_at_the_end_of_stage[ stage ] = 0;
      }

      t.score_at_the_end_of_stage[ STAGE3 ] = ( i % 2 == 0 ? -1 : -2 );
      t.score_at_the_end_of_stage[ STAGE6 ] = 0 - 1 - i;
    }

    std::vector< Trajectory > const results =
      get_final_trajectories( trajectories, fractions_to_keep );

    /*for( auto const & t : results ){
      for( int stage = STAGE1; stage < STAGE7; ++stage ){
	std::cout << "\t" << t.score_at_the_end_of_stage[ stage ] << std::endl;
      }      
      std::cout << std::endl;
    }
    return FAIL;*/

    if( results.size() != 2 ){
      std::cout << "run_test_3 case 2 failed!\n" <<
	"results.size() == " << results.size() << " instead of 2" << std::endl;
      return FAIL;
    }

    double final_score = 0;
    for( Trajectory const & t : results ){
      final_score += t.score_at_the_end_of_stage[ STAGE6 ];
    }

    constexpr double expectation = -8.0 - 6.0;

    if( ! vals_are_very_close( final_score, expectation ) ){
      std::cout << "run_test_3 case 2 failed!\n" <<
	"final_score == " << final_score << " instead of " << expectation << std::endl;
      return FAIL;
    }

  }

  //Case 3: Just sort on two value
  {
    //conditions:
    //num_initial_trajectories = 8
    
    // # scores                     i/2
    // 0 { 1, 0, -1, 0, 0, -1 }     0
    // 1 { 1, 0, -2, 0, 0, -2 }     0
    // 2 { 0, 0, -1, 0, 0, -3 }     1
    // 3 { 0, 0, -2, 0, 0, -4 }     1
    // 4 { 1, 0, -1, 0, 0, -5 }     2
    // 5 { 1, 0, -2, 0, 0, -6 }     2
    // 6 { 0, 0, -1, 0, 0, -7 }     3
    // 7 { 0, 0, -2, 0, 0, -8 }     3

    //After first filtration:
    // # scores
    // 0 { 1, 0, -1, 0, 0, -1 }
    // 1 { 1, 0, -2, 0, 0, -2 }
    // 4 { 1, 0, -1, 0, 0, -5 }
    // 5 { 1, 0, -2, 0, 0, -6 }

    //After second filtration:
    // # scores
    // 1 { 1, 0, -2, 0, 0, -2 }
    // 5 { 1, 0, -2, 0, 0, -6 }

    //After third filtration:
    // # scores
    // 5 { 1, 0, -2, 0, 0, -6 }

    std::array< double, 6 > fractions_to_keep;
    for( int stage = STAGE1; stage <= STAGE6; ++stage ){
      fractions_to_keep[ stage ] = 1.0;
    }
    fractions_to_keep[ STAGE1 ] = 0.5;
    fractions_to_keep[ STAGE3 ] = 0.5;
    fractions_to_keep[ STAGE6 ] = 0.5;

    std::vector< Trajectory > trajectories( 8 );
    for( int i=0; i<trajectories.size(); ++i ){
      Trajectory & t = trajectories[ i ];
      for( int stage = STAGE1; stage < STAGE6; ++stage ){
	t.score_at_the_end_of_stage[ stage ] = 0;
      }

      t.score_at_the_end_of_stage[ STAGE1 ] = ( (i/2) % 2 == 0 ? 1 : 0 );
      t.score_at_the_end_of_stage[ STAGE3 ] = ( i % 2 == 0 ? -1 : -2 );
      t.score_at_the_end_of_stage[ STAGE6 ] = 0 - 1 - i;
    }

    std::vector< Trajectory > const results =
      get_final_trajectories( trajectories, fractions_to_keep );

    if( results.size() != 1 ){
      std::cout << "run_test_3 case 3 failed!\n" <<
	"results.size() == " << results.size() << " instead of 1" << std::endl;
      return FAIL;
    }

    double final_score = 0;
    for( Trajectory const & t : results ){
      final_score += t.score_at_the_end_of_stage[ STAGE6 ];
    }

    constexpr double expectation = -6.0;

    if( ! vals_are_very_close( final_score, expectation ) ){
      std::cout << "run_test_3 case 3 failed!\n" <<
	"final_score == " << final_score << " instead of " << expectation << std::endl;
      return FAIL;
    }

  }



  return PASS;
}

bool run_test_4(){
  //Tests evaluate();

  //CASE 1
  {
    //conditions:
    // # scores
    // 0 { 0, 0, 0, 0, 0, 0, -1 }
    // 1 { 0, 0, 0, 0, 0, 0, -8 }
    // 2 { 0, 0, 0, 0, 0, 0, -2 }
    // 3 { 0, 0, 0, 0, 0, 0, -7 }
    // 4 { 0, 0, 0, 0, 0, 0, -3 }
    // 5 { 0, 0, 0, 0, 0, 0, -6 }
    // 6 { 0, 0, 0, 0, 0, 0, -4 }
    // 7 { 0, 0, 0, 0, 0, 0, -4 }


    std::vector< Trajectory > trajectories( 8 );
    for( int i=0; i<trajectories.size(); ++i ){
      Trajectory & t = trajectories[ i ];
      for( int stage = STAGE1; stage < STAGE7; ++stage ){
	t.score_at_the_end_of_stage[ stage ] = 0;
      }

      switch( i ){
	case 0:
	  t.score_at_the_end_of_stage[ STAGE7 ] = -1.0;
	  break;
	case 1:
	  t.score_at_the_end_of_stage[ STAGE7 ] = -8.0;
	  break;
	case 2:
	  t.score_at_the_end_of_stage[ STAGE7 ] = -2.0;
	  break;
	case 3:
	  t.score_at_the_end_of_stage[ STAGE7 ] = -7.0;
	  break;
	case 4:
	  t.score_at_the_end_of_stage[ STAGE7 ] = -3.0;
	  break;
	case 5:
	  t.score_at_the_end_of_stage[ STAGE7 ] = -6.0;
	  break;
	case 6:
	  t.score_at_the_end_of_stage[ STAGE7 ] = -4.0;
	  break;
	case 7:
	  t.score_at_the_end_of_stage[ STAGE7 ] = -4.0;
	  break;
      }
    }

    {//ensemble_size == 1
      constexpr int ensemble_size = 1;
      constexpr double expectation = -8;
      double const reality = evaluate( trajectories, ensemble_size );
      if( ! vals_are_very_close( reality, expectation ) ){
	std::cout << "run_test_4 case 1 with ensemble size " << ensemble_size << " failed!\n"
	  "reality == " << reality << " instead of " << expectation << std::endl;
	return FAIL;
      }
    }

    {
      constexpr int ensemble_size = 2;
      constexpr double expectation = -8-7;
      double const reality = evaluate( trajectories, ensemble_size );
      if( ! vals_are_very_close( reality, expectation ) ){
	std::cout << "run_test_4 case 1 with ensemble size " << ensemble_size << " failed!\n"
	  "reality == " << reality << " instead of " << expectation << std::endl;
	return FAIL;
      }
    }

    {
      constexpr int ensemble_size = 3;
      constexpr double expectation = -8-7-6;
      double const reality = evaluate( trajectories, ensemble_size );
      if( ! vals_are_very_close( reality, expectation ) ){
	std::cout << "run_test_4 case 1 with ensemble size " << ensemble_size << " failed!\n"
	  "reality == " << reality << " instead of " << expectation << std::endl;
	return FAIL;
      }
    }

    {
      constexpr int ensemble_size = 4;
      constexpr double expectation = -8-7-6-4;
      double const reality = evaluate( trajectories, ensemble_size );
      if( ! vals_are_very_close( reality, expectation ) ){
	std::cout << "run_test_4 case 1 with ensemble size " << ensemble_size << " failed!\n"
	  "reality == " << reality << " instead of " << expectation << std::endl;
	return FAIL;
      }
    }

    {
      constexpr int ensemble_size = 5;
      constexpr double expectation = -8-7-6-4-4;
      double const reality = evaluate( trajectories, ensemble_size );
      if( ! vals_are_very_close( reality, expectation ) ){
	std::cout << "run_test_4 case 1 with ensemble size " << ensemble_size << " failed!\n"
	  "reality == " << reality << " instead of " << expectation << std::endl;
	return FAIL;
      }
    }

    {
      constexpr int ensemble_size = 6;
      constexpr double expectation = -8-7-6-4-4-3;
      double const reality = evaluate( trajectories, ensemble_size );
      if( ! vals_are_very_close( reality, expectation ) ){
	std::cout << "run_test_4 case 1 with ensemble size " << ensemble_size << " failed!\n"
	  "reality == " << reality << " instead of " << expectation << std::endl;
	return FAIL;
      }
    }

    {
      constexpr int ensemble_size = 7;
      constexpr double expectation = -8-7-6-4-4-3-2;
      double const reality = evaluate( trajectories, ensemble_size );
      if( ! vals_are_very_close( reality, expectation ) ){
	std::cout << "run_test_4 case 1 with ensemble size " << ensemble_size << " failed!\n"
	  "reality == " << reality << " instead of " << expectation << std::endl;
	return FAIL;
      }
    }

    {
      constexpr int ensemble_size = 8;
      constexpr double expectation = -8-7-6-4-4-3-2-1;
      double const reality = evaluate( trajectories, ensemble_size );
      if( ! vals_are_very_close( reality, expectation ) ){
	std::cout << "run_test_4 case 1 with ensemble size " << ensemble_size << " failed!\n"
	  "reality == " << reality << " instead of " << expectation << std::endl;
	return FAIL;
      }
    }

  }


  return PASS;
}

int main(){
  if(
    run_test_1() &&
    run_test_2() &&
    run_test_3() &&
    run_test_4()
  ){
    std::cout << "All tests pass" << std::endl;
    return 0;
  } else {
    return 1;
  }
}
