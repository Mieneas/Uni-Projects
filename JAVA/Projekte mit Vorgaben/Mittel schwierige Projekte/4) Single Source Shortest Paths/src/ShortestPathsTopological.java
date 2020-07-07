import java.util.Stack;
import java.util.Collections;

public class ShortestPathsTopological {
    private int[] parent;
    private int s;
    private double[] dist;

    public ShortestPathsTopological(WeightedDigraph G, int s) {
        // TODO
        this.s = s;
        parent = new int[G.V()];
        TopologicalWD topo = new TopologicalWD(G);
        dist = new double[G.V()];
        for(int i = 0; i < dist.length; i++){
            dist[i] = Integer.MAX_VALUE;
        }
        dist[s] = 0;
        topo.dfs(s);
        Collections.reverse(topo.order());
        for(int v : topo.order()){
            for(DirectedEdge e: G.incident(v)){
                relax(e);
            }

        }

    }

    public void relax(DirectedEdge e) {
        // TODO
        if(dist[e.to()] > dist[e.from()] + e.weight()){
            parent[e.to()] = e.from();
            dist[e.to()] = dist[e.from()] + e.weight();
        }
    }

    public boolean hasPathTo(int v) {
        return parent[v] >= 0;
    }

    public Stack<Integer> pathTo(int v) {
        if (!hasPathTo(v)) {
            return null;
        }
        Stack<Integer> path = new Stack<>();
        for (int w = v; w != s; w = parent[w]) {
            path.push(w);
        }
        path.push(s);
        return path;
    }
}

