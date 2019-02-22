package tlacegraph;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import tlace.Node;
import tlace.Tlace;
import tlace.Transition;
import tlace.Value;

/**
 * TLACE Graph Builder is able to build the graph induced by a given TLACE.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class TlaceGraphBuilder {

	/**
	 * A global variable to give a unique id to each vertex of a TlaceGraph.
	 */
	private static int idVertex;

	/**
	 * Returns the TlaceGraph representing the given TLACE.
	 * 
	 * @param tlace
	 *            the TLACE to build the graph from.
	 * @return the graph of the given TLACE.
	 */
	public static TlaceGraph buildTlaceGraph(Tlace tlace) {
		idVertex = 0;
		return new TlaceGraph(tlace.getSpecification(),
				buildTlaceVertex(tlace.getNode()));
	}

	/**
	 * Returns a TlaceVertex representing the given node.
	 * 
	 * @param node
	 *            the node to build the vertex from.
	 * @return the vertex representing the given node.
	 */
	private static TlaceVertex buildTlaceVertex(Node node) {
		int id = idVertex++;

		Value[] state = node.getState();

		// Get all annotations: atomics, existentials and universals
		List<String> annotations = new ArrayList<String>();
		for (String atomic : node.getAtomics()) {
			annotations.add(atomic);
		}
		for (String existential : node.getExistentials().keySet()) {
			annotations.add(existential);
		}
		for (String universal : node.getUniversals()) {
			annotations.add(universal);
		}

		// Get all branches: all the branches of its children plus its own
		// branches
		boolean fullyExplained = true;
		Map<String, TlaceEdge> branches = new HashMap<String, TlaceEdge>();
		for (String existential : node.getExistentials().keySet()) {

			Node n = node.getExistentials().get(existential);

			// The node can be null if the spec is not explained
			if (n != null) {
				TlaceVertex path = buildPath(n);

				// Merge annotations
				for (String annotation : path.getAnnotations()) {
					annotations.add(annotation);
				}

				// Merge branches
				for (String spec : path.getBranches().keySet()) {
					branches.put(spec, path.getBranches().get(spec));
				}

				// Add this branch
				branches.put(
						existential,
						(path.getNext() == null ? TlaceEdge.getNull() : path
								.getNext()));
			} else {
				fullyExplained = false;
			}
		}

		TlaceVertex result = new TlaceVertex(id, state,
				annotations.toArray(new String[annotations.size()]), branches,
				fullyExplained);
		for (TlaceEdge edge : result.getBranches().values()) {
			if (edge != null) {
				edge.setFrom(result);
			}
		}
		return result;
	}

	/**
	 * Returns the initial vertex of the TLACE path starting at the given node.
	 * 
	 * @param node
	 *            the first node of the path to build a vertex path from.
	 * @return the initial vertex representing the path starting at the given
	 *         node.
	 */
	private static TlaceVertex buildPath(Node node) {
		TlaceVertex initial = buildTlaceVertex(node);
		TlaceVertex prev = initial;
		Transition tr = node.getNext();

		// vertices keeps track for each node to the corresponding vertex. It is
		// necessary for loops.
		Map<Node, TlaceVertex> vertices = new HashMap<Node, TlaceVertex>();
		vertices.put(node, initial);

		// When a loop appears, the transition leads to a node that doesn't have
		// this transition as previous.
		while (tr != null && tr.getTo().getPrev() == tr) {

			TlaceVertex vertex = buildTlaceVertex(tr.getTo());
			TlaceEdge edge = new TlaceEdge(tr.getInputs(), prev, vertex);
			vertex.setPrev(edge);
			prev.setNext(edge);

			vertices.put(tr.getTo(), vertex);

			tr = tr.getTo().getNext();
			prev = vertex;
		}

		// Loop
		if (tr != null) {
			prev.setNext(new TlaceEdge(tr.getInputs(), prev, vertices.get(tr
					.getTo())));
		}

		return initial;
	}

}
