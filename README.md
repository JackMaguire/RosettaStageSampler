# RosettaStageSampler


## Idea For Scripting Format:

```XML
<RosettaStageSampler>
   <Stage title="name" num_results_to_keep="[min,max,(sampling interval)]" optional="false">
      <Score metric="total_score" negative_score_is_good="true"/>
      <Time begin="t1" end="t2"/>
   </Stage>
      
   <Stage title="name" num_results_to_keep="[10,100,1]" optional="true">
      <Score metric="num_contacts" negative_score_is_good="false"/>
      <Time begin="t2" end="t3"/>
   </Stage>

   <Score metric="total_score" negative_score_is_good="true"/>
</RosettaStageSampler>
```

```sh
./RosettaStageSampler -script script.xml -scorefiles *.sc -max_cpu_hours 1000 -ensemble_size 50
```
