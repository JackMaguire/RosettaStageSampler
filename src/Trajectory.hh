#pragma once

#include <vector>
#include <string>

struct Trajectory {
  Trajectory();

  std::vector< int > cpu_seconds_for_stage_;
  std::vector< float > score_at_the_end_of_stage_;
  float final_score_;

  void read( std::string const & line );
};


void Trajectory::read( std::string const & line ){

}
