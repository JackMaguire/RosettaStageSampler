#include <Trajectory.hh>

#include <array>

enum STAGE_TAG {
  STAGE1 = 0,
  STAGE2 = 1,
  STAGE3 = 2,
  STAGE4 = 3,
  STAGE5 = 4,
  STAGE6 = 5,
  STAGE7 = 6,
  NUM_STAGES
};

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

  std::array< double, 6 > percents_to_keep { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

  for( percents_to_keep[ STAGE1 ] = 0.0; percents_to_keep[ STAGE1 ] <= 1.0; percents_to_keep[ STAGE1 ] += 0.01 ){

    for( percents_to_keep[ STAGE2 ] = 0.0; percents_to_keep[ STAGE2 ] <= 1.0; percents_to_keep[ STAGE2 ] += 0.01 ){

      for( percents_to_keep[ STAGE3 ] = 0.0; percents_to_keep[ STAGE3 ] <= 1.0; percents_to_keep[ STAGE3 ] += 0.01 ){

	for( percents_to_keep[ STAGE4 ] = 0.0; percents_to_keep[ STAGE4 ] <= 1.0; percents_to_keep[ STAGE4 ] += 0.01 ){

	  for( percents_to_keep[ STAGE5 ] = 0.0; percents_to_keep[ STAGE5 ] <= 1.0; percents_to_keep[ STAGE5 ] += 0.01 ){

	    for( percents_to_keep[ STAGE6 ] = 0.0; percents_to_keep[ STAGE6 ] <= 1.0; percents_to_keep[ STAGE6 ] += 0.01 ){




	    }//percents_to_keep[ STAGE6 ]

	  }//percents_to_keep[ STAGE5 ]

	}//percents_to_keep[ STAGE4 ]

      }//percents_to_keep[ STAGE3 ]

    }//percents_to_keep[ STAGE2 ]

  }//percents_to_keep[ STAGE1 ]


}
