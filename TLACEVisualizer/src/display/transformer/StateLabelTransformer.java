package display.transformer;

import java.util.HashSet;
import java.util.Set;

import org.apache.commons.collections15.Transformer;

import tlace.Value;

import display.graph.Vertex;

/**
 * A state label transformer is a customized node label transformer that labels
 * nodes with a subset of its state variables. The subset of displayed values
 * can be modified.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class StateLabelTransformer implements Transformer<Vertex, String> {

	/**
	 * The set of displayed variables.
	 */
	private Set<String> variables;

	/**
	 * Creates a new state label transformer.
	 */
	public StateLabelTransformer() {
		this.variables = new HashSet<String>();
	}

	/**
	 * Adds a variable to the set of displayed values.
	 * 
	 * @param variable
	 *            the variable to add.
	 */
	public void addVariable(String variable) {
		this.variables.add(variable);
	}

	/**
	 * Removes a variable from the set of displayed values.
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
	public String transform(Vertex vertex) {
		String result = "<html><center>";

		boolean hasContent = false;
		for (Value value : vertex.getVertex().getState()) {
			if (this.variables.contains(value.getVariable())) {
				result += value + "<br />";
				hasContent = true;
			}
		}
		if (!hasContent) {
			result += vertex.getVertex().getId();
		}

		return result + "</center></html>";
	}

}
