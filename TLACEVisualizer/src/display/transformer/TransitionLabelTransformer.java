package display.transformer;

import java.util.HashSet;
import java.util.Set;

import org.apache.commons.collections15.Transformer;

import tlace.Value;

import display.graph.Edge;

/**
 * A transition label transformer is a customized edge label transformer that
 * displays, for a given edge, a subset of its input variables. The subset can
 * be modified.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class TransitionLabelTransformer implements Transformer<Edge, String> {

	/**
	 * The set of variables to display.
	 */
	private Set<String> variables;

	/**
	 * Creates a new transition label transformer.
	 */
	public TransitionLabelTransformer() {
		this.variables = new HashSet<String>();
	}

	/**
	 * Adds a variable to the set of values to display.
	 * 
	 * @param variable
	 *            the variable to add.
	 */
	public void addVariable(String variable) {
		this.variables.add(variable);
	}

	/**
	 * Removes a variable from the set of values to display.
	 * 
	 * @param variable
	 *            the variable to remove.
	 */
	public void removeVariable(String variable) {
		this.variables.remove(variable);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.apache.commons.collections15.Transformer#transform(java.lang.Object)
	 */
	@Override
	public String transform(Edge edge) {
		String result = "<html><center>";

		for (Value value : edge.getEdge().getInputs()) {
			if (this.variables.contains(value.getVariable())) {
				result += value + "<br />";
			}
		}

		return result + "</center></html>";
	}

}
