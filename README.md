# RosettaStageSampler


## Idea For Scripting Format:

```XML
<RosettaStageSampler>
   <Stage title="name" num_results_to_keep="[min,max,(sampling interval)]">
      <Sort sort_metric="total_score" negative_score_is_good="true"/>
      <Time begin="t1" end="t2"/>
   </Stage>
      
   <Stage title="name" num_results_to_keep="[10,100,1]">
      <Sort sort_metric="num_contacts" negative_score_is_good="false"/>
      <Time begin="t1" end="t2"/>
   </Stage>

   <Score final_evaluation_term="total_score"/>
</RosettaStageSampler>
```

```sh
./RosettaStageSampler -script script.xml -scorefiles *.sc -max_cpu_hours 1000 -ensemble_size 50
```
