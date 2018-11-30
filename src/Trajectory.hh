#pragma once

#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <iterator>

#include <global.hh>

struct Trajectory {
  Trajectory();

  std::array< double, 7 > cpu_hours_for_stage;
  std::array< double, 7 > score_at_the_end_of_stage;

  void read(
    std::vector< std::string > const & line_tokens,
    std::unordered_map< std::string, int > const & column_for_title
  );
};


void Trajectory::read(
  std::vector< std::string > const & line_tokens,
  std::unordered_map< std::string, int > const & column_for_title
){
  for( int i = 0; i < 7; ++i ){
    //time
    std::string const ti = "t" + std::to_string( i + 1 ) + "_timing_profile";
    std::string const tf = "t" + std::to_string( i + 2 ) + "_timing_profile";

    int const column_i = column_for_title.at( ti );
    int const column_f = column_for_title.at( tf );

    double const start_time_minutes = std::stod( line_tokens[ column_i ] );
    double const end_time_minutes = std::stod( line_tokens[ column_f ] );

    double const minutes_passed = end_time_minutes - start_time_minutes;
    cpu_hours_for_stage[ i ] = minutes_passed / 60.0;

    //score
    std::string const title_for_score = "m" + std::to_string( i + 1 );
    int const score_col = column_for_title.at( title_for_score );
    score_at_the_end_of_stage[ i ] = std::stod( line_tokens[ score_col ] );
  }
}

namespace {

std::vector< std::string >
read_lines( std::string const & filename ){
  //https://stackoverflow.com/questions/3482064/counting-the-number-of-lines-in-a-text-file
  std::string line;
  std::ifstream myfile( filename );

  std::vector< std::string > all_lines;

  while( std::getline( myfile, line ) ){
    all_lines.push_back( line );
  }

  return all_lines;
}

std::vector< std::string >
split( std::string const & line ){
  //https://stackoverflow.com/questions/2275135/splitting-a-string-by-whitespace-in-c
  std::istringstream buffer( line );
  std::vector< std::string > tokens {
    std::istream_iterator< std::string >( buffer ), 
      std::istream_iterator< std::string >() };

  return tokens;
}

std::unordered_map< std::string, int >
get_column_for_title( std::string const & title_line ){
  std::vector< std::string > tokens = split( title_line );
  std::unordered_map< std::string, int > map;
  for( int i=0; i < tokens.size(); ++i ){
    map[ tokens[ i ] ] = i;
  }
  return map;
}

}

std::vector< Trajectory >
load( std::string const & filename ){
  std::vector< std::string > const file_lines = read_lines( filename );

  std::vector< Trajectory > all_trajectories;
  all_trajectories.reserve( file_lines.size() );

  

  return all_trajectories;
}
