
yijia@ccis:~/Workspace/abstract_interpretation/test_ppl$ g++ box_power.cc  -o boxPower -lppl -lgmpxx -lgmp
box_power.cc: In function ‘int main()’:
box_power.cc:70:3: error: ‘TBox’ was not declared in this scope
   TBox box(2);
   ^
box_power.cc:70:8: error: expected ‘;’ before ‘box’
   TBox box(2);
        ^
box_power.cc:71:3: error: ‘box’ was not declared in this scope
   box.propagate_constraints(cs);
   ^
yijia@ccis:~/Workspace/abstract_interpretation/test_ppl$ g++ box_power.cc  -o boxPower -lppl -lgmpxx -lgmp
yijia@ccis:~/Workspace/abstract_interpretation/test_ppl$ g++ box_power.cc  -o boxPower -lppl -lgmpxx -lgmp
yijia@ccis:~/Workspace/abstract_interpretation/test_ppl$ g++ box_power.cc  -o boxPower -lppl -lgmpxx -lgmp
yijia@ccis:~/Workspace/abstract_interpretation/test_ppl$ g++ box_power.cc  -o boxPower -lppl -lgmpxx -lgmp
yijia@ccis:~/Workspace/abstract_interpretation/test_ppl$ ./boxPower 
*** i_store.constraints ***A >= 0, -A >= -1, B >= 0, -B >= -4
yijia@ccis:~/Workspace/abstract_interpretation/test_ppl$ g++ box_power.cc  -o boxPower -lppl -lgmpxx -lgmp
yijia@ccis:~/Workspace/abstract_interpretation/test_ppl$ ./boxPower 
*** i_store.constraints ***A >= 0, -A >= -1, B >= 0, -B >= -3
yijia@ccis:~/Workspace/abstract_interpretation/test_ppl$ 