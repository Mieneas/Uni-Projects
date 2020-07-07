import java.util.LinkedList;
import java.util.Arrays;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.Test;

class PermutationTest {
	PermutationVariation p1;
	PermutationVariation p2;
	public int n1;
	public int n2;
	int cases=0;
	
	void initialize() {
		n1=4;
		n2=6;
		Cases c= new Cases();
		p1= c.switchforTesting(cases, n1);
		p2= c.switchforTesting(cases, n2);
	}
	

	@Test
	void testPermutation() {
		initialize();
		// TODO
			assertEquals(p1.original.length, n1, "Die Länge von dem Array Orginal wurde falsch gesetzt.\n");

			//p1
			for (int i = 0; i < p1.original.length - 1; i++) {
				for (int j = i + 1; j < p1.original.length; j++) {
					assertNotEquals(p1.original[i], p1.original[j], "Dein Array enthält zwei identischen Zahlen.\n");
				}
			}
			assertNotNull(p1.allDerangements, "Es darf hier keine Permutationen geben\n");
			//p2
			assertEquals(p2.original.length, n2, "Die Länge von dem Array Orginal wurde falsch gesetzt.\n");

			for (int i = 0; i < p2.original.length - 1; i++) {
				for (int j = i + 1; j < p2.original.length; j++) {
					assertNotEquals(p2.original[i], p2.original[j], "Dein Array enthält zwei identischen Zahlen.\n");
				}
			}
			assertNotNull(p2.allDerangements, "Es darf hier keine Permutationen geben\n");
	}

	@Test
	void testDerangements() {
		initialize();
		//in case there is something wrong with the constructor
		fixConstructor();
		// TODO
		double pZahl = 0;
		int fak = 1;
		int answer;

			//p1
			p1.derangements();
			p2.derangements();

			for(int i = 0; i <= n1; i++){
				if(i > 0)
					fak *= i;

				pZahl += Math.pow(-1, i)/fak;
			}
			pZahl = pZahl * fak;
			answer = (int)pZahl;
			assertEquals(p1.allDerangements.size(), answer, "Die Anzahl der Permutationen ist falsch.\n");

			for(int i = 0; i < p1.allDerangements.size(); i++){
					for (int j = 0; j < p1.original.length; j++) {
						assertNotEquals(p1.allDerangements.get(i)[j], p1.original[j], "Deine Permutationen sind falsch.\n");
					}
			}


			//p2
		    pZahl = 0;
			fak = 1;
			for (int i = 0; i <= n2; i++) {
				if (i > 0)
					fak *= i;

				pZahl += Math.pow(-1, i) / fak;
			}
			pZahl = pZahl * fak;
			answer = (int)pZahl;
			assertEquals(p2.allDerangements.size(), answer, "Die Anzahl der Permutationen ist falsch.\n");

			for (int i = 0; i < p2.allDerangements.size(); i++) {
				for (int j = 0; j < p2.original.length; j++){
					assertNotEquals(p2.allDerangements.get(i)[j], p2.original[j], "Deine Permutationen sind falsch.\n");
				}
			}
	}
	
	@Test
	void testsameElements() {
		initialize();
		//in case there is something wrong with the constructor
		fixConstructor();
		// TODO
		p1.derangements();
		p2.derangements();

		assertNotEquals(p1.allDerangements.size(), 0, "Es gibt keine Permutationen\n");
		assertNotEquals(p2.allDerangements.size(), 0, "Es gibt keine Permutationen\n");


		int z = 0;
		int z1 = 0;
		//p1
		for(int q = 0; q < p1.allDerangements.size(); q++) {
			for (int i = 0; i < p1.allDerangements.get(q).length; i++) {
				for (int j = 0; j < p1.original.length; j++) {
					if (p1.original[j] == p1.allDerangements.get(q)[i]) {
						z++;
					}
				}
			}
			if (z == p1.allDerangements.get(q).length) {
				z1++;
			}
			z = 0;
		}
		assertEquals(z1, p1.allDerangements.size(), "Es gibt eine fremde Zahl in einer oder mehreren Permutationen.");

		//p2:
		z1 = 0;
		for(int q = 0; q < p2.allDerangements.size(); q++) {
			for (int i = 0; i < p2.allDerangements.get(q).length; i++) {
				for (int j = 0; j < p2.original.length; j++) {
					if (p2.original[j] == p2.allDerangements.get(q)[i]) {
						z++;
					}
				}
			}
			if (z == p2.allDerangements.get(q).length) {
				z1++;
			}
			z = 0;
		}
		assertEquals(z1, p2.allDerangements.size(), "Es gibt eine fremde Zahl in einer oder mehreren Permutationen.");
	}
	
	void setCases(int c) {
		this.cases=c;
	}
	
	public void fixConstructor() {
		//in case there is something wrong with the constructor
		p1.allDerangements=new LinkedList<int[]>();
		for(int i=0;i<n1;i++)
			p1.original[i]=2*i+1;
		
		p2.allDerangements=new LinkedList<int[]>();
		for(int i=0;i<n2;i++)
			p2.original[i]=i+1;
	}
}


