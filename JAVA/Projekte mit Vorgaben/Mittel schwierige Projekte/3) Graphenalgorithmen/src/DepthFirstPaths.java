/******************************************************************************
 *  Compilation:  javac DepthFirstPaths.java
 *  Execution:    java DepthFirstPaths G s
 *  Dependencies: Graph.java
 ******************************************************************************/

/**
 *  The {@code DepthFirstPaths} class represents a data type for finding
 *  paths from a source vertex <em>s</em> to every other vertex
 *  in an undirected graph.
 *  <p>
 *  This implementation uses depth-first search.
 *  The constructor takes time proportional to <em>V</em> + <em>E</em>,
 *  where <em>V</em> is the number of vertices and <em>E</em> is the number of edges.
 *  Each call to {@link #hasPathTo(int)} takes constant time;
 *  each call to {@link #pathTo(int)} takes time proportional to the length
 *  of the path.
 *  It uses extra space (not including the graph) proportional to <em>V</em>.
 *  <p>
 *  For additional documentation, see <a href="https://algs4.cs.princeton.edu/41graph">Section 4.1</a>   
 *  of <i>Algorithms, 4th Edition</i> by Robert Sedgewick and Kevin Wayne.
 *
 *  @author Robert Sedgewick
 *  @author Kevin Wayne
 *  
 *  DISCLAIMER:
 *  These methods have been partly adjusted to fit the excersise.
 */
import java.util.*;


public class DepthFirstPaths {
    private boolean[] marked;    // marked[v] = is there an s-v path?
    private int[] edgeTo;        // edgeTo[v] = last edge on s-v path
    private int[] distTo;      // distTo[v] = number of edges shortest s-v path
    private final int s;         // source vertex
    private Queue<Integer> preorder;   // vertices in preorder
    private Queue<Integer> postorder;  // vertices in postorder

    int dis = 0;

    /**
     * Computes a path between {@code s} and every other vertex in graph {@code G}.
     * @param G the graph
     * @param s the source vertex
     * @throws IllegalArgumentException unless {@code 0 <= s < V}
     */
    public DepthFirstPaths(Graph G, int s) {
        this.s = s;
        edgeTo = new int[G.V()];
        marked = new boolean[G.V()];
        distTo = new int[G.V()];
        postorder = new LinkedList<Integer>();
        preorder  = new LinkedList<Integer>();

        validateVertex(s);
    }
    
    public void dfs(Graph G) {
    	dfs(G, s);
    }

    // depth first search from v
    private void dfs(Graph G, int v) {
    	//TODO: Zeilen hinzufuegen
        marked[v] = true;
        preorder.add(v);
        for (int w : G.adj(v)) {
            if (!marked[w]) {
                edgeTo[w] = v;
                distTo[w] = ++dis;
                System.out.println(distTo[v]);
                dfs(G, w);
            }
        }
        --dis;
        postorder.add(v);
    }
    public void nonrecursiveDFS(Graph G) {
    	//TODO: Zeilen hinzufuegen
        int dis = 0;
        preorder.add(s);
        marked = new boolean[G.V()];
        // to be able to iterate over each adjacency list, keeping track of which
        // vertex in each adjacency list needs to be explored next
        Iterator<Integer>[] adj = (Iterator<Integer>[]) new Iterator[G.V()];
        for (int v = 0; v < G.V(); v++)
            adj[v] = G.adj(v).iterator();

        // depth-first search using an explicit stack
        Stack<Integer> stack = new Stack<Integer>();
        marked[s] = true;
        stack.push(s);
        while (!stack.isEmpty()) {
            int v = stack.peek();
            if (adj[v].hasNext()) {
                int w = adj[v].next();
                if (!marked[w]) {
                    preorder.add(w);
                    edgeTo[w] = v;
                    distTo[w] = ++dis;
                    // discovered vertex w for the first time
                    marked[w] = true;
                    stack.push(w);
                }
            }
            else {
                --dis;
                postorder.add(v);
                stack.pop();
            }
        }
    }
    /**
     * Is there a path between the source vertex {@code s} and vertex {@code v}?
     * @param v the vertex
     * @return {@code true} if there is a path, {@code false} otherwise
     * @throws IllegalArgumentException unless {@code 0 <= v < V}
     */
    public boolean hasPathTo(int v) {
        validateVertex(v);
        return marked[v];
    }

