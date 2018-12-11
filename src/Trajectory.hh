#pragma once

#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <cassert>

#include <global.hh>

struct Trajectory {
  Trajectory(){}
  Trajectory(
    std::vector< std::string > const & line_tokens,
    std::unordered_map< std::string, int > const & column_for_title
  ){
    read( line_tokens, column_for_title );
  }

  std::array< double, 7 > score_at_the_end_of_stage;

  void read(
    std::vector< std::string > const & line_tokens,
    std::unordered_map< std::string, int > const & column_for_title
  );
};

struct TrajectorySorter {

  TrajectorySorter( int stage ) :
    stage_( stage )
  {}

  bool operator()( Trajectory const & a, Trajectory const & b ) const {
    return a.score_at_the_end_of_stage[ stage_ ] < b.score_at_the_end_of_stage[ stage_ ];
  }

private:
  int stage_;
};

struct ReverseTrajectorySorter {

  ReverseTrajectorySorter( int stage ) :
    stage_( stage )
  {}

  bool operator()( Trajectory const & a, Trajectory const & b ) const {
    return a.score_at_the_end_of_stage[ stage_ ] > b.score_at_the_end_of_stage[ stage_ ];
  }

private:
  int stage_;
};

void Trajectory::read(
  std::vector< std::string > const & line_tokens,
  std::unordered_map< std::string, int > const & column_for_title
){
  for( int i = 0; i < 7; ++i ){
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
    std::cout << i << " " << tokens[ i ] << std::endl;
  }
  return map;
}

}


bool
read_times(
  std::vector< std::string > const & line_tokens,
  std::unordered_map< std::string, int > const & column_for_title,
  std::array< double, 7 > & average_runtime_for_stage_in_hours
){
  for( int i = 0; i < 7; ++i ){
    //time
    std::string const ti = "t" + std::to_string( i + 1 ) + "_timing_profile";
    std::string const tf = "t" + std::to_string( i + 2 ) + "_timing_profile";

    int const column_i = column_for_title.at( ti );
    int const column_f = column_for_title.at( tf );

    if( line_tokens[ column_i ] == "0" && line_tokens[ column_f ] == "0" ){
      assert( i == 0 );
      return false;
    }

    double const start_time_minutes = std::stod( line_tokens[ column_i ] );
    double const end_time_minutes = std::stod( line_tokens[ column_f ] );

    double const minutes_passed = end_time_minutes - start_time_minutes;
    average_runtime_for_stage_in_hours[ i ] += minutes_passed / 60.0;
  }

  return true;
}

std::pair< std::vector< Trajectory >, std::array< double, 7 > >
load_trajectories( std::string const & filename ){
  std::array< double, 7 > average_runtime_for_stage_in_hours = {0,0,0,0,0,0,0};
  int num_time_points = 0;

  std::vector< std::string > const file_lines = read_lines( filename );

  std::unordered_map< std::string, int > const column_for_title =
    get_column_for_title( file_lines[ 0 ] );

  std::vector< Trajectory > all_trajectories;
  all_trajectories.reserve( file_lines.size() - 1 );

  for( int i = 1; i < file_lines.size(); ++i ){
    std::string const & line = file_lines[ i ];
    std::vector< std::string > const & tokens = split( line );
    all_trajectories.emplace_back( tokens, column_for_title );

    if( read_times( tokens, column_for_title, average_runtime_for_stage_in_hours ) ){
      ++num_time_points;
    }
  }

  std::cout << "Reading times from " << num_time_points << " time points" << std::endl;

  for( int i = 0; i < 7; ++i ){
    average_runtime_for_stage_in_hours[ i ] /= num_time_points;
  }

  return std::make_pair( all_trajectories, average_runtime_for_stage_in_hours );
}
