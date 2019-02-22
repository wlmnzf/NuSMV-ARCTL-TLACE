package display.graph;

import java.util.HashSet;
import java.util.Set;

import tlacegraph.TlaceEdge;
import tlacegraph.TlaceVertex;

/**
 * A Vertex represents a vertex of a TlaceGraph. It contains the vertex it
 * represents and a set of displayed branches.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class Vertex {

	/**
	 * The TlaceVertex this vertex represents.
	 */
	private TlaceVertex vertex;

	/**
	 * The set of displayed branches attached to this vertex.
	 */
	private Set<TlaceEdge> displayedBranches;

	/**
	 * Creates a new vertex representing the TlaceVertex.
	 * 
	 * @param vertex
	 *            the TlaceVertex the new vertex represents.
	 */
	public Vertex(TlaceVertex vertex) {
		this.vertex = vertex;
		this.displayedBranches = new HashSet<TlaceEdge>();
	}

	/**
	 * Returns the TlaceVertex this vertex represents.
	 * 
	 * @return the TlaceVertex this vertex represents.
	 */
	public TlaceVertex getVertex() {
		return this.vertex;
	}

	/**
	 * Set branch as a displayed (or not displayed, depending on displayed).
	 * 
	 * @param branch
	 *            the branch to display or not.
	 * @param displayed
	 *            whether or not display the branch.
	 */
	public void setDisplayed(TlaceEdge branch, boolean displayed) {
		if (!displayed && this.displayedBranches.contains(branch)) {
			this.displayedBranches.remove(branch);
		}
		if (displayed && !this.displayedBranches.contains(branch)) {
			this.displayedBranches.add(branch);
		}
	}

	/**
	 * Returns whether branch is currently displayed.
	 * 
	 * @param branch
	 *            the branch.
	 * @return true if the branch is set as displayed, false otherwise.
	 */
	public boolean isDisplayed(TlaceEdge branch) {
		return this.displayedBranches.contains(branch);
	}

}