    /**
     * Returns a path between the source vertex {@code s} and vertex {@code v}, or
     * {@code null} if no such path.
     * @param  v the vertex
     * @return the sequence of vertices on a path between the source vertex
     *         {@code s} and vertex {@code v}, as an Iterable
     * @throws IllegalArgumentException unless {@code 0 <= v < V}
     * 
     * This method is different compared to the original one.
     */
    public List<Integer> pathTo(int v) {
		// TODO
        int node = edgeTo[v];
        LinkedList<Integer> path = new LinkedList<>();
        path.add(v);

        while (node != s){
            path.add(node);
            node = edgeTo[node];
        }
        path.add(s);

        return path;
    }
    
    /**
     * Returns the vertices in postorder. This method differs from the original. 
     * @return the vertices in postorder, as a queue of vertices
     */
    public Queue<Integer> post() {
        return postorder;
    }

    /**
     * Returns the vertices in preorder. This method differs from the original.
     * @return the vertices in preorder, as a queue of vertices 
     */
    public Queue<Integer> pre() {
        return preorder;
    }   
    
    /**
     * Returns the class variable edgeTo. This method differs from the original.
     * @return egdeTo
     */
    public int [] edge() {
    	return edgeTo;
    }
    
    /**
     * Returns the class variable distTo. This method differs from the original.
     * @return distTo
     */
    public int [] dist() {
    	return distTo;
    }

    // throw an IllegalArgumentException unless {@code 0 <= v < V}
    private void validateVertex(int v) {
        int V = marked.length;
        if (v < 0 || v >= V)
            throw new IllegalArgumentException("vertex " + v + " is not between 0 and " + (V-1));
    }


    public static void main(String[] args) {
        /*int V = 7;
        int root = 0;
        Graph G = new Graph(V);
        G.addEdge(0, 1);
        G.addEdge(0, 2);
        G.addEdge(1, 3);
        G.addEdge(1, 4);
        G.addEdge(2, 5);
        G.addEdge(2, 6);
        DepthFirstPaths test = new DepthFirstPaths(G, root);

        //Test von dfs Rekursiv:
        test.dfs(G);
        System.out.println("Preorder:");
        for (int a : test.preorder)
            System.out.print(a + " ");

        System.out.println();
        System.out.println("Postorder:");
        for (int a : test.postorder)
            System.out.print(a + " ");

        System.out.println();
        System.out.println("distTo:");
        for (int a : test.distTo)
            System.out.print(a + " ");

        System.out.println();
        System.out.println("edgeTo:");
        for (int a : test.edgeTo)
            System.out.print(a + " ");
        System.out.println();
        System.out.println();

        int i = 0;
        while (test.preorder.size() > 0 || test.postorder.size() > 0) {
            test.preorder.remove(i);
            test.postorder.remove(i);
            i++;
        }*/
        /*
         * expected:
         * preorder: 0 1 3 4 2 5 6
         * postorder: 3 4 1 5 6 2 0
         * distTo: 0 1 1 2 2 2 2
         * EdgeTo: 0 0 0 1 1 2 2
         */

        //Test von dfs nonRekursiv:
        /*test.nonrecursiveDFS(G);
        System.out.println("Preorder:");
        for (int a : test.preorder)
            System.out.print(a + " ");

        System.out.println();
        System.out.println("Postorder:");
        for (int a : test.postorder)
            System.out.print(a + " ");

        System.out.println();
        System.out.println("distTo:");
        for (int a : test.distTo)
            System.out.print(a + " ");

        System.out.println();
        System.out.println("edgeTo:");
        for (int a : test.edgeTo)
            System.out.print(a + " ");
        System.out.println();*/
        /*
         * expected:
         * preorder: 0 1 3 4 2 5 6
         * postorder: 3 4 1 5 6 2 0
         * distTo: 0 1 1 2 2 2 2
         * EdgeTo: 0 0 0 1 1 2 2
         */

        //test für pathTo:
        /*System.out.println("pathTo:");
        for(int a : test.pathTo(5))
            System.out.print(a + " ");
        System.out.println();*/
        /*
         *expected: 5 2 0
         */
    }

}

