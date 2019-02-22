package tlace;

/**
 * A Tlace represents a tree-like annotated counter-example. It is composed of a
 * TLACE node and a specification. The specification is the formula the TLACE
 * node is a counter-example for.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class Tlace {

	/**
	 * The CTL formula this counter-example is for.
	 */
	private String specification;

	/**
	 * The TLACE node representing the explanation of the violation.
	 */
	private Node node;

	/**
	 * Creates a new counter-example for specification with node as the TLACE
	 * node explaining the violation.
	 * 
	 * @param specification
	 *            the specification of the counter-example.
	 * @param node
	 *            the TLACE node explaining the violation.
	 */
	public Tlace(String specification, Node node) {
		this.specification = specification;
		this.node = node;
	}
	
	/**
	 * Returns the specification of this counter-example.
	 * 
	 * @return the specification of this counter-example.
	 */
	public String getSpecification() {
		return specification;
	}

	/**
	 * Returns the node explaining the violation of the specification.
	 * 
	 * @return the node explaining the violation of the specification.
	 */
	public Node getNode() {
		return node;
	}
}
