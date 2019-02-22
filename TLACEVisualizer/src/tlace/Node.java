package tlace;

import java.util.Map;

/**
 * A node represents a TLACE node. It has an id, a set of values representing
 * its state, a set of atomic propositions, a set of existential temporal
 * formulas with their (possibly empty) explanation, a set of universal temporal
 * formulas, and (possibly null) ingoing and outgoing transitions.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */

public class Node {

	/**
	 * The id of this TLACE node.
	 */
	private int id;

	/**
	 * The set of values of the state of this TLACE node.
	 */
	private Value[] state;

	/**
	 * The set of atomic propositions annotating this node.
	 */
	private String[] atomics;

	/**
	 * The set of existential temporal formulas with their explanation. If there
	 * is no explanation to formula phi, existentials[phi] is null.
	 */
	private Map<String, Node> existentials;

	/**
	 * The set of universal temporal annotations of this node.
	 */
	private String[] universals;

	/**
	 * The preceding node. Is null if the node belongs to no path or if it is
	 * the first one.
	 */
	private Transition prev;

	/**
	 * The following node. Is null if the node belongs to no path or if it is
	 * the last one.
	 */
	private Transition next;

	/**
	 * Creates a new node.
	 * 
	 * @param id
	 *            the id of the new node.
	 * @param state
	 *            the state values of the new node.
	 * @param atomics
	 *            the (possibly empty) set of atomic propositions of the new
	 *            node.
	 * @param existentials
	 *            a map associating a set of existential temporal formulas to
	 *            their (possibly null) explanation.
	 * @param universals
	 *            the (possibly empty) set of universal temporal formulas of the
	 *            new node.
	 */
	public Node(int id, Value[] state, String[] atomics,
			Map<String, Node> existentials, String[] universals) {
		this.id = id;
		this.state = state;
		this.atomics = atomics;
		this.existentials = existentials;
		this.universals = universals;
	}

	/**
	 * Returns the id of this node.
	 * 
	 * @return the id of this node.
	 */
	public int getId() {
		return id;
	}

	/**
	 * Returns the state values of this node.
	 * 
	 * @return the state values of this node.
	 */
	public Value[] getState() {
		return state;
	}

	/**
	 * Returns the atomic propositions annotating this node.
	 * 
	 * @return the atomic propositions annotating this node.
	 */
	public String[] getAtomics() {
		return atomics;
	}

	/**
	 * Returns a map associating a set of existential temporal formulas to their
	 * (possibly null) explanation.
	 * 
	 * @return the existential temporal formulas annotating this node with their
	 *         (possibly null) explanation.
	 */
	public Map<String, Node> getExistentials() {
		return existentials;
	}

	/**
	 * Returns the universal temporal formulas annotating this node.
	 * 
	 * @return the universal temporal formulas annotating this node.
	 */
	public String[] getUniversals() {
		return universals;
	}

	/**
	 * Returns the (possibly null) ingoing transition.
	 * 
	 * @return the (possibly null) ingoing transition.
	 */
	public Transition getPrev() {
		return prev;
	}

	/**
	 * Returns the (possibly null) outgoing transition.
	 * 
	 * @return the (possibly null) outgoing transition.
	 */
	public Transition getNext() {
		return next;
	}

	/**
	 * Sets the ingoing transition of this node to prev.
	 * 
	 * @param prev
	 *            the new ingoing transition.
	 */
	public void setPrev(Transition prev) {
		this.prev = prev;
	}

	/**
	 * Sets the outgoing transition of this node to next.
	 * 
	 * @param next
	 *            the new outgoing transition.
	 */
	public void setNext(Transition next) {
		this.next = next;
	}
}
