package uia;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Random;
import java.util.TreeSet;

public class FloatSumError {
	private final static float MAX_BOUND = 1000.0f;
	private final static float MIN_BOUND = -1000.0f;
	private final static float MINUS_INFINITY = -100000.0f;
	private final static float PLUS_INFINITY = 100000.0f;

	private ArrayList<Float> nums;
	private BigDecimal exactSum = new BigDecimal(0);

	private float[][] n;
	private float[][] s;
	private int[][] m;

	class OrderInfo implements Comparable {
		public float value;
		public String order;

		@Override
		public int compareTo(Object o) {
			OrderInfo oi = (OrderInfo) o;
			BigDecimal sub = new BigDecimal(this.value)
					.subtract(new BigDecimal(oi.value));
			if (sub.compareTo(BigDecimal.ZERO) > 0) {
				return 1;
			} else if (sub.compareTo(BigDecimal.ZERO) < 0) {
				return -1;
			}
			return 0;
		}
	}
	
	class Fast2SumPair{
		public float s;
		public float t;
	}

	public FloatSumError(ArrayList<Float> nums) {
		this.nums = nums;
		s = new float[nums.size()][nums.size()];
		n = new float[nums.size()][nums.size()];
		m = new int[nums.size()][nums.size()];
		for (int i = 0; i < nums.size(); i++) {
			exactSum = exactSum.add(new BigDecimal(nums.get(i)));
		}
	}

	public ArrayList<String> getFloatSumBound() {
		ArrayList<OrderInfo> allSums = getAllSums(0, nums.size() - 1);
		TreeSet<OrderInfo> distError = new TreeSet<OrderInfo>();
		for (OrderInfo oi : allSums) {
			distError.add(oi);
		}
		getBound(true);
		String maxErrorOrderStr = constructParens(0, nums.size() - 1);
		System.out.println("Exact Sum: " + exactSum);
		System.out.println("All Sums: ");
		for (OrderInfo oi : distError) {
			System.out.println(oi.order + " : " + oi.value);
		}
		System.out.println("Greedy sum: ");
		System.out.println(maxErrorOrderStr + " " + n[0][nums.size() - 1]);
		s = new float[nums.size()][nums.size()];
		n = new float[nums.size()][nums.size()];
		m = new int[nums.size()][nums.size()];
		getBound(false);
		maxErrorOrderStr = constructParens(0, nums.size() - 1);
		System.out.println(maxErrorOrderStr + " " + n[0][nums.size() - 1]);
		return null;
	}

	private ArrayList<OrderInfo> getAllSums(int i, int j) {
		ArrayList<OrderInfo> result = new ArrayList<OrderInfo>();

		if (i == j) {
			OrderInfo o = new OrderInfo();
			o.value = nums.get(i);
			o.order = nums.get(i).toString();
			result.add(o);
			return result;
		}

		for (int k = i; k < j; k++) {
			ArrayList<OrderInfo> subParens1 = getAllSums(i, k);
			ArrayList<OrderInfo> subParens2 = getAllSums(k + 1, j);
			for (int m = 0; m < subParens1.size(); m++) {
				for (int n = 0; n < subParens2.size(); n++) {
					OrderInfo o = new OrderInfo();
					o.value = subParens1.get(m).value + subParens2.get(n).value;
					o.order = "(" + subParens1.get(m).order + "+"
							+ subParens2.get(n).order + ")";
					result.add(o);
				}
			}
		}

		return result;
	}

	private void getBound(boolean direction) {
		for (int i = 0; i < nums.size(); i++) {
			n[i][i] = nums.get(i);
		}
		for (int l = 2; l <= nums.size(); l++) {
			for (int i = 0; i < nums.size() - l + 1; i++) {
				int j = i + l - 1;
				if(direction){
					s[i][j] = MINUS_INFINITY;
				}else{
					s[i][j] = PLUS_INFINITY;				
				}
				for (int k = i; k < j; k++) {
					Fast2SumPair p = null;
					if(Math.abs(n[i][k]) >= Math.abs(n[k+1][j])){
						p = fast2Sum(n[i][k], n[k+1][j]);
					}else{
						p = fast2Sum(n[k+1][j], n[i][k]);
					}
					float t = s[i][k] + s[k + 1][j] + 
							+ p.t;
					if(direction){
						if (t > s[i][j]) {
							s[i][j] = t;
							m[i][j] = k;
							n[i][j] = p.s;
						}
					}else{
						if (t < s[i][j]) {
							s[i][j] = t;
							m[i][j] = k;
							n[i][j] = p.s;
						}
					}
				}
			}
		}
	}

	private String constructParens(int i, int j) {
		if (i == j) {
			return nums.get(i).toString();
		}
		return "(" + constructParens(i, m[i][j]) + "+"
				+ constructParens(m[i][j] + 1, j) + ")";
	}
	
	private Fast2SumPair fast2Sum(float a, float b){
		Fast2SumPair p = new Fast2SumPair();
		p.s = a + b;
		float z = p.s - a;
		p.t = b - z;
		return p;
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		int num = 13;
		Random rand = new Random();
		ArrayList<Float> nums = new ArrayList<Float>();

		for (int i = 0; i < num; i++) {
			float f = rand.nextFloat() * (MAX_BOUND - MIN_BOUND) + MIN_BOUND;
			nums.add(f);
		}

		
		System.out.println(nums.toString());

		FloatSumError fe = new FloatSumError(nums);
		fe.getFloatSumBound();
	}

}
