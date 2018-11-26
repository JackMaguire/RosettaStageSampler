# RosettaStageSampler


# Idea For Scripting Format:

```XML
<TOPLEVEL>
   <Stage title="name" sort_metric="total_score" negative_score_is_good="true" num_results_to_keep="[min,max,(interval)]"/>
   <Stage title="name" sort_metric="num_contacts" negative_score_is_good="false" num_results_to_keep="[10,100,1]"/>
   <Eval final_evaluation_term="total_score">
</TOPLEVEL>
```

```sh
./RosettaStageSampler -script script.xml -scorefiles *.sc
```
