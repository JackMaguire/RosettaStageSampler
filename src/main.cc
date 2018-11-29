#include <Trajectory.hh>

#include <array>

constexpr std::array< float, 1 > values_for_stage1(){
  std::array< float, 1 > vals;
  vals[ 0 ] = 1.1;
  return vals;
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





    Stages 6 and 7 are optional

   */

  std::array< double, 7 > percents_to_keep;

  for( double perc1 = 0; perc1 <= 1.0; perc1 += 0.01 ){

    for( double perc2 = 0; perc2 <= 1.0; perc2 += 0.01 ){

      for( double perc3 = 0; perc3 <= 1.0; perc3 += 0.01 ){

	for( double perc4 = 0; perc4 <= 1.0; perc4 += 0.01 ){

	  for( double perc5 = 0; perc5 <= 1.0; perc5 += 0.01 ){

	    for( double perc6 = 0; perc6 <= 1.0; perc6 += 0.01 ){




	    }//perc6

	  }//perc5

	}//perc4

      }//perc3

    }//perc2

  }//perc1


}
