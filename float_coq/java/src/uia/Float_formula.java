package uia;

public class Float_formula {
 public static void main(String argv[]){
	 int var_num = 1000;
	 String var_dec = "";
	 String universal = "forall ";
	 String type_dec = ": Z,";
	 String left = "(";
	 String right = "(";
	 System.out.println("Theorem comm_test:");
	 
	 int i = 0;
	 for (; i < var_num; i ++){
		 var_dec += "a" + i + " ";
	 }
	 i = 0;
	 for (; i < var_num/2 - 1; i ++){
		 left += "a" + i + "+";
	 }
	 left += "a" + i + ")";
	 i++;
	 for(; i < var_num - 1; i ++){
		 right += "a" + i + "+";
	 }
	 right += "a" + i + ")";
	 System.out.println(universal + var_dec + type_dec + left + "+" + right + "=" + right + "+" + left + ".");
 }
}
