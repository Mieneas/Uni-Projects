import java.util.*;

public class Maze{
    private final int N;
    private Graph M;
    public int startnode;

	public Maze(int N, int startnode) {
		
        if (N < 0) throw new IllegalArgumentException("Number of vertices in a row must be nonnegative");
        this.N = N;
        this.M= new Graph(N*N);
        this.startnode= startnode;
        buildMaze();
	}
	
    public Maze (In in) {
    	this.M = new Graph(in);
    	this.N= (int) Math.sqrt(M.V());
    	this.startnode=0;
    }

	
    /**
     * Adds the undirected edge v-w to the graph M.
     *
     * @param  v one vertex in the edge
     * @param  w the other vertex in the edge
     * @throws IllegalArgumentException unless both {@code 0 <= v < V} and {@code 0 <= w < V}
     */
    public void addEdge(int v, int w) {
		// TODO
        M.addEdge(v, w);
    }
    
    /**
     * Returns true if there is an edge between 'v' and 'w'
     * @param v
     * @param w
     * @return true or false
     */
    public boolean hasEdge( int v, int w){
		// TODO
        if(M.adj(v).contains(w) && M.adj(w).contains(v)) {
            return true;
        }else return v == w;
    }

     /**
     * Builds a grid as a graph.
     * @return Graph G -- Basic grid on which the Maze is built
     */
    public Graph mazegrid() {
		// TODO
        int nodeNur = N * N;
        int wurzel = N;

        Graph G = new Graph(nodeNur);

        for(int i = 0; i <= nodeNur - wurzel; i += wurzel){
            for(int j = i; j < wurzel + i - 1; j++){
                G.addEdge(j, j + 1);
            }

        }

        for(int i = 0; i < wurzel; i++){
            for(int j = i; j < nodeNur - wurzel + i; j += wurzel){
                G.addEdge(j, j + wurzel);
            }
        }
        return G;
    }
    
    private void buildMaze() {
        // TODO
        RandomDepthFirstPaths random = new RandomDepthFirstPaths(this.mazegrid(), startnode);

        random.randomDFS(this.mazegrid());

        for(int j = 0; j < N * N; j++) {
            if(!hasEdge(j, random.edge()[j]))
                M.addEdge(j, random.edge()[j]);
        }
    }

    public List<Integer> findWay(int v, int w){
        // TODO
        LinkedList<Integer> path = new LinkedList<>();
        boolean[] marked;

        path.add(v);
        marked = new boolean[this.M.V()];
        // to be able to iterate over each adjacency list, keeping track of which
        // vertex in each adjacency list needs to be explored next
        Iterator<Integer>[] adj = (Iterator<Integer>[]) new Iterator[this.M.V()];
        for (int k = 0; k < this.M.V(); k++)
            adj[k] = this.M.adj(k).iterator();

        // depth-first search using an explicit stack
        Stack<Integer> stack = new Stack<Integer>();
        marked[startnode] = true;
        stack.push(startnode);
        while (!stack.isEmpty()) {
            int v1 = stack.peek();
            if (adj[v1].hasNext()) {
                int w1 = adj[v1].next();
                if (!marked[w1]) {
                    path.add(w1);
                    // discovered vertex w for the first time
                    marked[w1] = true;
                    if(w == w1)
                        break;;
                    stack.push(w1);
                }
            }
            else {
                path.removeLast();
                stack.pop();
            }
        }

        return path;
    }


    public Graph M() {
        return M;
    }

    public static void main(String[] args) {
        // FOR TESTING
        /*int vertex = 90;
        Maze test = new Maze(vertex,0);

        //Test mazegrid:
        //Graph G =  test.mazegrid();
        //System.out.println(test.M.toString());
        //GridGraph test1 = new GridGraph(G);
        //expected: ein Quadrat(Bild) und alle Konotennummer.

        //Test addEdge:
        //test.M.addEdge(0, (test.N * test.N) - 1);
        //test.M.addEdge(test.N - 1, (test.N * test.N) - test.N );
        //GridGraph test2 = new GridGraph(test.M);
        //expected: X

        //Trst hasEdge
        if(test.hasEdge(0, (test.N * test.N) - 1))
            System.out.println("treu");
        else System.out.println("false");
        //expected: true, falls addEdge nicht kommentiert, falls nicht false.

        if(test.hasEdge(0, 1))
            System.out.println("treu");
        else System.out.println("false");
        //expected: false

        //Test biuldMaze:
        LinkedList<Integer> path;
        path = (LinkedList<Integer>) test.findWay(0, (vertex * vertex) - 1);
        GridGraph test3 = new GridGraph(test.M, path);
        System.out.println(test.M.toString());

        System.out.println();
        //Test findWay:
        for(int a : path)
            System.out.print(a + " ");
        System.out.println();*/
    }
}
