package tlacegraph;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import tlace.Value;

/**
 * A TLACE vertex represents a node and recursively all the first nodes of its
 * branches. It has an id, state values, a set of annotations, a set of branches
 * and (possibly null) ingoing and outgoing edges.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class TlaceVertex {

	/**
	 * The id of this vertex.
	 */
	private int id;

	/**
	 * The state values of this vertex.
	 */
	private Value[] state;

	/**
	 * The annotations of this vertex. Contains all the annotations, including
	 * the one explained by a branch.
	 */
	private String[] annotations;

	/**
	 * The set of branches of this vertex. A null value for a specification
	 * means that the branch is composed of only this vertex.
	 */
	private Map<String, TlaceEdge> branches;

	/**
	 * The ingoing edge of this vertex.
	 */
	private TlaceEdge prev;

	/**
	 * The outgoing edge of this vertex.
	 */
	private TlaceEdge next;

	/**
	 * Whether or not this TLACE vertex contains the explanation of all its
	 * existential temporal annotations.
	 */
	private boolean fullyExplained;

	/**
	 * Creates a new TlaceVertex with id, state as state values, annotated with
	 * annotations and having branches. Prev and Next of the new vertex are
	 * null.
	 * 
	 * @param id
	 *            the id of the new vertex.
	 * @param state
	 *            the state values of the new vertex.
	 * @param annotations
	 *            the annotations of the new vertex.
	 * @param branches
	 *            the branches of the new vertex.
	 * @param fullyExplained
	 *            whether or not the new vertex contains all the explanations of
	 *            its existential temporal annotations.
	 */
	public TlaceVertex(int id, Value[] state, String[] annotations,
			Map<String, TlaceEdge> branches, boolean fullyExplained) {
		this.id = id;
		this.state = state;
		this.annotations = annotations;
		this.branches = branches;
		this.fullyExplained = fullyExplained;
	}

	/**
	 * Returns the id of this vertex.
	 * 
	 * @return the id of this vertex.
	 */
	public int getId() {
		return id;
	}

	/**
	 * Returns the state values of this vertex.
	 * 
	 * @return the state values of this vertex.
	 */
	public Value[] getState() {
		return state;
	}

	/**
	 * Returns the annotations of this vertex.
	 * 
	 * @return the annotations of this vertex.
	 */
	public String[] getAnnotations() {
		return annotations;
	}

	/**
	 * Returns the branches of this vertex.
	 * 
	 * @return the branches of this vertex.
	 */
	public Map<String, TlaceEdge> getBranches() {
		return branches;
	}

	/**
	 * Returns the ingoing edge of this vertex.
	 * 
	 * @return the ingoing edge of this vertex, if any, null otherwise.
	 */
	public TlaceEdge getPrev() {
		return prev;
	}

	/**
	 * Returns the outgoing edge of this vertex.
	 * 
	 * @return the outgoing edge of this vertex, if any, null otherwise.
	 */
	public TlaceEdge getNext() {
		return next;
	}

	/**
	 * Returns whether this vertex contains all the explanations of its
	 * existential temporal annotations.
	 * 
	 * @return true if this vertex contains all the explanations of its
	 *         existential temporal annotations, false otherwise.
	 */
	public boolean isFullyExplained() {
		return this.fullyExplained;
	}

	/**
	 * Sets the ingoing transition of this vertex to prev.
	 * 
	 * @param prev
	 *            the new ingoing transition.
	 */
	public void setPrev(TlaceEdge prev) {
		this.prev = prev;
	}

	/**
	 * Sets the outgoing transition of this vertex to next.
	 * 
	 * @param next
	 *            the new outgoing transition.
	 */
	public void setNext(TlaceEdge next) {
		this.next = next;
	}

	/**
	 * Returns the list of all outgoing edges of this vertex.
	 * 
	 * @return the list of all outgoing edges of this vertex.
	 */
	public TlaceEdge[] getNexts() {
		List<TlaceEdge> edges = new ArrayList<TlaceEdge>();
		if (this.getNext() != null) {
			edges.add(this.getNext());
		}
		for (String branch : this.getBranches().keySet()) {
			if (this.getBranches().get(branch) != TlaceEdge.getNull()) {
				edges.add(this.getBranches().get(branch));
			}
		}
		return edges.toArray(new TlaceEdge[edges.size()]);
	}
}
