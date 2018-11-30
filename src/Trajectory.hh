#pragma once

#include <vector>
#include <array>
#include <string>
#include <unordered_map>

struct Trajectory {
  Trajectory();

  std::array< int, 7 > cpu_seconds_for_stage;
  std::array< double, 7 > score_at_the_end_of_stage;
  double final_score;

  void read( std::string const & line, std::unordered_map< std::string, int > column_for_title );
};


void Trajectory::read(
  std::string const & line,
  std::unordered_map< std::string, int > column_for_title
){
  
}
